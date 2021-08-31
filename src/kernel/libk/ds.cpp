#include <ds.hpp>
#include <kpkheap.hpp>

void BitMap::initBitMap(size_t len) {
    this->length = len;

    len >>= 3;

    this->buffer = (uint8_t*)KPKHEAP::kpkZalloc(len);
}

bool BitMap::get(size_t idx) {
    uint8_t bitIdx = 0b10000000 >> (idx % 8);

    if ((this->buffer[(idx / 8)] & bitIdx) > 0) {
        return true;
    }

    return false;
}

bool BitMap::set(size_t idx, bool val) {
    if (idx > this->length * 8) {
        return false;
    }

    uint8_t bitIdx = 0b10000000 >> (idx % 8);

    this->buffer[(idx / 8)] &= ~bitIdx;

    if (val) {
        this->buffer[(idx / 8)] |= bitIdx;
    }

    return true;
}

size_t BitMap::getSize() {
    return this->length;
}
