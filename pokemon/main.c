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
                if(Player.name[0] == '\0')
                    askName(&Player);
                askPkmn(&Player);
                combat(&Player);
                break;
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
    strncpy(message.monster, monster.name, sizeof(message.monster) - 1);
    message.monster[sizeof(message.monster) - 1] = '\0';
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
    pkmnSet(monster, message.monster, 1);
    if (enemy_name && enemy_size > 0)
    {
        strncpy(enemy_name, message.player_name, enemy_size - 1);
        enemy_name[enemy_size - 1] = '\0';
    }
    return 1;
}

void combat(ply *cur)
{
    pkmn enemy = {0};
    char opponent_name[32] = {0};
    sendInfo(cur->monster, cur->name);
    reciveInfo(&enemy, opponent_name, sizeof(opponent_name));
    wclear(stdscr);
    wrefresh(stdscr);
    
    int xM, yM;
    getmaxyx(stdscr, yM, xM);
    
    int enemy_h = enemy.n_ascii;
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
        replace_fmt(init, 1, opponent_name[0] ? opponent_name : enemy.name);
        int init_lines = (int)(sizeof(init)/sizeof(char*));
        int init_h = 10 - init_lines;
        txt_box *combat_box = custTxtBox_str(init, init_lines, NULL, 1, (yM - init_h - 2), 40, init_h, ALIGN_CENTER);
        char *move_names[4];
        for (int i = 0; i < 4; i++)
            move_names[i] = cur->monster.move_set[i].name;
        int mv = menu("Que deseas hacer?", move_names, 4, 1, (yM - 6));
        delBox(combat_box);

        //Ataca
        char *used[] = {
            "%s uso:",
            "%s!"
        };
        replace_fmt(used, 0, cur->monster.name);
        replace_fmt(used, 1, cur->monster.move_set[mv].name);
        int used_lines = (int)(sizeof(used)/sizeof(char*));
        int used_h = 10 - used_lines;
        txt_box *use = custTxtBox_str(used, used_lines, NULL, 1, (yM - used_h - 2), 40, used_h, ALIGN_CENTER);
        napms(1000);
        flushinp(); /* descartar teclas pulsadas durante la pausa */
        delBox(use);
        int damage = formula(cur->monster, cur->monster.move_set[mv], enemy);
        if(enemy.hp > damage)
            enemy.hp -= damage;
        else
        {
            enemy.hp = 0;
            for(int i = 0; i < enemy_h; i++)
            {
                if(enemy.n_ascii > 0)
                    enemy.n_ascii--;
                printPkmnW(&enemy, (xM - enemy.w - 2), 0);
                napms(45);
                flushinp();
            }
            clearWin(enemy_win);
            clearWin(player_win);
            break;
        }
        printPkmnW(&enemy, (xM - enemy.w - 2), 0);
        /*
        txt_box *use = custTxtBox_str(used, used_lines, NULL, 1, (yM - used_h - 2), 40, used_h, ALIGN_CENTER);

        for (int i = 0; i < 4; i++)
            free(move_names[i]);
        free(move_names);

        for (int i = 0; i < 4; i++)
            move_names[i] = enemy.move_set[i].name;
        mv = rand() % 3;
        replace_fmt(used, 0, enemy.name);
        replace_fmt(used, 1, enemy.move_set[mv].name);
        napms(1000);
        int damage = formula(enemy, enemy.move_set[mv], cur->monster);
        if(cur->monster.hp > damage)
            cur->monster.hp -= damage;
        else
        {
            cur->monster.hp = 0;
            for(int i = 0; i < player_h; i++)
            {
                if(cur->monster.n_ascii > 0)
                    cur->monster.n_ascii--;
                printPkmnW(&cur->monster, (xM - cur->monster.w - 2), 0);
                napms(45);
                flushinp();
            }
            clearWin(enemy_win);
            clearWin(player_win);
            break;
        }

        */
        clearWin(enemy_win);
        clearWin(player_win);
    }
    char *end[] = {
        "Felicidades %s!",
        "Haz ganado el combate pokemon"
    };
    replace_fmt(end, 0, cur->name);
    dialFromStr(end, 2, "Winner!", -1, -1, ALIGN_CENTER);

    wclear(stdscr);
    wrefresh(stdscr);
}

void closeGame()
{
    echo();
    endwin();
    quitAudio();
}
