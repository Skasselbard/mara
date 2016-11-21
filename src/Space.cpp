//
// Created by tom on 20.11.16.
//

#include "../include/Space.h"
#include "../include/CodeBlock.h"
#include "../include/Logger.h"

byte *Space::getLeftMostEnd() const {
    return leftMostEnd;
}

byte *Space::getRightMostEnd() const {
    return rightMostEnd;
}

size_t Space::computeCodeBlockSize(size_t sizeOfBlockOfInterestInByte) {
    size_t codeBlockSize = 0;
    byte codeBlock[MAX_CODEBLOCK_ARRAY_SIZE];
    CodeBlock::getCodeBlock(&codeBlock[0], sizeOfBlockOfInterestInByte, codeBlockSize);
    return codeBlockSize;
}

size_t Space::getSize() {
    return rightMostEnd - leftMostEnd;
}

bool Space::copyCodeBlocAtEnd(byte *startOfBlock, size_t sizeOfBlock) {
    byte* currentPosition = rightMostEnd-sizeOfBlock;
    for (int i = 0; i < sizeOfBlock; i++){
        if(currentPosition <= rightMostEnd) {
            *currentPosition = *(startOfBlock + i);
        }else{
            Logger::error("trying to write over the space boundary");
            return false;
        }
    }
    return true;
}
