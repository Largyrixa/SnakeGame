#include "bot_genetico.h"
#include "jogo.h"
#include "cobra.h"

#include <cstring>
#include <iostream>
#include <ncurses.h>
#include <unistd.h>

#define CELL_H_SIZE 2
#define CELL_V_SIZE 1

using namespace std;

string save_path = "data/melhor_cerebro.txt";

const int largura_tab = 30;
const int altura_tab  = 30;

int max_y, max_x;

void printTab();
void fillTab();
void printCobra (const Cobra&);
void printComida (const comida_t&);

int main(int argc, char** argv) 
{
    initscr();
    noecho();
    start_color();
    curs_set(0);

    if (argc > 1) 
    {
        save_path = string(argv[1]);
    }

    getmaxyx(stdscr, max_y, max_x);

    if (max_y < altura_tab*CELL_V_SIZE + 4 || max_x < largura_tab*CELL_H_SIZE + 2)
    {
        mvprintw(0, 0, "Por favor, feche o programa, aumente o tamanho do terminal e tente novamente.");
        refresh();
        getch();
        endwin();
        return 0;
    }

    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_RED, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_RED);
    init_pair(4, COLOR_WHITE, COLOR_GREEN);

    clear();
    char msg[] = "pressione enter para começar";
    mvprintw(max_y / 2, (max_x - strlen(msg)) / 2, msg);
    refresh();

    int high_score = 0;
    auto bot = BotGenetico(altura_tab, largura_tab);

    if (!bot.cerebro.carregar_rede(save_path))
    {
        endwin();
        return 1;
    }

    while (getch() != 'q')
    {
        auto bot_atual = bot;
        clear();
        printTab();

        printCobra(bot_atual.jogo.get_cobra());
        printComida(bot_atual.jogo.get_comida());

        int score = 0;

        int passos = 0;
        int max_passos = altura_tab*largura_tab;

        mvprintw(altura_tab + 2, 1, "SCORE: %d  PASSOS: %d", score, passos);
        refresh();

        while (!bot_atual.jogo.isWin() && !bot_atual.jogo.isGameOver())
        {
            auto jogada = bot_atual.calcular_jogada();
            bot_atual.jogo.input(jogada);
            bot_atual.jogo.tick();
            passos ++;

            fillTab();
            printCobra(bot.jogo.get_cobra());
            printComida(bot.jogo.get_comida());


            mvprintw(altura_tab + 2, 1, "SCORE: %d  EFICIENCIA: %.2lf %", score, (1.0/(passos + 1.0) * 100.0));

            if (bot.jogo.get_score() != score)
            {
                passos = 0;
                score = bot.jogo.get_score();
            }

            if (passos >= max_passos)
            {
                break;
            }

            refresh();
            usleep(40000);
        }

        if (bot.jogo.isWin())
        {
            char msg[] = " GANHOU! ";
            attron (A_BLINK | A_BOLD | COLOR_PAIR(4));

            mvprintw((altura_tab - 1) / 2,  ((largura_tab + 1)*2 - strlen(msg)) / 2, msg);

            refresh();
            attroff(A_BLINK | A_BOLD | COLOR_PAIR(4));

            usleep(5000000);
        } 
        
        else 
        {
            char msg[] = " PERDEU! ";
            attron (A_BLINK | A_BOLD | COLOR_PAIR(3));
            mvprintw((altura_tab - 1) / 2, ((largura_tab + 1)*2 - strlen(msg)) / 2, msg);
            refresh();

            attroff (A_BLINK | A_BOLD | COLOR_PAIR(3));
        }

        clear();
        char msg[] = "pressione enter para começar ou 'q' para sair";
        mvprintw(max_y / 2, (max_x - strlen(msg)) / 2, msg);
        refresh();
    }

    endwin();
    return 0;
}

void printTab() {
    mvaddch(0, 0, ACS_ULCORNER);
    mvaddch(height+1, 0, ACS_LLCORNER);
    mvaddch(0, width*2+2, ACS_URCORNER);
    mvaddch(height+1, width*2+2, ACS_LRCORNER);

    for (int y = 0; y <= height+1; y++) {
        if (y != 0 && y != height+1) {
            mvaddch(y, 0, ACS_VLINE);
            mvaddch(y, width*2+2, ACS_VLINE);
        }
        for (int x = 1; x < width*2+2; x++) {
            if (y == 0 || y == height+1) {
                mvaddch(y,x, ACS_HLINE);
            }
        }
    }
}

void fillTab() {
    for (int y = 1; y <= height; y++) {
        for (int x = 2; x <= width*2; x+=2) {
            mvprintw(y, x, "  ");
        }
    }
}

void printCobra (const Cobra& cobra) {
    auto parte = cobra.get_cabeca_ptr();

    attron(COLOR_PAIR(1));

    while (parte != nullptr) {
        mvprintw((parte->y+1)*1, (parte->x+1)*2, "  ");
        parte = parte->next;
    }
    
    attroff(COLOR_PAIR(1));
}

void printComida (const comida_t& comida) {
    attron(COLOR_PAIR(2));
    attron(A_BOLD);

    mvprintw(comida.y + 1, comida.x*2 + 2, "  ");

    attroff(COLOR_PAIR(2));
    attroff(A_BOLD);
}