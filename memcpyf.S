.global memcpyasm
memcpyasm:
        push {r4, r5, r6, r7}
.loop:  cmp r2, #0
        beq .done
        subs r2, #16
        ldmia   r1!, {r4, r5, r6, r7}
        stmia   r0!, {r4, r5, r6, r7}
        b .loop
.done:  pop {r3, r4, r5, r6}
        bx lr
