#ifndef _COBRA_H
#define _COBRA_H

const int width  = 30;
const int height = 30;

typedef enum {CIMA, BAIXO, DIREITA, ESQUERDA} Direcao;

struct comida_t {
    int x = 3*width/4;
    int y = height/2;
};

class Cobra {
private:
    struct pedaco
    {
        pedaco (const int& y, const int& x) : x(x), y(y) {}
        int x;
        int y;

        pedaco *past = nullptr;
        pedaco *next = nullptr;
    };

    void add_pedaco 
    (const int& y, const int& x) {
        auto new_cabeca = 
        new pedaco (y, x);

        new_cabeca->next = cabeca;
        cabeca->past = new_cabeca;

        cabeca = new_cabeca;
        tamanho ++;
    }

    void rm_cauda () {
        if (tamanho <= 1) return;

        auto antiga_cauda = cauda;
        
        this->cauda = antiga_cauda->past;

        if(this->cauda != nullptr) {
            this->cauda->next = nullptr;
        }
        
        delete antiga_cauda;
        tamanho--;
    }

    int tamanho = 1;
    pedaco *cabeca;
    pedaco *cauda;

public:
    Cobra () {
        pedaco* pedaco_inicial = new pedaco(height/2, width/4);
        this->cabeca = pedaco_inicial;
        this->cauda  = pedaco_inicial;
    }


    int get_tamanho () const {
        return this->tamanho;
    }

    void andar (Direcao direcao, bool comeu) {

        switch (direcao) {
            case CIMA:
            add_pedaco
            (this->cabeca->y - 1, this->cabeca->x);

            break;

            case BAIXO:
            add_pedaco
            (this->cabeca->y + 1, this->cabeca->x);
            break;

            case ESQUERDA:
            add_pedaco
            (this->cabeca->y, this->cabeca->x - 1);
            break;

            case DIREITA:
            add_pedaco
            (this->cabeca->y, this->cabeca->x + 1);
            break;
        }

        if (!comeu) {
            rm_cauda();
        }
    }

    bool is_pedaco (const int& y, const int& x) const {
        auto head = this->cabeca;

        while (head != nullptr) {
            if (head->x == x && head->y == y) {
                return true;
            }

            head = head->next;
        }

        return false;
    }

    bool check_colision () const {
        if (this->cabeca->x >= width || this->cabeca->x < 0 || 
            this->cabeca->y >= height|| this->cabeca->y < 0) {
            
            return true;
        }

        auto check = this->cabeca->next;

        while (check != nullptr) {
            if (check->x == this->cabeca->x && check->y == this->cabeca->y) {
                return true;
            }

            check = check->next;
        } 

        return false;
    }

    pedaco get_cabeca () const {
        return *this->cabeca;
    }

    pedaco* get_cabeca_ptr () const {
        return this->cabeca;
    }

    pedaco get_cauda () const {
        return *this->cauda;
    }

    Cobra (const Cobra& other) {
        this->tamanho = 0;
        this->cabeca  = nullptr;
        this->cauda   = nullptr;

        if (other.cabeca == nullptr) return;

        this->cabeca =
        new pedaco(other.cabeca->y, other.cabeca->x);
        this->tamanho = 1;
        this->cauda = this->cabeca;
        pedaco* current_other_node = other.cabeca->next;
        pedaco* current_this_node  = this->cabeca;

        while (current_other_node != nullptr) {
            pedaco* novo_pedaco =
            new pedaco(current_other_node->y, current_other_node->x);
            current_this_node->next = novo_pedaco;
            novo_pedaco->past = current_this_node;

            this->cauda = novo_pedaco;
            current_this_node = novo_pedaco;
            current_other_node = current_other_node->next;
            this->tamanho++;
        }
    }

    Cobra& operator= (const Cobra& other) {
        if (this == &other) {
            return *this;
        }

        // Deleta a lista antiga desta cobra para evitar leak
        pedaco* head = this->cabeca;
        while (head != nullptr) {
            auto to_delete = head;
            head = head->next;
            delete to_delete;
        }

        // Copia profunda
        if (other.cabeca == nullptr) {
            this->cabeca = nullptr;
            this->cauda = nullptr;
            this->tamanho = 0;
            return *this;
        }

        this->cabeca = 
        new pedaco (other.cabeca->y, other.cabeca->x);

        this->tamanho = 1;
        this->cauda = this->cabeca;

        pedaco* current_other_node = other.cabeca->next;
        pedaco* current_this_node  = this->cabeca;

        while (current_other_node != nullptr) {
            pedaco* novo_pedaco =
            new pedaco(current_other_node->y, current_other_node->x);
            current_this_node->next = novo_pedaco;
            novo_pedaco->past = current_this_node;

            this->cauda = novo_pedaco;
            current_this_node = novo_pedaco;
            current_other_node = current_other_node->next;
            this->tamanho++;
        }

        return *this;
    }
    
    ~Cobra() {
        pedaco *head = cabeca;

        while (head != nullptr) {
            auto to_delete = head;
            head = head->next;
            delete to_delete;
        }
    }

};
#endif