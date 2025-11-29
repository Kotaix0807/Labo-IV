#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <stdbool.h>
#include <ncurses.h>
#include <sys/select.h>
#include <string.h>
#include "sound.h"
#include "structs.h"
#include "tools.h"
#include "opt.h"
#include "ipc.h"
#include "net.h"

#define WIN_X_MIN 216
#define WIN_Y_MIN 45

static int g_sock = -1;
static int g_player_id = 0;
static int g_opponent_id = 0;

static int read_port(void)
{
    const char *env = getenv("PKMN_PORT");
    if (env && env[0] != '\0')
    {
        int p = atoi(env);
        if (p > 0 && p < 65536)
            return p;
    }
    return NET_PORT;
}

static const char *read_host(void)
{
    const char *env = getenv("PKMN_HOST");
    if (env && env[0] != '\0')
        return env;
    return NET_HOST;
}

void warn()
{
    const char *warn_msg = "Por favor, maximice su pantalla y presione 'ctrl -'";

    while (1)
    {
        int yM, xM;
        getmaxyx(stdscr, yM, xM);

        if (xM == WIN_X_MIN && yM == WIN_Y_MIN)
        {
            clear();
            refresh();
            break;
        }

        int pos_y = yM / 4;
        int pos_x = (xM - (int)strlen(warn_msg)) / 2;
        clear();
        mvprintw(pos_y, pos_x, "%s", warn_msg);
        refresh();
        napms(1);
    }
}

static int initGame()
{
    setlocale(LC_ALL, "");
    initscr();
    curs_set(0);
    noecho();
    cbreak();
   // warn();
    if(!initAudio())
    {
        endwin();
        return 0;
    }
    return 1;
}

static int send_command(CommandType cmd, int arg1, const char *text)
{
    if (g_sock == -1)
        return 0;

    net_packet pkt;
    memset(&pkt, 0, sizeof(pkt));
    pkt.kind = PKT_CMD;
    pkt.code = cmd;
    pkt.player_id = g_player_id;
    pkt.arg1 = arg1;
    if (text)
    {
        strncpy(pkt.text, text, sizeof(pkt.text) - 1);
        pkt.text[sizeof(pkt.text) - 1] = '\0';
    }

    return net_send_packet(g_sock, &pkt) == 0;
}

static int recv_event(event_data *ev)
{
    if (g_sock == -1)
        return 0;

    net_packet pkt;
    if (net_recv_packet(g_sock, &pkt) != 0)
        return 0;
    if (pkt.kind != PKT_EVT)
        return 0;

    ev->evt = (EventType)pkt.code;
    ev->player_id = pkt.player_id;
    ev->opponent_id = pkt.arg1;
    ev->connected = pkt.arg2;
    ev->data1 = pkt.arg1;
    strncpy(ev->text, pkt.text, sizeof(ev->text) - 1);
    ev->text[sizeof(ev->text) - 1] = '\0';
    return 1;
}

static int wait_for_events(const EventType *wanted, int n, event_data *out)
{
    event_data ev;
    while (recv_event(&ev))
    {
        if (ev.evt == EVT_SHUTDOWN)
            return 0;
        for (int i = 0; i < n; i++)
        {
            if (ev.evt == wanted[i])
            {
                *out = ev;
                return 1;
            }
        }
    }
    return 0;
}

static int connect_to_admin(int *out_id)
{
    const char *host = read_host();
    int port = read_port();
    int sock = net_connect(host, port);
    if (sock < 0)
    {
        perror("net_connect");
        return 0;
    }

    g_sock = sock;
    g_player_id = 0;
    if (!send_command(CMD_JOIN, 0, NULL))
        return 0;

    event_data ev;
    EventType wanted[] = {EVT_WAITING, EVT_START};
    while (wait_for_events(wanted, 2, &ev))
    {
        if (ev.evt == EVT_WAITING)
            printf("Esperando más jugadores... conectados=%d\n", ev.connected);
        else if (ev.evt == EVT_START)
        {
            g_player_id = ev.player_id;
            g_opponent_id = ev.opponent_id;
            if (out_id)
                *out_id = ev.player_id;
            return 1;
        }
    }

    fprintf(stderr, "No se pudo conectar con el admin.\n");
    return 0;
}

int main(void)
{
    int player_id = 0;
    if (!connect_to_admin(&player_id))
        return 1;
    if(!initGame())
        return 1;
    printTitle();
    ply Player = initPly();

    while(mainMenu(&Player) != 3);

    if (g_sock != -1)
    {
        send_command(CMD_EXIT, 0, NULL);
        net_close(g_sock);
        g_sock = -1;
    }

    free(Player.name);
    echo();
    endwin();
    quitAudio();
    return 0;
}

static int wait_opponent_selection(char *name_out, size_t len)
{
    EventType wanted[] = {EVT_OPP_SELECT};
    event_data ev;
    if (!wait_for_events(wanted, 1, &ev))
        return 0;
    strncpy(name_out, ev.text, len - 1);
    name_out[len - 1] = '\0';
    return 1;
}

