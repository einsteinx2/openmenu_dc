!   This file is part of Sylverant PSO Patcher
!   Copyright (C) 2013 Lawrence Sebald
!
    .globl      _clear_and_load
    .text
_clear_and_load:
    ! Disable interrupts
    mov.l       .no_ints_sr, r0
    ldc         r0, sr
    ! Clear out the loader
    mov.l       .loader_base, r0
    mov.l       .loader_end, r1
    mov         #0, r2
.clear_loop:
    mov.l       r2, @r0
    add         #4, r0
    cmp/eq      r0, r1
    bf          .clear_loop
    ! Clean up the various registers...
    ldc         r4, sr
    ldc         r5, vbr
    lds         r6, fpscr
    mov         r7, r15
    ! Jump to IP.BIN 
    mov.l       .bin_base, r0
    jmp         @r0
    nop
    .balign     4
.no_ints_sr:
    .long       0x500000F0
.loader_base:
    .long       0xACD00000
.loader_end:
    .long       0xAD000000
.bin_base:
    ! This is actually the first stage of the IP.BIN file.
    .long       0xAC00B800
