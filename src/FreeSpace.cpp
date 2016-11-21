//
// Created by tom on 15.11.16.
//

#include <malloc.h>
#include "../include/FreeSpace.h"
#include "../include/CodeBlock.h"

bool FreeSpace::isFreeMemory() {
    return true;
}

bool FreeSpace::isOccupiedMemory() {
    return false;
}

FreeSpace *FreeSpace::resize(byte *firstByte) {

    leftMostEnd = firstByte;
    size_t codeBlockSize = 0;
    byte* codeBlockStart = CodeBlock::getCodeBlock(rightMostEnd-leftMostEnd, codeBlockSize);


    //free(codeBlockStart); //TODO: free memory?
    return nullptr;
}
