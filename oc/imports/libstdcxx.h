#ifndef IMPORTS_LIBSTDCXX_H
#define IMPORTS_LIBSTDCXX_H

void *cxa_allocate_exception(unsigned long thrown_size);
int cxa_begin_catch();
int cxa_end_catch();
int cxa_guard_abort();
int cxa_guard_acquire();
int cxa_guard_release();
int cxa_pure_virtual();
int cxa_rethrow();
__attribute__((noreturn))
void cxa_throw(void *thrown_exception, void *tinfo, void (*dest)(void *));
int gxx_personality_v0();

#endif
