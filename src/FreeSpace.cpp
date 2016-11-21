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
                getLeftNext(codeBlockSize),
                getRightNext(codeBlockSize-1)
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

void FreeSpace::setNext(FreeSpace *next) {
    size_t codeBlockSize = 0;
    if (getLeftMostEnd() == CodeBlock::getCodeBlock(getLeftMostEnd(), getRightMostEnd()-getLeftMostEnd(), codeBlockSize)) {
        uint32_t offset = (uint32_t) ((byte *) next - getRightMostEnd());
        uint32_t *leftNext = getLeftNext(codeBlockSize);
        uint32_t *rightNext = getRightNext(codeBlockSize);
        *leftNext = offset;
        *rightNext = offset;
    }else{
        Logger::error("Unable to build CodeBlock");
    }
}

uint32_t *FreeSpace::getLeftNext(size_t codeBlockSize) {
    return (uint32_t *) (getLeftMostEnd() + codeBlockSize);
}

uint32_t *FreeSpace::getRightNext(size_t codeBlockSize) {
    return (uint32_t*)((getRightMostEnd()-(codeBlockSize-1))- sizeof(uint32_t));//uint32_t is 4 byte in contrast to the one byte rightMostEnd pointer
}

FreeSpace *FreeSpace::getNext() {
    size_t codeBlockSize = 0;
    if (getLeftMostEnd() == CodeBlock::getCodeBlock(getLeftMostEnd(), getRightMostEnd()-getLeftMostEnd(), codeBlockSize)) {
        return (FreeSpace*)(getRightMostEnd()+(*getLeftNext(codeBlockSize)));
    }else{
        Logger::error("Unable to build CodeBlock");
        return nullptr;
    }
}
