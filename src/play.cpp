#include "jogo.h"
#include <ncurses.h>
#include <unistd.h>

void printTab();
void fillTab();
void printCobra (const Cobra&);
void printComida (const comida_t&);

int main() {
    initscr();
    noecho();
    cbreak();
    start_color();

    novo_jogo:
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);

    Jogo jogo(30, 30);
    bool movendo = false;

    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_RED, COLOR_RED);

    clear();

    printTab();
    printCobra(jogo.get_cobra());
    printComida(jogo.get_comida());

    refresh();

    while (!jogo.isGameOver()) {
        int ch = getch();

        if (ch != ERR) {
            if (ch == 'q') {
                goto fim;
            }
        
            switch (ch) {
                case KEY_UP: case 'w':
                jogo.input(CIMA);
                break;

                case KEY_DOWN: case 's':
                jogo.input(BAIXO);
                break;

                case KEY_LEFT: case 'a':
                jogo.input(ESQUERDA);
                break;

                case KEY_RIGHT: case 'd':
                jogo.input(DIREITA);
                break;
            }

            if (!movendo) {
                movendo = true;
            }
        }

        if (movendo) {
            jogo.tick();
        }

        fillTab();
        printCobra(jogo.get_cobra());
        printComida(jogo.get_comida());

        mvprintw(height + 2, 1, "SCORE: %d", jogo.get_score());
        refresh();
        usleep(125000);
    }

    init_pair(3, COLOR_WHITE, COLOR_RED);
    attron(COLOR_PAIR(3));
    attron(A_BOLD);
    attron(A_BLINK);
    mvprintw(height / 2, (width*2 - 11) / 2, " GAME OVER ");
    attroff(COLOR_PAIR(3));
    attroff(A_BLINK);
    attroff(A_BOLD);
    refresh();

    nodelay(stdscr, FALSE);
    while (true) {
        int ch = getch();

        if (ch == 'q') {
            break;
        }

        if (ch == ' ') {
            clear();
            goto novo_jogo;
        }
    }

    fim:
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