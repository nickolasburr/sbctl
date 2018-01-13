/**
 * mem.h
 *
 * Copyright (C) 2017 Nickolas Burr <nickolasburr@gmail.com>
 */

#ifndef SBCTL_MEM_H
#define SBCTL_MEM_H

#include "common.h"
#include "assert.h"
#include "except.h"

extern const Except_T Mem_Failed;

extern void *Mem_alloc(long nbytes, const char *file, int line);
extern void *Mem_calloc(long count, long nbytes, const char *file, int line);
extern void Mem_free(void *ptr, const char *file, int line);
extern void *Mem_resize(void *ptr, long nbytes, const char *file, int line);

#define Func_apply(type, func, ...) {                                 \
    int index;                                                        \
    void *stop_for_apply = (int[]){0};                                \
    type **list_for_apply = (type*[]){__VA_ARGS__, stop_for_apply};   \
    for (index = 0; list_for_apply[index] != stop_for_apply; index++) \
        func(list_for_apply[index]);                                  \
}

#define ALLOC(nbytes)         Mem_alloc((nbytes), __FILE__, __LINE__)
#define CALLOC(count, nbytes) Mem_calloc((count), (nbytes), __FILE__, __LINE__)
#define NEW(p)                ((p) = ALLOC((long)sizeof *(p)))
#define NEW0(p)               ((p) = CALLOC(1, (long)sizeof *(p)))
#define RESIZE(ptr, nbytes)   ((ptr) = Mem_resize((ptr), (nbytes), __FILE__, __LINE__))
#define FREE(ptr)             ((void)(Mem_free((ptr), __FILE__, __LINE__), (ptr) = 0))
#define FREE_ALL(...)         Func_apply(void, FREE, __VA_ARGS__)

#endif /* SBCTL_MEM_H */
