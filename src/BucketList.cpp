//
// Created by tom on 15.11.16.
//

#include "../include/BucketList.h"
#include "../include/Logger.h"
#include "../include/CodeBlock.h"
#include <assert.h>
#include <string>

FreeSpace *BucketList::getFreeSpace(size_t sizeInByte) {
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
    if(returnSpace) {
        assert(CodeBlock::readFromLeft((byte*)returnSpace) >= sizeInByte);
    }
    return returnSpace;
}

bool BucketList::addToList(FreeSpace *freeSpace) {
    size_t size = CodeBlock::readFromLeft((byte*)freeSpace);
    Logger::info((std::string("adding element to bucketList position ")+std::to_string(lookupBucket(size))).c_str());
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
    FreeSpace* currentElement = bucketList[lookupBucket(CodeBlock::readFromLeft((byte *) freeSpace))];
    while (currentElement->getNext(startOfPage) != nullptr && currentElement != freeSpace ){
        predecessor = currentElement;
        currentElement = currentElement->getNext(startOfPage);
    }
    return currentElement;
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
    size_t size = CodeBlock::readFromLeft((byte*)freeSpace);
    Logger::info((std::string("deleting element from bucketList position ")+std::to_string(lookupBucket(size))).c_str());
    FreeSpace* predecessor = nullptr;
    if (freeSpace == searchInList(freeSpace, predecessor)){
        if (predecessor == nullptr){
            bucketList[lookupBucket(size)] = freeSpace->getNext(startOfPage);
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
    while(returnSpace && CodeBlock::readFromLeft((byte *) returnSpace) < minimumSize){
        returnSpace = returnSpace->getNext(startOfPage);
    }
    return returnSpace;
}

BucketList::BucketList() {
    for(int i = 0; i < blSize; i++){
        bucketList[i] = nullptr;
    }
}
