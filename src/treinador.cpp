#include "treinador.h"
#include <fstream>
#include <random>
#include <algorithm>
#include <cmath>

Treinador::Treinador
(int tamanho_populacao, int altura_tab, int largura_tab, double taxa_elitismo, double taxa_mutacao, double taxa_selecao):
tamanho_populacao(tamanho_populacao),
populacao(tamanho_populacao, BotGenetico(altura_tab, largura_tab)),
altura_tab(altura_tab),
largura_tab(largura_tab),
taxa_elitismo(taxa_elitismo),
taxa_mutacao(taxa_mutacao),
taxa_selecao(taxa_selecao)
{}


double Treinador::calcular_fitness(const BotGenetico& bot) const {
    double fitness = 0;

    for (int i = 0; i < 2; i ++) 
    {
        auto bot_teste = bot;
        
        int passos_totais = 0;
        int passos_desde_comida = 0;
        const int max_passos = 1000;
        
        int score = 0;

        while (!bot_teste.jogo.isWin() && !bot_teste.jogo.isGameOver() && passos_desde_comida < max_passos) 
        {
            auto proxima_jogada = bot_teste.calcular_jogada();
            bot_teste.jogo.input(proxima_jogada);
            bot_teste.jogo.tick();
            
            passos_desde_comida++;
            passos_totais++;

            if (score != bot_teste.jogo.get_score()) 
            {
                double bonus_base = 500 * (score * 1.1);
                fitness += bonus_base * (100.0 / (passos_desde_comida + 1.0));
                
                passos_desde_comida = 0;
                score = bot_teste.jogo.get_score();

            }
        }

        // fitness += (passos_totais) + (pow(score, 2) * 500);

        if (bot_teste.jogo.isWin()) {
            fitness += 1000000;
        } else if (!bot_teste.jogo.isGameOver()) {
            fitness -= passos_desde_comida;
        }

        fitness += passos_totais;

        if (bot_teste.jogo.isWin()) {
            fitness *= 10;
        } else if (!bot_teste.jogo.isGameOver()) {
            fitness -= passos_desde_comida*2;
            if (bot_teste.jogo.get_score() > 10) {
                fitness /= 20;
            }
        } else {
            fitness /= 100;
        }
    }

    fitness /= 2;
    
    return fitness;
}

bool compare_bot (const BotGenetico& a, const BotGenetico& b) {
    return a.fitness_score > b.fitness_score;
}

BotGenetico selecionar_por_torneio 
(const std::vector<BotGenetico>& populacao, int tamanho_torneio) {
    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_int_distribution<int> dist(0, populacao.size() - 1);

    BotGenetico melhor_bot = populacao[dist(generator)];
    double maior_fitness   = melhor_bot.fitness_score;

    for (int i = 1; i < tamanho_torneio; i++) {
        int i_aleatorio = dist(generator);
        const BotGenetico& competidor_atual = populacao[i_aleatorio];

        if (competidor_atual.fitness_score > maior_fitness) {
            maior_fitness = competidor_atual.fitness_score;
            melhor_bot = competidor_atual;
        }
    }

    return melhor_bot;
}

BotGenetico gerar_filho 
(const BotGenetico& pai, const BotGenetico& mae, const double& taxa_mutacao);

BotGenetico Treinador::evoluir_uma_geracao() {

    populacao[0].fitness_score = calcular_fitness(populacao[0]);

    double melhor_fitness = populacao[0].fitness_score;
    auto campeao = populacao[0];
    
    for (int i = 1; i < tamanho_populacao; i++) {
        populacao[i].fitness_score = calcular_fitness(populacao[i]);

        if (populacao[i].fitness_score > melhor_fitness) {
            campeao = populacao[i];
            melhor_fitness = populacao[i].fitness_score;
        }
    }
    
    // selecionar a população de cruzamento
    const int selec_size = tamanho_populacao * taxa_selecao;
    
    std::vector<BotGenetico> selecionados;
    selecionados.reserve(selec_size);
    
    int elite_size = selec_size*taxa_elitismo;
    
    // Gerar nova população
    std::vector<BotGenetico> nova_populacao;
    nova_populacao.reserve(tamanho_populacao);
    
    for (int i = 0; i < 10; i++) nova_populacao.push_back(populacao[i]);

    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_int_distribution<int> coin(0,1);

    const int tamanho_torneio = 6;

    for (int i = 0; i < tamanho_populacao/2.0; i++) 
    {
        auto pai = selecionar_por_torneio(populacao, tamanho_torneio);
        auto mae = selecionar_por_torneio(populacao, tamanho_torneio);

        auto filho = gerar_filho(pai, mae, taxa_mutacao);
        nova_populacao.push_back(filho);
        
        if (coin(generator) == 0)
        {
            nova_populacao.push_back(pai);
        }
        else
        {
            nova_populacao.push_back(mae);
        }
    }
    populacao = nova_populacao;

    return campeao;
}

