#include <stdlib.h>
#include <stdio.h>
#include "processo.h"
#include "err.h"
#include "es.h"
#include "contr.h"


#ifndef LISTA_H
#define LISTA_H

typedef struct no_t no_t;

//cria um nó de tabela de processos
no_t* cria_no(processo_t* processo);

//insere processo na tabela de processos
void insere(no_t **head, processo_t* processo);

//busca processo por seu numero na tabela de processos
processo_t* busca(no_t *self, int num);

//retorna próximo processo pronto para execução.
processo_t* retorna_proximo_pronto(no_t* self);

//finaliza o processo atualmente em execução
err_t finaliza_processo_em_exec(no_t** self, rel_t* rel);

//bloqueia o processo atualmente em execução
//salva junto ao processo:
//- estados da cpu e memoria no momento do bloqueio;
//- dispositivo que causou o bloqueio;
//- tipo de chamada que causou o bloqueio (e/s).
void bloqueia_processo_em_exec(no_t **self, mem_t* mem, 
                               cpu_estado_t* cpu_estado, int disp, 
                               acesso_t chamada, rel_t *rel);

//varre todos os processos da tabela, desbloqueando
//os processos que devem estar prontos para exec
//(utiliza es_pronto para conferir se o dispositivo
//que causou o bloqueio agora está disponível).
void varre_processos(no_t** self, contr_t* contr, rel_t *rel);

//retorna true se houve processo em execução.
bool tem_processo_executando(no_t* self);

// retorna false se não houver mais processos a serem executados
bool tem_processo_vivo(no_t* self);
//imprime a tabela de processos
void imprime_tabela(no_t* self);

#endif