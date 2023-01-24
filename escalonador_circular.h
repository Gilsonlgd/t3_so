#include <stdlib.h>
#include <stdio.h>
#include "processo.h"
#include "err.h"
#include "es.h"
#include "contr.h"

#ifndef ESCALONADOR_CIRC_H
#define ESCALONADOR_CIRC_H
typedef struct no_t no_t;

typedef struct esc_circ_t esc_circ_t;

//cria o escalonador
esc_circ_t* esc_cria(int quantum);

//destroi o escalonador
void esc_destroi(esc_circ_t* self);

// cria um nó de fila ou lista
no_t* cria_no(processo_t* processo);

//insere um processo na fila de processos prontos do esc
void insere_fila(esc_circ_t *self, processo_t* processo);

//insere no inicio de uma lista
void insereI_lista(no_t** head, processo_t* processo);

//retorna o próximo processo da fila de processos
processo_t* retorna_proximo_pronto(esc_circ_t* self);

//faz o que o nome diz
err_t finaliza_processo_em_exec(esc_circ_t* self, rel_t *rel);

//faz o que o nome diz também
void bloqueia_processo_em_exec(esc_circ_t* self, mem_t *mem, 
                               cpu_estado_t *cpu_estado, int disp, 
                               acesso_t chamada, rel_t *rel);

//realiza uma varredura na lista de processos bloqueados
//caso algum tenha desbloqueado, realiza preempção
void varre_processos_bloqueados(esc_circ_t* self, contr_t *contr, rel_t *rel);

//caso haja um processo em execução retorna true
bool tem_processo_executando(esc_circ_t* self);

//se tiver algum processo dentro do escalonador retorna true
bool tem_processo_vivo(esc_circ_t* self);

//inicializa o primeiro processo no escalonador.
void esc_init(esc_circ_t* self, processo_t* processo);

//retorna o processo em execução no momento
processo_t* esc_processo_executando(esc_circ_t* self);

//realiza a iteração do quantum do processo em execução. 
//após, verifica seu quantum atual. 
//caso tenha ultrapassado o quantum definido, realiza preempção
void esc_check_quantum(esc_circ_t* self, mem_t *mem, cpu_estado_t *cpu_estado, rel_t *rel);

//retorna o quantum definido pelo escalonador
int esc_quantum(esc_circ_t* self);

//funções de impressão para facilitar o debbug
void imprime_tabela(no_t* self);
void imprime_em_exec(esc_circ_t* self);

void esc_imprime_metricas(esc_circ_t* self, FILE* arq);

#endif

