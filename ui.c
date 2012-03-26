#include<ncurses.h>
#include<stdio.h>
#include<pthread.h>
#include<sys/time.h>
#include "search.h"
#include "ui.h"

int xmax, ymax;
int highlight = 0;
int offset = 0;
int sorted_results[MAX_RESULTS];

extern result *results;
extern int count;

extern long long progress;
extern int res_index;

static pthread_mutex_t m_s = PTHREAD_MUTEX_INITIALIZER;

void dc_ui_init()
{
  initscr();
  keypad(stdscr, TRUE);
  noecho();
  getmaxyx(stdscr, ymax, xmax);
  ymax -= DOWNLOAD_LINES;
}

void dc_ui_quit()
{
  pthread_mutex_destroy(&m_s);
  endwin();
}

void dc_ui_search_columns()
{
  int x, y, t;
  printw("Searching....\n");
  t = xmax / 8;
  getyx(stdscr, y, x);
  mvaddnstr(y, 0, "Nick", 4);
  mvaddnstr(y, t, "File", 4);
  //mvaddnstr(y, 4 * t, "Directory", 9);
  mvaddnstr(y, 7 * t, "Size(MB)\n", 9);
  refresh();
}

void dc_ui_print_downloads()
{
  int i, hash;
  struct timeval current;
  long long dt;
  
  static long long last_progress = 0;
  static struct timeval last;
  static float rate = 0;
  
  pthread_mutex_lock(&m_s);
  
  move(ymax, 0);
  clrtobot();
  mvprintw(ymax, 0, "Download: ");
  
  if(res_index >= 0)
  {
    
    
    addnstr(results[res_index].filename, xmax - 10);
    move(ymax + 1, 0);
    
    gettimeofday(&current, NULL);
    dt = current.tv_usec - last.tv_usec + (current.tv_sec - last.tv_sec) * 1000000;
    if(dt > 1000000)
    {
      rate = (((float)(progress - last_progress)) * 1000000) / (1024 * 1024 * dt);
      last = current;
      last_progress = progress;
    }
    printw("%.2f MB/s ", rate); 
    
    addch('[');
    hash = (((float)progress) * (xmax - 24)) / results[res_index].size;
    for(i = 0; i < hash; i++)
      addch('#');
    for(i = hash; i < xmax - 24; i++)
      addch(' ');
    addch(']');
    
    printw("%.0f/%.0f MB", ((float)progress) / (1024 * 1024), ((float)results[res_index].size) / (1024 * 1024));
  }
  refresh();
  
  pthread_mutex_unlock(&m_s);
}

void dc_ui_print_results()
{
  int y, t;
  
  pthread_mutex_lock(&m_s);
  
  move(2, 0);
  clrtoeol();
  if(count < ymax - SKIP_LINES)
    t = count;
  else
    t = ymax - SKIP_LINES;
  mvprintw(2, 0, "Searching... %d-%d of %d Results\n", 1 + offset, t + offset, count);
  
  t = xmax / 8;
  
  for(y = SKIP_LINES; y < ymax; y++)
  {
    move(y, 0);
    clrtoeol();
  }
  
  for(y = 0; (y < ymax - SKIP_LINES) && (y < count); y++)
  {
    if(highlight == y)
      attron(A_STANDOUT);
    mvaddnstr(y + SKIP_LINES, 0, results[sorted_results[y + offset]].nick, t - 1);
    mvaddnstr(y + SKIP_LINES, t, results[sorted_results[y + offset]].filename, 6 * t - 1);
    //mvaddnstr(y, 4 * t, "Directory", 9);
    mvprintw(y + SKIP_LINES, 7 * t, "%.2f\n", ((float)results[sorted_results[y + offset]].size) / (1024 * 1024));
    if(highlight == y)
      attroff(A_STANDOUT);
  }
  refresh();
  
  pthread_mutex_unlock(&m_s);
}

void dc_ui_sort_result(int index)
{
  int y, t;
  for(y = 0; y < index; y++)
  {
    if(results[index].size > results[sorted_results[y]].size)
    {
      for(t = index; t > y; t--)
        sorted_results[t] = sorted_results[t - 1];
      sorted_results[y] = index;
      return;
    }
  }
  
  sorted_results[index] = index;
}
