//
// Created by Jack Noordhuis on 23/12/18.
//

#include "BinaryStream.h"

#include <memory>
#include <sstream>

unsigned int BinaryStream::read(byte *data, unsigned int size) {
    if(size == 0) {
        return 0;
    }

    unsigned int remaining = this->size - offset;
    if(size > remaining) {
        std::stringstream err;
        err << "Not enough bytes in buffer to read: need " << size << ", have " << remaining;
        throw std::out_of_range(err.str());
    }

    memcpy(data, &this->buffer[offset], sizeof(byte) * size);
    offset += size;

    return size;
}

void BinaryStream::write(const byte *data, unsigned int size) {
    unsigned int remaining = offset + size;
    if(size > remaining) {
        if(resizable) {
            resize(offset + size);
        } else {
            std::stringstream err;
            err << "Not enough bytes in buffer to write: need " << size << ", have " << remaining;
            throw std::out_of_range(err.str());
        }
    }

    memcpy(&this->buffer[offset], data, size);
    offset += size;
}

void BinaryStream::resize(unsigned int minimalSize) {
    if(!resizable) {
        throw std::bad_function_call();
    }

    unsigned int newSize = size;
    while(newSize < minimalSize) {
        newSize *= 2;
    }

    if(newSize == size) {
        return;
    }

    byte *newBuf = new byte[newSize];
    memcpy(&newBuf[0], &buffer[0], size);

    buffer.reset(newBuf);
    size = newSize;
}

unsigned int BinaryStream::skip(unsigned int count) {
    if(offset + count > size) {
        count = (size - offset);
    }
    offset += count;
    return count;
}

byte* BinaryStream::getBuffer(bool release) {
    byte *b = buffer.get();
    if(release) {
        buffer.reset();
    }

    return b;
}

void BinaryStream::swapBytes(byte *bytes, size_t size) {
    byte temp_bytes[size];
    memcpy(temp_bytes, bytes, size);
    for (ssize_t i = size - 1; i >= 0; i--) {
        bytes[size - 1 - i] = temp_bytes[i];
    }
}