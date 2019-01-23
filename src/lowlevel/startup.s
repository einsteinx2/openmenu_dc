!   This file is part of Sylverant PSO Patcher
!   Copyright (C) 2013 Lawrence Sebald
!
    .globl      _clear_and_load
    .text
_clear_and_load:
    ! Disable interrupts
    mov.l       .no_ints_sr, r0
    ldc         r0, sr
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
.bin_base:
    ! This is actually the first stage of the IP.BIN file.
    .long       0xAC00B800