static int wait_opponent_move(int *move_idx, int *attacker_id)
{
    EventType wanted[] = {EVT_OPP_MOVE};
    event_data ev;
    if (!wait_for_events(wanted, 1, &ev))
        return 0;
    *move_idx = ev.data1;
    *attacker_id = ev.player_id;
    return 1;
}

static void show_attack(const char *attacker, const char *move_name)
{
    char *lines[] = {
        "%s uso:",
        "%s!"
    };
    replace_fmt(lines, 0, attacker);
    replace_fmt(lines, 1, move_name);
    int used_lines = (int)(sizeof(lines) / sizeof(char *));
    int yM = getmaxy(stdscr);
    int used_h = 10 - used_lines;
    txt_box *box = custTxtBox_str(lines, used_lines, NULL, 1, (yM - used_h - 2), 40, used_h, ALIGN_CENTER);
    napms(750);
    flushinp();
    delBox(box);
}

void Combat(ply *cur)
{
    if (!cur || !cur->monster)
        return;

    if (!send_command(CMD_SELECT, 0, cur->monster->name))
        return;

    char enemy_name[64] = {0};
    if (!wait_opponent_selection(enemy_name, sizeof(enemy_name)))
        return;

    pkmn *enemy = NULL;
    pkmnSet(&enemy, enemy_name);

    wclear(stdscr);
    wrefresh(stdscr);

    int xM, yM;
    getmaxyx(stdscr, yM, xM);

    while (cur->monster->hp > 0 && enemy->hp > 0)
    {
        WINDOW *player_win = printPkmnW(cur->monster, 2, yM - cur->monster->n_ascii - 3);
        WINDOW *enemy_win = printPkmnW(enemy, (xM - enemy->w - 2), 0);

        char *move_names[4];
        for (int i = 0; i < 4; i++)
            move_names[i] = cur->monster->move_set[i].name;

        int mv = menu("Que deseas hacer?", move_names, 4, 1, (yM - 6));

        if (mv < 0 || mv > 3)
            continue;

        if (!send_command(CMD_MOVE, mv, NULL))
            break;

        int opp_mv = 0;
        int opp_id = 0;
        if (!wait_opponent_move(&opp_mv, &opp_id))
            break;

        bool i_first = false;
        if (cur->monster->speed > enemy->speed)
            i_first = true;
        else if (cur->monster->speed == enemy->speed)
            i_first = (g_player_id < opp_id);

        if (i_first)
        {
            int dmg = formula(*(cur->monster), cur->monster->move_set[mv], *enemy);
            show_attack(cur->monster->name, cur->monster->move_set[mv].name);
            enemy->hp -= dmg;
            if (enemy->hp < 0)
                enemy->hp = 0;
            if (enemy->hp == 0)
                break;

            int dmg2 = formula(*enemy, enemy->move_set[opp_mv], *(cur->monster));
            show_attack(enemy->name, enemy->move_set[opp_mv].name);
            cur->monster->hp -= dmg2;
            if (cur->monster->hp < 0)
                cur->monster->hp = 0;
        }
        else
        {
            int dmg2 = formula(*enemy, enemy->move_set[opp_mv], *(cur->monster));
            show_attack(enemy->name, enemy->move_set[opp_mv].name);
            cur->monster->hp -= dmg2;
            if (cur->monster->hp < 0)
                cur->monster->hp = 0;
            if (cur->monster->hp == 0)
                break;

            int dmg = formula(*(cur->monster), cur->monster->move_set[mv], *enemy);
            show_attack(cur->monster->name, cur->monster->move_set[mv].name);
            enemy->hp -= dmg;
            if (enemy->hp < 0)
                enemy->hp = 0;
        }
        clearWin(player_win);
        clearWin(enemy_win);
    }

    wclear(stdscr);
    wrefresh(stdscr);

    if (enemy->hp == 0 && cur->monster->hp == 0)
    {
        char *end[] = {
            "Empate!",
            "Ambos pokemon se debilitaron"
        };
        dialFromStr(end, 2, "Resultado", -1, -1, ALIGN_CENTER);
    }
    else if (enemy->hp == 0)
    {
        char *end[] = {
            "Felicidades %s!",
            "Haz ganado el combate pokemon"
        };
        replace_fmt(end, 0, cur->name);
        dialFromStr(end, 2, "Winner!", -1, -1, ALIGN_CENTER);
    }
    else
    {
        char *end[] = {
            "Tu pokemon se debilitó",
            "Has perdido el combate"
        };
        dialFromStr(end, 2, "Loser", -1, -1, ALIGN_CENTER);
    }

    if (enemy)
    {
        if (enemy->ascii)
        {
            for (int i = 0; i < enemy->n_ascii; i++)
                free(enemy->ascii[i]);
            free(enemy->ascii);
        }
        free(enemy);
    }

    wclear(stdscr);
    wrefresh(stdscr);
}
