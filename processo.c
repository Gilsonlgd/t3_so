#include <stdlib.h>
#include <stdbool.h>
#include "processo.h"
#include "mem.h"
#include "cpu_estado.h"
#include "es.h"
#include "contr.h"
#include "tela.h"

struct processo_t {
  int num;
  int disp;
  int quantum;
  int t_criacao;
  int t_finalizacao;
  int* metricas;
  acesso_t chamada_bloqueio;
  processo_estado_t estado;
  mem_t* memoria;
  cpu_estado_t* cpu_estado;
};

processo_t *processo_cria(int num, processo_estado_t estado, int agora)
{
    processo_t *self;
    self = malloc(sizeof(processo_t));
    self->num = num;
    self->estado = estado;
    self->cpu_estado = cpue_cria();
    self->disp = 0;
    self->chamada_bloqueio = 0;
    self->memoria = mem_cria(MEM_TAM);
    self->t_criacao = agora;
    self->t_finalizacao = 0;
    self->metricas = calloc(8, sizeof(int));
    self->quantum = 0;
    return self;
}

err_t processo_init_mem(processo_t *self)
{
    int num = self->num;

    switch (num)
    {
    case 1:
        int progr1[] = {
            #include "p1.maq"
        };
        int tam_progr1 = sizeof(progr1)/sizeof(progr1[0]);
        return transf_mem(self, progr1, tam_progr1);
    case 2:
        int progr2[] = {
            #include "p2.maq"
        };
        int tam_progr2 = sizeof(progr2)/sizeof(progr2[0]);
        return transf_mem(self, progr2, tam_progr2);
    case 3:
        int progr3[] = {
            #include "p3.maq"
        };
        int tam_progr3 = sizeof(progr3)/sizeof(progr3[0]);
        return transf_mem(self, progr3, tam_progr3);
    default:
        int* progr = NULL;
        return transf_mem(self, progr, 0);
    }
}

err_t transf_mem(processo_t *self, int* progr, int tam_progr)
{
  if(progr != NULL) {
    mem_t *mem = self->memoria;
    for (int i = 0; i < tam_progr; i++) {
        err_t err = mem_escreve(mem, i, progr[i]);
        if (err != ERR_OK) {
            t_printf("processo.transf_mem: erro de memória, endereco %d\n", i);
            return err;
        }
    }
    return ERR_OK;
  }

  return ERR_END_INV;
}

void processo_destroi(processo_t* self, int agora)
{
    self->t_finalizacao = agora;
    if (self != NULL) {
        if (self->metricas != NULL) {
            free(self->metricas);
        }
        free(self);
    } else {
        t_printf("Erro ao destruir processo");
    }
}

void processo_executa(processo_t* self, int agora, int quantum) {
    self->estado = em_execucao;
    self->quantum = quantum;
    self->metricas[REL_ULTIMA_EXEC] = agora;
    self->metricas[TEMPO_PRONTO] += agora - self->metricas[REL_ULTIMO_DESBLOQUEIO];
}

void processo_es_bloqueia(processo_t* self, mem_t* memoria, cpu_estado_t* cpu_estado, 
                      int disp, acesso_t chamada, int agora)
{
    self->estado = bloqueado;
    self->disp = disp;
    self->chamada_bloqueio = chamada;
    cpue_copia(cpu_estado, self->cpu_estado);
    mem_copia(self->memoria, memoria);
    self->metricas[NUM_BLOQUEIOS]++;
    self->metricas[REL_ULTIMO_BLOQUEIO] = agora;
    self->metricas[TEMPO_EXECUTANDO] += agora - self->metricas[REL_ULTIMA_EXEC];
}

void processo_preempta(processo_t* self, mem_t* memoria, cpu_estado_t* cpu_estado, int agora){
    self->estado = pronto;
    cpue_copia(cpu_estado, self->cpu_estado);
    mem_copia(self->memoria, memoria);
    self->metricas[NUM_PREEMPCOES]++;
    self->metricas[TEMPO_EXECUTANDO] += agora - self->metricas[REL_ULTIMA_EXEC];
}

void processo_desbloqueia(processo_t* self, int agora)
{
    self->estado = pronto;
    self->metricas[REL_ULTIMO_DESBLOQUEIO] = agora;
    self->metricas[TEMPO_BLOQUEADO] += agora - self->metricas[REL_ULTIMO_BLOQUEIO];
}

void processo_tik(processo_t* self)
{
    if (self->quantum >= 0) self->quantum--;
}

void processo_setQuantum(processo_t* self, int quantum)
{
    self->quantum = quantum;
}

mem_t* processo_mem(processo_t* self) {
    return self->memoria;
}

acesso_t processo_chamada(processo_t* processo)
{
    return processo->chamada_bloqueio;
}

cpu_estado_t* processo_cpu(processo_t* self) {
    return self->cpu_estado;
}

processo_estado_t processo_estado(processo_t* self){
    return self->estado;
}

int processo_num(processo_t* self) {
    return self->num;
}

int processo_disp(processo_t* processo)
{
    return processo->disp;
}

int processo_quantum(processo_t* processo)
{
    return processo->quantum;
}

float processo_tmedio_exec(processo_t* self)
{
    return (float)self->metricas[TEMPO_EXECUTANDO] / ((float)(self->metricas[NUM_PREEMPCOES] + (float)self->metricas[NUM_BLOQUEIOS]));
}

float processo_tmedio_retorno(processo_t* self)
{
    return (float)self->metricas[TEMPO_PRONTO] / ((float)(self->metricas[NUM_PREEMPCOES] + (float)self->metricas[NUM_BLOQUEIOS]));
}

int processo_t_retorno(processo_t* self)
{
    return self->t_finalizacao - self->t_criacao;
}

void processo_muda_estado(processo_t* self, processo_estado_t estado)
{
    self->estado = estado;
}

void processo_finaliza(processo_t* self, int agora)
{
    self->t_finalizacao = agora;
}

void processo_imprime_metricas(processo_t* self, FILE* arq)
{
    if(self->num != 0) {
        fprintf(arq, 
                "num:%d, tempo_retorno: %d, tempo_bloq: %d,\n "
                "tempo_exec: %d, tempo_esp: %d, tempoM_retorno: %.10f,\n "
                "num_bloq: %d, num_preemp: %d \n\n",
                self->num,
                self->t_finalizacao - self->t_criacao,
                self->metricas[TEMPO_BLOQUEADO],
                self->metricas[TEMPO_EXECUTANDO],
                self->metricas[TEMPO_PRONTO],
                processo_tmedio_retorno(self),
                self->metricas[NUM_BLOQUEIOS],
                self->metricas[NUM_PREEMPCOES]);
    }
}



