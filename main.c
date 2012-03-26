#include<stdio.h>
#include<curses.h>
#include "ui.h"
#include "hub.h"
#include "search.h"
#include "download.h"

extern result *results;
extern int highlight, ymax, count;
extern int offset;
extern int res_index;

config conf = { "10.7.7.7", 
                411,
                "firshy",
                10737418240
              };


/* TODO
 * mutex for screen (refresh) write
 * implement waiting for slot
 * Error messages from server in command form
 * 
 * Bugs:
 * fixing realloc crash bug
 */

int main(int argc, char **argv)
{
  
  char term[STRINGMAX];
  int ch;
  
  printf("Connecting to Hub %s.....", conf.hub_ip);
  fflush(stdout);
  dc_connect_to_hub();
  
  dc_ui_init();
  
  while(1)
  {
    printw("Hub: %s\n", conf.hubname);
    printw("Search For: ");
    refresh();
    echo();
    getnstr(term, STRINGMAX);
    noecho();
    refresh();
    
    dc_ui_search_columns();
    dc_ui_print_downloads();
    dc_search(term);
    
    while(1)
    {
      ch  = getch();
      if(ch == 's')
      {
        highlight = 0;
        offset = 0;
        clear();
        break;
      }
      switch(ch)
      {
        case KEY_DOWN:
        if((highlight + offset) < (count - 1))
        {
          if(highlight < ymax - (SKIP_LINES + 1))
            highlight++;
          else
            offset++;
          dc_ui_print_results();
        }
        break;
        case KEY_UP:
        if(highlight > 0)
        {
          highlight--;
          dc_ui_print_results();
        }
        else if(offset > 0)
        {
          offset--;
          dc_ui_print_results();
        }
        break;
        case 'q':
        dc_disconnect_from_hub(0);
        return 0;
        break;
        case 'c':
        if(res_index >= 0)
        {
          dc_cancel_download_thread();
          dc_disconnect_from_client();
          dc_ui_print_downloads();
        }
        break;
        case 'd':
        if(res_index < 0)
        {
          dc_download(offset + highlight);
          dc_ui_print_downloads();
        }
        break;
        default:
        break;
      }
    }
  }
  
  return 0;
} 
