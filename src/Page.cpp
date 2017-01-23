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
#include "../include/Statistic.h"

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
    this->staticEnd = (byte*)startOfPage+sizeInBytes;
    this->dynamicEnd = (byte*)startOfPage;
    CodeBlock::setFree((byte*)startOfPage, true);
    initializeBucketList();
}

Page::Page(size_t sizeInBytes, bool isFirst):Page(sizeInBytes) {
    if(isFirst) nextPage = this;
}

void Page::initializeBucketList() {
    Logger::info("initialize bucket list");
    bucketList.setStartOfPage((byte*) startOfPage);
    bucketList.addToList(generateFirstBucketEntry());
#ifdef POSTCONDITION
    for ( int i = 0; i < bucketList.blSize-1; i++){
        assert(bucketList.getFromBucketList(i)== nullptr);
    }
    size_t blockSize = CodeBlock::getBlockSize((byte*)bucketList.getFromBucketList(bucketList.blSize-1));
    assert(CodeBlock::readFromLeft((byte*)bucketList.getFromBucketList(bucketList.blSize-1))== pageSize-2*blockSize);
#endif
}

Page::~Page() {
    Logger::warning("Page destructor called");
    free(startOfPage);
}

void* Page::getStaticBlock(size_t sizeInByte) {
    Logger::info("static block requested");
#ifdef PRECONDITION
    assert(sizeInByte!=0);
    assert(staticEnd>dynamicEnd);//static end should come after the dynamic end
#endif
#ifdef ALIGN_STATIC
    sizeInByte = align(sizeInByte);
#endif
    if (staticBlockFitInPage(sizeInByte)) {
        byte* codeBlockLeft = nullptr;
        FreeSpace* lastFreeSpace = (FreeSpace*)(staticEnd - CodeBlock::readFromRight(staticEnd-1, codeBlockLeft)-
                (2*CodeBlock::getBlockSize(codeBlockLeft)));
        bucketList.deleteFromList(lastFreeSpace);
        cutRightFromFreeSpace(lastFreeSpace, sizeInByte);
        bucketList.addToList(lastFreeSpace);//lastFreeSpace might get too small for its current bucket
        this->staticEnd = this->staticEnd - sizeInByte;
#ifdef POSTCONDITION
        assert(staticEnd>dynamicEnd);//see above
#endif
        return this->staticEnd;
    }else{
        Logger::warning("requested block does not fit in page");
#ifdef POSTCONDITION
        assert(staticEnd>dynamicEnd);//see above
        assert(staticEnd - dynamicEnd < 6 + sizeInByte);//there actually shouldn't be enough space
#endif
        return nullptr;
    }
}

bool Page::staticBlockFitInPage(size_t blockSizeInByte) {
    //no assertions because state isn't altered
    return (blockSizeInByte <= (staticEnd - dynamicEnd - 1) && (staticEnd - dynamicEnd >= 6 + blockSizeInByte));
}

size_t Page::align(size_t requestedSizeInByte) {
    Logger::error("Alignment is not implemented and returns the given block size");
    return requestedSizeInByte;
}

OccupiedSpace * Page::getDynamicBlock(size_t sizeInByte) {
    Logger::info("dynamic block requested");
#ifdef PRECONDITION
    assert(sizeInByte > 0);
    assert(staticEnd>dynamicEnd);
#endif
#ifdef ALIGN_DYNAMIC
    sizeInByte = align(sizeInByte);
#endif
    FreeSpace* freeSpace = bucketList.getFreeSpace(sizeInByte);
    OccupiedSpace* returnBlock = (OccupiedSpace*)freeSpace;
    if(freeSpace == nullptr){
        Logger::info("no applicable freeSpace in this page. Switching to the next one");
#ifdef POSTCONDITION
        assert(staticEnd>dynamicEnd);
#endif
        return nullptr;
    }else {
        bucketList.deleteFromList(freeSpace);
        FreeSpace *remainingSpace = cutLeftFromFreeSpace(freeSpace,
                                                         sizeInByte + (2 * CodeBlock::getNeededCodeBlockSize(sizeInByte)));
        if (remainingSpace) {
            bucketList.addToList(remainingSpace);
            returnBlock->toOccupied(sizeInByte);
        }else{
            //Edge Case: If the remaining space is too small to be used again, simply return a larger block
            CodeBlock::setFree((byte *) returnBlock, false);
            returnBlock->copyCodeBlockToEnd((byte *) returnBlock, CodeBlock::getBlockSize((byte *) returnBlock));
        }

        if (returnBlock->getRightMostEnd() > dynamicEnd) {
            dynamicEnd = returnBlock->getRightMostEnd();
        }
    }
#ifdef POSTCONDITION
    assert(returnBlock != nullptr);
    assert(dynamicEnd < staticEnd);
    assert(dynamicEnd > startOfPage);
    assert(returnBlock >= startOfPage);
    assert(!CodeBlock::isFree((byte*) returnBlock));
#endif
    return returnBlock;
}

