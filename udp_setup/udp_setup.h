#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close() */
#include <string.h> /* memset() */
#include <sys/time.h> 
#include <errno.h>

#include "../common_util/common_util.h"

#define FRAME_BUFSIZE 65535

#define SERVER_COMMAND_PORT 50000
#define SERVER_STREAM_PORT 50001
#define CLIENT_COMMAND_PORT 50000
#define CLIENT_STREAM_PORT 50001

void udp_set_server_ip(const char* server_ip);

void udp_client_command_setup();
void udp_client_stream_setup();

void udp_client_command_close();
void udp_client_stream_close();

void udp_send_command(const char* cmd);
int udp_recevie_stream(uint8_t* buf, uint32_t* len);
