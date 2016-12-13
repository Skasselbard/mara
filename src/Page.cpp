//
// Created by tom on 14.11.16.
//

#include <stdlib.h>
#include <malloc.h>
#include <new>
#include <assert.h>
#include "../include/Page.h"
#include "../include/Logger.h"
#include "../include/PageList.h"
#include "../include/OccupiedSpace.h"
#include "../include/CodeBlock.h"

Page::Page(size_t sizeInBytes):pageSize(sizeInBytes){
    Logger::info("new page requested");
    if(sizeInBytes > MAX_PAGE_SIZE){
        sizeInBytes = MAX_PAGE_SIZE;
        Logger::warning("maximum page size exceeded. Decreased size to the maximum of 4294967295 byte");
    }
    this->nextPage = nullptr;
    this->startOfPage = malloc(sizeInBytes);
    if (startOfPage == nullptr){
        Logger::error("unable to allocate memory for new page");
    }
    this->staticEnd = (byte*)this->startOfPage+sizeInBytes;
    CodeBlock::setFree((byte*)startOfPage, true);
    initializeBucketList();
}

void Page::initializeBucketList() {
    Logger::info("initialize bucket list");
    bucketList.setStartOfPage((byte*) startOfPage);
    bucketList.addToList(generateFirstBucketEntry());
}

Page::~Page() {
    Logger::warning("Page destructor called");
    free(startOfPage);
}

void* Page::getStaticBlock(size_t sizeInByte) {
    Logger::info("static block requested");
#ifdef ALIGN_STATIC
    sizeInByte = align(sizeInByte);
#endif
    if (staticBlockFitInPage(sizeInByte)) {
        byte* codeBlockLeft = nullptr;
        //TODO: catch if dynamic space (left from static end) is less than 6 byte
        FreeSpace* lastFreeSpace = (FreeSpace*)(staticEnd - CodeBlock::readFromRight(staticEnd-1, codeBlockLeft)-(2*CodeBlock::getBlockSize(codeBlockLeft)));
        cutRightFromFreeSpace(lastFreeSpace, sizeInByte);
        this->staticEnd = this->staticEnd - sizeInByte;
        return this->staticEnd;
    }else{
        Logger::error("requested block does not fit in page");
        return nullptr;
    }
}

bool Page::staticBlockFitInPage(size_t blockSizeInByte) {
    return (blockSizeInByte <= (staticEnd - dynamicEnd - 1));
}

size_t Page::align(size_t requestedSizeInByte) {
    Logger::error("Alignment is not implemented and returns the given block size");
    return requestedSizeInByte;
}

OccupiedSpace * Page::getDynamicBlock(size_t sizeInByte) {
    Logger::info("dynamic block requested");
#ifdef ALIGN_DYNAMIC
    sizeInByte = align(sizeInByte);
#endif
    FreeSpace* freeSpace = bucketList.getFreeSpace(sizeInByte);
    OccupiedSpace* returnBlock = (OccupiedSpace*)freeSpace;
    if(freeSpace == nullptr){
        Logger::info("no applicable freeSpace in this page. Switching to the next one");
        return nullptr;
    }else {
        bucketList.deleteFromList(freeSpace);
        FreeSpace *remainingSpace = cutLeftFromFreeSpace(freeSpace,
                                                         sizeInByte + (2 * Space::computeCodeBlockSize(sizeInByte)));
        if (remainingSpace) {
            bucketList.addToList(remainingSpace);
        }
        returnBlock->toOccupied(sizeInByte);
        if (returnBlock->getRightMostEnd() > dynamicEnd) {
            dynamicEnd = returnBlock->getRightMostEnd();
        }
    }
    return returnBlock;
}

FreeSpace *Page::cutLeftFromFreeSpace(FreeSpace *freeSpace, size_t bytesToCutOf) {
    Logger::info("cut left");
    assert(freeSpace->getSize()>=bytesToCutOf);
    if ((freeSpace->getSize() - bytesToCutOf) < SMALLEST_POSSIBLE_FREE_SPACE) {
        return nullptr;
    }else{
        freeSpace = freeSpace->pushBeginningRight(((byte *) freeSpace) + bytesToCutOf);
        return freeSpace;
    }
}

FreeSpace *Page::generateFirstBucketEntry() {
    FreeSpace* freeSpace = (FreeSpace*) startOfPage;
    size_t codeBlockSize = 0;
    CodeBlock::getCodeBlockForInternalSize((byte *) startOfPage, pageSize, codeBlockSize);
    freeSpace->copyCodeBlockToEnd((byte *) freeSpace, codeBlockSize);
    freeSpace->setNext(nullptr, (byte*)startOfPage);
    return freeSpace;
}

void Page::setNextPage(Page *nextPage) {
    if(nextPage != nullptr){
        Logger::warning("resetting non empty nextPage pointer. Old Pointer will be overwritten");
    }
    this->nextPage = nextPage;
}

