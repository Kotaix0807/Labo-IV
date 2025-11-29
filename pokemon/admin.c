#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>

#include "ipc.h"
#include "net.h"

int main(void)
{
    struct {
        int fd;
        int id;
    } players[MAX_PLAYERS] = {0};
    int connected = 0;

    int lfd = net_listen(NET_PORT);
    if (lfd < 0)
    {
        perror("listen");
        return 1;
    }

    printf("Admin: escuchando en puerto %d\n", NET_PORT);

    while (connected < MAX_PLAYERS)
    {
        int cfd = net_accept(lfd);
        if (cfd < 0)
        {
            perror("accept");
            continue;
        }
        players[connected].fd = cfd;
        players[connected].id = connected + 1;
        connected++;
        printf("Admin: se conectó jugador %d (fd=%d)\n", players[connected - 1].id, cfd);

        net_packet wait_pkt = {0};
        wait_pkt.kind = PKT_EVT;
        wait_pkt.code = EVT_WAITING;
        wait_pkt.player_id = players[connected - 1].id;
        wait_pkt.arg1 = connected;
        net_send_packet(cfd, &wait_pkt);
    }

    /* Enviar START a todos */
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        int opp_idx = (i + 1) % MAX_PLAYERS;
        net_packet start = {0};
        start.kind = PKT_EVT;
        start.code = EVT_START;
        start.player_id = players[i].id;
        start.arg1 = players[opp_idx].id; /* opponent id */
        net_send_packet(players[i].fd, &start);
    }

    int exited = 0;
    while (exited < MAX_PLAYERS)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        int maxfd = -1;
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            if (players[i].fd > 0)
            {
                FD_SET(players[i].fd, &rfds);
                if (players[i].fd > maxfd)
                    maxfd = players[i].fd;
            }
        }
        if (maxfd < 0)
            break;

        int ready = select(maxfd + 1, &rfds, NULL, NULL, NULL);
        if (ready <= 0)
        {
            if (errno == EINTR)
                continue;
            perror("select");
            break;
        }

        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            if (players[i].fd > 0 && FD_ISSET(players[i].fd, &rfds))
            {
                net_packet pkt;
                if (net_recv_packet(players[i].fd, &pkt) != 0)
                {
                    printf("Admin: jugador %d desconectado\n", players[i].id);
                    close(players[i].fd);
                    players[i].fd = -1;
                    exited++;
                    continue;
                }

                if (pkt.kind != PKT_CMD)
                    continue;

                if (pkt.code == CMD_EXIT)
                {
                    exited++;
                    printf("Admin: jugador %d salió (%d/%d)\n", pkt.player_id, exited, MAX_PLAYERS);
                    close(players[i].fd);
                    players[i].fd = -1;
                }
                else if (pkt.code == CMD_SELECT || pkt.code == CMD_MOVE)
                {
                    int target_idx = (i + 1) % MAX_PLAYERS;
                    if (players[target_idx].fd > 0)
                    {
                        net_packet ev = {0};
                        ev.kind = PKT_EVT;
                        ev.code = (pkt.code == CMD_SELECT) ? EVT_OPP_SELECT : EVT_OPP_MOVE;
                        ev.player_id = pkt.player_id;
                        ev.arg1 = pkt.arg1;
                        strncpy(ev.text, pkt.text, sizeof(ev.text) - 1);
                        ev.text[sizeof(ev.text) - 1] = '\0';
                        net_send_packet(players[target_idx].fd, &ev);
                    }
                }
            }
        }
    }

    /* Notificar shutdown */
    net_packet shut = {0};
    shut.kind = PKT_EVT;
    shut.code = EVT_SHUTDOWN;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].fd > 0)
        {
            net_send_packet(players[i].fd, &shut);
            close(players[i].fd);
        }
    }
    close(lfd);
    return 0;
}