FreeSpace *Page::cutLeftFromFreeSpace(FreeSpace *freeSpace, size_t bytesToCutOf) {
    Logger::info("cut left");
#ifdef PRECONDITION
    assert(freeSpace >= startOfPage && (byte*)freeSpace < getStaticEnd());
    assert(freeSpace->getSize()>=bytesToCutOf);
#endif
    if ((freeSpace->getSize() - bytesToCutOf) < SMALLEST_POSSIBLE_FREE_SPACE) {
#ifdef POSTCONDITION

#endif
        return nullptr;
    }else{
        freeSpace = freeSpace->pushBeginningRight(((byte *) freeSpace) + bytesToCutOf);
#ifdef POSTCONDITION
        assert(freeSpace->getNext((byte*)startOfPage) == nullptr
               || (freeSpace->getNext((byte*) startOfPage) >= startOfPage
                   && (byte*) freeSpace->getNext((byte*) startOfPage) < staticEnd));
        assert(freeSpace->getSize() >= 6);
#endif
        return freeSpace;
    }
}

FreeSpace *Page::generateFirstBucketEntry() {
    FreeSpace* freeSpace = (FreeSpace*) startOfPage;
    size_t codeBlockSize = 0;
    CodeBlock::getCodeBlockForInternalSize((byte *) startOfPage, pageSize, codeBlockSize, true);
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
#ifdef PRECONDITION
    assert(freeSpace->getSize()>=bytesToCutOf);//there must be enough space in the freespace
    assert(freeSpace >= startOfPage && freeSpace < startOfPage + pageSize);//the freespace must be in the page
#endif
    if ((freeSpace->getSize() - bytesToCutOf) < SMALLEST_POSSIBLE_FREE_SPACE){
#ifdef POSTCONDITION
    //see if clause
#endif
        return nullptr;
    }else{
        freeSpace = freeSpace->pushEndLeft((freeSpace->getRightMostEnd()) - bytesToCutOf);
#ifdef POSTCONDITION
        //the next pointer must either be the invalid pointer or must point into the page
        assert(freeSpace->getNext((byte*) startOfPage) == nullptr
               || (freeSpace->getNext((byte*) startOfPage) >= startOfPage
                                 && (byte*) freeSpace->getNext((byte*) startOfPage) < staticEnd));
        assert(freeSpace >= startOfPage);//freespace must still be in the page
        assert(freeSpace->getRightMostEnd() < staticEnd);//freespace may not go into the static area
#endif
        return freeSpace;
    }
}

