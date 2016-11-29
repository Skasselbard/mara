//
// Created by tom on 14.11.16.
//

#include <stdlib.h>
#include <malloc.h>
#include <new>
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
    this->startOfPage = malloc(sizeInBytes);
    if (startOfPage == nullptr){
        Logger::error("unable to allocate memory for new page");
    }
    this->staticEnd = (byte*)this->startOfPage+sizeInBytes;
    bucketList.addToList(generateFirstBucketEntry());
}

Page::~Page() {
    Logger::warning("Page destructor called");
    free(startOfPage);
}

void* Page::getStaticBlock(size_t sizeInByte) {
    Logger::info("static block requested");
#ifdef ALLIGN_STATIC
    sizeInByte = allign(sizeInByte);
#endif
    if (staticBlockFitInPage(sizeInByte)) {
        this->staticEnd = this->staticEnd - sizeInByte;
        FreeSpace* lastFreeSpace = (FreeSpace*)(dynamicEnd+1);
        cutRightFromFreeSpace(lastFreeSpace, sizeInByte);
        return this->staticEnd;
    }else{
        Logger::error("requested block does not fit in page");
        return nullptr;
    }
}

size_t Page::getDynamicSectorSize() {
    Logger::error("getDynamicSectorSize is not implemented and returns wrong results");
    return 0;
}

bool Page::staticBlockFitInPage(size_t blockSizeInByte) {
    return (blockSizeInByte <= (staticEnd - dynamicEnd));
}

size_t Page::allign(size_t requestetSizeInByte) {
    Logger::error("Allignment is not implemented and returns the given block size");
    return requestetSizeInByte;
}

OccupiedSpace * Page::getDynamicBlock(size_t sizeInByte) {
#ifdef ALLIGN_DAYNAMIC
    sizeInByte = allign(sizeInByte);
#endif
    FreeSpace* freeSpace = bucketList.getFreeSpace(sizeInByte);
    OccupiedSpace* returnBlock = (OccupiedSpace*)freeSpace;
    if(freeSpace == nullptr){
        Logger::info("no applicable freespace in this page. Switching to the next one");
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
            dynamicEnd = freeSpace->getRightMostEnd();
        }
    }
    return returnBlock;
}

FreeSpace *Page::cutLeftFromFreeSpace(FreeSpace *freeSpace, size_t bytesToCutOf) {
    if ((freeSpace->getSize() - bytesToCutOf) < SMALLEST_POSSIBLE_FREESPACE) {
        return nullptr;
    }else{
        freeSpace = freeSpace->pushBeginningRight(((byte *) freeSpace) + bytesToCutOf);
        return freeSpace;
    }
}

FreeSpace *Page::generateFirstBucketEntry() {
    FreeSpace* freeSpace = (FreeSpace*) startOfPage;
    size_t codeBlockSize = 0;
    //TODO: Calculate CodeBlock correctly
    CodeBlock::getCodeBlockForPayloadSize((byte *) startOfPage, pageSize, codeBlockSize);
    freeSpace->copyCodeBlockToEnd((byte *) freeSpace, codeBlockSize);
    freeSpace->setNext(nullptr);
    return freeSpace;
}

void Page::setNextPage(Page *nextPage) {
    if(nextPage != nullptr){
        Logger::warning("reseting non empty nextPage pointer. Old Pointer will be overwritten");
    }
    this->nextPage = nextPage;
}

Page *Page::getNextPage() {
    return nextPage;
}

bool Page::blockIsInSpace(void *firstByte) {
    return (startOfPage <= firstByte && firstByte <= staticEnd);
}

FreeSpace *Page::cutRightFromFreeSpace(FreeSpace *freeSpace, size_t bytesToCutOf) {
    if ((freeSpace->getSize() - bytesToCutOf) < SMALLEST_POSSIBLE_FREESPACE) {
        return nullptr;
    }else{
        freeSpace = freeSpace->pushEndLeft((freeSpace->getRightMostEnd()) + bytesToCutOf);
        return freeSpace;
    }
}

bool Page::deleteBlock(void *firstByte) {
    OccupiedSpace* occupiedSpace = nullptr;
    size_t memoryBlockSize = CodeBlock::readFromRight(((byte*)firstByte-1), (byte*)occupiedSpace);
    size_t codBlockSize = CodeBlock::getBlockSize((byte*)occupiedSpace);
    if(((byte*)occupiedSpace+(2*codBlockSize)+memoryBlockSize) > staticEnd){
        Logger::fatal("dynamic block to delete overlaps with static sector", ERROR_CODES::STATIC_AND_DYNAMIC_SECTORS_OVERLAP);
        return false;
    }
    Space* leftNeighbor = nullptr;
    Space* rightNeighbor = (Space*)((byte*)occupiedSpace+(2*codBlockSize)+memoryBlockSize+1);
    if( (byte*)rightNeighbor > staticEnd){
        Logger::fatal("dynamic block to delete overlaps with static sector", ERROR_CODES::STATIC_AND_DYNAMIC_SECTORS_OVERLAP);
        return false;
    }
    if(startOfPage < occupiedSpace){
        leftNeighbor = Space::getleftNeighbor(((byte*)occupiedSpace)-1);
    }
    if (!CodeBlock::isFree((byte *)leftNeighbor)){
        leftNeighbor = nullptr;
    }
    if (!CodeBlock::isFree((byte *)rightNeighbor)){
        rightNeighbor = nullptr;
    }
    mergeFreeSpace(leftNeighbor,occupiedSpace,rightNeighbor);
    return false;
}

FreeSpace * Page::mergeFreeSpace(Space *leftBlock, Space *middleBlock, Space *rightBlock) {
    if ( leftBlock == nullptr){
        if (rightBlock != nullptr){
            bucketList.deleteFromList((FreeSpace*)rightBlock);
            mergeWithRight(middleBlock, rightBlock);
        }
        bucketList.addToList((FreeSpace*)middleBlock);
        return (FreeSpace*)middleBlock;
    } else{
        if(rightBlock != nullptr){
            bucketList.deleteFromList((FreeSpace*)rightBlock);
            mergeWithRight(middleBlock, rightBlock);
        }
        bucketList.deleteFromList((FreeSpace*)leftBlock);
        mergeWithLeft(leftBlock,middleBlock);
        bucketList.addToList((FreeSpace*)leftBlock);
        return (FreeSpace*)leftBlock;
    }
}

void Page::mergeWithRight(Space *middleBlock, Space *rightBlock) {
    byte* leftEnd = (byte*) middleBlock;
    byte* rightEnd = rightBlock->getRightMostEnd();
    size_t codeBLockSize = 0;
    CodeBlock::getCodeBlockForInternalSize(leftEnd, rightEnd-leftEnd, codeBLockSize);
    middleBlock->copyCodeBlockToEnd(leftEnd, codeBLockSize);
}
