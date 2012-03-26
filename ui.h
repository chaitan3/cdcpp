#ifndef UI_H
#define UI_H

void dc_ui_init();
void dc_ui_quit();
void dc_ui_print_results();
void dc_ui_search_columns();
void dc_ui_sort_result(int index);

void dc_ui_print_downloads();

#define SKIP_LINES 4
#define DOWNLOAD_LINES 2

#endif
