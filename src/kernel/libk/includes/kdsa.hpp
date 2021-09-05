#include <stdint.h>
#include <stddef.h>

#ifndef DATA_STRUCTURES_HPP
#define DATA_STRUCTURES_HPP

class BitMap {
    private:
        size_t length;
        uint8_t* buffer;

    public:
        void initBitMap(size_t len);

        bool get(size_t idx);

        bool set(size_t index, bool value);

        size_t getSize();
};

#endif  /* DATA_STRUCTURES_HPP */
