#ifndef _BOT_GENETICO_H
#define _BOT_GENETICO_H

#include "jogo.h"
#include "rede_neural.h"

class BotGenetico {
public:
    BotGenetico(int altura_tab, int largura_tab);

    Direcao calcular_jogada() const;
    Jogo jogo;
    RedeNeural cerebro;

    double fitness_score;

    BotGenetico& operator=(const BotGenetico& other);
};

#endif