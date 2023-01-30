#ifndef MMU_lru_H
#define MMU_lru_H

// esta implementação da lru coloca monitora as páginas no momento do acesso
// quando uma página é acessada para leitura/escrita, ela é colocada no fim da fila
// fazendo assim com que a página com acesso menos recente seja a próxima a sair

#include "err.h"
#include "mem.h"
#include "tab_pag.h"
// tipo opaco que representa o gerenciador de páginas 
typedef struct lru_t lru_t;
// tipo opaco que representa as paginas do gerenciador
typedef struct pagina_t pagina_t;

lru_t *lru_cria();

void lru_destroi(lru_t* self);
// insere uma página no fim da fila. 
// as páginas tb mantém um ponteiro para a tabela de pags do processo
// ao qual referencia (para facilitar nos swaps)
void lru_insere_pagina(lru_t* self, int num, int quadro, tab_pag_t* tab);

// retira a primeira página da fila
void lru_retira_pagina(lru_t* self);

// retorna o numero da primeira página da fila
int lru_prox_pag_num(lru_t* self);

// retorna o processo da primeira página da fila
int lru_prox_pag_processo(lru_t* self);

// retorna o quadro da primeira página da fila
int lru_prox_pag_quadro(lru_t* self);

// retorna o ponteiro para a tab de pags do processo
// relativo a primeira página da fila
tab_pag_t* lru_prox_pag_tab(lru_t* self);

// coloca a página em questão no final da fila. 
// utilizada quando a página é acessada
void lru_atualiza_pagina(lru_t* self, int num_pagina, int quadro);

//retorna dados sobre a fila
bool lru_vazia(lru_t* self);
int lru_num_pags(lru_t* self);
void lru_imprime(lru_t* self);

// retira da lru todas as páginas relativas ao processo em questão.
// utilizada na finalização de um processo 
void lru_liberaPags_processo(lru_t* self, int processo);

#endif // MMU_lru_H