Page *Page::getNextPage() {
    return nextPage;
}

bool Page::blockIsInSpace(void *firstByte) {
    return (startOfPage <= firstByte && firstByte < staticEnd);
}

FreeSpace *Page::cutRightFromFreeSpace(FreeSpace *freeSpace, size_t bytesToCutOf) {
    Logger::info("cut right");
    assert(freeSpace->getSize()>=bytesToCutOf);
    if ((freeSpace->getSize() - bytesToCutOf) < SMALLEST_POSSIBLE_FREE_SPACE) {
        return nullptr;
    }else{
        freeSpace = freeSpace->pushEndLeft((freeSpace->getRightMostEnd()) - bytesToCutOf);
        return freeSpace;
    }
}

bool Page::deleteBlock(void *firstByte) {
    Logger::info("deleting block");
    byte* codeBlockStart = nullptr;
    size_t memoryBlockSize = CodeBlock::readFromRight(((byte*)firstByte-1), codeBlockStart);
    size_t codBlockSize = CodeBlock::getBlockSize(codeBlockStart);
    assert((codeBlockStart+(2*codBlockSize)+memoryBlockSize) < staticEnd);
    if((codeBlockStart+(2*codBlockSize)+memoryBlockSize) >= staticEnd){
        Logger::fatal("dynamic block to delete overlaps with static sector", ERROR_CODES::STATIC_AND_DYNAMIC_SECTORS_OVERLAP);
        return false;
    }
    Space* leftNeighbor = nullptr;
    Space* rightNeighbor = (Space*)(codeBlockStart+(2*codBlockSize)+memoryBlockSize);
    if( (byte*)rightNeighbor > staticEnd){
        Logger::fatal("dynamic block to delete overlaps with static sector", ERROR_CODES::STATIC_AND_DYNAMIC_SECTORS_OVERLAP);
        return false;
    }
    if(startOfPage < codeBlockStart){
        leftNeighbor = Space::getLeftNeighbor(codeBlockStart - 1);
    }
    if (leftNeighbor && !CodeBlock::isFree((byte *)leftNeighbor)){
        leftNeighbor = nullptr;
    }
    if (rightNeighbor &&!CodeBlock::isFree((byte *)rightNeighbor)){
        rightNeighbor = nullptr;
    }
    mergeFreeSpace(leftNeighbor,(Space*)codeBlockStart,rightNeighbor);
    return true;
}

FreeSpace * Page::mergeFreeSpace(Space *leftBlock, Space *middleBlock, Space *rightBlock) {
    Logger::info("merge block");
    if ( leftBlock == nullptr){
        if (rightBlock != nullptr){
            CodeBlock::setFree((byte*)rightBlock, false);
            bucketList.deleteFromList((FreeSpace*)rightBlock);
            mergeWithRight(middleBlock, rightBlock);
        }
        CodeBlock::setFree((byte*)middleBlock, true);
        middleBlock->copyCodeBlockToEnd((byte*)middleBlock, CodeBlock::getBlockSize((byte*)middleBlock));
        bucketList.addToList((FreeSpace*)middleBlock);
        return (FreeSpace*)middleBlock;
    } else{
        if(rightBlock != nullptr){
            CodeBlock::setFree((byte*)rightBlock, false);
            bucketList.deleteFromList((FreeSpace*)rightBlock);
            mergeWithRight(middleBlock, rightBlock);
        }
        CodeBlock::setFree((byte*)leftBlock, false);
        bucketList.deleteFromList((FreeSpace*)leftBlock);
        mergeWithLeft(leftBlock,middleBlock);
        CodeBlock::setFree((byte*)leftBlock, true);
        middleBlock->copyCodeBlockToEnd((byte*)leftBlock, CodeBlock::getBlockSize((byte*)leftBlock));
        bucketList.addToList((FreeSpace*)leftBlock);
        return (FreeSpace*)leftBlock;
    }
}

void Page::mergeWithRight(Space *middleBlock, Space *rightBlock) {
    Logger::info("merge with right");
    byte* leftEnd = (byte*) middleBlock;
    byte* rightEnd = rightBlock->getRightMostEnd();
    size_t codeBLockSize = 0;
    CodeBlock::getCodeBlockForInternalSize(leftEnd, rightEnd-leftEnd + 1, codeBLockSize);
    middleBlock->copyCodeBlockToEnd(leftEnd, codeBLockSize);
}

void Page::mergeWithLeft(Space *leftBlock, Space *middleBlock) {
    Logger::info("merge with left");
    byte* leftEnd = (byte*) leftBlock;
    byte* rightEnd = middleBlock->getRightMostEnd();
    size_t codeBLockSize = 0;
    CodeBlock::getCodeBlockForInternalSize(leftEnd, rightEnd-leftEnd + 1, codeBLockSize);
    middleBlock->copyCodeBlockToEnd(leftEnd, codeBLockSize);
}
