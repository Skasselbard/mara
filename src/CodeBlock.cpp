//
// Created by tom on 15.11.16.
//
#include <assert.h>
#include "../include/Logger.h"
#include "../include/CodeBlock.h"

size_t CodeBlock::readFromLeft(byte *firstByte) {
    size_t size = 0;
    if(*firstByte >= 128){
        //block is single byte
        size = *firstByte & 63;
        return size;
    }else{
        //block is more than one byte
        byte *currentByte = firstByte+1;
        size = *firstByte & 63;
        size <<= 7;
        while(*currentByte >=128){
            size |= (*currentByte & 127); //insert the last 7 bits of the current byte at the end of size
            currentByte++;
            size <<= 7; //shift the old byte 7  bits to the left to make space for the next 7 bits
        }
        size |= (*currentByte & 127); //insert the last 7 bits of the current byte at the end of size
    }
    return size;
}

size_t CodeBlock::readFromRight(byte *firstByte, byte* &outLeftByte) {
    outLeftByte = firstByte;
    size_t size = 0;
    if(*firstByte >= 128){
        //block is single byte
        size = *firstByte & 63;
        return size;
    }else{
        //block is more than one byte
        byte *currentByte = firstByte-1;
        size = *firstByte & 127;
        int m = 1;
        while(*currentByte >=128){
            size_t tmp = *currentByte & 127; //stuff the 7 bits into a temporary size_t
            tmp <<= (7*m);//shift them to the appropriate position
            size |= tmp; //merge size and tmp
            currentByte--;
            m++;
        }
        size_t tmp = *currentByte & 63; //stuff the 7 bits into a temporary size_t
        tmp <<= (7*m);//shift them to the appropriate position
        size |= tmp; //merge size and tmp
        outLeftByte = currentByte;
        return size;
    }
}

byte *CodeBlock::getCodeBlockForPayloadSize(byte *leftStartOfBlock, size_t memoryBlockSize, size_t &returnArraySize,
                                            bool isFree) {
    if(memoryBlockSize <= 63){
        returnArraySize = 1;
        *leftStartOfBlock = memoryBlockSize | 128;
        setFree(leftStartOfBlock, isFree);
        return leftStartOfBlock;
    }
    //calculate how many bytes are needed
    size_t t = (memoryBlockSize >> 6);
    returnArraySize = 2;
    while(t>127){
        t >>= 7;
        returnArraySize++;
    }
    return CodeBlock::getCodeBlockForPayloadSize(leftStartOfBlock, memoryBlockSize, isFree, returnArraySize);
}

byte *CodeBlock::getCodeBlockForPayloadSize(byte *leftStartOfBlock, size_t memoryBlockSize,
                                            bool isFree, size_t codeBlockSize) {
    //write the bytes right to left
    byte* current = leftStartOfBlock+(codeBlockSize-1);
    int last = 1;
    for(int i = 0; i< codeBlockSize; i++){
        if(last){ //current is the rightmost byte
            *current = (byte) (memoryBlockSize & 127);
            memoryBlockSize >>= 7;
            last = 0;
            current--;
        }else if(current == leftStartOfBlock){ //current is the leftmost byte
            *current = (memoryBlockSize & 63);
            setFree(leftStartOfBlock, isFree);
            return leftStartOfBlock;
        }else{
            *current = ((memoryBlockSize & 127) | 128);
            memoryBlockSize >>= 7;
            current--;
        }
    }
    Logger::error("This code should never be reached!");
    return leftStartOfBlock;
}

byte *CodeBlock::getCodeBlockForInternalSize(byte *leftStartOfBlock, size_t internallyNeededSize, size_t &returnArraySize,
                                             bool isFree) {
    returnArraySize = 1;
    while(CodeBlock::getNeededCodeBlockSize(internallyNeededSize - 2 * returnArraySize) > returnArraySize) {
        returnArraySize++;
    }
    CodeBlock::getCodeBlockForPayloadSize(leftStartOfBlock, internallyNeededSize - 2 * returnArraySize, isFree, returnArraySize);

}

size_t CodeBlock::getBlockSize(byte *firstByte) {
    if(*firstByte >= 128) return 1;
    byte *currentByte = firstByte+1;
    size_t size = 2;
    while(*currentByte >127){
        currentByte++;
        size++;
    }
    return size;
}

int CodeBlock::isFree(byte *firstByte) {
    return *firstByte & 64;
}

void CodeBlock::setFree(byte *firstByte, int free) {
    if(free) *firstByte |= 64;
    else *firstByte &= 191;

}

size_t CodeBlock::getNeededCodeBlockSize(size_t sizeToEncode) {
    if(sizeToEncode < 64) return 1;
    size_t size = 1;
    sizeToEncode >>= 6;
    while(sizeToEncode){
        size++;
        sizeToEncode >>= 7;
    }
    return size;
}


