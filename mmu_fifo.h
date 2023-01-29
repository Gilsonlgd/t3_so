#ifndef MMU_FIFO_H
#define MMU_FIFO_H


#include "err.h"
#include "mem.h"
#include "mmu.h"
#include "tab_pag.h"
// tipo opaco que representa o gerenciador de páginas 
typedef struct fifo_t fifo_t;

typedef struct pagina_t pagina_t;

fifo_t *fifo_cria();

void fifo_destroi(fifo_t* self);

pagina_t* fifo_cria_no(int num, int processo_num, int quadro, bool* valida_ptr, mem_t* mem_ptr, tab_pag_t* tab);

void fifo_insere_pagina(fifo_t* self, int num, int num_processo, int quadro, bool* valida_ptr, mem_t* mem_ptr, tab_pag_t* tab);

void fifo_retira_pagina(fifo_t* self);

int fifo_prox_pag_num(fifo_t* self);

int fifo_prox_pag_processo(fifo_t* self);

int fifo_prox_pag_quadro(fifo_t* self);

mem_t* fifo_prox_pag_mem(fifo_t* self);

bool fifo_vazia(fifo_t* self);

int fifo_num_pags(fifo_t* self);

void fifo_imprime(fifo_t* self);

void fifo_liberaPags_processo(fifo_t* self, int processo);

tab_pag_t* fifo_prox_pag_tab(fifo_t* self);

#endif // MMU_FIFO_H
