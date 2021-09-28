#include <kpkheap.hpp>
#include <kstring.hpp>
#include <kstdio.hpp>
#include <kdsa.hpp>

using namespace KPKHEAP;

static Queue<chunk*> bucket[KPKBUCKET_SIZE];
void* KPKHEAP::topChunk = (void*)KPKHEAP_START;

static inline uintptr_t msb (size_t size)
{
    uintptr_t ret = 0;
    __asm__ __volatile__("bsrq %1, %0" : "=r" (ret) : "r"(size));

    return ret;
}

static inline uintptr_t lsb (size_t size)
{
    uintptr_t ret = 0;
    __asm__ __volatile__("bsfq %1, %0" : "=r" (ret) : "r"(size));

    return ret;
}

static inline size_t alignSize(size_t size) {
    uint8_t sizeMSB = (uint8_t)msb(size);
    uint8_t sizeLSB = (uint8_t)lsb(size);

    if (sizeMSB != sizeLSB && sizeLSB < 4) {
        if (size < MIN_CHUNK_SIZE) {
            return MIN_CHUNK_SIZE;
        }

        return ((size >> 4) << 5);
    }

    return size;
}

static inline chunk* chunkEaddr(chunk *it) {
    return (chunk*)((char*)it + it->metadata.size);
}

static void pushChunk(chunk *c) {
    uint8_t bucketIdx = (uint8_t)msb(c->metadata.size) - 4;
    bucket[bucketIdx].push(c);
}

static chunk* makeChunk(void *addr, size_t size) {
    if (addr >= (void*)KPKHEAP_END) {
        kpwarn("No more Kernel Heap Space!");
        return NULL;
    }

    chunk *it = (chunk*)addr;
    it->metadata.free = 1;
    it->metadata.size = size;

    it->link.next = it->link.prev = NULL;

    return it;
}

static chunk* checkBucket(size_t size) {
    uint8_t bucketIdx = size;
    if (size > MAX_CHUNK_SIZE) {
        bucketIdx = KPKBUCKET_SIZE - 1;
    }

    bucketIdx = msb(size) - 4;

    if (bucket[bucketIdx].isEmpty()) {
        return NULL;

    } else {
        return *bucket[bucketIdx].pop();
    }
}

extern "C" void* kpkMalloc(size_t size) {
    size_t __size = alignSize(size);

    chunk *ret = checkBucket(__size);

    if (ret == NULL) {
        ret = makeChunk(KPKHEAP::topChunk, __size);

        if (ret != NULL) {
            KPKHEAP::topChunk = (void*)((char*)topChunk + size + sizeof(chunk));
        }
    }

    ret->metadata.free = 0;

    return (void*)((char*)ret + sizeof(chunk));
}

extern "C" void* kpkZalloc(size_t size) {
    void *mem = kpkMalloc(size);

    if (mem == NULL) {
        return NULL;
    }

    memset(mem, 0, size);

    return mem;
}

extern "C" void kpkFree(void *mem) {
    chunk *it = (chunk*)((char*)mem - sizeof(chunk));

    if (it->metadata.free == 1) {
        return;
    }

    it->metadata.free = 1;
    pushChunk(it);
}