bool Page::deleteBlock(void *firstByte) {
#ifdef PRECONDITION
    assert(staticEnd>dynamicEnd);
#endif
    Logger::info("deleting block");
    byte* codeBlockStart = nullptr;
    size_t memoryBlockSize = CodeBlock::readFromRight(((byte*)firstByte-1), codeBlockStart);
    size_t codeBlockSize = CodeBlock::getBlockSize(codeBlockStart);
#ifdef STATISTIC
    Statistic::freeDynamic(memoryBlockSize, firstByte);
#endif
    assert((codeBlockStart+(2*codeBlockSize)+memoryBlockSize) < staticEnd);
    if((codeBlockStart+(2*codeBlockSize)+memoryBlockSize) >= staticEnd){
        Logger::fatal("dynamic block to delete overlaps with static sector", ERROR_CODES::STATIC_AND_DYNAMIC_SECTORS_OVERLAP);
        assert(false);
        return false;
    }
    Space* leftNeighbor = nullptr;
    Space* rightNeighbor = (Space*)(codeBlockStart+(2*codeBlockSize)+memoryBlockSize);
    if( (byte*)rightNeighbor > staticEnd){
        Logger::fatal("dynamic block to delete overlaps with static sector", ERROR_CODES::STATIC_AND_DYNAMIC_SECTORS_OVERLAP);
        assert(false);
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
#ifdef POSTCONDITION
    assert((leftNeighbor == nullptr && bucketList.searchInList((FreeSpace*)codeBlockStart)&&CodeBlock::isFree(codeBlockStart))
           ||(bucketList.searchInList((FreeSpace*)leftNeighbor)&&CodeBlock::isFree((byte*)leftNeighbor)));
    assert(staticEnd>dynamicEnd);
#endif
    return true;

}

FreeSpace * Page::mergeFreeSpace(Space *leftBlock, Space *middleBlock, Space *rightBlock) {
#ifdef PRECONDITION
    assert(!CodeBlock::isFree((byte *)middleBlock));
    assert(rightBlock== nullptr||bucketList.searchInList((FreeSpace*)rightBlock));
    assert(leftBlock== nullptr||bucketList.searchInList((FreeSpace*)leftBlock));
#endif
    Logger::info("merge block");
    if ( leftBlock == nullptr){
        if (rightBlock != nullptr){
            bucketList.deleteFromList((FreeSpace*)rightBlock);
            mergeWithRight(middleBlock, rightBlock);
        }
        CodeBlock::setFree((byte*)middleBlock, true);
        middleBlock->copyCodeBlockToEnd((byte*)middleBlock, CodeBlock::getBlockSize((byte*)middleBlock));
        bucketList.addToList((FreeSpace*)middleBlock);
#ifdef POSTCONDITION
        assert(CodeBlock::isFree((byte *)middleBlock));
        assert(bucketList.searchInList((FreeSpace*)middleBlock));
#endif
        return (FreeSpace*)middleBlock;
    } else{
        if(rightBlock != nullptr){
            bucketList.deleteFromList((FreeSpace*)rightBlock);
            mergeWithRight(middleBlock, rightBlock);
        }
        bucketList.deleteFromList((FreeSpace*)leftBlock);

        mergeWithLeft(leftBlock,middleBlock);
        CodeBlock::setFree((byte*)leftBlock, true);
        middleBlock->copyCodeBlockToEnd((byte*)leftBlock, CodeBlock::getBlockSize((byte*)leftBlock));
        bucketList.addToList((FreeSpace*)leftBlock);
#ifdef POSTCONDITION
        assert(CodeBlock::isFree((byte *)leftBlock));
        assert(bucketList.searchInList((FreeSpace*)leftBlock));
#endif
        return (FreeSpace*)leftBlock;
    }
}

void Page::mergeWithRight(Space *middleBlock, Space *rightBlock) {
#ifdef PRECONDITION
    assert(CodeBlock::isFree((byte *)rightBlock));
#endif
    Logger::info("merge with right");
    byte* leftEnd = (byte*) middleBlock;
    byte* rightEnd = rightBlock->getRightMostEnd();
    size_t codeBLockSize = 0;
    CodeBlock::getCodeBlockForInternalSize(leftEnd, rightEnd - leftEnd + 1, codeBLockSize, true);
    middleBlock->copyCodeBlockToEnd(leftEnd, codeBLockSize);
#ifdef POSTCONDITION
    assert(CodeBlock::isFree((byte *)middleBlock));
    assert(CodeBlock::readFromLeft(leftEnd)==rightEnd-leftEnd-2*codeBLockSize+1);
#endif
}

void Page::mergeWithLeft(Space *leftBlock, Space *middleBlock) {
#ifdef PRECONDITION
    assert(CodeBlock::isFree((byte *)leftBlock));
#endif
    Logger::info("merge with left");
    byte* leftEnd = (byte*) leftBlock;
    byte* rightEnd = middleBlock->getRightMostEnd();
    size_t codeBLockSize = 0;
    CodeBlock::getCodeBlockForInternalSize(leftEnd, rightEnd - leftEnd + 1, codeBLockSize, true);
    middleBlock->copyCodeBlockToEnd(leftEnd, codeBLockSize);
#ifdef POSTCONDITION
    assert(CodeBlock::isFree(leftEnd));
    assert(CodeBlock::readFromLeft(leftEnd)==rightEnd-leftEnd-2*codeBLockSize+1);
#endif
}

void *Page::getStartOfPage() {
    return startOfPage;
}

byte *Page::getStaticEnd() {
    return staticEnd;
}

byte *Page::getDynamicEnd() {
    return dynamicEnd;
}

BucketList * Page::getBucketList() {
    return &bucketList;
}
