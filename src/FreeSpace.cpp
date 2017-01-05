//
// Created by tom on 15.11.16.
//

#include <malloc.h>
#include <assert.h>
#include "../include/FreeSpace.h"
#include "../include/CodeBlock.h"
#include "../include/Logger.h"


FreeSpace *FreeSpace::pushBeginningRight(byte *firstByte) {
    size_t codeBlockSize = 0;
    assert(firstByte<getRightMostEnd()); //Never cross the pointers!
    if (firstByte ==
            CodeBlock::getCodeBlockForInternalSize(firstByte, (getRightMostEnd() - firstByte) + 1, codeBlockSize, true)){
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
    CodeBlock::getCodeBlockForInternalSize(getLeftMostEnd(), (lastByte - getLeftMostEnd()) + 1, codeBlockSize, true);//get the needed size
    copyCodeBlockToEnd(
            getLeftMostEnd(),
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

void FreeSpace::setNext(FreeSpace *next, byte *startOfPage) {
    size_t codeBlockSize = CodeBlock::getBlockSize(getLeftMostEnd());
    uint32_t *leftNext = getLeftNext(codeBlockSize);
    uint32_t *rightNext = getRightNext(codeBlockSize);
    if (next == nullptr){
        *leftNext = 0;
        *rightNext = 0;
        return;
    }
    assert((byte*)next >= startOfPage);
    assert(((byte*) next - startOfPage) < 4294967295 );// offset is less than uint 32
    uint32_t offset = (uint32_t) ((byte*) next - startOfPage);
    *leftNext = offset;
    if (CodeBlock::readFromLeft((byte*)this)>=8){//overlapping pointers if the size is too little
        *rightNext = offset;
    }
}

uint32_t *FreeSpace::getLeftNext(size_t codeBlockSize) {
    return (uint32_t *) (getLeftMostEnd() + codeBlockSize);
}

uint32_t *FreeSpace::getRightNext(size_t codeBlockSize) {
    return (uint32_t*)((getRightMostEnd()-(codeBlockSize))- sizeof(uint32_t) + 1);//uint32_t is 4 byte in contrast to the one byte rightMostEnd pointer
}

FreeSpace *FreeSpace::getNext(byte *startOfPage) {
    size_t codeBlockSize = CodeBlock::getBlockSize(getLeftMostEnd());
    uint32_t *leftNext = getLeftNext(codeBlockSize);
    if (*leftNext == 0){
        return nullptr;
    }
    return (FreeSpace*)(startOfPage+(*leftNext));
}

bool FreeSpace::copyNextPointerFromFrontToEnd(uint32_t *front, uint32_t *end) {
    *end = *front;
    return true;
}


