#include <stdlib.h>
#include <stdio.h>
#include "escalonador.h"
#include "processo.h"
#include "err.h"
#include "es.h"
#include "tela.h"
#include "rel.h"


// Estrutura que representa um nó da lista de processos
struct no_t {
    processo_t* processo;
    no_t* next;
};

no_t* cria_no(processo_t* processo) {
    no_t* self = malloc(sizeof(no_t));
    self->processo = processo;
    self->next = NULL;
    return self;
}

void insere(no_t **head, processo_t* processo) {
    no_t* novo_no = cria_no(processo);
    novo_no->next = *head;
    *head = novo_no;
}

processo_t* busca(no_t *self, int num) {
    no_t* atual = self;
    while (atual != NULL) {
        if (processo_num(atual->processo) == num) {
            return atual->processo;
        }
        atual = atual->next;
    }
    return NULL;
}

processo_t* retorna_proximo_pronto(no_t* self) {
    no_t* atual = self;
    while (atual != NULL) {
        if (processo_estado(atual->processo) == pronto) {
            return atual->processo;
        }
        atual = atual->next;
    }
    return NULL;
}

err_t finaliza_processo_em_exec(no_t** self, rel_t* rel)
{
    no_t* atual = *self;
    no_t* anterior = NULL;
    err_t err = ERR_OK;

    while (atual != NULL) {
        if (processo_estado(atual->processo) == em_execucao) {
            if (anterior == NULL) {
                *self = atual->next;
            } else {
                anterior->next = atual->next;
            }
            processo_destroi(atual->processo, rel_agora(rel));
            if (atual != NULL) free(atual);
            return err;
        }
        anterior = atual;
        atual = atual->next;
    }
    //retornando qualquer erro para identificar que a finalização deu errado
    return ERR_INSTR_INV; 
}

void bloqueia_processo_em_exec(no_t **self, mem_t* mem, 
                               cpu_estado_t* cpu_estado, int disp, 
                               acesso_t chamada, rel_t *rel)
{
    no_t* atual = *self;
    while (atual != NULL) {
        if (processo_estado(atual->processo) == em_execucao) {
            processo_bloqueia(atual->processo, mem, cpu_estado, disp, chamada, rel_agora(rel));
            break;
        }
        atual = atual->next;
    }
}

void varre_processos(no_t** self, contr_t* contr, rel_t *rel)
{
    no_t* atual = *self;
    bool pronto;
    while (atual != NULL) {
        if(processo_estado(atual->processo) == bloqueado) {
            pronto = es_pronto(contr_es(contr), processo_disp(atual->processo), 
                           processo_chamada(atual->processo));
            if(pronto) {
                processo_desbloqueia(atual->processo, rel_agora(rel));
            }
        }
        atual = atual->next;
    }
}

bool tem_processo_executando(no_t* self)
{
    no_t* atual = self;
    while (atual != NULL) {
        if(processo_estado(atual->processo) == em_execucao){
            return true;
        }
        atual = atual->next;
    }
    return false;
}

bool tem_processo_vivo(no_t* self)
{
    no_t* head = self;

    if(head == NULL) return false;
    return true;
}

void imprime_tabela(no_t* self)
{
    no_t* atual = self;
    while (atual != NULL) {
        t_printf("num: %d estado: %d", processo_num(atual->processo), processo_estado(atual->processo));
        atual = atual->next;
    }
}

