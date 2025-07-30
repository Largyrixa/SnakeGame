#include "bot_genetico.h"
#include <vector>

const int n_inputs = 24;
const int n_hidden = 16;
const int n_outputs=  4;

BotGenetico::BotGenetico
(int altura_tab, int largura_tab): 
jogo(altura_tab, largura_tab),
cerebro(n_inputs, n_hidden, n_outputs),
fitness_score(0)
{}

using namespace std;

Direcao BotGenetico::calcular_jogada() const {
    vector<double> visao  = jogo.calcular_visao();
    vector<double> saidas = cerebro.feedFoward(visao);

    double maior_valor = saidas[0];
    Direcao melhor_dir = CIMA;

    for (int i = 1; i < saidas.size(); i++) {
        if (saidas[i] > maior_valor) {
            maior_valor = saidas[i];
            melhor_dir  = (Direcao) i;
        }
    }

    return melhor_dir;
}

BotGenetico& BotGenetico::operator=(const BotGenetico& other) {
    // verifica se o objeto não está sendo atribuído a si mesmo
    if (this != &other) {
        // copia cada membro individualmente
        this->jogo = other.jogo;
        this->cerebro = other.cerebro;
        this->fitness_score = other.fitness_score;
    }

    return *this;
}