 !try to run a thing at 0x8C010000
    .balign     2
    .globl      _run_blindly
_run_blindly:
    ! Disable interrupts
    mov.l       init_sr, r0
    ldc         r0, sr
    ! Copy the binary over to the appropriate location
    mov.l       ip_boot1, r2
    mov         r2, r4
    jmp         @r4
    nop

    .balign     4
init_sr:
    .long       0x500000F0
ip_boot1:
!    .long       0xAC00B800
    .long   0xAC010000
