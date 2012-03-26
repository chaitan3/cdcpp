#ifndef HUB
#define HUB

#define RECVMAX 50000
#define CMDMAX 500
#define STRINGMAX 50
#define MAX_LISTENERS 10

typedef struct {
  char hub_ip[STRINGMAX];
  short hub_port;
  char nick[STRINGMAX];
  long long share;
  char hubname[STRINGMAX];
} config;

void dc_send_to_socket(int s_hub, char *msg);
void * dc_receive_from(void *args);
void dc_connect_to_hub();
void dc_disconnect_from_hub(int status);
int dc_receive_cmd();
void dc_thread_read(int index);
void dc_thread_close(int close_listener, int index);
int dc_add_listener(char *command);
void dc_remove_listener(int index);
void dc_destroy_mutexes();
void dc_init_mutexes();

#endif
