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
  int tam_quadros;
  int num_quadros;
  bool* mem_bitmap;
  int ultimo_endereco; // o último endereço virtual traduzido pela MMU
};

mmu_t *mmu_cria(mem_t *mem, int tam_quadros)
{
  mmu_t *self;
  int tam_mem = mem_tam(mem);
  int num_quadros = tam_mem / tam_quadros;

  self = malloc(sizeof(*self));
  if (self != NULL) {
    self->mem_bitmap = (bool *) malloc(num_quadros * sizeof(bool));
    memset(self->mem_bitmap, 1, num_quadros * sizeof(bool));
    
    self->mem = mem;
    self->tab_pag = NULL;
    self->tam_quadros = tam_quadros;
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
