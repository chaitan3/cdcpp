#ifndef SEARCH
#define SEARCH
#include "hub.h"

typedef struct
{
  char nick[STRINGMAX];
  char filename[CMDMAX];
  long long size;
  int slots;
}result;

void * dc_search_thread(void *args);
void dc_search(char *term);
void dc_parse_search_result(int k);
void dc_free_results();
void dc_cancel_search_thread();

#define MAX_RESULTS 1000

#endif
