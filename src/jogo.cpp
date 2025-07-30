#include "jogo.h"
#include <cstdlib>
#include <time.h>
#include <algorithm>
#include <vector>

Jogo::Jogo(int altura, int largura) {
    this->altura = altura;
    this->largura= largura;

    this->game_over = false;
    
    this->score = 0;
    this->dir_atual = DIREITA;
    
    gen_comida();

    srand(time(NULL));
}

void Jogo::tick() {
    auto cabeca_atual = cobra.get_cabeca();
    int next_y = cabeca_atual.y;
    int next_x = cabeca_atual.x;

    switch (dir_atual) {
        case CIMA: next_y --; break;
        case BAIXO: next_y ++; break;
        case ESQUERDA: next_x --; break;
        case DIREITA: next_x ++; break;
    }

    bool comeu = (next_x == comida.x && next_y == comida.y);

    cobra.andar(dir_atual, comeu);

    if (cobra.check_colision()) {
        game_over = true;
    }

    if (comeu) {
        score += 1;
        gen_comida();
    }
}

void Jogo::input(Direcao nova_direcao) {
    if (
        dir_atual == nova_direcao ||
        dir_atual == CIMA && nova_direcao == BAIXO ||
        dir_atual == BAIXO && nova_direcao == CIMA ||
        dir_atual == ESQUERDA && nova_direcao == DIREITA ||
        dir_atual == DIREITA && nova_direcao == ESQUERDA 
    ){
        return;
    }

    dir_atual = nova_direcao;
}

struct Posicao
{
    int y, x;
};


void Jogo::gen_comida() {
    std::vector<Posicao> pos_disp;

    for (int y = 0; y < altura; y++)
    for (int x = 0; x < largura; x++)
    {
        if (!cobra.is_pedaco(y, x)) {
            pos_disp.push_back({y, x});
        }
    }

    if (!pos_disp.empty()) {
        Posicao nova = pos_disp[ rand() % pos_disp.size() ];

        comida.x = nova.x;
        comida.y = nova.y;
    }

}

double paraRede (int dist) {
    return 1.0 / (dist + 1.0);
}

using namespace std;

vector<double> Jogo::calcular_visao() const {
    auto cabeca = cobra.get_cabeca();

    vector<double> visao(24, 0.0);

    int dist;
    // ===========================================
    // BLOCO 1 : distâncias para a parede (0 a 7)
    // ===========================================

    visao[0] = paraRede(cabeca.y);
    visao[1] = paraRede(std::min(cabeca.y, cabeca.x));
    visao[2] = paraRede(cabeca.x);
    visao[3] = paraRede(std::min((altura - 1) - cabeca.y, cabeca.x));
    visao[4] = paraRede((altura - 1) - cabeca.y);
    visao[5] = paraRede(std::min((altura - 1) - cabeca.y, (largura - 1) - cabeca.x));
    visao[6] = paraRede((largura - 1) - cabeca.x);
    visao[7] = paraRede(std::min(cabeca.y,  (largura - 1) - cabeca.x));

    // =================================================
    // BLOCO 2: distâncias para o próprio corpo (8 a 15)
    // =================================================

    int d;
    d = distCorpo(-1, 0);   visao[8] = (d > 0) ? paraRede(d) : 0.0;
    d = distCorpo(-1, -1);  visao[9] = (d > 0) ? paraRede(d) : 0.0;
    d = distCorpo(0, -1);   visao[10] = (d > 0) ? paraRede(d) : 0.0;
    d = distCorpo(1, -1);   visao[11] = (d > 0) ? paraRede(d) : 0.0;
    d = distCorpo(1, 0);    visao[12] = (d > 0) ? paraRede(d) : 0.0;
    d = distCorpo(1, 1);    visao[13] = (d > 0) ? paraRede(d) : 0.0;
    d = distCorpo(0, 1);    visao[14] = (d > 0) ? paraRede(d) : 0.0;
    d = distCorpo(-1, 1);   visao[15] = (d > 0) ? paraRede(d) : 0.0;

    // ========================================
    // BLOCO 3: Localização da comida (16 a 23)
    // ========================================

    visao[16] = isComidaDir(-1, 0);
    visao[17] = isComidaDir(-1, -1);
    visao[18] = isComidaDir(0, -1);
    visao[19] = isComidaDir(1, -1);
    visao[20] = isComidaDir(1, 0);
    visao[21] = isComidaDir(1, 1);
    visao[22] = isComidaDir(0, 1);
    visao[23] = isComidaDir(-1, 1);

    return visao;
}

int Jogo::distCorpo(int var_y, int var_x) const {
    auto pos_atual = cobra.get_cabeca();

    int distancia = 0;

    while (true) {
        pos_atual.y += var_y;
        pos_atual.x += var_x;
        distancia ++;

        if (pos_atual.y < 0 || pos_atual.y >= altura ||
            pos_atual.x < 0 || pos_atual.x >= largura) {
                return 0;
            }

        if (cobra.is_pedaco(pos_atual.y, pos_atual.x)) {
            return distancia;
        }
    }
}

double Jogo::isComidaDir (int var_y, int var_x) const {
    auto pos_atual = cobra.get_cabeca();

    while (true) {
        pos_atual.y += var_y;
        pos_atual.x += var_x;

        if (pos_atual.y < 0 || pos_atual.y >= altura ||
            pos_atual.x < 0 || pos_atual.x >= largura) {
                return 0.0;
            }
        
        if (pos_atual.y == comida.y && pos_atual.x == comida.x) {
            return 1.0;
        }

        if (cobra.is_pedaco(pos_atual.y, pos_atual.x)) {
            return 0.0;
        }
    }
}

bool Jogo::isWin() const { return cobra.get_tamanho() == altura*largura; }
int Jogo::get_altura() const { return altura; }
int Jogo::get_largura() const{ return largura; }
int Jogo::get_score() const { return score; }
bool Jogo::isGameOver() const { return game_over; }
const Cobra& Jogo::get_cobra() const { return cobra; }
const comida_t& Jogo::get_comida() const { return comida; }
Direcao Jogo::get_direcao() const { return dir_atual; }