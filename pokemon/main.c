#include <SDL2/SDL_mixer.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <errno.h>

#include <ncurses.h>

#include <sys/msg.h>
#include <sys/types.h>

#include "box.h"
#include "ipc.h"
#include "opt.h"
#include "sound.h"
#include "structs.h"
#include "tools.h"

#define WIN_X_MIN 216
#define WIN_Y_MIN 45

int connect();
int loadGame();
int sendInfo(pkmn monster, const char *name);
int reciveInfo(pkmn *monster, char *enemy_name, size_t enemy_size);
int sendMovement(pkmn monster, int move);
int reciveTurnResult(int *my_hp, int *enemy_hp, int *enemy_move);
void combat(ply *cur);
void closeGame();

key_t key;
int msgid;
int ply_id;

int main(void)
{
    if(!connect())
        return 1;
    if(!loadGame())
        return 1;
    ply Player = initPly();
    int selection;
    while((selection = mainMenu()) != 3)
    {
        switch (selection)
        {
            case 0: /* Combate */
            {
                if(Player.name[0] == '\0')
                    askName(&Player);
                askPkmn(&Player);
                combat(&Player);
                break;
            }
            case 1: /* Cambiar nombre */
                askName(&Player);
                break;
            case 2: /* Creditos */
                credits();
                break;
            default:
                break;
        }
    }
    msg_data message = {
        .mtype = ADMIN,
        .player_cmd = CMD_EXIT,
        .player_id = ply_id
    };
    if (msgsnd(msgid, &message, sizeof(message) - sizeof(message.mtype), 0) == -1)
    {
        perror("msgsnd");
        return 1;
    }
    closeGame();
    return 0;
}

int connect()
{
    key = ftok(FTOK_PATH, ID);
    msg_data message = {0};
    ply_id = (int)getpid();

    printf("Esperando a conectarse...\n");
    fflush(stdout);
    while ((msgid = msgget(key, 0666)) == -1)
    {
        if (errno == ENOENT)
        {
            sleep(1);
            continue;
        }
        perror("msgget");
        return 0;
    }
    message.mtype = ADMIN;
    message.player_id = ply_id;
    message.player_cmd = CMD_JOIN;

    // msgsnd to send message
    if (msgsnd(msgid, &message, sizeof(message) - sizeof(message.mtype), 0) == -1)
    {
        perror("msgsnd");
        return 0;
    }
    printf("Conexion exitosa, esperando jugadores...\n");
    // Esperar respuesta del admin con el mismo ID
    message = (msg_data){0};
    long target_type = PLAYERS + ply_id;
    if (msgrcv(msgid, &message, sizeof(message) - sizeof(message.mtype), target_type, 0) == -1)
    {
        perror("msgrcv");
        return 0;
    }
    printf("Asignado ID jugador: %d\n", message.player_id);
    printf("Iniciando juego...\n");
    fflush(stdout); /* aseguramos que todo lo impreso salga antes de ncurses */
    sleep(1);
    return 1;
}

int loadGame()
{
    fflush(stdout);
    setlocale(LC_ALL, "");
    initscr();
    curs_set(0);
    noecho();
    cbreak();
    clear();
    refresh(); /* limpiar realmente la pantalla antes de dibujar */
    return 1;
}

int sendInfo(pkmn monster, const char *name)
{
    pkmn_info_msg message = {
        .mtype = ADMIN,
        .player_id = ply_id,
        .player_cmd = CMD_SEND_INFO
    };
    strncpy(message.pkmn_name, monster.name, sizeof(message.pkmn_name) - 1);
    message.pkmn_name[sizeof(message.pkmn_name) - 1] = '\0';
    if (name)
    {
        strncpy(message.player_name, name, sizeof(message.player_name) - 1);
        message.player_name[sizeof(message.player_name) - 1] = '\0';
    }
    // msgsnd to send message
    if (msgsnd(msgid, &message, sizeof(message) - sizeof(message.mtype), 0) == -1)
    {
        perror("msgsnd");
        return 0;
    }
    return 1;
}

int reciveInfo(pkmn *monster, char *enemy_name, size_t enemy_size)
{
    pkmn_info_msg message = {0};
    long target_type = PLAYERS + ply_id;
    if (msgrcv(msgid, &message, sizeof(message) - sizeof(message.mtype), target_type, 0) == -1)
    {
        perror("msgrcv");
        return 0;
    }
    pkmnSet(monster, message.pkmn_name, 1);
    if (enemy_name && enemy_size > 0)
    {
        strncpy(enemy_name, message.player_name, enemy_size - 1);
        enemy_name[enemy_size - 1] = '\0';
    }
    return 1;
}

int sendMovement(pkmn monster, int move)
{
    mv_msg_data message = {
        .mtype = ADMIN,
        .player_id = ply_id,
        .move = move
    };
    strncpy(message.pkmn_name, monster.name, sizeof(message.pkmn_name) - 1);
    message.pkmn_name[sizeof(message.pkmn_name) - 1] = '\0';
    if (msgsnd(msgid, &message, sizeof(message) - sizeof(message.mtype), 0) == -1)
    {
        perror("msgsnd");
        return 0;
    }
    return 1;
}

