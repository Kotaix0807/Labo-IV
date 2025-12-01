#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>
#include "structs.h"
#include "ipc.h"

#define MAX_PLAYERS 2

int recive_players();
int recive_pkmn();
int recive_movement();
void send_turn_results(int movements[2]);

int player_ids[MAX_PLAYERS] = {0};
char player_pkmn[MAX_PLAYERS][32] = {{0}};
char player_names[MAX_PLAYERS][32] = {{0}};
pkmn player_strc[MAX_PLAYERS] = {0};
key_t key;
int msgid;

int main()
{
    if(!recive_players())
        return 1;
    
    while(1)
    {
        if(!recive_pkmn())
            return 1;
        while (recive_movement() == -1);
    }

    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}

int recive_players()
{
    msg_data message = {0};
    key = ftok(FTOK_PATH, ID);

    msgid = msgget(key, 0666 | IPC_CREAT);

    for(int i = 0; i < MAX_PLAYERS ; i++)
    {
        printf("Esperando jugadores... (%d/%d)\n", i, MAX_PLAYERS);
        if (msgrcv(msgid, &message, sizeof(message) - sizeof(message.mtype), ADMIN, 0) == -1)
        {
            perror("msgrcv");
            msgctl(msgid, IPC_RMID, NULL);
            return 0;
        }
        if(message.player_cmd == CMD_JOIN)
            printf("Jugador '%d' se unio!\n", message.player_id);
        player_ids[i] = message.player_id;
        message = (msg_data){0};
    }
    printf("\n");
    usleep(100);
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        msg_data respuesta = {0};
        respuesta.mtype = PLAYERS + player_ids[i];
        respuesta.player_id = player_ids[i];
        if (msgsnd(msgid, &respuesta, sizeof(respuesta) - sizeof(respuesta.mtype), 0) == -1)
        {
            perror("msgsnd");
            msgctl(msgid, IPC_RMID, NULL);
            return 0;
        }
        printf("Respuesta enviada a ID %d (mtype %ld)\n", respuesta.player_id, respuesta.mtype);
    }
    return 1;
}

int recive_pkmn()
{
    printf("Esperando seleccion de pokemon...\n");
    int info_count = 0;
    while (info_count < MAX_PLAYERS)
    {
        pkmn_info_msg info = {0};
        if (msgrcv(msgid, &info, sizeof(info) - sizeof(info.mtype), ADMIN, 0) == -1)
        {
            perror("msgrcv");
            msgctl(msgid, IPC_RMID, NULL);
            return 0;
        }
        if (info.player_cmd == CMD_EXIT)
        {
            printf("Jugador '%d' se fue... Abortando...\n", info.player_id);
            return 0;
        }
        int idx = -1;
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            if (player_ids[i] == info.player_id)
            {
                idx = i;
                break;
            }
        }
        if (idx == -1)
        {
            printf("ID %d no registrado, descartando\n", info.player_id);
            continue;
        }
        strncpy(player_pkmn[idx], info.pkmn_name, sizeof(player_pkmn[idx]) - 1);
        player_pkmn[idx][sizeof(player_pkmn[idx]) - 1] = '\0';
        pkmnSet(&player_strc[idx], player_pkmn[idx], 1);
        strncpy(player_names[idx], info.player_name, sizeof(player_names[idx]) - 1);
        player_names[idx][sizeof(player_names[idx]) - 1] = '\0';
        info_count++;
        printf("Jugador %d (%s) eligio %s\n", info.player_id, player_names[idx], player_pkmn[idx]);
    }

    int change = MAX_PLAYERS - 1;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        pkmn_info_msg respuesta = {0};
        respuesta.mtype = PLAYERS + player_ids[i];
        respuesta.player_id = player_ids[i];
        respuesta.player_cmd = CMD_SEND_INFO;
        strncpy(respuesta.pkmn_name, player_pkmn[change], sizeof(respuesta.pkmn_name));
        respuesta.pkmn_name[sizeof(respuesta.pkmn_name) - 1] = '\0';
        strncpy(respuesta.player_name, player_names[change], sizeof(respuesta.player_name));
        respuesta.player_name[sizeof(respuesta.player_name) - 1] = '\0';
        if (msgsnd(msgid, &respuesta, sizeof(respuesta) - sizeof(respuesta.mtype), 0) == -1)
        {
            perror("msgsnd");
            msgctl(msgid, IPC_RMID, NULL);
            return 0;
        }
        printf("Respuesta enviada a ID %d (mtype %ld)\n", respuesta.player_id, respuesta.mtype);
        change--;
    }

    return 1;
}

int recive_movement()
{
    printf("Esperando seleccion de movimiento...\n");
    int info_count = 0;
    int movements[MAX_PLAYERS];
    for (int i = 0; i < MAX_PLAYERS; i++)
        movements[i] = -1;

    while (info_count < MAX_PLAYERS)
    {
        mv_msg_data info = {0};
        if (msgrcv(msgid, &info, sizeof(info) - sizeof(info.mtype), ADMIN, 0) == -1)
        {
            perror("msgrcv");
            msgctl(msgid, IPC_RMID, NULL);
            return 0;
        }
        int idx = -1;
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            if (player_ids[i] == info.player_id)
            {
                idx = i;
                break;
            }
        }
        if (idx == -1)
        {
            printf("ID %d no registrado, descartando\n", info.player_id);
            continue;
        }
        if (movements[idx] != -1)
        {
            printf("Jugador %d ya envio movimiento, ignorando duplicado\n", info.player_id);
            continue;
        }
        if (info.move < 0 || info.move >= 4)
        {
            printf("Movimiento invalido %d de jugador %d\n", info.move, info.player_id);
            continue;
        }
        if (strcmp(info.pkmn_name, player_pkmn[idx]) != 0)
        {
            printf("Nombre de pkmn no coincide para jugador %d (%s vs %s)\n",
                   info.player_id, info.pkmn_name, player_pkmn[idx]);
            continue;
        }
        movements[idx] = info.move;
        info_count++;
        printf("Jugador %d (%s) eligio %s\n", info.player_id, player_names[idx], player_strc[idx].move_set[info.move].name);
    }

    int damage = formula(player_strc[0], player_strc[0].move_set[movements[0]], player_strc[1]);
    if (player_strc[1].hp > damage)
        player_strc[1].hp -= damage;
    else
    {
        player_strc[1].hp = 0;
        send_turn_results(movements);
        return 1;
    }

    damage = formula(player_strc[1], player_strc[1].move_set[movements[1]], player_strc[0]);
    if (player_strc[0].hp > damage)
        player_strc[0].hp -= damage;
    else
    {
        player_strc[0].hp = 0;
        send_turn_results(movements);
        return 1;
    }
    send_turn_results(movements);
    return -1;
}

void send_turn_results(int movements[2])
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        int opp = (i + 1) % MAX_PLAYERS;
        turn_msg_data resp = {0};
        resp.mtype = PLAYERS + player_ids[i];
        resp.player_id = player_ids[i];
        resp.player_cmd = CMD_TURN_RESULT;
        resp.my_hp = player_strc[i].hp;
        resp.enemy_hp = player_strc[opp].hp;
        resp.enemy_move = movements[opp];
        if (msgsnd(msgid, &resp, sizeof(resp) - sizeof(resp.mtype), 0) == -1)
            perror("msgsnd");
    }
}


/*
FUNCIONAMIENTO DE MSG.H:

-llave -> ftok()

-Crear/Buscar -> msgget()

-Enviar mensaje -> msgsnd()

-Recibir mensajes -> msgrcv()

-Destruir cola -> msgctl()

*/
