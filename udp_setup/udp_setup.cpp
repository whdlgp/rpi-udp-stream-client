#include "udp_setup.h"

static int client_command_socket;
static int client_stream_socket;
static struct sockaddr_in server_addr;
static struct sockaddr_in client_addr;
static socklen_t server_addr_len;

void udp_set_server_ip(const char* server_ip)
{
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
}

void udp_client_command_setup()
{
    client_command_socket = socket(AF_INET, SOCK_DGRAM, 0);
    
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = htons(CLIENT_COMMAND_PORT);
    if(bind(client_command_socket
            , (struct sockaddr*)&client_addr
            , sizeof(client_addr)) < 0)
    {
        DEBUG_ERR("command socket bind error\n");
        exit(0);
    }
}

void udp_client_stream_setup()
{
    client_stream_socket = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    client_addr.sin_port = htons(CLIENT_STREAM_PORT);
    if(bind(client_stream_socket
            , (struct sockaddr*)&client_addr
            , sizeof(client_addr)) < 0)
    {
        DEBUG_ERR("stream socket bind error\n");
        exit(0);
    }
}

void udp_client_command_close()
{
    close(client_command_socket);
}

void udp_client_stream_close()
{
    close(client_stream_socket);
}

void udp_send_command(const char* cmd)
{
    server_addr.sin_port = htons(SERVER_COMMAND_PORT);
    if(sendto(client_command_socket
            , cmd
            , strlen(cmd)
            , 0
            , (struct sockaddr*)&server_addr
            , sizeof(server_addr)) < 0)
    {
        DEBUG_ERR("command send error\n");
        exit(0);
    }
}

int udp_recevie_stream(uint8_t* buf, uint32_t* len)
{
    server_addr_len = sizeof(server_addr);
    if((*len = recvfrom(client_stream_socket
            , buf
            , FRAME_BUFSIZE
            , 0
            , (struct sockaddr*)&server_addr
            , &server_addr_len)) < 0)
    {
        DEBUG_ERR("receive stream error\n");
        return 0;
    }
    
    return 1;
}
