
#include <kos.h>

extern void clear_and_load(void *) __attribute__((noreturn));

void runit() __attribute__((noreturn));
void runit(){ 
        /* The binary is in place, so let's try to boot it, shall we? */
        void (*f)(void) __attribute__((noreturn));
        f = (void *)(&clear_and_load);
        f();
}
