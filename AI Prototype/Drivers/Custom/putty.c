/*
 * putty.c
 *
 *  Created on: Oct 5, 2024
 *      Author: bnigg2
 */

#include "../../Core/Inc/main.h"

#define CHAR_ROWS 30
#define CHAR_COLS 80

UART_HandleTypeDef * huart_inst;

void init_putty(UART_HandleTypeDef * huart){
    huart_inst = huart;
}

void putty_println(const char * string){
    uint8_t txBuff[CHAR_COLS];
    int eos_flag = 0;
    for(int i = 0; i < CHAR_COLS; i++){
        if(string[i] == '/0'){
            eos_flag = 1;
        }
        if(!eos_flag){
            txBuff[i] = string[i];
        }
        else{
            txBuff[i] = ' ';
        }
    }
    (void)HAL_UART_Transmit(huart_inst, txBuff, CHAR_COLS, 0xFFFF);
}

/*
Inteneded for recieve input for PuTTy terminal
*/
void putty_recieve(uint8_t * rxBuff){
    return;
}

