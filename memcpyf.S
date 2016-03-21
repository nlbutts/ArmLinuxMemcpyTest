.global memcpyasm
memcpyasm:
        push {r3, r4, r5, r6, r7, r8, r9, r10}
.loop:  ldmia   r1!, {r3, r4, r5, r6, r7, r8, r9, r10}
        stmia   r0!, {r3, r4, r5, r6, r7, r8, r9, r10}
        subs r2, #32
        bne .loop
.done:  pop {r3, r4, r5, r6, r7, r8, r9, r10}
        bx lr
