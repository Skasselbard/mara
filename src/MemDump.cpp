//
// Created by jg on 12/20/16.
//

#include <cstddef>
#include <cstdio>
#include <string>
#include <cstring>
#include "../include/MemDump.h"
#include "../include/CodeBlock.h"

const int MemDump::bytesPerLine = 8;


const char *byte_to_binary(int x) {
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1) {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

void MemDump::dumpFullBlock(void *left) {
    size_t cbSize = CodeBlock::getBlockSize((byte *) left);
    size_t memSize = CodeBlock::readFromLeft((byte *) left);
    dumpArray(left, 2 * cbSize + memSize);
}

void MemDump::dumpCodeBlock(void *left) {
    size_t cbSize = CodeBlock::getBlockSize((byte *) left);
    dumpArray(left, cbSize);
}

void MemDump::dumpMemoryBlock(void *left) {
    size_t cbSize = CodeBlock::getBlockSize((byte *) left);
    size_t memSize = CodeBlock::readFromLeft((byte *) left);
    byte *start = ((byte *) left) + cbSize;
    dumpArray(start, memSize);
}

void MemDump::dumpArray(void *left, size_t size) {
    for (int i = 0; i <= 8 + 1 + (2 + 8) * bytesPerLine; i++) {
        printf("-");
    }
    printf("%c", '\n');
    unsigned char *startOfLinePointer = (unsigned char *) left;
    for (int i = 0; i < size / (bytesPerLine + 1); i++) {
        printf("%p:", (void *) (startOfLinePointer + i * bytesPerLine));
        for (int j = 0; j < bytesPerLine; j++) {
            printf("  %s", byte_to_binary(*((startOfLinePointer) + i * bytesPerLine + j)));
        }
        printf("%c", '\n');
    }
    printf("%p:", (void *) (startOfLinePointer + size / (bytesPerLine + 1) * bytesPerLine));
    for (int j = 0; j < size % (bytesPerLine + 1); j++) {
        printf("  %s", byte_to_binary(*((startOfLinePointer) + size / (bytesPerLine + 1) * bytesPerLine + j)));
    }
    printf("%c", '\n');
    for (int i = 0; i <= 8 + 1 + (2 + 8) * bytesPerLine; i++) {
        printf("-");
    }
    printf("%c", '\n');
}
