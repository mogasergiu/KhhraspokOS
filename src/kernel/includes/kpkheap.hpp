#include <stdint.h>
#include <stddef.h>

#ifndef KPKHEAP_HPP
#define KPKHEAP_HPP

#define KPKHEAP_START 0x200000
#define KPKHEAP_END 0x3000000
#define KPKBUCKET_SIZE 21
#define MIN_CHUNK_SIZE (1 << 4)
#define MAX_CHUNK_SIZE (1 << 23)
#define MISALIGNED_MASK 0x1111

namespace KPKHEAP {
    struct chunk {
        struct {
            uint8_t free;
            size_t size;
        } metadata;

        union {
            struct {
                chunk *next;
                chunk *prev;
            } link;
            uint8_t takenChunk[MIN_CHUNK_SIZE - sizeof(metadata)];
        };
    }__attribute__((packed));

    extern "C" void* topChunk;

    extern "C" void* kpkMalloc(size_t size);

    extern "C" void* kpkZalloc(size_t size);

    extern "C" void kpkFree(void *mem);
} 

extern void* KPKHEAP::topChunk;

#endif  /* KPKHEAP_HPP */
