;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Detecta si el procesador es un 386 o superior, en el caso que lo sea se ;
; fija si el ordenador tiene un coprocesador matem tico o no.             ;
; (c)1996 Luc Hos Oft                                                     ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

cpu         segment
            assume cs:cpu,ds:cpu,es:cpu,ss:cpu
            org    100h   ; El c¢digo comienza en 100h, es un fichero COM

inicio:     pushf            ; Se guardan los flags en la pila
            mov    ax, 7000h
            push   ax
            popf             ; Se mete 7000h en los flags
            pushf
            pop    ax
            popf             ; Se lee el valor de los flags y se restaura el
                             ; valor original
            and    ax, 7000h
            ; Seg£n lo que he le¡do, el 386 puede poner el bit 15 de los flags
            ; a 1, cosa que el 286 e inferiores no pueden, esta es la forma de
            ; detectar si el procesador es un 386 o superior.

            jz     no_386    ; Si el valor le¡do no es 7000h, la cpu no es un
                             ; 386. No se contin£a.

            mov    dl, 0     ; por defecto se piensa que hay copro

            int    11h       ; Se verifica la presencia de un copro
            and    ax, 2     ; pidi‚ndole la informaci¢n a la BIOS
            jz     no_copro
            jmp    fin

no_copro:   mov    dl, 1     ; Si no hay copro se devuelve 1 y se sale
            jmp    fin

no_386:     mov    dl, 8     ; Si el ordenador no es un 386 se devuelve un
                             ; 8 y se sale
fin:        mov    ax, dx
            mov    ah, 4ch
            int    21h

cpu         ends
            end inicio