#include <stdint.h>
#include <stddef.h>

#ifndef KPKHEAP_HPP
#define KPKHEAP_HPP

#define KPKHEAP_START 0x1000000
#define KPKHEAP_END 0x20000000
#define KPKHEAP_SIZE 0x1f000000  // ~ 500 MiB
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

#endif  /* KPKHEAP_HPP */
