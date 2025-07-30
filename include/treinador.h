#ifndef _TREINADOR_H
#define _TREINADOR_H

#include "bot_genetico.h"
#include <vector>

class Treinador {
public:
    Treinador(int tamanho_populacao, int altura_tab, int largura_tab, double taxa_elitismo, double taxa_mutacao, double taxa_selecao);

    BotGenetico evoluir_uma_geracao();

    bool carregar_bot (const std::string& path);
    
    double taxa_mutacao;

private:
    std::vector<BotGenetico> populacao;


    double calcular_fitness(const BotGenetico& bot) const;

    int tamanho_populacao;
    double taxa_elitismo;
    double taxa_selecao;
    int altura_tab;
    int largura_tab;
};

#endif