#include "bot_genetico.h"
#include "treinador.h"
#include "cobra.h"
#include "jogo.h"

#include <iostream>
#include <ncurses.h>
#include <string>
#include <cstring>
#include <unistd.h>

#define CELL_H_SIZE 2
#define CELL_V_SIZE 1

std::string save_path = "data/melhor_cerebro.txt";

const int largura_tab = 30;
const int altura_tab  = 30;

int max_y, max_x;

void printTab();
void fillTab();
void printCobra (const Cobra&);
void printComida (const comida_t&);
void mostrar_bot (BotGenetico&, const int& geracao);

int main (int argc, char** argv) {
    initscr();
    noecho();
    start_color();

    
    bool carregar_cerebro = false;
    double taxa_elitismo = 0.8;
    double taxa_mutacao  = 0.015;
    double taxa_selecao = 0.2;
    int tamanho_populacao = 10000;
    
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            std::string arg(argv[i]);
            
            if (arg == "-c") {
                carregar_cerebro = true;
            }
            
            if (arg == "-tm") {
                if (argc < i + 2) {
                    return 1;
                }
                double input = std::stod(std::string(argv[i+1]));
                
                if (input > 0.0 && input <= 1.0) taxa_mutacao = input;
                
                i++;
            }
            
            if (arg == "-te") {
                if (argc < i + 2) {
                    return 1;
                }
                double input = std::stod(std::string(argv[i+1]));
                
                if (input > 0.1 && input <= 1.0) taxa_elitismo = input;
                i++;
            }
            
            if (arg == "-tp") {
                if (argc < i + 2) {
                    return 1;
                }
                
                int input = std::stoi(std::string(argv[i+1]));
                
                if (input >= 10) tamanho_populacao = input;
                
                i++;
            }

            if (arg == "-ts") {
                if (argc < i + 2 ) {
                    return 1;
                }

                int input = std::stod(std::string(argv[i+1]));

                if (input > 0.1 && input <= 1.0) taxa_selecao = input;

                i++;
            }

            if (arg == "-sp") {
                if (argc < i + 2) {
                    return 1;
                }

                save_path = argv[i+1];
                
                i++;
            }
        }
    }
    
    curs_set(0);
    
    getmaxyx(stdscr, max_y, max_x);
    
    if (max_y < altura_tab*CELL_V_SIZE + 4 || max_x < largura_tab*CELL_H_SIZE + 4) {
        mvprintw(0, 0, "Por favor, feche o programa, aumente o tamanho do terminal e tente novamente");
        getch();
        endwin();
        return 0;
    }
    
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_RED, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_RED);
    init_pair(4, COLOR_WHITE, COLOR_GREEN);
    
    int geracao = 0;
    
    clear();
    const char msg1[] = "Pressione qualquer tecla para começar";
    mvprintw(max_y / 2, (max_x - strlen(msg1)) / 2, msg1);
    refresh();
    getch();
    
    double melhor_fitness = 0;
    Treinador populacao
    (tamanho_populacao, altura_tab, largura_tab, taxa_elitismo, taxa_mutacao, taxa_selecao);

    if (carregar_cerebro) {
        populacao.carregar_bot(save_path);
    }

    
    while (true) 
    {
        
        attron(A_BLINK | A_BOLD);

        /*
        char* msg = "AVALIANDO POPULACAO, AGUARDE...";
        mvprintw(max_y / 2, (max_x - strlen(msg)) / 2, msg);
        refresh();
        */

        attroff(A_BLINK | A_BOLD);
        move(0,0);

        auto melhor_bot = populacao.evoluir_uma_geracao();

        if (melhor_bot.fitness_score > melhor_fitness) {
            melhor_bot.cerebro.salvar_rede(save_path);
            melhor_fitness = melhor_bot.fitness_score;
            
            clear();

            attron(A_BOLD | A_REVERSE);

            char msg[] = "NOVO RECORDE DE FITNESS SCORE!";
            mvprintw(max_y / 2, (max_x - strlen(msg)) / 2, msg);

            attroff(A_BOLD | A_REVERSE);
            refresh();

            usleep(1000000);
        }
        mostrar_bot(melhor_bot, geracao);

        geracao++;
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

void mostrar_bot (BotGenetico& bot, const int& geracao) {
    clear();
    printTab();

    printCobra(bot.jogo.get_cobra());
    printComida(bot.jogo.get_comida());

    int score = 0;

    int passos = 0;
    int max_passos = altura_tab*largura_tab;

    mvprintw(altura_tab + 3, 1, "SCORE: %d", 0);
    mvprintw(altura_tab + 2, 1, "GERACAO: %d   FITNESS: %.1lf", geracao, bot.fitness_score);
    refresh();
    
    while (!bot.jogo.isWin() && !bot.jogo.isGameOver()) 
    {
        auto jogada = bot.calcular_jogada();
        bot.jogo.input(jogada);
        bot.jogo.tick();
        passos++;

        fillTab();
        printCobra(bot.jogo.get_cobra());
        printComida(bot.jogo.get_comida());
        
        mvprintw(altura_tab + 3, 1, "SCORE: %d", bot.jogo.get_score());
        if (bot.jogo.get_score() != score) {
            passos = 0;
            score = bot.jogo.get_score();
        }

        if (passos >= max_passos) {
            break;
        }

        refresh();
        usleep(20000);
    }

    if (bot.jogo.isWin()) {
        char msg[] = " GANHOU! ";
        attron (A_BLINK | A_BOLD | COLOR_PAIR(4));

        mvprintw((altura_tab - 1) / 2,  ((largura_tab + 1)*2 - strlen(msg)) / 2, msg);

        refresh();
        attroff(A_BLINK | A_BOLD | COLOR_PAIR(4));

        usleep(1000000);
    } else {
        char msg[] = " PERDEU! ";
        attron (A_BLINK | A_BOLD | COLOR_PAIR(3));

        mvprintw((altura_tab - 1) / 2, ((largura_tab + 1)*2 - strlen(msg)) / 2, msg);

        refresh();

        attroff(A_BLINK | A_BOLD | COLOR_PAIR(3));
    }
}