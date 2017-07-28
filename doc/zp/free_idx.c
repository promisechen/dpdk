#include <stdio.h>

size_t get_index(size_t size)
{
#define MINSIZE_LOG2    8
#define LOG2_INCREMENT  2

    size_t log2, idx = 0;

    if(size <= (1UL << MINSIZE_LOG2))
        goto RET;

    log2 = sizeof(size) * 8 - __builtin_clzl(size-1);
    printf("size %lu, log2: %lu, ", size, log2);

    idx = (log2 - MINSIZE_LOG2 + LOG2_INCREMENT - 1) /
        LOG2_INCREMENT;

RET:
    idx = (idx <= (13-1) ? idx : (13-1));
    printf("idx: %lu\n", idx);

    return idx;
}

int main(int argc, char** argv)
{
    size_t n;

    for(n=7; n<33; n++)
    {
        get_index((1UL << n));
        get_index((1UL << n) - 2);
    }

    return 0;
}
