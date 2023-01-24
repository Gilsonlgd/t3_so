#include "rand.h"
#include "rel.h"
#include "es.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

struct rand_t {
    int limite;
    int tempo_parado;
    int intervalo_le;
    rel_t *relogio;
};

rand_t *rand_cria(int limite, rel_t* relogio, int intervalo_le)
{
    rand_t *self;
    self = malloc(sizeof(rand_t));
    srand(time(NULL));
    if(self != NULL) {
        self->limite = limite;
        self->relogio = relogio;
        self->tempo_parado = 0;
        self->intervalo_le = intervalo_le;
    }
    return self;
}

void rand_destroi(rand_t* self)
{
    free(self);
}

err_t rand_le(void *disp, int id, int *pvalor)
{
    err_t err = ERR_OK;
    rand_t* self = (rand_t*)disp;
    
    if(self == NULL) {
        err = ERR_INSTR_INV;
        return err;
    }
    
    if(rand_pronto(disp, id, leitura)){
        *pvalor = rand() % self->limite;
        self->tempo_parado = rel_agora(self->relogio);
    } else {
        err = ERR_OCUP;
    }
    return err;
}

bool rand_pronto(void *disp, int id, acesso_t acesso)
{
    rand_t* self = (rand_t*)disp;
    
    if((rel_agora(self->relogio) - (self->tempo_parado + self->intervalo_le)) >= 0) {
        return true;
    }
    return false;
}
