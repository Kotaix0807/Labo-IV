#define ADMIN 1
#define PLAYERS 100
#define ID 123
#define FTOK_PATH "msgs"

typedef enum {
    CMD_JOIN = 1,
    CMD_SEND_INFO,
    CMD_CHOOSE_MOVE,
    CMD_EXIT
}CMD;

typedef struct msg{
    long mtype; //Tipo de mensaje
    int player_id;
    CMD player_cmd;
}msg_data;

typedef struct pk_msg{
    long mtype; //Tipo de mensaje
    int player_id;
    CMD player_cmd;
    char monster[32];
    char player_name[32];
}pkmn_info_msg;

/*
    Notas: 
    -No funcionaba porque enviabamos un mensaje muy grande, y SysV solo soporta 8kb.
    -No se pueden enviar punteros.



*/
