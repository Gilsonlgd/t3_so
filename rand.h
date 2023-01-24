#include "err.h"
#include "rel.h"
#include <stdbool.h>
#include "es.h"
#ifndef RAND_H
#define RAND_H

typedef struct rand_t rand_t;

//cria e inicializa um dispositivo que gera numeros aleatórios
rand_t *rand_cria(int limite, rel_t* rel, int intervalo_le);

//destroi um dispositivo random
// nada pode ser feito depois desta chamada
void rand_destroi(rand_t* self);
err_t rand_le(void *disp, int id, int *pvalor);
bool rand_pronto(void *disp, int id, acesso_t acesso);
#endif