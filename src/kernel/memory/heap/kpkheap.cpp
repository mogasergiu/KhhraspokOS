#include <kpkheap.hpp>
#include <kstring.hpp>

using namespace KPKHEAP;

static chunk *bucket[KPKBUCKET_SIZE];
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

static inline void popChunk(chunk **bucket, chunk *it) {
    if (it->link.prev == NULL) {
        *bucket = it->link.next;

    } else if (it->link.next == NULL) {
        it->link.prev->link.next = NULL;

    } else {
        it->link.prev->link.next = it->link.next;
        it->link.next->link.prev = it->link.prev;
    }
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

static inline void consolidate(chunk *l, chunk *r) {
    l->metadata.size <<= 1;
}

static void pushChunk(chunk *c) {
    uint8_t bucketIdx = (uint8_t)msb(c->metadata.size) - 4;
    chunk *it = bucket[bucketIdx];

    if (it == NULL) {
        bucket[bucketIdx] = c;
        return;
    }

    while (it != NULL && it->link.next) {
        if (it == chunkEaddr(c)) {
            consolidate(c, it);
            popChunk(&bucket[bucketIdx], it);
            pushChunk(c);
            return;

        } else if (c == chunkEaddr(it)) {
            consolidate(it, c);
            popChunk(&bucket[bucketIdx], it);
            pushChunk(it);
            return;
        }

        it = it->link.next;
    }

    it->link.next = c;
    c->link.prev = it;
}

static chunk* makeChunk(void *addr, size_t size) {
    if (addr >= (void*)KPKHEAP_END) {
        return NULL;
    }

    chunk *it = (chunk*)addr;
    it->metadata.free = 1;
    it->metadata.size = size;

    it->link.next = it->link.prev = NULL;

    return it;
}

static inline void fragmentChunk(chunk *it, size_t size) {
    if (it->metadata.size > size) {
        pushChunk(makeChunk(it, (size >> 1)));
    }
}

static chunk* checkBucket(size_t size) {
    uint8_t bucketIdx = size;
    if (size > MAX_CHUNK_SIZE) {
        bucketIdx = KPKBUCKET_SIZE - 1;
    }

    bucketIdx = msb(size) - 4;

    chunk *it = bucket[bucketIdx];
    while (it != NULL && it->metadata.size < size) {
        it = it->link.next;
    }

    if (it != NULL) {
        if (it->metadata.size == size) {
            popChunk(&bucket[bucketIdx], it);
            it->metadata.free = 0;
            return it;

        } else {
            fragmentChunk(it, size);
            popChunk(&bucket[bucketIdx], it);
            return it;
        }
    }

    return NULL;
}

extern "C" void* kpkMalloc(size_t size) {
    size = alignSize(size);

    chunk *ret = checkBucket(size);

    if (ret == NULL) {
        ret = makeChunk(topChunk, size);

        if (ret != NULL) {
            KPKHEAP::topChunk = (void*)((char*)topChunk + size + sizeof(ret->metadata));
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
