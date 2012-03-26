#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include "ui.h"
#include "search.h"
#include "hub.h"

result *results = NULL;
extern char res[CMDMAX];
extern int s_hub;
extern config conf;

static int num = -1;
static pthread_t p_search;
int count;

void dc_search(char *term)
{
  int i, j;
  char term_sent[STRINGMAX], sendbuf[CMDMAX];
  
  j = strlen(term);
  strcpy(term_sent, term);
  for(i = 0; i < j; i++)
  {
    if(term_sent[i] == ' ')
      term_sent[i] = '$';
  }
  
  dc_free_results();
  
  if(num < 0)
  {
    num = dc_add_listener("$SR");
    pthread_create(&p_search, NULL, &dc_search_thread, NULL);
  }
  
  sprintf(sendbuf,"$Search Hub:%s F?F?0?1?%s|", conf.nick, term_sent);
  dc_send_to_socket(s_hub, sendbuf);
  count = 0;
}

void *dc_search_thread(void *args)
{
  while(1)
  {
    dc_thread_read(num);
    results = (result *)realloc(results, sizeof(result) * (count + 1));
    dc_parse_search_result(count);
    dc_thread_close(0, num);
    
    count++;
    
    dc_ui_sort_result(count - 1);
    dc_ui_print_results();
    
  }
  
  return NULL;
}

void dc_parse_search_result(int k)
{
  int i, j;
  sscanf(res, "$SR %s ", results[k].nick);
  i = 0;
  while(res[i++] != ' ');
  while(res[i++] == ' ');
  while(res[i++] != ' ');
  while(res[i++] == ' ');
  i--;
  j = i;
  
  while(res[i++] != '/');
  while(res[i--] != ' ');
  sscanf(res + i + 2, "%d/", &results[k].slots);
  while(res[i--] == ' ');
  i++;
  strncpy(results[k].filename, res + j, (i - j) + 1);
  results[k].filename[i - j + 1] = '\0';
  while(i >= 0)
  {
    if(res[i] == 0x05)
    {
      sscanf(&(results[k].filename[i - j + 1]), "%lld", &results[k].size);
      results[k].filename[i - j] = '\0';
      return;
    }
    i--;
  }
  results[k].size = 0;
}

void dc_free_results()
{
  if(results != NULL)
    free(results);
  results = NULL; 
}

void dc_cancel_search_thread()
{
  pthread_cancel(p_search);
  pthread_join(p_search, NULL);
}
