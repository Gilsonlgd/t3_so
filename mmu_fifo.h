#ifndef MMU_FIFO_H
#define MMU_FIFO_H

#include "err.h"
#include "mem.h"
#include "mmu.h"
#include "tab_pag.h"
// tipo opaco que representa o gerenciador de páginas 
typedef struct fifo_t fifo_t;
// tipo opaco que representa as paginas do gerenciador
typedef struct pagina_t pagina_t;

fifo_t *fifo_cria();

void fifo_destroi(fifo_t* self);
// insere uma página no fim da fila
// as páginas tb mantém um ponteiro para a tabela de pags do processo
// ao qual referencia (para facilitar nos swaps)
void fifo_insere_pagina(fifo_t* self, int num, int quadro, tab_pag_t* tab);

// retira a primeira página da fila
void fifo_retira_pagina(fifo_t* self);

// retorna o numero da primeira página da fila
int fifo_prox_pag_num(fifo_t* self);

// retorna o processo da primeira página da fila
int fifo_prox_pag_processo(fifo_t* self);

// retorna o quadro da primeira página da fila
int fifo_prox_pag_quadro(fifo_t* self);

// retorna o ponteiro para a tab de pags do processo
// relativo a primeira página da fila
tab_pag_t* fifo_prox_pag_tab(fifo_t* self);

//retorna dados sobre a fila
bool fifo_vazia(fifo_t* self);
int fifo_num_pags(fifo_t* self);
void fifo_imprime(fifo_t* self);

// retira da fifo todas as páginas relativas ao processo em questão
// utilizada na finalização de um processo 
void fifo_liberaPags_processo(fifo_t* self, int processo);

#endif // MMU_FIFO_H
