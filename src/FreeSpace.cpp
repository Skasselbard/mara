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
    assert(firstByte<getRightMostEnd()); //Never cross the pointers!
#endif
    size_t codeBlockSize = CodeBlock::getBlockSize(getLeftMostEnd());
    byte* rightMostEnd = getRightMostEnd();
    uint32_t nextPointer = *getRightNext(codeBlockSize);
    if (firstByte ==
            CodeBlock::getCodeBlockForInternalSize(firstByte, (rightMostEnd - firstByte) + 1, codeBlockSize, true)){
        copyCodeBlockToEnd(firstByte, codeBlockSize);
        writeNextPointer(nextPointer, firstByte);
#ifdef POSTCONDITION
        byte* foo = nullptr;
        FreeSpace* newFreeSpace = (FreeSpace*) firstByte;
        assert(CodeBlock::readFromLeft(firstByte) == CodeBlock::readFromRight(rightMostEnd, foo));
        assert(CodeBlock::readFromLeft(firstByte)<8 || *(newFreeSpace->getLeftNext(CodeBlock::getBlockSize(firstByte)))
               == *(newFreeSpace->getRightNext(CodeBlock::getBlockSize(firstByte))));
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
    uint32_t nextPointer = *getLeftNext(codeBlockSize);
    writeNextPointer(nextPointer, getLeftMostEnd());

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

void FreeSpace::writeNextPointer(uint32_t nextPointer, byte* leftCodeBlock){
#ifdef PRECONDITION
    assert(CodeBlock::isFree(leftCodeBlock));//We shouldn't write NextPointers in used areas
#endif
    size_t codeBlockSize = CodeBlock::getBlockSize(leftCodeBlock);
    size_t spaceSize = CodeBlock::readFromLeft(leftCodeBlock);
    uint32_t * leftNext = (uint32_t *) (leftCodeBlock + codeBlockSize);
    uint32_t * rightNext = (uint32_t*) (leftCodeBlock+spaceSize+codeBlockSize- sizeof(uint32_t));
    *leftNext = nextPointer;
    if(spaceSize >= 8){
        *rightNext = nextPointer;
    }
#ifdef POSTCONDITION
    assert(CodeBlock::isFree(leftCodeBlock));
    assert(CodeBlock::readFromLeft(leftCodeBlock) == CodeBlock::readFromLeft(leftCodeBlock+spaceSize+codeBlockSize));
    assert(spaceSize < 8 || *leftNext == *rightNext);
#endif

}


