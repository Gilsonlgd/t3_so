## T3 — Memória Virtual - FIFO (First In First Out)
- Esta versão do trabalho 3 corresponde a parte 2 solicitada, onde se implementa um algoritmo de substituição de página.  
- O algoritmo escolhido foi o FIFO, que retira da memória a página que está há mais tempo lá.  
- Foi utilizada uma fila para implementação. Uma página é inserida no final da fila.
- A página a ser retirada é a primeira da fila.

### Métricas:
#### Programa focado em CPU:
```
MEMÓRIA FOLGADA: 200
|Informacoes do SO:|
TempoT_exec: 1881, Tempo_CPU: 1881, Num_Interrup: 154
----------------------------------------------
|Metricas dos processos: quantum = 5|
num:6, tempo_retorno: 1878, tempo_bloq: 0,
 tempo_exec: 1348, tempo_esp: 50152, tempoM_retorno: 1114,4888916016,
 num_bloq: 0, num_preemp: 45 

num:5, tempo_retorno: 1053, tempo_bloq: 0,
 tempo_exec: 510, tempo_esp: 9810, tempoM_retorno: 577,0588378906,
 num_bloq: 0, num_preemp: 17 


MEMÓRIA PEQUENA: 30
|Informacoes do SO:|
TempoT_exec: 2778, Tempo_CPU: 2778, Num_Interrup: 1080
----------------------------------------------
|Metricas dos processos: quantum = 5|
num:6, tempo_retorno: 2775, tempo_bloq: 0,
 tempo_exec: 2009, tempo_esp: 109501, tempoM_retorno: 1634,3432617188,
 num_bloq: 0, num_preemp: 67 

num:5, tempo_retorno: 1492, tempo_bloq: 0,
 tempo_exec: 720, tempo_esp: 18875, tempoM_retorno: 786,4583129883,
 num_bloq: 0, num_preemp: 24 
```
#### Programa focado em E/S:
```
MEMÓRIA FOLGADA: 200
|Informacoes do SO:|
TempoT_exec: 949, Tempo_CPU: 949, Num_Interrup: 109
----------------------------------------------
|Metricas dos processos: quantum = 5|
num:7, tempo_retorno: 944, tempo_bloq: 0,
 tempo_exec: 477, tempo_esp: 8623, tempoM_retorno: 538,9375000000,
 num_bloq: 0, num_preemp: 16 

num:8, tempo_retorno: 861, tempo_bloq: 0,
 tempo_exec: 420, tempo_esp: 6375, tempoM_retorno: 455,3571472168,
 num_bloq: 0, num_preemp: 14 


MEMÓRIA PEQUENA: 30
|Informacoes do SO:|
TempoT_exec: 1452, Tempo_CPU: 1452, Num_Interrup: 629
----------------------------------------------
|Metricas dos processos: quantum = 5|
num:7, tempo_retorno: 1447, tempo_bloq: 0,
 tempo_exec: 719, tempo_esp: 18816, tempoM_retorno: 784,0000000000,
 num_bloq: 0, num_preemp: 24 

num:8, tempo_retorno: 1399, tempo_bloq: 0,
 tempo_exec: 690, tempo_esp: 16680, tempoM_retorno: 725,2174072266,
 num_bloq: 0, num_preemp: 23  
```

