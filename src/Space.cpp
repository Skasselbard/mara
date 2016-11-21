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
    size_t codeBlockSize = 0;
    CodeBlock::getCodeBlock(getLeftMostEnd(), memoryBlockSize, codeBlockSize);
    return (getLeftMostEnd()+(2*codeBlockSize)+memoryBlockSize);
}

size_t Space::computeCodeBlockSize(size_t sizeOfBlockOfInterestInByte) {
    size_t codeBlockSize = 0;
    byte codeBlock[MAX_CODEBLOCK_ARRAY_SIZE];
    CodeBlock::getCodeBlock(&codeBlock[0], sizeOfBlockOfInterestInByte, codeBlockSize);
    return codeBlockSize;
}

size_t Space::getSize() {
    return getRightMostEnd() - getLeftMostEnd();
}

bool Space::copyCodeBlocAtEnd(byte *startOfBlock, size_t sizeOfBlock) {
    byte* currentPosition = getRightMostEnd()-sizeOfBlock;
    for (int i = 0; i < sizeOfBlock; i++){
        if(currentPosition <= getRightMostEnd()) {
            *currentPosition = *(startOfBlock + i);
        }else{
            Logger::error("trying to write over the space boundary");
            return false;
        }
    }
    return true;
}

void Space::toOccupied(size_t newSize) {
    size_t codeBlockSize = 0;
    CodeBlock::getCodeBlock(getLeftMostEnd(), newSize, codeBlockSize);
    copyCodeBlocAtEnd(getLeftMostEnd(), codeBlockSize);
}

void *Space::getStartOfSpace() {
    size_t codeBlockSize = 0;
    CodeBlock::getCodeBlock(getLeftMostEnd(), this->getSize(), codeBlockSize);
    return (getLeftMostEnd()+codeBlockSize);
}
