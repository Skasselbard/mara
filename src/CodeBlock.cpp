//
// Created by tom on 15.11.16.
//

#include "../include/CodeBlock.h"

size_t CodeBlock::readFromLeft(byte *firstByte) {
    size_t size = 0;
    if(*firstByte >= 128){
        //block is single byte
        size = *firstByte & 127;
        return size;
    }else{
        //block is more than one byte
        byte *currentByte = firstByte+1;
        size = *firstByte & 127;
        while(*currentByte >=128){
            size <<= 7; //shift the old byte 7  bits to the left to make space for the next 7 bits
            size &= (*currentByte & 127); //insert the last 7 bits of the current byte at the end of size
            currentByte++;
        }
        size <<= 7; //shift the old byte 7  bits to the left to make space for the next 7 bits
        size &= (*currentByte & 127); //insert the last 7 bits of the current byte at the end of size
        currentByte++;
    }
    return size;
}

size_t CodeBlock::readFromRight(byte *firstByte) {
    return 0;
}

byte *CodeBlock::getCodeBlock(byte* leftStartOfBlock, size_t memoryBlockSize, size_t &returnArraySize){
    return leftStartOfBlock;
}
