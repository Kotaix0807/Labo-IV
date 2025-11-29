#include "net.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

static int send_all(int fd, const void *buf, size_t len)
{
    const char *p = buf;
    size_t sent = 0;
    while (sent < len)
    {
        ssize_t n = send(fd, p + sent, len - sent, 0);
        if (n <= 0)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        sent += (size_t)n;
    }
    return 0;
}

static int recv_all(int fd, void *buf, size_t len)
{
    char *p = buf;
    size_t recvd = 0;
    while (recvd < len)
    {
        ssize_t n = recv(fd, p + recvd, len - recvd, 0);
        if (n <= 0)
        {
            if (n == 0)
                return -1; /* cierre */
            if (errno == EINTR)
                continue;
            return -1;
        }
        recvd += (size_t)n;
    }
    return 0;
}

int net_listen(int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return -1;

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((uint16_t)port);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        close(fd);
        return -1;
    }
    if (listen(fd, 4) < 0)
    {
        close(fd);
        return -1;
    }
    return fd;
}

int net_accept(int listen_fd)
{
    return accept(listen_fd, NULL, NULL);
}

int net_connect(const char *host, int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return -1;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    if (inet_pton(AF_INET, host, &addr.sin_addr) != 1)
    {
        close(fd);
        return -1;
    }
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        close(fd);
        return -1;
    }
    return fd;
}

int net_send_packet(int fd, const net_packet *pkt)
{
    return send_all(fd, pkt, sizeof(*pkt));
}

int net_recv_packet(int fd, net_packet *pkt)
{
    return recv_all(fd, pkt, sizeof(*pkt));
}

void net_close(int fd)
{
    if (fd >= 0)
        close(fd);
}
