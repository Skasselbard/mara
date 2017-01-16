//
// Created by tom on 15.11.16.
//

#include <malloc.h>
#include <assert.h>
#include "../include/FreeSpace.h"
#include "../include/CodeBlock.h"
#include "../include/Logger.h"


FreeSpace *FreeSpace::pushBeginningRight(byte *firstByte) {
#ifdef PRECONDITION
    assert(firstByte > getLeftMostEnd() && firstByte < getRightMostEnd());
#endif
    size_t codeBlockSize = 0;
    assert(firstByte<getRightMostEnd()); //Never cross the pointers!
    if (firstByte ==
            CodeBlock::getCodeBlockForInternalSize(firstByte, (getRightMostEnd() - firstByte) + 1, codeBlockSize, true)){
        copyCodeBlockToEnd(firstByte, codeBlockSize);
        copyNextPointerFromEndToFront(
                getLeftNext(codeBlockSize),
                getRightNext(codeBlockSize)
        );
#ifdef POSTCONDITION
        byte* foo = nullptr;
        assert(CodeBlock::readFromLeft(firstByte) == CodeBlock::readFromRight(getRightMostEnd(), foo));
        assert(*getLeftNext(CodeBlock::getBlockSize(firstByte)) == *getRightNext(CodeBlock::getBlockSize(firstByte)));
#endif
        return (FreeSpace*)firstByte;
    }else{
        Logger::error("Unable to build CodeBlock");
#ifdef POSTCONDITION
    assert(false);
#endif
        return nullptr;
    }
}

FreeSpace *FreeSpace::pushEndLeft(byte *lastByte) {
#ifdef PRECONDITION
    assert(lastByte > getLeftMostEnd());//the new last byte must be in the old freespace
    assert(lastByte <= getRightMostEnd());//see above
#endif
    size_t codeBlockSize = CodeBlock::getBlockSize(getLeftMostEnd());
    uint32_t currentNext = *getLeftNext(codeBlockSize);//Needed incase the new CodeBlocks are smaller
    CodeBlock::getCodeBlockForInternalSize(getLeftMostEnd(), (lastByte - getLeftMostEnd()) + 1, codeBlockSize, true);//get the needed size
    copyCodeBlockToEnd(
            getLeftMostEnd(),
            codeBlockSize
    );
    uint32_t* leftNext = getLeftNext(codeBlockSize);
    *leftNext = currentNext; //Edge Case: if the code block gets smaller the nextpointer moves left
    copyNextPointerFromFrontToEnd(
            getLeftNext(codeBlockSize),
            getRightNext(codeBlockSize)
    );
#ifdef POSTCONDITION
    byte* foo = nullptr;
    //the new code blocks must have the same value
    assert(CodeBlock::readFromLeft(getLeftMostEnd()) == CodeBlock::readFromRight(getRightMostEnd(),foo));
    //the next pointers must be the same
    assert(*getLeftNext(CodeBlock::getBlockSize(getLeftMostEnd())) == *getRightNext(CodeBlock::getBlockSize(getLeftMostEnd())));
#endif
    return (FreeSpace*)getLeftMostEnd();
}

bool FreeSpace::copyNextPointerFromEndToFront(uint32_t *front, uint32_t *end) {
    *front = *end;
    return true;
}

void FreeSpace::setNext(FreeSpace *next, byte *startOfPage) {
#ifdef PRECONDITION
    assert(next == nullptr || (byte*)next >= startOfPage);
    assert(((byte*) next - startOfPage) < 4294967295 );// offset is less than uint 32
#endif
    size_t codeBlockSize = CodeBlock::getBlockSize(getLeftMostEnd());
    uint32_t *leftNext = getLeftNext(codeBlockSize);
    uint32_t *rightNext = getRightNext(codeBlockSize);
    if (next == nullptr){
        *leftNext = ERROR_NEXT_POINTER;
        *rightNext = ERROR_NEXT_POINTER;
#ifdef POSTCONDITION

#endif
        return;
    }
    uint32_t offset = (uint32_t) ((byte*) next - startOfPage);
    *leftNext = offset;
    if (CodeBlock::readFromLeft((byte*)this)>=8){//overlapping pointers if the size is too little
        *rightNext = offset;
    }
#ifdef POSTCONDITION
    assert((byte*)this->getNext(startOfPage) >= startOfPage);
    assert(*leftNext != ERROR_NEXT_POINTER);
    assert(*rightNext != ERROR_NEXT_POINTER);
#endif
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
    if (*leftNext == ERROR_NEXT_POINTER){
        return nullptr;
    }
    return (FreeSpace*)(startOfPage+(*leftNext));
}

bool FreeSpace::copyNextPointerFromFrontToEnd(uint32_t *front, uint32_t *end) {
    *end = *front;
    return true;
}


