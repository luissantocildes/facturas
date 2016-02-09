cpu         segment
            assume cs:cpu,ds:cpu,es:cpu,ss:cpu
            org    100h

inicio:     pushf
            mov    ax, 7000h
            push   ax
            popf
            pushf
            pop    ax
            popf
            and    ax, 7000h
            jz     no_386

            mov    dl, 0

            int    11h
            and    ax, 2
            jz     no_copro
            jmp    fin

no_copro:   mov    dl, 1
            jmp    fin

no_386:     mov    dl, 8

fin:        mov    ax, dx
            mov    ah, 4ch
            int    21h

cpu         ends
            end inicio