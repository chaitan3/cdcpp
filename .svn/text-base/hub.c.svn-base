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
#include<semaphore.h>
#include "ui.h"
#include "hub.h"
#include "search.h"
#include "download.h"
#include "lock2key.h"

static char buf[RECVMAX + 1];
static char listener[MAX_LISTENERS][STRINGMAX];
static pthread_mutex_t m, m_l;
static sem_t s_lock[MAX_LISTENERS], s_lock_end;
static int insert_l = 0;
static pthread_t p_hub;

char res[RECVMAX + 1];
int s_hub;

extern config conf;

void dc_send_to_socket(int s, char *msg)
{
  int ret = send(s, msg, strlen(msg), 0);
  
  if(ret == -1)
  {
    perror("send");
    dc_disconnect_from_hub(1);
    exit(1);
  }
}

void * dc_receive_from(void *args)
{
  int len, i;
  char cmd[STRINGMAX], *e;
  
  while(1)
  {
    len = recv(s_hub, buf, RECVMAX, 0);
    if(len <= 0)
    {
      printf("\nDisconnected / Failed to Connect to server\n");
      perror("recv");
      dc_disconnect_from_hub(1);
      exit(1);
    }
    buf[len] = '\0';
    
    pthread_mutex_lock(&m_l);
    if(strcmp(listener[0], "IgnoreMessage") == 0)
    {
      
      e = strchr(buf, '$');
      if(e == NULL)
      {
        pthread_mutex_unlock(&m_l);
        continue;
      }
      else
        sem_post(&s_lock[0]);
    }
    pthread_mutex_unlock(&m_l);
    
    
    while(1)
    {
      pthread_mutex_lock(&m);
      
      if(!dc_receive_cmd())
      {
        pthread_mutex_unlock(&m);
        break;
      }
      sscanf(res,"%s ", cmd);
      
      pthread_mutex_unlock(&m);
      
      pthread_mutex_lock(&m_l);
      for(i = 0; i < MAX_LISTENERS; i++)
      {
        if((strcmp(listener[i], cmd) == 0) && (cmd[0] != '\0'))
        {
          sem_post(&s_lock[i]);
          sem_wait(&s_lock_end);
        }
      }
      pthread_mutex_unlock(&m_l);
      
    }
  }
}

int dc_receive_cmd()
{
  static char *s = buf;
  static int inc = 0;
  char *e;
  
  
  
  if((s[0] == '\0'))
  {
    s = buf;
    return 0;
  }
  e = strchr(s, '|');
  
  if(inc)
  {
    *e = '\0';
    strcpy(res + inc, s);
    s = e + 1;
    inc = 0;
    return 1;
  }
  
  if(e == NULL)
  {
    inc = strlen(s);
    strcpy(res, s);
    s = buf;
    return 0;
  }
  
  *e = '\0';
  strcpy(res, s);
  s = e + 1;
  return 1;
}

int dc_add_listener(char *command)
{
  int i;
  pthread_mutex_lock(&m_l);
  strcpy(listener[insert_l], command);
  i = insert_l;
  while(listener[++insert_l][0] != '\0');
  pthread_mutex_unlock(&m_l);
  return i;
}

void dc_remove_listener(int index)
{
  pthread_mutex_lock(&m_l);
  listener[index][0] = '\0';
  if(insert_l > index)
    insert_l = index;
  pthread_mutex_unlock(&m_l);
}

void dc_thread_read(int index)
{
  sem_wait(&s_lock[index]);
  pthread_mutex_lock(&m);
}
void dc_thread_close(int close_listener, int index)
{
  sem_post(&s_lock_end);
  if(close_listener)
    dc_remove_listener(index);
  pthread_mutex_unlock(&m);
}

void dc_connect_to_hub()
{
  int i, j, index;
  struct sockaddr_in a_hub;
  
  char *key, lock[STRINGMAX], sendbuf[CMDMAX];
  
  s_hub = socket(AF_INET, SOCK_STREAM, 0);
  a_hub.sin_family = AF_INET;
  a_hub.sin_port = htons(conf.hub_port);
  inet_pton(AF_INET, conf.hub_ip, &a_hub.sin_addr);
  memset(&a_hub.sin_zero, '\0', sizeof(a_hub.sin_zero));
  
  if(connect(s_hub, (struct sockaddr *)&a_hub, sizeof(a_hub)) == -1)
  {
    perror("\nconnect");
    dc_disconnect_from_hub(1);
    exit(1);
  }
  dc_init_mutexes();
  index = dc_add_listener("$Lock");
  pthread_create(&p_hub, NULL, &dc_receive_from, (void *)&s_hub);
  
  dc_thread_read(index);
  i = 0;
  j = strlen(res) - 3;
  while(res[i] != ' ') i++;
  while(res[i] == ' ') i++;
  while(strncmp(res + j, "Pk=", 3) != 0) j--;
  j--;
  while(res[j] == ' ') j--;
  j++;
  res[j] = '\0';
  strcpy(lock, res + i);
  dc_thread_close(1, index);
    
  key = lock_to_key(lock);
  sprintf(sendbuf, "$Key %s|", key);
  free(key);
  
  index = dc_add_listener("$HubName");
  dc_send_to_socket(s_hub, sendbuf);
  dc_thread_read(index);
  sscanf(res, "$HubName %s", conf.hubname);
  dc_thread_close(1, index);
  
  sprintf(sendbuf, "$ValidateNick %s|", conf.nick);
  index = dc_add_listener("$Hello");
  dc_send_to_socket(s_hub, sendbuf);
  dc_thread_read(index);
  dc_thread_close(1, index);
  
  sprintf(sendbuf, "$MyINFO $ALL %s <++ V:0.673,M:P,H:1/0/0,S:2>$ $0.1.$$%lld$|", conf.nick, conf.share);
  index = dc_add_listener("IgnoreMessage");
  dc_send_to_socket(s_hub, sendbuf);
  sem_wait(&s_lock[index]);
  
  dc_remove_listener(index);
}

void dc_init_mutexes()
{
  int i;
  for(i = 0; i < MAX_LISTENERS; i++)
  {
    listener[i][0] = '\0';
    sem_init(&s_lock[i], 0, 0);
  }
  sem_init(&s_lock_end, 0, 0);
  pthread_mutex_init(&m, NULL);
  pthread_mutex_init(&m_l, NULL);
}
void dc_destroy_mutexes()
{
  int i;
  pthread_mutex_destroy(&m);
  pthread_mutex_destroy(&m_l);
  for(i = 0; i < MAX_LISTENERS; i++)
    sem_destroy(&s_lock[i]);
  sem_destroy(&s_lock_end);
}

void dc_disconnect_from_hub(int status)
{
  pthread_cancel(p_hub);
  pthread_join(p_hub, NULL);
  
  dc_destroy_mutexes();
  
  dc_free_results();
  dc_cancel_search_thread();
  
  dc_cancel_download_thread();
  dc_disconnect_from_client();
  
  close(s_hub);
  
  dc_ui_quit();
  
  exit(status);
}

