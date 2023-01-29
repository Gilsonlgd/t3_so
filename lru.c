#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "err.h"
#include "mem.h"
#include "tab_pag.h"
#include "lru.h"
#include "tela.h"

struct pagina_t {
    int num;
    int processo_num;
    int quadro_num;
    tab_pag_t* tab_pag;;
    pagina_t* next;
};

struct lru_t {
    int num_pags;
    pagina_t* head;
    pagina_t* last;
};

static pagina_t* lru_cria_no(int num, int processo_num, int quadro, tab_pag_t* tab);

lru_t* lru_cria()
{
    lru_t* self = malloc(sizeof(*self));
    self->num_pags = 0;
    self->head = NULL;
    self->last = NULL;

    return self;
}

void lru_destroi(lru_t* self)
{
    if(self != NULL) free(self);
    else t_printf("ERRO: a lru já foi destruida\n");
}

static pagina_t* lru_cria_no(int num, int processo_num, int quadro, tab_pag_t* tab) 
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
        t_printf("Erro ao alocar nova página na lru. \n");
        return NULL;
    }
}

void lru_insere_pagina(lru_t* self, int num, int quadro, tab_pag_t* tab)
{
    int num_processo = tab_pag_processo(tab);
    pagina_t** last = &self->last;
    pagina_t* novo_no = lru_cria_no(num, num_processo, quadro, tab);
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

void lru_retira_pagina(lru_t* self)
{
    pagina_t** head = &self->head;
    if (*head == NULL) {
        t_printf("A lru ja estah vazia");
        return;
    }

    pagina_t* new_head = (*head)->next;
    free(*head);       
    *head = new_head;
    if(*head == NULL) self->last = NULL;    
    self->num_pags--;
}

void lru_atualiza_pagina(lru_t* self, int num_pagina) {
    pagina_t** head = &self->head;
    pagina_t* atual = *head;
    pagina_t* anterior = NULL;

    while(atual != NULL && atual->num != num_pagina) {
        anterior = atual;
        atual = atual->next;
    }

    if(atual == NULL) {
        return;
    }

    if(anterior != NULL) {
        anterior->next = atual->next;
    } else {
        if(atual->next == NULL) {
            self->last = NULL;
            *head = NULL;
            return;
        }
        *head = atual->next;
    }

    if(self->last != NULL) {
        self->last->next = atual;
    }

    if(self->head == NULL) {
        self->head = atual;
    }

    atual->next = NULL;
    self->last = atual;
}


int lru_prox_pag_num(lru_t* self)
{
    return self->head->num;
}

int lru_prox_pag_processo(lru_t* self)
{
    return self->head->processo_num;
}

int lru_prox_pag_quadro(lru_t* self)
{
    return self->head->quadro_num;
}

tab_pag_t* lru_prox_pag_tab(lru_t* self)
{
    return self->head->tab_pag;
}

bool lru_vazia(lru_t* self)
{
    if (self->head == NULL) return true;
    else return false;
}

int lru_num_pags(lru_t* self)
{
    return self->num_pags;
}

void lru_imprime(lru_t* self)
{
    if (self->head == NULL) t_printf("lru VAZIAA");
    pagina_t* atual = self->head;
    while (atual != NULL) {
        t_printf("pag: %d, quadro: %d proc: %d\n", atual->num, atual->quadro_num, atual->processo_num);
        atual = atual->next;
    }
}

void lru_liberaPags_processo(lru_t* self, int processo)
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
