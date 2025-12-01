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

int player_ids[MAX_PLAYERS] = {0};
char player_pkmn[MAX_PLAYERS][32] = {{0}};
char player_names[MAX_PLAYERS][32] = {{0}};
key_t key;
int msgid;

int main()
{
    
    if(!recive_players())
        return 1;
    if(!recive_pkmn())
        return 1;

    pkmn player_1;
    pkmn player_2;
    pkmnSet(&player_1, player_pkmn[0], 1);
    pkmnSet(&player_2, player_pkmn[1], 1);

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
        if (info.player_cmd != CMD_SEND_INFO)
        {
            printf("Mensaje inesperado de %d (cmd %d)\n", info.player_id, info.player_cmd);
            continue;
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
        strncpy(player_pkmn[idx], info.monster, sizeof(player_pkmn[idx]) - 1);
        player_pkmn[idx][sizeof(player_pkmn[idx]) - 1] = '\0';
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
        strncpy(respuesta.monster, player_pkmn[change], sizeof(respuesta.monster));
        respuesta.monster[sizeof(respuesta.monster) - 1] = '\0';
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





/*
FUNCIONAMIENTO DE MSG.H:

-llave -> ftok()

-Crear/Buscar -> msgget()

-Enviar mensaje -> msgsnd()

-Recibir mensajes -> msgrcv()

-Destruir cola -> msgctl()

*/
