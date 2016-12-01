//
// Created by tom on 15.11.16.
//

#include "../include/BucketList.h"
#include "../include/Logger.h"

FreeSpace *BucketList::getFreeSpace(size_t sizeInByte) {
    Logger::error("getFreeSpace is not implemented");
    SIZE_CLASS currentSizeClass = getCorrectBucket(sizeInByte);
    while (bucketList[currentSizeClass] == nullptr){
        if(currentSizeClass < BYTE_MORE_THAN512){
            currentSizeClass = (SIZE_CLASS)((int)currentSizeClass+1);
        }else{
            return nullptr;
        }
    }
    return bucketList[currentSizeClass];
}

bool BucketList::addToList(FreeSpace *freeSpace) {
    size_t size = freeSpace->getSize();
    FreeSpace* predecessor = getLastInBucket(getCorrectBucket(size));
    if (predecessor == nullptr){
        bucketList[getCorrectBucket(size)] = freeSpace;
    } else {
        predecessor->setNext(freeSpace);
    }
    freeSpace->setNext(nullptr);
    return true;
}

BucketList::SIZE_CLASS BucketList::getCorrectBucket(size_t blockSizeInByte) {
    if (blockSizeInByte > 32){//(32,512+]
        if(blockSizeInByte > 64){//(64,512+]
            if(blockSizeInByte > 256) {//(256,512+]
                if(blockSizeInByte > 512){
                    return BYTE_MORE_THAN512;
                }else{
                    return BYTE512;
                }
            }else{//(64,256]
                if (blockSizeInByte > 128){
                    return BYTE256;
                }else{
                    return BYTE128;
                }
            }
        }else{//(32,64]
            if(blockSizeInByte > 48){//(48,64]
                if(blockSizeInByte > 56){
                    return BYTE64;
                }else{
                    return BYTE56;
                }
            }else{//(32,48]
                if(blockSizeInByte > 40){
                    return BYTE48;
                }else{
                    return BYTE40;
                }
            }
        }
    }else{//[4,32]
        if (blockSizeInByte > 16){ //(16,32]
            if(blockSizeInByte > 24){//(24,32]
                if(blockSizeInByte > 28){
                    return BYTE32;
                }else{
                    return BYTE28;
                }
            }else{//(16,24]
                if(blockSizeInByte > 20){
                    return BYTE24;
                }else{//(16,20]
                    return BYTE20;
                }
            }
        }else {//[4,16]
            if (blockSizeInByte > 8){//(8,16]
                if (blockSizeInByte > 12){
                    return BYTE16;
                }
                else{
                    return BYTE12;
                }
            }else{//[4,8]
                if (blockSizeInByte > 4){
                    return BYTE8;
                }else{
                    return BYTE4;
                }
            }
        }
    }
    return BYTE_MORE_THAN512;
}

FreeSpace *BucketList::searchInList(FreeSpace *freeSpace, FreeSpace* &predecessor) {
    predecessor = nullptr;
    FreeSpace* currentElement = bucketList[getCorrectBucket(freeSpace->getSize())];
    while (currentElement != nullptr && currentElement != freeSpace ){
        predecessor = currentElement;
        currentElement = freeSpace->getNext();
    }
    return currentElement;
}

FreeSpace *BucketList::getLastInBucket(BucketList::SIZE_CLASS sizeClass) {
    FreeSpace* currentElement = bucketList[sizeClass];
    while(currentElement != nullptr){
        currentElement = currentElement->getNext();
    }
    return currentElement;
}

bool BucketList::deleteFromList(FreeSpace *freeSpace) {
    Logger::info("deleting element from bucketList");
    FreeSpace* predecessor = nullptr;
    if (freeSpace == searchInList(freeSpace, predecessor)){
        if (predecessor == nullptr){
            bucketList[getCorrectBucket(freeSpace->getSize())] = freeSpace->getNext();
        }else{
            predecessor->setNext(freeSpace->getNext());
        }
        return true;
    }
    Logger::warning("element not found in bucketList");
    return false;
}
