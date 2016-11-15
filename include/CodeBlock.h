//
// Created by tom on 15.11.16.
//

#ifndef MARA_CODEBLOCK_H
#define MARA_CODEBLOCK_H


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

};


#endif //MARA_CODEBLOCK_H
