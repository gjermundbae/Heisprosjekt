/**
 * @file 
 * @brief Wrapper for libComedi I/O.
 * These functions provide and interface to libComedi limited to use in
 * the real time lab.
 * 2006, Martin Korsgaard
 */

#ifndef __INCLUDE_IO_H__
#define __INCLUDE_IO_H__



/**
 * @brief Initialize libComedi in "Sanntidssalen"
 * @return Non-zero on success and 0 on failure
 */
int io_init();



/**
 * @brief Sets a digital channel bit.
 * @param channel Channel bit to set.
*/
void io_setBit(int channel);



/**
 * @brief Clears a digital channel bit.
 * @param channel Channel bit to set.
*/
void io_clearBit(int channel);



/**
 * @brief Writes a value to an analog channel.
 * @param channel Channel to write to.
 * @param value Value to write.
*/
void io_writeAnalog(int channel, int value);



/**
 * @brief Reads a bit value from a digital channel.
 * @param channel Channel to read from.
 * @return Value read.
 */
int io_readBit(int channel);



/**
 * @brief Reads a bit value from an analog channel.
 * @param channel Channel to read from.
 * @return Value read.
 */
int io_readAnalog(int channel);


#endif // #ifndef __INCLUDE_IO_H__