#ifndef DOWNLOAD
#define DOWNLOAD
#include "hub.h"

int dc_connect_to_client(int index);
void dc_download_from_client(int index);
void dc_disconnect_from_client();
void dc_download(int index);
void * dc_download_thread(void *data);
void dc_cancel_download_thread();

#endif
