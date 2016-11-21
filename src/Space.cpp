//
// Created by tom on 20.11.16.
//

#include "../include/Space.h"
#include "../include/CodeBlock.h"

byte *Space::getLeftMostEnd() const {
    return leftMostEnd;
}

byte *Space::getRightMostEnd() const {
    return rightMostEnd;
}

size_t Space::computeCodeBlockSize(size_t sizeOfBlockOfInterestInByte) {
    size_t codeBlockSize = 0;
    CodeBlock::getCodeBlock(sizeOfBlockOfInterestInByte, codeBlockSize);
    return codeBlockSize;
}

size_t Space::getSize() {
    return rightMostEnd - leftMostEnd;
}
