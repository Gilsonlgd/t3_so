#include "mmu.h"
#include "tab_pag.h"
#include "mem.h"
#include "tela.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tipo de dados opaco para representar o controlador de memória
struct mmu_t {
  mem_t *mem;          // a memória física
  tab_pag_t *tab_pag;  // a tabela de páginas
  int num_quadros;
  bool* mem_bitmap;
  int ultimo_endereco; // o último endereço virtual traduzido pela MMU
};

mmu_t *mmu_cria(mem_t *mem)
{
  mmu_t *self;
  int tam_mem = mem_tam(mem);
  int num_quadros = tam_mem / TAM_QUADRO;

  self = malloc(sizeof(*self));
  if (self != NULL) {
    self->mem_bitmap = (bool *) malloc(num_quadros * sizeof(bool));
    memset(self->mem_bitmap, 1, num_quadros * sizeof(bool));
    
    self->mem = mem;
    self->tab_pag = NULL;
    self->num_quadros = num_quadros;
  }
  return self;
}

void mmu_destroi(mmu_t *self)
{
  if (self != NULL) {
    free(self);
  }
}

void mmu_usa_tab_pag(mmu_t *self, tab_pag_t *tab_pag)
{
  self->tab_pag = tab_pag;
}

// função auxiliar, traduz um endereço virtual em físico
static err_t traduz_endereco(mmu_t *self, int end_v, int *end_f)
{
  self->ultimo_endereco = end_v;
  // se não tem tabela de páginas, não traduz
  if (self->tab_pag == NULL) {
    *end_f = end_v;
    return ERR_OK;
  }
  return tab_pag_traduz(self->tab_pag, end_v, end_f);
}

err_t mmu_le(mmu_t *self, int endereco, int *pvalor)
{
  int end_fis;
  err_t err = traduz_endereco(self, endereco, &end_fis);
  if (err != ERR_OK) {
    return err;
  }
  return mem_le(self->mem, end_fis, pvalor);
}

err_t mmu_escreve(mmu_t *self, int endereco, int valor)
{
  int end_fis;
  err_t err = traduz_endereco(self, endereco, &end_fis);
  if (err != ERR_OK) {
    return err;
  }
  return mem_escreve(self->mem, end_fis, valor);
}

int mmu_proxQuadro_livre(mmu_t *self)
{
  for (int i = 0; i < self->num_quadros; i++) {
    if(self->mem_bitmap[i]) return i;
  }

  t_printf("Memoria cheia, nao ha quadro livre!\n");
  return -1;
}

int mmu_ultimo_endereco(mmu_t *self)
{
  return self->ultimo_endereco;
}

void mmu_ocupa_quadro(mmu_t* self, int id_quadro)
{
  self->mem_bitmap[id_quadro] = false;
}

void mmu_libera_quadro(mmu_t* self, int id_quadro)
{
  self->mem_bitmap[id_quadro] = true;
}

// ATENÇÃO
// talve não use pra nada
err_t mmu_escreve_quadro(mmu_t* self, int id_quadro, int* progr, int pag)
{
  err_t err = ERR_OK;
  int inicio_quadro = id_quadro * TAM_QUADRO;
  int fim_quadro = inicio_quadro + TAM_PAG;

  int p_pagina = pag * TAM_PAG;

  for (int i = inicio_quadro; i < fim_quadro; i++, p_pagina++) {
    err = mem_escreve(self->mem, i, progr[p_pagina]);
    if (err != ERR_OK) {
      t_printf("mmu.escreve_quadro: problema ao escrever mem. Pos: %d", i);
      return err;
    }
  }
  return err;
}

mem_t* mmu_mem(mmu_t* self)
{
  return self->mem;
}

tab_pag_t* mmu_tab_pag(mmu_t* self)
{
  return self->tab_pag;
}