
extern void boot_stub(void *, unsigned int) __attribute__((noreturn));
extern unsigned int boot_stub_len;

void runit() __attribute__((noreturn));

void runit(){ 
        /* The binary is in place, so let's try to boot it, shall we? */
        void (*f)(void) __attribute__((noreturn));
        f = (void *)((unsigned int)(&boot_stub) | 0xa0000000);
        f();
}
