//
// Created by tom on 15.11.16.
//

#ifndef MARA_CODEBLOCK_H
#define MARA_CODEBLOCK_H


#include <cstddef>
#include "predefined.h"

/**
 * The code block which encodes the size of the memory block dynamically
 *
 * A code block consist of one ore more bytes. The first bit encodes if there belong more bytes to the block
 * or if this one is the last. The rest of the bytes encoding the size of the memory block
 *
 * To encode and decode the codeBlock, an automaton is used
 * In the first state, it examines if there are more than on byte needed to encode the memory block size.
 * If more than one byte is needed to encode the size, the second state is entered and the first bit of each byte
 * have another meaning
 *
 * If the codeBlock size is encoded with one byte, the first bit is a 1
 * If the codeBlock size is encoded with more than one byte, the first bit is a 0 if it is an ending byte and 1 if
 * it is a non ending byte
 *
 * Examples:
 * Legend:
 *      0 or 1  - bit which is used to encode the codeBlock size
 *      x       - bit which is used to encode the memory block size
 *      |       - byte delimiter
 *      .       - half byte delimiter
 *
 *  1. Memory block size < 2⁷ byte
 *      CodeBlock: 1xxx.xxxx
 *
 *  2. 2⁷ < Memory block size < 2¹⁴
 *      CodeBlock: 0xxx.xxxx | 0xxx.xxxx
 *
 *  3. 2¹⁴ < Memory block size
 *      CodeBlock: 0xxx.xxxx | 1xxx.xxxx | 0xxx.xxxx
 *      CodeBlock: 0xxx.xxxx | 1xxx.xxxx | 1xxx.xxxx | 0xxx.xxxx
 *                  .
 *                  .
 *                  .
 */
class CodeBlock {
public:
    /**
     * Reads the CodeBlock from the left
     * @param firstByte the first byte from the CodeBlock
     * @return the size of the memory block
     */
    static size_t readFromLeft(byte* firstByte);
    /**
     * Reads the CodeBlock from the right
     * @param firstByte the first byte from the CodeBlock
     * @param outLeftByte output for the leftmost byte of the block
     * @return the size of the memory block
     */
    static size_t readFromRight(byte* firstByte, byte* &outLeftByte);

    /**
     * Build a CodeBlock
     * @param leftStartOfBlock the beginning of the codeBlock starting from the left (return and this pointer should be the same)
     * @param memoryBlockSize size of the memory block which should be represented by the CodeBlock
     * @param returnArraySize size of the array returned by this function
     * @return an array of bytes, containing the codeBlock representing the memory block size.
     * The size of the array is stored in the last parameter. It should correspond to the leftStartOfBlock parameter
     */
    static byte* getCodeBlock(byte* leftStartOfBlock, size_t memoryBlockSize, size_t &returnArraySize);

    /**
     * reads if the given CodeBlock describes a free or used block.
     * @param firstByte  the first byte of the codeBlock, from the left
     * @return 0 if used, !=0 otherwise
     */
    static int isFree(byte* firstByte);

    /**
     * reads the size of the block in bytes
     * @param firstByte the first byte of the codeBlock, from the left
     * @return the number of bytes used by this block
     */
    static size_t getBlockSize(byte* firstByte);
    /**
     * set if the CodeBlock represents a free block or a used one
     * @param firstByte the first byte of the codeBlock, from the left
     * @param free 0 to mark it as free, != 0 otherwise
     */
    static void setFree(byte* firstByte, int free);

};


#endif //MARA_CODEBLOCK_H
