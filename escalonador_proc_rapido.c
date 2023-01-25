#include <stdlib.h>
#include <stdio.h>
#include "escalonador_proc_rapido.h"
#include "processo.h"
#include "err.h"
#include "es.h"
#include "tela.h"
#include "rel.h"

struct no_t {
    processo_t* processo;
    no_t* next;
};

struct esc_rap_t {
    int quantum;
    no_t* head;
    no_t* last;
    processo_t* em_exec;
    no_t* lista_bloqueados;
    no_t* lista_finalizados;
};

esc_rap_t* esc_cria(int quantum) {
    esc_rap_t* self = malloc(sizeof(*self));
    self->head = NULL;
    self->last = NULL;
    self->em_exec = NULL;
    self->lista_bloqueados = NULL;
    self->lista_finalizados = NULL;
    self->quantum = quantum;
    return self;
}

void esc_init(esc_rap_t* self, processo_t* processo)
{
    self->em_exec = processo;
}

void esc_destroi(esc_rap_t* self)
{
    //apenas usado para registrar métricas
    no_t* atual = self->lista_finalizados;
    
    while (atual != NULL) {
        no_t* next = atual->next;
        free(atual);
        atual = next;
    }

    if(self != NULL) {
        free(self);
    }
}

no_t* cria_no(processo_t* processo) {
    no_t* self = malloc(sizeof(no_t));
    if(self != NULL) {
        self->processo = processo;
        self->next = NULL;
        return self;
    } else {
        t_printf("Erro ao alocar novo nó. \n");
        return NULL;
    }
}

void insereOrdenado_lista(esc_rap_t *self, processo_t* processo) {
    no_t** head = &self->head;
    no_t** last = &self->last;
    no_t* novo_no = cria_no(processo);
    int p_tmedio;
    if (processo_quantum(processo) == 0) {
        p_tmedio = self->quantum;
    } else {
        p_tmedio = processo_tmedio_exec(processo);
    }

    if(*head == NULL) {
        *head = novo_no;
        *last = novo_no;
    } else {
        if (p_tmedio < processo_tmedio_exec((*head)->processo)) {
            novo_no->next = *head;
            *head = novo_no;
        } else if (p_tmedio > processo_tmedio_exec((*last)->processo)) {
            (*last)->next = novo_no;
            *last = novo_no;
        } else {
            no_t* atual = *head;
            while (atual->next != NULL && p_tmedio > processo_tmedio_exec(atual->next->processo)) {
                atual = atual->next;
            }
            novo_no->next = atual->next;
            atual->next = novo_no;
        }
    }
}

void insereI_lista(no_t** head, processo_t* processo)
{
    no_t* novo_no = cria_no(processo);
    novo_no->next = *head;
    *head = novo_no;
}

processo_t* retorna_proximo_pronto(esc_rap_t* self) {
    no_t** head = &self->head;
    if (*head == NULL) {
        return NULL;
    }

    self->em_exec = (*head)->processo;
    no_t* new_head = (*head)->next;
    free(*head);       
    *head = new_head;
    if(*head == NULL) self->last = NULL;

    
    return self->em_exec;
}

err_t finaliza_processo_em_exec(esc_rap_t* self, mmu_t* mmu, rel_t *rel)
{   
    err_t err = ERR_OK;
    //Se não resgistrar as métricas de cada processo:
    //processo_destroi(self->em_exec, rel_agora(rel));

    //Registrando as métricas de cada processo:
    processo_finaliza(self->em_exec, mmu, rel_agora(rel));
    insereI_lista(&self->lista_finalizados, self->em_exec);
    self->em_exec = NULL;
    return err;   
}

void bloqueia_processo_em_exec(esc_rap_t* self, mmu_t* mmu, 
                               cpu_estado_t *cpu_estado, int disp, 
                               acesso_t chamada, rel_t *rel)
{
    processo_es_bloqueia(self->em_exec, cpu_estado, 
                      disp, chamada, rel_agora(rel));
    mmu_usa_tab_pag(mmu, NULL);
    insereI_lista(&self->lista_bloqueados, self->em_exec);
    self->em_exec = NULL;
}

void varre_processos_bloqueados(esc_rap_t* self, contr_t *contr, rel_t *rel)
{
    no_t** lista = &self->lista_bloqueados;
    no_t* atual = *lista;
    no_t* anterior = NULL;
    bool pronto;

    while (atual != NULL) {
        pronto = es_pronto(contr_es(contr), processo_disp(atual->processo), 
                        processo_chamada(atual->processo));
        if(pronto) {
            processo_desbloqueia(atual->processo, rel_agora(rel));
            insereOrdenado_lista(self, atual->processo);
            if (anterior == NULL) {
                *lista = atual->next;
            } else {
                anterior->next = atual->next;
            }
            free(atual);
            break;
        }
        anterior = atual;
        atual = atual->next;
    }
}

void esc_check_quantum(esc_rap_t* self, mmu_t* mmu, cpu_estado_t *cpu_estado, rel_t *rel)
{
    if (self->em_exec == NULL) {
        return;
    } else {
        processo_tik(self->em_exec);
    }

    if (processo_quantum(self->em_exec) < 0) {
        processo_preempta(self->em_exec, cpu_estado, rel_agora(rel));
        mmu_usa_tab_pag(mmu, NULL);
        insereOrdenado_lista(self, self->em_exec);
        self->em_exec = NULL;
    }
}

bool tem_processo_executando(esc_rap_t* self)
{
    if(self->em_exec != NULL) return true;
    return false;
}

bool tem_processo_vivo(esc_rap_t* self)
{
    no_t* head = self->head;
    no_t* head_bloqueados = self->lista_bloqueados;
    processo_t* executando = self->em_exec;

    if(head == NULL && head_bloqueados == NULL && executando == NULL) return false;
    return true;
}

int esc_quantum(esc_rap_t* self)
{
    return self->quantum;
}

processo_t* esc_processo_executando(esc_rap_t* self) 
{
    return self->em_exec;
}

void imprime_tabela(no_t* self)
{
    no_t* atual = self;
    if(atual == NULL) t_printf("vazia");
    while (atual != NULL) {
        t_printf("num: %d estado: %d quantum: %d", processo_num(atual->processo), processo_estado(atual->processo), processo_quantum(atual->processo));
        atual = atual->next;
    }
}

void imprime_em_exec(esc_rap_t* self)
{
    if (self->em_exec != NULL){
        t_printf("num: %d estado: %d quantum: %d", processo_num(self->em_exec), processo_estado(self->em_exec), processo_quantum(self->em_exec));
    } else {
        t_printf("NULL");
    }
}

void esc_imprime_metricas(esc_rap_t *self, FILE* arq)
{
    no_t* atual = self->lista_finalizados;
    while (atual != NULL) {
        processo_imprime_metricas(atual->processo, arq);
        atual = atual->next;
    }
}

