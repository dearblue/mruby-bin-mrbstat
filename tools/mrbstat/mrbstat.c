#include <mruby.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <execinfo.h>

struct mstat
{
    size_t malloc;
    size_t realloc;
    size_t free;
    size_t totalsize;
    size_t deltasize; /* realloc する毎の差分の合計 */
    size_t absdelta; /* realloc する毎の差分の絶対値 */
    size_t chunks[32]; /* 使用中となるメモリサイズの2の対数を切り上げたもの */
};

struct mwrap
{
    union {
        size_t size;
        const char padding[32];
    };
};

int
ceil_log2(uint64_t n)
{
    union { uint32_t u32; float f; } d;
    uint64_t w = n - 1;

    w |= w >> 32;
    w |= w >> 16;
    w |= w >> 8;
    w |= w >> 4;
    w |= w >> 2;
    w |= w >> 1;
    d.f = w + 0.5f;

    return (d.u32 >> (FLT_MANT_DIG - 1)) + (FLT_MIN_EXP - 1);
}

void *
mstat_alloc(mrb_state *mrb, void *addr, size_t size, void *user)
{
    struct mstat *mstat = (struct mstat *)user;

    if (size == 0) {
        mstat->free ++;

        if (addr) {
            struct mwrap *m = (struct mwrap *)addr - 1;
            mstat->totalsize -= m->size;
            mstat->chunks[ceil_log2(m->size)] --;
            free(m);
        }

        return NULL;
    }

    if (size > 4096) {
#if 0
        fflush(stdout);
        const size_t max_depth = 100;
        size_t stack_depth;
        void *stack_addrs[max_depth];
        char **stack_strings;

        stack_depth = backtrace(stack_addrs, max_depth);
        stack_strings = backtrace_symbols(stack_addrs, stack_depth);

        printf("Call stack about %p+%zd:\n", addr, size);

        for (size_t i = 0; i < stack_depth; i++) {
            printf("    %s\n", stack_strings[i]);
        }
        free(stack_strings); // malloc()ed by backtrace_symbols
#endif
    }

    if (addr) {
        struct mwrap *m = (struct mwrap *)addr - 1;
        ssize_t delta = size - m->size;

        mstat->realloc ++;
        mstat->totalsize -= m->size;
        mstat->chunks[ceil_log2(m->size)] --;
        addr = realloc(m, sizeof(struct mwrap) + size);
        if (addr) {
            m = (struct mwrap *)addr;
            m->size = size;
            mstat->totalsize += size;
            mstat->deltasize += delta;
            mstat->absdelta += (delta < 0 ? -delta : delta);
            mstat->chunks[ceil_log2(size)] ++;
            return (void *)(m + 1);
        } else {
            mstat->totalsize += m->size;
            mstat->chunks[ceil_log2(m->size)] ++;
            return NULL;
        }
    } else {
        mstat->malloc ++;
        addr = malloc(sizeof(struct mwrap) + size);
        if (!addr) { return NULL; }
        struct mwrap *m = (struct mwrap *)addr;
        m->size = size;
        mstat->totalsize += size;
        mstat->chunks[ceil_log2(size)] ++;
        return (void *)(m + 1);
    }
}

double
fold_with_unit(double n, const char **unit)
{
    static const char *units[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB" };
    const char **unp;

    for (unp = units; unp < units + (sizeof(units) / sizeof(units[0]) - 1); unp ++) {
        if (n < 1024) { break; }
        n /= 1024.0;
    }

    *unit = *unp;

    return n;
}

void
print_mstat(const struct mstat *m, const char mesg[])
{
    if (mesg) {
        printf("%s%s\n", "memory stat - ", mesg);
    } else {
        printf("%s\n", "memory stat");
    }

    printf("\tcount of malloc:   %8d times (realloc: %d times)\n", (int)m->malloc, (int)m->realloc);
    printf("\tcount of free:     %8d times\n", (int)m->free);
    printf("\tallocated memory:%10d bytes\n", (int)m->totalsize);
    printf("\trealloc delta:   %10d bytes (absolute total: %d bytes)\n", (int)m->deltasize, (int)m->absdelta);
    puts("\tallocated chunks (grouped by log2):");

    for (unsigned int i = 0; i < 32; i ++) {
        if (i % 4 == 0) {
            const char *unit;
            double n = fold_with_unit(1u << i, &unit);
            printf("\t%6d %-3s |", (int)n, unit);
        }

        printf(" %6d", (int)m->chunks[i]);

        if (i % 4 == 3) {
            puts("");
        }
    }
}

struct symbol_entry
{
    int flags;
    const char *name;
};

void
print_mruby_stat(mrb_state *mrb)
{
    printf("\truntime symbols:   %8d symbols\n", (int)mrb->symidx);
#ifdef MRB_BUILTIN_SYMBOLS_MAX
    printf("\tbuiltin symbols:   %8d symbols\n", MRB_BUILTIN_SYMBOLS_MAX);
#endif
    printf("\tsymbol capacity:   %8d symbols (+1) (sizeof mrb->symtbl: %d bytes)\n",
            (int)mrb->symcapa, (int)(sizeof(struct symbol_entry[mrb->symcapa + 1])));
}

int
main(int argc, char *argv[])
{
    struct mstat mstat = { 0 };
    mrb_state *mrb = mrb_open_allocf(mstat_alloc, (void *)&mstat);

    print_mstat(&mstat, "after mrb_open_allocf()");
    print_mruby_stat(mrb);

    mrb_full_gc(mrb);

    print_mstat(&mstat, "after mrb_full_gc()");
    print_mruby_stat(mrb);

    mrb_close(mrb);

    return 0;
}
