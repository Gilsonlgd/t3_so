#include "mmu.h"
#include "err.h"
#include "tab_pag.h"
#include "mem.h"
#include "processo.h"
#include "escalonador_proc_rapido.h"
#include "tela.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tipo de dados opaco para representar o controlador de memória
struct mmu_t {
  mem_t *mem;          // a memória física
  tab_pag_t *tab_pag;  // a tabela de páginas
  int num_quadros;
  bool* mem_bitmap; //bit map dos quadro da memória principal
  int ultimo_endereco; // o último endereço virtual traduzido pela MMU
};

// atualiza a página da memória secundária de acordo com a mem principa
static err_t att_mem_sec(mmu_t* self, int pagina, mem_t* mem_sec);
// transfere uma página da memória secundária para a memória principal 
static err_t transf_pagina(mmu_t* self, int pagina);

mmu_t *mmu_cria(mem_t *mem)
{
  mmu_t *self;
  int tam_mem = mem_tam(mem);
  int num_quadros = tam_mem / TAM_QUADRO + (tam_mem % TAM_QUADRO == 0 ? 0 : 1);

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
static err_t traduz_endereco(mmu_t *self, int end_v, int *end_f,
                             int *ppag, int *pdesl, int *pquadro)
{
  self->ultimo_endereco = end_v;
  // se não tem tabela de páginas, não traduz
  if (self->tab_pag == NULL) {   
    *end_f = end_v;
    return ERR_OK;
  }
  return tab_pag_traduz(self->tab_pag, end_v, end_f, ppag, pdesl, pquadro);
}

static err_t att_mem_sec(mmu_t* self, int pagina, mem_t* mem_sec)
{
  err_t err = ERR_OK;
  // lembrando que na mem virtual pag_num = quadro_num
  int inicio = pagina * TAM_QUADRO;
  int fim = inicio + TAM_PAG;
  int val = 0;

  for (int i = inicio; i < fim; i++) {
    err = mmu_le(self, i, &val);
    if (err != ERR_OK) {
      t_printf("mmu.att_mem_sec: Erro ao ler da mem principal");
      return err;
    }
    err = mem_escreve(mem_sec, i, val);
    if (err != ERR_OK) {
      t_printf("mmu.transf_pag: Erro ao escrever na mem virtual");
      return err;
    }
  }
  return err;
}

static err_t transf_pagina(mmu_t* self, int pagina)
{
  mem_t* mem_sec = tab_pag_mem_sec(self->tab_pag);
  err_t err = ERR_OK;
  // lembrando que na mem virtual pag_num = quadro_num
  int inicio = pagina * TAM_QUADRO;
  int fim = inicio + TAM_PAG;
  int val = 0;

  for (int i = inicio; i < fim; i++) {
    err = mem_le(mem_sec, i, &val);
    if (err != ERR_OK) {
      t_printf("mmu.transf_pag: Erro ao ler memoria do processo");
      return err;
    }
    err = mmu_escreve(self, i, val);
    if (err != ERR_OK) {
      t_printf("mmu.transf_pag: Erro ao escrever na mem principal");
      return err;
    }
  }

  return err;
}

err_t mmu_swap_in(mmu_t* self, int pagina, int id_quadro)
{
  err_t err = ERR_OK;
  tab_pag_muda_valida(self->tab_pag, pagina, true);
  tab_pag_muda_acessada(self->tab_pag, pagina, false);
  tab_pag_muda_alterada(self->tab_pag, pagina, false);

  mmu_ocupa_quadro(self, id_quadro);
  tab_pag_muda_quadro(self->tab_pag, pagina, id_quadro);
  err = transf_pagina(self, pagina);

  return err;
}

err_t mmu_swap_out(mmu_t *self, int pagina, tab_pag_t* tab)
{
  err_t err = ERR_OK;
  tab_pag_t* tab_atual = mmu_tab_pag(self);
  mmu_usa_tab_pag(self, tab);

  if(tab_pag_alterada(mmu_tab_pag(self), pagina)) {
    err = att_mem_sec(self, pagina, tab_pag_mem_sec(self->tab_pag));
    tab_pag_muda_valida(self->tab_pag, pagina, false);
    tab_pag_muda_alterada(self->tab_pag, pagina, false);
  }

  mmu_usa_tab_pag(self, tab_atual);
  return err;
}

err_t mmu_le(mmu_t *self, int endereco, int *pvalor)
{
  int end_fis;
  int pagina;
  err_t err = traduz_endereco(self, endereco, &end_fis, &pagina, NULL, NULL);
  if (err != ERR_OK) {
    return err;
  }
  tab_pag_muda_acessada(self->tab_pag, pagina, true);
  return mem_le(self->mem, end_fis, pvalor);
}

err_t mmu_escreve(mmu_t *self, int endereco, int valor)
{
  int end_fis;
  int pagina;
  err_t err = traduz_endereco(self, endereco, &end_fis, &pagina, NULL, NULL);  
  if (err != ERR_OK) {
    return err;
  }
  tab_pag_muda_acessada(self->tab_pag, pagina, true);
  tab_pag_muda_alterada(self->tab_pag, pagina, true);
  return mem_escreve(self->mem, end_fis, valor);
}

int mmu_proxQuadro_livre(mmu_t *self)
{
  for (int i = 0; i < self->num_quadros; i++) {
    if(self->mem_bitmap[i]) return i;
  }

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

tab_pag_t* mmu_tab_pag(mmu_t* self)
{
  return self->tab_pag;
}