BotGenetico gerar_filho
(const BotGenetico& pai, const BotGenetico& mae, const double& taxa_mutacao) {
    const int altura_tab = pai.jogo.get_altura();
    const int largura_tab= pai.jogo.get_largura();

    BotGenetico filho(altura_tab, largura_tab);

    int decisao;


    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_int_distribution<int> dist_gene(0, 1);
    std::uniform_real_distribution<double> dist_mutacao_valor(-0.1, 0.1);
    std::uniform_real_distribution<double> dist_mutacao_chance(0.0, 1.0);

    // crossover para pesos_ih
    for (size_t i = 0; i < filho.cerebro.pesos_ih.size(); i++) {
        for (size_t j = 0; j < filho.cerebro.pesos_ih[i].size(); j++) {
            decisao = dist_gene(generator);
            if (decisao == 0) {
                filho.cerebro.pesos_ih[i][j] = pai.cerebro.pesos_ih[i][j];
            } else {
                filho.cerebro.pesos_ih[i][j] = mae.cerebro.pesos_ih[i][j];
            }
        }
    }

    // crossover para pesos_ho
    for (size_t i = 0; i < filho.cerebro.pesos_ho.size(); i++) {
        for (size_t j = 0; j < filho.cerebro.pesos_ho[i].size(); j++) {
            decisao = dist_gene(generator);
            if (decisao == 0) {
                filho.cerebro.pesos_ho[i][j] = pai.cerebro.pesos_ho[i][j];
            } else {
                filho.cerebro.pesos_ho[i][j] = mae.cerebro.pesos_ho[i][j];
            }
        }
    }

    // crossover para bias_h
    for (size_t i = 0; i < filho.cerebro.bias_h.size(); i++) {
        decisao = dist_gene(generator);
        if (decisao == 0) {
            filho.cerebro.bias_h[i] = pai.cerebro.bias_h[i];
        } else {
            filho.cerebro.bias_h[i] = mae.cerebro.bias_h[i];
        }
    }

    // crossover para bias_o
    for (size_t i = 0; i < filho.cerebro.bias_o.size(); i++) {
        decisao = dist_gene(generator);
        if (decisao == 0) {
            filho.cerebro.bias_o[i] = pai.cerebro.bias_o[i];
        } else {
            filho.cerebro.bias_o[i] = mae.cerebro.bias_o[i];
        }
    }

    // mutação para pesos_ih
    for (size_t i = 0; i < filho.cerebro.pesos_ih.size(); i++) {
        for (size_t j = 0; j < filho.cerebro.pesos_ih[i].size(); j++) {
            if (dist_mutacao_chance(generator) < taxa_mutacao) {
                filho.cerebro.pesos_ih[i][j] += dist_mutacao_valor(generator);
            }
        }
    }

    // mutação para pesos_ho
    for (size_t i = 0; i < filho.cerebro.pesos_ho.size(); i++) {
        for (size_t j = 0; j < filho.cerebro.pesos_ho[i].size(); j++) {
            if (dist_mutacao_chance(generator) < taxa_mutacao) {
                filho.cerebro.pesos_ho[i][j] += dist_mutacao_valor(generator);
            }
        }
    }

    // mutação para bias_h
    for (size_t i = 0; i < filho.cerebro.bias_h.size(); i++) {
        if (dist_mutacao_chance(generator) < taxa_mutacao) {
            filho.cerebro.bias_h[i] += dist_mutacao_valor(generator);
        }
    }

    // mutação para bias_o
    for (size_t i = 0; i < filho.cerebro.bias_o.size(); i++) {
        if (dist_mutacao_chance(generator) < taxa_mutacao) {
            filho.cerebro.bias_o[i] += dist_mutacao_valor(generator);
        }
    }

    return filho;
}

bool Treinador::carregar_bot(const std::string& path) {
    BotGenetico bot(altura_tab, largura_tab);
    
    if (!bot.cerebro.carregar_rede(path)) {
        return false;
    }

    std::vector<BotGenetico> nova_pop(tamanho_populacao, bot);

    return true;
}