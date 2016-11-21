//
// Created by tom on 15.11.16.
//

#include <malloc.h>
#include "../include/FreeSpace.h"
#include "../include/CodeBlock.h"
#include "../include/Logger.h"

bool FreeSpace::isFreeMemory() {
    return true;
}

bool FreeSpace::isOccupiedMemory() {
    return false;
}

FreeSpace *FreeSpace::resize(byte *firstByte) {
    leftMostEnd = firstByte;
    size_t codeBlockSize = 0;
    if (firstByte == CodeBlock::getCodeBlock(firstByte, rightMostEnd-leftMostEnd, codeBlockSize)){
        copyCodeBlocAtEnd(firstByte,codeBlockSize);
        copyNextPointerFromEndToFront(
                (uint32_t*)(leftMostEnd+codeBlockSize),
                (uint32_t*)((rightMostEnd-(codeBlockSize-1))- sizeof(uint32_t))//uint32_t is 4 byte in contrast to the one byte rightMostEnd pointer
                );
        return (FreeSpace*)leftMostEnd;
    }else{
        Logger::error("Unable to build CodeBlock");
        return nullptr;
    }
    return nullptr;
}

bool FreeSpace::copyNextPointerFromEndToFront(uint32_t *front, uint32_t *end) {
    *front = *end;
    return true;
}
