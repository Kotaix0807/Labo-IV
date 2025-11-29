// ipc.h
#ifndef IPC_H
#define IPC_H

#include <sys/types.h>

#define MSG_KEY_PATH "ipc.h"   /* legacy (no usado en red) */
#define MSG_KEY_ID   'P'       /* legacy (no usado en red) */
#define MAX_PLAYERS  2
#define MAX_TEXT_LEN 64
#define NET_PORT     5000
#define NET_HOST     "100.100.40.46"

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

typedef enum {
    PKT_CMD = 1,
    PKT_EVT
} PacketKind;

// Paquete genérico sobre sockets (admin <-> jugadores)
typedef struct net_packet {
    int kind;                   // PKT_CMD o PKT_EVT
    int code;                   // CommandType o EventType según kind
    int player_id;              // id del emisor
    int arg1;                   // datos adicionales
    int arg2;
    char text[MAX_TEXT_LEN];    // payload textual (nombre pkmn, etc.)
} net_packet;

typedef struct event_data {
    EventType evt;
    int player_id;
    int opponent_id;
    int connected;
    int data1;
    char text[MAX_TEXT_LEN];
} event_data;

#endif
