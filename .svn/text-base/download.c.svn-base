#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<pthread.h>
#include "download.h"
#include "search.h"
#include "ui.h"
#include "lock2key.h"

extern result *results;
extern char res[CMDMAX];
extern int s_hub;
extern config conf;
extern int sorted_results[MAX_RESULTS];

int s_client = 0;
int res_index = -1;
long long progress = 0;

static pthread_t p_download;

void dc_download(int index)
{
  
  res_index = sorted_results[index];
  pthread_create(&p_download, NULL, &dc_download_thread, NULL);
}

void * dc_download_thread(void *data)
{
  
  if(!dc_connect_to_client(res_index))
  {
    s_client = 0;
    return NULL;
  }
  dc_download_from_client(res_index);
  dc_disconnect_from_client();
  dc_ui_print_downloads();
  return NULL;
}

void dc_receive_from_client(char *buf)
{
  int len;
  len = recv(s_client, buf, CMDMAX, 0);
  if(len <= 0)
  {
    printf("\nDisconnected from client\n");
    perror("recv");
    dc_disconnect_from_hub(1);
  }
  buf[len] = '\0';
}

int dc_connect_to_client(int index)
{
  char client_buf[CMDMAX], client_ip[STRINGMAX];
  int client_port, i, j;
  struct sockaddr_in a_client;
  char *lock_pos, lock[STRINGMAX], *key;
  
  sprintf(client_buf, "$RevConnectToMe %s %s|", conf.nick, results[index].nick);
  j = dc_add_listener("$ConnectToMe");
  dc_send_to_socket(s_hub, client_buf);
  
  dc_thread_read(j);
  i = strlen(res);
  while(res[--i] != ':');
  res[i] = '\0';
  i++;
  sscanf(res, "$ConnectToMe %*s %s", client_ip);
  sscanf(res + i,"%d", &client_port);
  dc_thread_close(1, j);
  
  s_client = socket(AF_INET, SOCK_STREAM, 0);
  a_client.sin_family = AF_INET;
  a_client.sin_port = htons(client_port);
  inet_pton(AF_INET, client_ip, &a_client.sin_addr);
  memset(&a_client.sin_zero, '\0', sizeof(a_client.sin_zero));
  
  if(connect(s_client, (struct sockaddr *)&a_client, sizeof(a_client)) == -1)
  {
    close(s_client);
    return 0;
  }
  
  sprintf(client_buf, "$MyNick %s|$Lock EXTENDEDPROTOCOLABCABCABCABCABCABC Pk=DCPLUSPLUS0.673|", conf.nick);
  dc_send_to_socket(s_client, client_buf);
  dc_receive_from_client(client_buf);
  
  
  lock_pos = strstr(client_buf, "$Lock");
  sscanf(lock_pos, "$Lock %s ", lock);
  key = lock_to_key(lock);
  sprintf(client_buf, "$Supports |$Direction Download 30000|$Key %s|", key);
  free(key);
  dc_send_to_socket(s_client, client_buf);
  return 1;
}

void dc_download_from_client(int index)
{
  char client_buf[RECVMAX + 1], filename[CMDMAX];
  //int file;
  FILE *f;
  int len;
  
  sprintf(client_buf, "$Get %s$1|", results[index].filename);
  dc_send_to_socket(s_client, client_buf);
  dc_receive_from_client(client_buf);
  
  dc_send_to_socket(s_client, "$Send|");
  
  len = strlen(results[index].filename);
  while(results[index].filename[--len] != '\\');
  len++;
  strcpy(filename, results[index].filename + len);
  
  //file = open(filename, O_RDWR | O_CREAT);
  f = fopen(filename, "w");
  
  if(f == NULL)
  {
    printf("Unable to open file for writing\n");
    dc_disconnect_from_hub(1);
    exit(1);
  }
  
  while(progress < results[index].size)
  {
    dc_ui_print_downloads();
    //mmap
    len = recv(s_client, client_buf, RECVMAX, 0);
    progress += len;
    fwrite(client_buf, sizeof(char), len, f);
  }
  fclose(f);
}

void dc_disconnect_from_client()
{
  if(s_client)
    close(s_client);
  s_client = 0;
  res_index = -1;
  progress = 0;
}

void dc_cancel_download_thread()
{
  pthread_cancel(p_download);
  pthread_join(p_download, NULL);
}
