#include "cpu_estado.h"
#include <stdlib.h>
#include <stdio.h>

struct cpu_estado_t {
  int PC;
  int A;
  int X;
  err_t erro;
  int complemento;
  cpu_modo_t modo;
  int tempo_parado; // em num de instruc
  int ultima_leitura_rel;
};

cpu_estado_t *cpue_cria(void)
{
  cpu_estado_t *self;
  self = malloc(sizeof(*self));
  if (self != NULL) {
    self->PC = 0;
    self->A = 0;
    self->X = 0;
    self->erro = ERR_OK;
    self->complemento = 0;
    self->modo = supervisor;
    self->tempo_parado = 0;
  }
  return self;
}

void cpue_destroi(cpu_estado_t *self)
{
  free(self);
}

void cpue_copia(cpu_estado_t *self, cpu_estado_t *outro)
{
  *outro = *self;
}


int cpue_PC(cpu_estado_t *self)
{
  return self->PC;
}

int cpue_A(cpu_estado_t *self)
{
  return self->A;
}

int cpue_X(cpu_estado_t *self)
{
  return self->X;
}

err_t cpue_erro(cpu_estado_t *self)
{
  return self->erro;
}

int cpue_complemento(cpu_estado_t *self)
{
  return self->complemento;
}

cpu_modo_t cpue_modo(cpu_estado_t *self)
{
  return self->modo;
}

void cpue_muda_PC(cpu_estado_t *self, int val)
{
  self->PC = val;
}

void cpue_muda_A(cpu_estado_t *self, int val)
{
  self->A = val;
}

void cpue_muda_X(cpu_estado_t *self, int val)
{
  self->X = val;
}

void cpue_muda_erro(cpu_estado_t *self, err_t err, int complemento)
{
  self->erro = err;
  self->complemento = complemento;
}

void cpue_muda_modo(cpu_estado_t *self, cpu_modo_t modo, rel_t *rel)
{
  if (self->modo != zumbi && modo == zumbi) {
    self->ultima_leitura_rel = rel_agora(rel);
  }

  if (self->modo == zumbi && modo != zumbi) {
    self->tempo_parado += rel_agora(rel) - self->ultima_leitura_rel;
  }
  
  self->modo = modo;
}

int cpu_tempo_total(cpu_estado_t *self, int so_tempo_total)
{
  return so_tempo_total - self->tempo_parado;
}
