//
// Created by tom on 15.11.16.
//

#include "../include/BucketList.h"
#include "../include/Logger.h"
#include "../include/CodeBlock.h"
#include <assert.h>

FreeSpace *BucketList::getFreeSpace(size_t sizeInByte) {
    Logger::info((std::string("free space requested with size: ")+std::to_string(sizeInByte)).c_str());
    unsigned int bucketIndex = lookupBucket(sizeInByte);
    FreeSpace* returnSpace = nullptr;
    while (!returnSpace && bucketIndex<(blSize-1)){
        bucketIndex = findNonEmptyBucket(bucketIndex);
        returnSpace = findFittingSpaceInBucket(sizeInByte, bucketIndex);
        !returnSpace ? bucketIndex++ : bucketIndex;
    }
    return returnSpace;
}

bool BucketList::addToList(FreeSpace *freeSpace) {
    Logger::info("adding element to bucketList");
    size_t size = CodeBlock::readFromLeft((byte*)freeSpace);
    FreeSpace* predecessor = getLastInBucket(lookupBucket(size));
    if (predecessor == nullptr){
        bucketList[lookupBucket(size)] = freeSpace;
    } else {
        predecessor->setNext(freeSpace, startOfPage);
    }
    freeSpace->setNext(nullptr, startOfPage);
    return true;
}

FreeSpace *BucketList::searchInList(FreeSpace *freeSpace, FreeSpace* &predecessor) {
    predecessor = nullptr;
    FreeSpace* currentElement = bucketList[lookupBucket(freeSpace->getSize())];
    while (currentElement != nullptr && currentElement != freeSpace ){
        predecessor = currentElement;
        currentElement = currentElement->getNext(startOfPage);
    }
    return currentElement;
}

FreeSpace *BucketList::getLastInBucket(size_t size) {
    FreeSpace* currentElement = bucketList[size];
    while(currentElement != nullptr){
        currentElement = currentElement->getNext(startOfPage);
    }
    return currentElement;
}

bool BucketList::deleteFromList(FreeSpace *freeSpace) {
    Logger::info("deleting element from bucketList");
    FreeSpace* predecessor = nullptr;
    if (freeSpace == searchInList(freeSpace, predecessor)){
        if (predecessor == nullptr){
            bucketList[lookupBucket(freeSpace->getSize())] = freeSpace->getNext(startOfPage);
        }else{
            predecessor->setNext(freeSpace->getNext(startOfPage), startOfPage);
        }
        return true;
    }
    Logger::warning("element not found in bucketList");
    return false;
}

void BucketList::setStartOfPage(byte *startOfPage) {
    BucketList::startOfPage = startOfPage;
}



unsigned int BucketList::lookupBucket(size_t size) {
    assert(size != 0);
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
    while (bucketList[index] == nullptr){
        if(index < blSize-1){
            index++;
        }else{
            return index;
        }
    }
    return index;
}

FreeSpace *BucketList::findFittingSpaceInBucket(size_t minimumSize, unsigned int index) {
    FreeSpace* returnSpace = bucketList[index];
    while(returnSpace && returnSpace->getSize() < minimumSize){
        returnSpace = returnSpace->getNext(startOfPage);
    }
    return returnSpace;
}
