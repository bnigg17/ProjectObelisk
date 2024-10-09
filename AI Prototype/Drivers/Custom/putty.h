/*
 * putty.h
 *
 *  Created on: Oct 7, 2024
 *      Author: bnigg2
 */

#ifndef CUSTOM_PUTTY_H_
#define CUSTOM_PUTTY_H_

void init_putty(UART_HandleTypeDef * huart);
void println(const char * string);
void putty_recieve(uint8_t * rxBuff);
void printHex(uint8_t data);

#endif /* CUSTOM_PUTTY_H_ */
