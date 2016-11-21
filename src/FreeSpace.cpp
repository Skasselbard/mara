//
// Created by tom on 15.11.16.
//

#include <malloc.h>
#include "../include/FreeSpace.h"
#include "../include/CodeBlock.h"
#include "../include/Logger.h"


FreeSpace *FreeSpace::resize(byte *firstByte) {
    size_t codeBlockSize = 0;
    if (firstByte == CodeBlock::getCodeBlock(firstByte, getRightMostEnd()-firstByte, codeBlockSize)){
        copyCodeBlocAtEnd(firstByte,codeBlockSize);
        copyNextPointerFromEndToFront(
                (uint32_t*)(firstByte+codeBlockSize),
                (uint32_t*)((getRightMostEnd()-(codeBlockSize-1))- sizeof(uint32_t))//uint32_t is 4 byte in contrast to the one byte rightMostEnd pointer
                );
        return (FreeSpace*)firstByte;
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
