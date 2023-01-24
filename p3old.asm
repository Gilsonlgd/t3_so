;programa de exemplo
;implementação de teste
;le 10 valores aleatórios do dispositivo rand
;caso dispositivo ocupado, tenta ler dnv
;essa implementação está adequada a não utilização de bloqueio de processos.

; chamadas de sistema
SO_LE   define 1
SO_ESCR define 2
SO_FIM  define 3
; dispositivos de E/S
TELA    DEFINE 0
RAND    DEFINE 103

            ; inicializando
            cargi 0         ; A=0
            mvax            ; x=0

            ; lê rand
ledenovo 
            cargi RAND      ; A=103=num do dispositivo a ser lido
            sisop SO_LE     ; retorna A=err, X=dado
            desvnz ledenovo ; se a leitura não aconteceu, tenta ler dnv

            ; escreve rand
escdenovo   cargi TELA
            sisop SO_ESCR   ; impr X em A, retorna A=err
            desvnz escdenovo ; se der erro, tenta escrever dnv

            ;xis++
            cargm xis       ;A=xis
            mvax
            incx            ;xis++
            mvxa
            armm xis        ;armazena xis
            SUB cont
            DESVNZ ledenovo ; if xis != cont goto ali
            sisop           SO_FIM
cont valor 10       ; int cont=10
xis  valor  0       ; int xis=1