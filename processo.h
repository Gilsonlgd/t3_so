#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
#include "cpu_estado.h"
#include "es.h"
#include "tab_pag.h"
#include "mmu.h"


#ifndef PROCESSO_H
#define PROCESSO_H
typedef struct processo_t processo_t;
typedef enum { pronto, em_execucao, bloqueado } processo_estado_t;
typedef enum {
    TEMPO_PRONTO, 
    TEMPO_EXECUTANDO, 
    TEMPO_BLOQUEADO,
    REL_ULTIMO_DESBLOQUEIO, 
    REL_ULTIMA_EXEC, 
    REL_ULTIMO_BLOQUEIO,
    NUM_BLOQUEIOS,
    NUM_PREEMPCOES
} processo_metricas_t;

processo_t *processo_cria(int num, processo_estado_t estado, int agora);

//inicia a memória do processo quando criado
err_t processo_init_mem(processo_t *self);

// faz o que o nome diz, apenas delegando a localização das páginas na memória
void processo_init_tab_pag(processo_t* self, int tam_progr);

// transfere o .maq para a memória do progrma 
err_t transf_mem(processo_t *self, int* progr, int tam_progr);

void processo_destroi(processo_t* self, mmu_t* mmu, int agora);

//muda o estado do processo para em_execução
void processo_executa(processo_t* self, int agora, int quantum);

//muda o estado do processo para bloqueado
//salva junto ao processo:
//- estados da cpu e memoria no momento do bloqueio;
//- dispositivo que causou o bloqueio;
//- tipo de chamada que causou o bloqueio (e/s).
void processo_es_bloqueia(processo_t* self, cpu_estado_t* cpu_estado, 
                      int disp, acesso_t chamada, int agora);

// faz o que o nome diz
void processo_preempta(processo_t* self, cpu_estado_t* cpu_estado, int agora);

// decrementa o quantum do processo
void processo_tik(processo_t* self);

// faz o que o nome diz
void processo_setQuantum(processo_t* self, int quantum);

//muda o estado do processo para pronto
void processo_desbloqueia(processo_t* self, int agora);

// retorna a última chamada de sistema feita pelo processo que causou bloqueio
acesso_t processo_chamada(processo_t* processo);

// retorna o estado da cpu do processo
cpu_estado_t* processo_cpu(processo_t* self);

tab_pag_t* processo_tab_pag(processo_t* self);

// retorna o estado do processo (pronto, em_execucao, bloqueado)
processo_estado_t processo_estado(processo_t* self);

//retorna o numero do processo
int processo_num(processo_t* self) ;

//retorna o dispositivo que causou o último bloqueio do processo
int processo_disp(processo_t* processo);

int processo_quantum(processo_t* processo);

void processo_muda_estado(processo_t* self, processo_estado_t estado);

void processo_finaliza(processo_t* self, mmu_t* mmu, int agora);

void processo_imprime_metricas(processo_t* self, FILE* arq);

float processo_tmedio_retorno(processo_t* self);
float processo_tmedio_exec(processo_t* self);



#endif
