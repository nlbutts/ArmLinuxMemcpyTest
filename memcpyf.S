.global memcpyasm
memcpyasm:
        push {r4, r5, r6, r7}
.loop:  ldmia   r1!, {r4, r5, r6, r7}
        stmia   r0!, {r4, r5, r6, r7}
        subs r2, #16
        bne .loop
.done:  pop {r3, r4, r5, r6}
        bx lr
