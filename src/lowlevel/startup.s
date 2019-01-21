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

! EDITED FOR BREVITY

    ! This MUST always be run from a non-cacheable area of memory, since it
    ! messes with the CCR register.
    .globl      _boot_stub
_boot_stub:
    ! Set up some registers we will need to deal with later...
    mov.l       newr15, r15
    mov.l       newgbr, r0
    mov.l       ccr_addr, r3
    ldc         r0, gbr
    mov.l       newvbr, r1
    mov.w       ccr_data, r2
    mov.l       newsr, r4
    ldc         r1, vbr
    mov.l       newpc, r14
    mov.l       newfpscr, r5
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
    mov.l       startaddr, r12
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
ccr_addr:
    .long       0xff00001c
newr15:
    .long       0x8c00f400          ! r15
newgbr:
    .long       0x8c000000          ! gbr
newvbr:
    .long       0x8c00f400          ! vbr
newsr:
    .long       0x700000f0          ! sr
newpc:
    .long       0xac00b800          ! pc
newfpscr:
    .long       0x00040001          ! fpscr
startaddr:
    .long       start
p2mask:
    .long       0xa0000000
ccr_data:
    .word       0x0909

    .globl      _boot_stub_len
    .balign     4
_boot_stub_len:
    .long       _boot_stub_len - _boot_stub
