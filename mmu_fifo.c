#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "err.h"
#include "mem.h"
#include "tab_pag.h"
#include "mmu_fifo.h"
#include "tela.h"

struct pagina_t {
    int num;
    int processo_num;
    int quadro_num;
    tab_pag_t* tab_pag;;
    pagina_t* next;
};

struct fifo_t {
    int num_pags;
    pagina_t* head;
    pagina_t* last;
};

static pagina_t* fifo_cria_no(int num, int processo_num, int quadro, tab_pag_t* tab);

fifo_t* fifo_cria()
{
    fifo_t* self = malloc(sizeof(*self));
    self->num_pags = 0;
    self->head = NULL;
    self->last = NULL;

    return self;
}

void fifo_destroi(fifo_t* self)
{
    if(self != NULL) free(self);
    else t_printf("ERRO: a fifo já foi destruida\n");
}

static pagina_t* fifo_cria_no(int num, int processo_num, int quadro, tab_pag_t* tab) 
{
    pagina_t* self = malloc(sizeof(pagina_t));
    if(self != NULL) {
        self->num = num;
        self->processo_num = processo_num;
        self->quadro_num = quadro;
        self->tab_pag = tab;
        self->next = NULL;
        return self;
    } else {
        t_printf("Erro ao alocar nova página na fifo. \n");
        return NULL;
    }
}

void fifo_insere_pagina(fifo_t* self, int num, int quadro, tab_pag_t* tab)
{
    int num_processo = tab_pag_processo(tab);
    pagina_t** last = &self->last;
    pagina_t* novo_no = fifo_cria_no(num, num_processo, quadro, tab);
    if(*last == NULL) {
        *last = novo_no;
        pagina_t** head = &self->head;
        *head = novo_no;
    } else {
        (*last)->next = novo_no;
        *last = novo_no;   
    }
    self->num_pags++;
}

void fifo_retira_pagina(fifo_t* self)
{
    pagina_t** head = &self->head;
    if (*head == NULL) {
        t_printf("A fifo ja estah vazia");
        return;
    }

    pagina_t* new_head = (*head)->next;
    free(*head);       
    *head = new_head;
    if(*head == NULL) self->last = NULL;    
    self->num_pags--;
}

int fifo_prox_pag_num(fifo_t* self)
{
    return self->head->num;
}

int fifo_prox_pag_processo(fifo_t* self)
{
    return self->head->processo_num;
}

int fifo_prox_pag_quadro(fifo_t* self)
{
    return self->head->quadro_num;
}

tab_pag_t* fifo_prox_pag_tab(fifo_t* self)
{
    return self->head->tab_pag;
}

bool fifo_vazia(fifo_t* self)
{
    if (self->head == NULL) return true;
    else return false;
}

int fifo_num_pags(fifo_t* self)
{
    return self->num_pags;
}

void fifo_imprime(fifo_t* self)
{
    if (self->head == NULL) t_printf("FIFO VAZIAA");
    pagina_t* atual = self->head;
    while (atual != NULL) {
        t_printf("pag: %d, quadro: %d proc: %d\n", atual->num, atual->quadro_num, atual->processo_num);
        atual = atual->next;
    }
}

void fifo_liberaPags_processo(fifo_t* self, int processo)
{
    pagina_t* atual = self->head;
    pagina_t* anterior = NULL;
    while (atual != NULL) {
        if (atual->processo_num == processo) {
            self->num_pags--;
            if (anterior != NULL) {
                anterior->next = atual->next;
            } else {
                self->head = atual->next;
                self->last = atual->next;
            }
            pagina_t* temp = atual;
            atual = atual->next;
            free(temp);
        } else {
            anterior = atual;
            atual = atual->next;
        }
    }
}
