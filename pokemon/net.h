#ifndef NET_H
#define NET_H

#include "ipc.h"

int net_listen(int port);
int net_accept(int listen_fd);
int net_connect(const char *host, int port);
int net_send_packet(int fd, const net_packet *pkt);
int net_recv_packet(int fd, net_packet *pkt);
void net_close(int fd);

#endif