int reciveTurnResult(int *my_hp, int *enemy_hp, int *enemy_move)
{
    turn_msg_data msg = {0};
    long target_type = PLAYERS + ply_id;
    if (msgrcv(msgid, &msg, sizeof(msg) - sizeof(msg.mtype), target_type, 0) == -1)
    {
        perror("msgrcv");
        return 0;
    }
    if (msg.player_cmd != CMD_TURN_RESULT)
        return 0;
    if (my_hp)
        *my_hp = msg.my_hp;
    if (enemy_hp)
        *enemy_hp = msg.enemy_hp;
    if (enemy_move)
        *enemy_move = msg.enemy_move;
    return 1;
}

void combat(ply *cur)
{
    pkmn enemy = {0};
    char opponent_name[32] = {0};
    sendInfo(cur->monster, cur->name);
    char *wait[] = {
    "Esperando al otro jugador..."
    };
    txt_box *wait_ply = txtBox_str(wait, (int)(sizeof(wait)/sizeof(char*)), NULL, -1, -1, ALIGN_CENTER);
    if(!reciveInfo(&enemy, opponent_name, sizeof(opponent_name)))
    {
        delBox(wait_ply);
        char *error[] = {
            "El otro jugador se desconecto"
        };
        dialFromStr(error, (int)(sizeof(error)/sizeof(char*)), "Error", -1, -1, ALIGN_CENTER);
        return;
    }
    delBox(wait_ply);
    wclear(stdscr);
    wrefresh(stdscr);

    int enemy_h = enemy.n_ascii;
    int player_h = cur->monster.n_ascii;
    int xM, yM;
    getmaxyx(stdscr, yM, xM);
    
    //int player_h = cur->monster.n_ascii;
    while(1)
    {
        //Parte 1
        WINDOW *player_win = printPkmnW(&cur->monster, 45, 15);
        WINDOW *enemy_win = printPkmnW(&enemy, (xM - enemy.w - 2), 0);
        char *init[] = {
            "Jugador: %s",
            "Tu oponente es: %s"
        };
        replace_fmt(init, 0, cur->name);
        replace_fmt(init, 1, opponent_name);
        int init_lines = (int)(sizeof(init)/sizeof(char*));
        int init_h = 10 - init_lines;
        txt_box *combat_box = custTxtBox_str(init, init_lines, NULL, 1, (yM - init_h - 2), 40, init_h, ALIGN_CENTER);
        char *move_names[4];
        for (int i = 0; i < 4; i++)
            move_names[i] = cur->monster.move_set[i].name;
        int mv = menu("Que deseas hacer?", move_names, 4, 1, (yM - 6));
        sendMovement(cur->monster, mv);
        delBox(combat_box);

        wait_ply = txtBox_str(wait, (int)(sizeof(wait)/sizeof(char*)), NULL, -1, -1, ALIGN_CENTER);
        int my_hp = cur->monster.hp;
        int enemy_hp = enemy.hp;
        int enemy_move = 0;
        if (!reciveTurnResult(&my_hp, &enemy_hp, &enemy_move))
            break;
        delBox(wait_ply);

        cur->monster.hp = my_hp;
        enemy.hp = enemy_hp;

        //Ataca jugador
        char *used[] = {
            "%s uso:",
            "%s!"
        };
        replace_fmt(used, 0, cur->monster.name);
        replace_fmt(used, 1, cur->monster.move_set[mv].name);
        int used_lines = (int)(sizeof(used)/sizeof(char*));
        int used_h = 10 - used_lines;
        txt_box *use = custTxtBox_str(used, used_lines, NULL, 1, (yM - used_h - 2), 40, used_h, ALIGN_CENTER);
        getch();
        delBox(use);

        // Ataca enemigo
        char *enm_used[] = {
            "%s uso:",
            "%s!"
        };
        replace_fmt(enm_used, 0, enemy.name);
        replace_fmt(enm_used, 1, enemy.move_set[enemy_move].name);
        int en_lines = (int)(sizeof(enm_used)/sizeof(char*));
        int en_h = 10 - en_lines;
        txt_box *enm_box = custTxtBox_str(enm_used, en_lines, NULL, 1, (yM - en_h - 2), 40, en_h, ALIGN_CENTER);
        getch();
        delBox(enm_box);

        if (enemy.hp <= 0 || cur->monster.hp <= 0)
            break;
        clearWin(enemy_win);
        clearWin(player_win);
    }
    if(enemy.hp <= 0)
    {
        for(int i = 0; i < enemy_h; i++)
        {
            if(enemy.n_ascii > 0)
                enemy.n_ascii--;
            printPkmnW(&enemy, (xM - enemy.w - 2), 0);
            napms(45);
            flushinp();
        }
        napms(100);
        char *end[] = {
            "Felicidades %s!",
            "Haz ganado el combate pokemon"
        };
        replace_fmt(end, 0, cur->name);
        dialFromStr(end, 2, "Winner!", -1, -1, ALIGN_CENTER);
    }
    else{
        for(int i = 0; i < player_h; i++)
        {
            if(cur->monster.n_ascii > 0)
                cur->monster.n_ascii--;
            printPkmnW(&cur->monster, 45, 15);
            napms(45);
            flushinp();
        }
        napms(100);
        char *end[] = {
            "Lastima %s...!",
            "Tu pokemon se debilito"
        };
        replace_fmt(end, 0, cur->name);
        dialFromStr(end, 2, "Perdedor...", -1, -1, ALIGN_CENTER);
    }

    wclear(stdscr);
    wrefresh(stdscr);
}

void closeGame()
{
    echo();
    endwin();
    quitAudio();
}
