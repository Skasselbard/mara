//
// Created by tom on 20.11.16.
//

#include "../include/Space.h"
#include "../include/CodeBlock.h"
#include "../include/Logger.h"

byte *Space::getLeftMostEnd() const {
    return (byte*)this;
}

byte *Space::getRightMostEnd() const {
    size_t memoryBlockSize = CodeBlock::readFromLeft(getLeftMostEnd());
    size_t codeBlockSize = CodeBlock::getBlockSize(getLeftMostEnd());
    return ((getLeftMostEnd()+(2*codeBlockSize)+memoryBlockSize)-1);
}

size_t Space::getSize() {
    return (getRightMostEnd() - getLeftMostEnd()) + 1;
}

bool Space::copyCodeBlockToEnd(byte *startOfBlock, size_t sizeOfBlock) {
    byte* currentPosition = (getRightMostEnd()-sizeOfBlock)+1;
    for (int i = 0; i < sizeOfBlock; i++){
        if(currentPosition <= getRightMostEnd()) {
            *currentPosition = *(startOfBlock + i);
        }else{
            Logger::error("trying to write over the space boundary");
            return false;
        }
        currentPosition++;
    }
    return true;
}

void Space::toOccupied(size_t newSize) {
    CodeBlock::setFree(getLeftMostEnd(), false);
    size_t codeBlockSize = 0;
    CodeBlock::getCodeBlockForPayloadSize(getLeftMostEnd(), newSize, codeBlockSize, false);
    copyCodeBlockToEnd(getLeftMostEnd(), codeBlockSize);
}

void *Space::getStartOfSpace() {
    size_t codeBlockSize = CodeBlock::getBlockSize(getLeftMostEnd());
    return (getLeftMostEnd()+codeBlockSize);
}

bool Space::copyCodeBlockToFront(byte *startOfBlock, size_t sizeOfBlock) {
    byte* currentPosition = getLeftMostEnd();
    for (int i = 0; i < sizeOfBlock; i++){
        currentPosition[i] = startOfBlock[i];
        currentPosition ++;
    }
    return true;
}

Space *Space::getLeftNeighbor(byte *lastByte) {
    byte* leftByte = nullptr;
    size_t memorySize = CodeBlock::readFromRight(lastByte,leftByte);
    size_t codeBlockSize = CodeBlock::getNeededCodeBlockSize(memorySize);
    return (Space*)((leftByte-memorySize)-codeBlockSize);
}
