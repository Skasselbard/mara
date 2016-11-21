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
    }
    return size;
}

size_t CodeBlock::readFromRight(byte *firstByte) {
    size_t size = 0;
    if(*firstByte >= 128){
        //block is single byte
        size = *firstByte & 127;
        return size;
    }else{
        //block is more than one byte
        byte *currentByte = firstByte-1;
        size = *firstByte & 127;
        int m = 1;
        while(*currentByte >=128){
            size_t tmp = *currentByte & 127; //stuff the 7 bits into a temporary size_t
            tmp <<= (7*m);//shift them to the appropriate position
            size &= tmp; //merge size and tmp
            currentByte++;
            m++;
        }
        size_t tmp = *currentByte & 127; //stuff the 7 bits into a temporary size_t
        tmp <<= (7*m);//shift them to the appropriate position
        size &= tmp; //merge size and tmp
    }
}

byte *CodeBlock::getCodeBlock(byte* leftStartOfBlock, size_t memoryBlockSize, size_t &returnArraySize){
    if(memoryBlockSize <= 127){
        returnArraySize = 1;
        *leftStartOfBlock = memoryBlockSize | 128;
        return leftStartOfBlock;
    }
    //calculate how many bytes are needed
    size_t t = (memoryBlockSize >> 7);
    returnArraySize = 2;
    while(t>=127){
        t >>= 7;
        returnArraySize++;
    }
    //write the bytes left to right
    byte* current = leftStartOfBlock+(returnArraySize-1);
    int last = 1;
    for(int i = 0; i< returnArraySize; i++){
        if(last){
            *current = (memoryBlockSize & 127);
            memoryBlockSize >>= 7;
            last = 0;
            current--;
        }else if(current == leftStartOfBlock){
            *current = (memoryBlockSize & 127);
            current--;
        }else{
            *current = ((memoryBlockSize & 127) | 128);
            memoryBlockSize >>= 7;
        }
    }

    return leftStartOfBlock;
}
