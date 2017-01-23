//
// Created by tom on 15.11.16.
//

#include "../include/BucketList.h"
#include "../include/Logger.h"
#include "../include/CodeBlock.h"
#include <assert.h>
#include <string>

FreeSpace *BucketList::getFreeSpace(size_t sizeInByte) {
#ifdef PRECONDITION
    assert(sizeInByte > 0);
#endif
    Logger::info((std::string("free space requested with size: ")+std::to_string(sizeInByte)).c_str());
    unsigned int bucketIndex = lookupBucket(sizeInByte);
    FreeSpace* returnSpace = nullptr;
    do {
        bucketIndex = findNonEmptyBucket(bucketIndex);
        returnSpace = findFittingSpaceInBucket(sizeInByte, bucketIndex);
        !returnSpace ? bucketIndex++ : bucketIndex;
    } while (!returnSpace && bucketIndex<(blSize-1));
    if (bucketIndex == blSize-1){
        returnSpace = findFittingSpaceInBucket(sizeInByte, blSize-1);
    }
#ifdef POSTCONDITION
    if(returnSpace) {
        assert(CodeBlock::readFromLeft((byte*)returnSpace) >= sizeInByte);
        FreeSpace* foo = nullptr;
        assert(searchInList(returnSpace, foo));
    }
#endif
    return returnSpace;
}

bool BucketList::addToList(FreeSpace *freeSpace) {
#ifdef PRECONDITION
    assert((byte*)freeSpace >= startOfPage);
    FreeSpace* foo = nullptr;
    assert(searchInList(freeSpace, foo) == nullptr);
#endif
    size_t size = CodeBlock::readFromLeft((byte*)freeSpace);
    Logger::info((std::string("adding element to bucketList position ")+std::to_string(lookupBucket(size))).c_str());
    FreeSpace* successor = bucketList[lookupBucket(size)];
    freeSpace->setNext(successor, startOfPage);
    bucketList[lookupBucket(size)] = freeSpace;
#ifdef POSTCONDITION
    FreeSpace* bar = nullptr;
    assert(searchInList(freeSpace, bar) != nullptr);
#endif
    return true;
}

FreeSpace *BucketList::searchInList(FreeSpace *freeSpace, FreeSpace* &predecessor) {
#ifdef PRECONDITION
    assert(CodeBlock::isFree((byte*)freeSpace));
#endif
    predecessor = nullptr;
    FreeSpace* currentElement = bucketList[lookupBucket(CodeBlock::readFromLeft((byte *) freeSpace))];
    if(currentElement == nullptr) return nullptr;
    while (currentElement->getNext(startOfPage) != nullptr && currentElement != freeSpace ){
        predecessor = currentElement;
        currentElement = currentElement->getNext(startOfPage);
    }
    if(currentElement!=freeSpace) currentElement = nullptr;
#ifdef POSTCONDITION
    assert(currentElement == nullptr
           ||freeSpace == nullptr
           || predecessor == nullptr
           || predecessor->getNext(startOfPage) == freeSpace);
    assert(currentElement == nullptr || currentElement == freeSpace);
#endif
    return currentElement;
}

FreeSpace *BucketList::searchInList(FreeSpace *freeSpace){
    FreeSpace* foo = nullptr;
    return searchInList(freeSpace, foo);
}

FreeSpace *BucketList::getLastInBucket(size_t size) {
    FreeSpace* currentElement = bucketList[size];
    if(currentElement == nullptr) return currentElement;
    while(currentElement->getNext(startOfPage) != nullptr){
        currentElement = currentElement->getNext(startOfPage);
    }
    return currentElement;
}

bool BucketList::deleteFromList(FreeSpace *freeSpace) {
#ifdef PRECONDITION

#endif
    size_t size = CodeBlock::readFromLeft((byte*)freeSpace);
    Logger::info((std::string("deleting element from bucketList position ")+std::to_string(lookupBucket(size))).c_str());
    FreeSpace* predecessor = nullptr;
    if (freeSpace == searchInList(freeSpace, predecessor)){
        if (predecessor == nullptr){
            bucketList[lookupBucket(size)] = freeSpace->getNext(startOfPage);
        }else{
            predecessor->setNext(freeSpace->getNext(startOfPage), startOfPage);
        }
#ifdef POSTCONDITION
    assert(searchInList(freeSpace, predecessor) == nullptr);
#endif
        return true;
    }
    Logger::error("element not found in bucketList");
#ifdef POSTCONDITION
    assert(false);
#endif
    return false;
}

void BucketList::setStartOfPage(byte *startOfPage) {
    BucketList::startOfPage = startOfPage;
}



unsigned int BucketList::lookupBucket(size_t size) {
#ifdef PRECONDITION
    assert(size > 0);
#endif
    if (size <= lastLinear4Scaling) {
        return (unsigned int)((size-1) / 4);
    } else if (size <= lastLinear16Scaling) {
        return (unsigned int)(lookupBucket(lastLinear4Scaling) + 1 + (size-lastLinear4Scaling-1)/16);
    } else if (size <= largestBucketSize) {
        return (unsigned int)(lookupBucket(lastLinear16Scaling) + 1 + (size_t) (log2(size-1)-log2(lastLinear16Scaling)));
    } else {
        return (unsigned int)blSize-1;
    }
}

unsigned int BucketList::findNonEmptyBucket(unsigned int index) {
#ifdef PRECONDITION
    assert(index < blSize);
#endif
    while (bucketList[index] == nullptr){
        if(index < blSize-1){
            index++;
        }else{
            break;
        }
    }
#ifdef POSTCONDITION
    assert(bucketList[index] != nullptr || index == blSize -1);
#endif
    return index;
}

FreeSpace *BucketList::findFittingSpaceInBucket(size_t minimumSize, unsigned int index) {
#ifdef PRECONDITION
    assert(minimumSize > 0);
    assert(index < blSize);
#endif
    FreeSpace* returnSpace = bucketList[index];
    while(returnSpace && CodeBlock::readFromLeft((byte *) returnSpace) < minimumSize){
        returnSpace = returnSpace->getNext(startOfPage);
    }
#ifdef POSTCONDITION
    assert(returnSpace == nullptr || CodeBlock::readFromLeft((byte*) returnSpace) >= minimumSize);
#endif
    return returnSpace;
}

BucketList::BucketList() {
    for(int i = 0; i < blSize; i++){
        bucketList[i] = nullptr;
    }
}

FreeSpace *BucketList::getFromBucketList(int index) {
    return bucketList[index];
}

