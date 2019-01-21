!   This file is part of Sylverant PSO Patcher
!   Copyright (C) 2013 Lawrence Sebald
!
!   This program is free software: you can redistribute it and/or modify
!   it under the terms of the GNU General Public License version 3 as
!   published by the Free Software Foundation.
!
!   This program is distributed in the hope that it will be useful,
!   but WITHOUT ANY WARRANTY; without even the implied warranty of
!   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!   GNU General Public License for more details.
!
!   You should have received a copy of the GNU General Public License
!   along with this program.  If not, see <http://www.gnu.org/licenses/>.

!   The startup code was adapted from Marcus' startup code in all of his old
!   DC examples. It shouldn't be too hard to see what is new in here.
    .globl      __start
    .text
__start:
    ! First, make sure to run in the P2 area
    mov.l       _setup_cache_addr, r0
    mov.l       _p2_mask, r1
    or          r1, r0
    jmp         @r0
    nop
__setup_cache:
    ! Now that we are in P2, it's safe to enable the cache
    mov.l       _ccr_addr, r0
    mov.w       _ccr_data, r1
    mov.l       r1, @r0
    ! After changing CCR, eight instructions must be executed before it's safe
    ! to enter a cached area such as P1
    mov.l       _initaddr, r0        ! 1
    mov         #0, r1              ! 2
    nop                             ! 3
    nop                             ! 4
    nop                             ! 5
    nop                             ! 6
    nop                             ! 7
    nop                             ! 8
    jmp         @r0                 ! go
    mov         r1, r0
_init:
    ! Clear BSS section
    mov.l       _bss_start_addr, r0
    mov.l       _bss_end_addr, r2
    sub         r0, r2
    shlr2       r2
    mov         #0, r1
.loop:
    dt          r2
    mov.l       r1, @r0
    bf/s        .loop
    add         #4, r0
    mov         #0, r2
    mov         #0, r1
    mov.l       _mainaddr, r0
    mov.l       _stack_ptr, r15
    jmp         @r0
    mov         #0, r0

    .align      2
_mainaddr:
    .long       _main
_initaddr:
    .long       _init
_bss_start_addr:
    .long       __bss_start
_bss_end_addr:
    .long       _end
_setup_cache_addr:
    .long       __setup_cache
_p2_mask:
    .long       0xa0000000
_stack_ptr:
    .long       0x8d000000

    ! This MUST always be run from a non-cacheable area of memory, since it
    ! messes with the CCR register.
    .globl      _boot_stub
_boot_stub:
    ! Set up some registers we will need to deal with later...
    mov.l       _newr15, r15
    mov.l       _newgbr, r0
    mov.l       _ccr_addr, r3
    ldc         r0, gbr
    mov.l       _newvbr, r1
    mov.w       _ccr_data, r2
    mov.l       _newsr, r4
    ldc         r1, vbr
    mov.l       _newpc, r14
    mov.l       _newfpscr, r5
    ldc         r4, sr
    lds         r5, fpscr
    mov.l       r2, @r3             ! Set the CCR to clear the caches and such.
    ! Now, we need to execute a few instructions before we can go back to any
    ! cached areas, like P1. Of course, we actually are not going to jump to P1
    ! at the end of this anyway, so it really does not matter all that much...
    mov         #0, r0              ! 1
    mov         #0, r1              ! 2
    mov         #0, r2              ! 3
    mov         #0, r3              ! 4
    mov         #0, r4              ! 5
    mov         #0, r6              ! 7
    mov         #0, r8              ! 8 (safe to go back to P1 now)
    mov.l       _startaddr, r12
    mov         #0, r9
    lds         r12, pr
    mov         #0, r10
    mov         #0, r11
    mov         #0, r12
    mov         #0, r13
    clrmac
    clrt
    jmp         @r14                ! Go!
    mov         #0, r14
    ! We should not ever get back here.

    .balign     4
_ccr_addr:
    .long       0xff00001c
_newr15:
    .long       0x8c00f400          ! r15
_newgbr:
    .long       0x8c000000          ! gbr
_newvbr:
    .long       0x8c00f400          ! vbr
_newsr:
    .long       0x700000f0          ! sr
_newpc:
    .long       0xac00b800          ! pc
_newfpscr:
    .long       0x00040001          ! fpscr
_startaddr:
    .long       __start
_p2mask:
    .long       0xa0000000
_ccr_data:
    .word       0x0909

    .globl     __boot_stub_len
    .balign     4
_boot_stub_len:
    .long       _boot_stub_len - _boot_stub
