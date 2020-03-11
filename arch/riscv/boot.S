.section .text

.global start
start:
        csrw sie, zero
        csrw sip, zero
        la sp, init_stack_top
        call start_kernel
        j halt

halt:
        wfi
        j halt

.bss
.align 16
init_stack_bottom = .
. = . + 4096
init_stack_top = .
