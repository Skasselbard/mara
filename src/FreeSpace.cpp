//
// Created by tom on 15.11.16.
//

#include <malloc.h>
#include "../include/FreeSpace.h"
#include "../include/CodeBlock.h"
#include "../include/Logger.h"


FreeSpace *FreeSpace::pushBeginningRight(byte *firstByte) {
    size_t codeBlockSize = 0;
    if (firstByte == CodeBlock::getCodeBlockForPayloadSize(firstByte, getRightMostEnd() - firstByte, codeBlockSize)){
        CodeBlock::setFree(firstByte, true);
        copyCodeBlockToEnd(firstByte, codeBlockSize);
        copyNextPointerFromEndToFront(
                getLeftNext(codeBlockSize),
                getRightNext(codeBlockSize)
        );
        return (FreeSpace*)firstByte;
    }else{
        Logger::error("Unable to build CodeBlock");
        return nullptr;
    }
    return nullptr;
}

FreeSpace *FreeSpace::pushEndLeft(byte *lastByte) {
    size_t codeBlockSize = 0;
    CodeBlock::getCodeBlockForPayloadSize(getLeftMostEnd(), lastByte - getLeftMostEnd(), codeBlockSize);//get the needed size
    copyCodeBlockToFront(
            CodeBlock::getCodeBlockForPayloadSize(
                    getRightMostEnd() - codeBlockSize,
                    lastByte - getLeftMostEnd(),
                    codeBlockSize
            ),
            codeBlockSize
    );
    copyNextPointerFromFrontToEnd(
            getLeftNext(codeBlockSize),
            getRightNext(codeBlockSize)
    );
    return (FreeSpace*)getLeftMostEnd();
}

bool FreeSpace::copyNextPointerFromEndToFront(uint32_t *front, uint32_t *end) {
    *front = *end;
    return true;
}

void FreeSpace::setNext(FreeSpace *next) {
    size_t codeBlockSize = CodeBlock::getBlockSize(getLeftMostEnd());
    uint32_t *leftNext = getLeftNext(codeBlockSize);
    uint32_t *rightNext = getRightNext(codeBlockSize);
    if (next == nullptr){
        *leftNext = 0;
        *rightNext = 0;
        return;
    }
    uint32_t offset = (uint32_t) ((byte *) next - getRightMostEnd());
    *leftNext = offset;
    *rightNext = offset;
}

uint32_t *FreeSpace::getLeftNext(size_t codeBlockSize) {
    return (uint32_t *) (getLeftMostEnd() + codeBlockSize);
}

uint32_t *FreeSpace::getRightNext(size_t codeBlockSize) {
    return (uint32_t*)((getRightMostEnd()-(codeBlockSize-1))- sizeof(uint32_t));//uint32_t is 4 byte in contrast to the one byte rightMostEnd pointer
}

FreeSpace *FreeSpace::getNext() {
    size_t codeBlockSize = CodeBlock::getBlockSize(getLeftMostEnd());
    uint32_t *leftNext = getLeftNext(codeBlockSize);
    if (*leftNext == 0){
        return nullptr;
    }
    return (FreeSpace*)(getRightMostEnd()+(*leftNext));
}

bool FreeSpace::copyNextPointerFromFrontToEnd(uint32_t *front, uint32_t *end) {
    *end = *front;
    return true;
}


