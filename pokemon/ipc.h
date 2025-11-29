// ipc.h
#ifndef IPC_H
#define IPC_H

#include <sys/types.h>

#define MSG_KEY_PATH "ipc.h"   /* archivo existente usado para ftok */
#define MSG_KEY_ID   'P'   /* mismo char en admin y jugadores  */
#define MAX_PLAYERS  2
#define MAX_TEXT_LEN 64

typedef enum {
    CMD_JOIN = 1,   /* jugador se quiere conectar */
    CMD_EXIT,       /* jugador se va */
    CMD_SELECT,     /* jugador selecciona pokemon (text) */
    CMD_MOVE        /* jugador elige movimiento (arg1 = idx) */
} CommandType;

// Mensaje JUGADOR -> ADMIN
struct msg_command {
    long mtype;                // SIEMPRE long y SIEMPRE el primer campo
    pid_t player_pid;          // PID real del proceso jugador
    int   player_id;           // 1, 2, ...
    CommandType cmd;           // tipo de comando
    int   arg1;                // extra (por ahora 0)
    int   arg2;                // extra (por ahora 0)
    char  text[MAX_TEXT_LEN];  // nombre de pokemon u otros datos
};

typedef enum {
    EVT_WAITING = 1,   /* admin avisa que se conecto */
    EVT_START,         /* ya estan todos y puede comenzar */
    EVT_SHUTDOWN,      /* admin elimina cola */
    EVT_OPP_SELECT,    /* el rival eligió pokemon (text) */
    EVT_OPP_MOVE       /* el rival usó movimiento (data1 = idx) */
} EventType;

// Mensaje ADMIN -> JUGADOR
struct msg_event {
    long mtype;                 // se envía al PID del jugador
    int  player_id;             // id asignado (1..MAX_PLAYERS)
    int  opponent_id;           // id del oponente, 0 si aun no hay
    EventType evt;              // tipo de evento
    int  connected;             // cuantos hay conectados
    int  data1;                 // movimiento idx, etc.
    char text[MAX_TEXT_LEN];    // nombre de pokemon, etc.
};

#endif
