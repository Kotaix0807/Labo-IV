#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "ipc.h"

int main(void)
{
    struct {
        pid_t pid;
        int   id;
    } players[MAX_PLAYERS] = {0};
    int connected = 0;

    key_t key = ftok(MSG_KEY_PATH, MSG_KEY_ID);
    if (key == -1)
    {
        perror("ftok");
        exit(1);
    }

    int qid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (qid == -1)
    {
        if (errno == EEXIST)
        {
            /* limpiar cola previa para evitar mensajes antiguos con tamaños distintos */
            qid = msgget(key, 0666);
            if (qid != -1)
                msgctl(qid, IPC_RMID, NULL);
            qid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
        }
        if (qid == -1)
        {
            perror("msgget");
            exit(1);
        }
    }

    printf("Admin: cola creada, qid=%d\n", qid);
    printf("Admin: esperando a %d jugadores...\n", MAX_PLAYERS);

    struct msg_command cmd;
    struct msg_event ev;

    while (connected < MAX_PLAYERS)
    {
        if (msgrcv(qid, &cmd, sizeof(cmd) - sizeof(long), 1, MSG_NOERROR) == -1)
        {
            perror("msgrcv");
            exit(1);
        }

        if (cmd.cmd == CMD_JOIN)
        {
            if (connected >= MAX_PLAYERS)
                continue;

            players[connected].pid = cmd.player_pid;
            players[connected].id = connected + 1;
            connected++;

            printf("Admin: se conectó jugador_id=%d, PID=%d (total=%d)\n",
                   players[connected - 1].id, players[connected - 1].pid, connected);

            ev.mtype = players[connected - 1].pid;
            ev.player_id = players[connected - 1].id;
            ev.opponent_id = 0;
            ev.evt = EVT_WAITING;
            ev.connected = connected;
            if (msgsnd(qid, &ev, sizeof(ev) - sizeof(long), 0) == -1)
                perror("msgsnd EVT_WAITING");
        }
    }

    printf("Admin: ya están los %d jugadores, empezamos el juego.\n", MAX_PLAYERS);

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        int opponent_idx = (i + 1) % MAX_PLAYERS;
        ev.mtype = players[i].pid;
        ev.player_id = players[i].id;
        ev.opponent_id = players[opponent_idx].id;
        ev.evt = EVT_START;
        ev.connected = connected;
        if (msgsnd(qid, &ev, sizeof(ev) - sizeof(long), 0) == -1)
            perror("msgsnd EVT_START");
    }

    int exited = 0;
    while (exited < MAX_PLAYERS)
    {
        if (msgrcv(qid, &cmd, sizeof(cmd) - sizeof(long), 1, MSG_NOERROR) == -1)
        {
            perror("msgrcv CMD_EXIT");
            break;
        }

        if (cmd.cmd == CMD_EXIT)
        {
            exited++;
            printf("Admin: jugador %d (PID=%d) salió (%d/%d)\n",
                   cmd.player_id, cmd.player_pid, exited, MAX_PLAYERS);
        }
        else if (cmd.cmd == CMD_SELECT)
        {
            pid_t target = 0;
            int opp_id = 0;
            for (int i = 0; i < MAX_PLAYERS; i++)
            {
                if (players[i].pid != 0 && players[i].pid != cmd.player_pid)
                {
                    target = players[i].pid;
                    opp_id = players[i].id;
                    break;
                }
            }
            if (target != 0)
            {
                ev.mtype = target;
                ev.player_id = cmd.player_id;
                ev.opponent_id = opp_id;
                ev.evt = EVT_OPP_SELECT;
                ev.connected = connected;
                ev.data1 = 0;
                strncpy(ev.text, cmd.text, sizeof(ev.text) - 1);
                ev.text[sizeof(ev.text) - 1] = '\0';
                if (msgsnd(qid, &ev, sizeof(ev) - sizeof(long), 0) == -1)
                    perror("msgsnd EVT_OPP_SELECT");
            }
        }
        else if (cmd.cmd == CMD_MOVE)
        {
            pid_t target = 0;
            int opp_id = 0;
            for (int i = 0; i < MAX_PLAYERS; i++)
            {
                if (players[i].pid != 0 && players[i].pid != cmd.player_pid)
                {
                    target = players[i].pid;
                    opp_id = players[i].id;
                    break;
                }
            }
            if (target != 0)
            {
                ev.mtype = target;
                ev.player_id = cmd.player_id;
                ev.opponent_id = opp_id;
                ev.evt = EVT_OPP_MOVE;
                ev.connected = connected;
                ev.data1 = cmd.arg1;
                ev.text[0] = '\0';
                if (msgsnd(qid, &ev, sizeof(ev) - sizeof(long), 0) == -1)
                    perror("msgsnd EVT_OPP_MOVE");
            }
        }
    }

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].pid == 0)
            continue;
        ev.mtype = players[i].pid;
        ev.player_id = 0;
        ev.opponent_id = 0;
        ev.evt = EVT_SHUTDOWN;
        ev.connected = connected;
        ev.data1 = 0;
        ev.text[0] = '\0';
        if (msgsnd(qid, &ev, sizeof(ev) - sizeof(long), 0) == -1)
            perror("msgsnd EVT_SHUTDOWN");
    }

    msgctl(qid, IPC_RMID, NULL);
    return 0;
}
