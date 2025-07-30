#ifndef _JOGO_H
#define _JOGO_H

#include "cobra.h"
#include <vector>

enum index
{
    VIS_PAREDE_C,
    VIS_PAREDE_CE,
    VIS_PAREDE_E,
    VIS_PAREDE_BE,
    VIS_PAREDE_B,
    VIS_PAREDE_BD,
    VIS_PAREDE_D,
    VIS_PAREDE_CD,

    VIS_CORPO_C,
    VIS_CORPO_CE,
    VIS_CORPO_E,
    VIS_CORPO_BE,
    VIS_CORPO_B,
    VIS_CORPO_BD,
    VIS_CORPO_D,
    VIS_CORPO_DC,

    VIS_COMIDA_C,
    VIS_COMIDA_CE,
    VIS_COMIDA_E,
    VIS_COMIDA_BE,
    VIS_COMIDA_B,
    VIS_COMIDA_BD,
    VIS_COMIDA_D,
    VIS_COMIDA_DC
};

class Jogo 
{
public:
    Jogo (int altura, int largura);
    void tick ();
    void input (Direcao nova_direcao);

    int get_altura() const;
    int get_largura() const;
    int get_score() const;
    bool isGameOver() const;
    bool isWin() const;
    Direcao get_direcao() const;

    const Cobra& get_cobra() const;
    const comida_t& get_comida() const;

    std::vector<double> calcular_visao() const;
    
private:
    int altura;
    int largura;
    int score;
    
    bool game_over;
    
    Cobra cobra;
    comida_t comida;
    Direcao dir_atual;

    void gen_comida();

    int distCorpo(int, int) const;
    double isComidaDir (int, int) const;
};

#endif