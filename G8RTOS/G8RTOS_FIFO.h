/*
 * G8RTOS_FIFO.h
 *
 *  Created on: Sep 30, 2017
 *      Author: brons
 */

#ifndef G8RTOS_FIFO_H_
#define G8RTOS_FIFO_H_

#include "G8RTOS.h"

#define MAX_FIFOS 4
#define FIFOSIZE 16

typedef struct {
    int32_t buffer[FIFOSIZE];
    int32_t *head;
    int32_t *tail;
    int32_t lostDataCount;
    semaphore_t *currentSize;
    semaphore_t *mutex;
} fifo_t;

typedef struct {
    uint32_t buf[4];
    uint32_t *head;
    uint32_t *tail;
} buf_t;

enum {
    Accelx = 0,
    Accely = 1,
    Ballx = 2,
    Bally = 3
};

fifo_t fifo[MAX_FIFOS];
buf_t RMSbuffer;

void initRMSBuf();

void G8RTOS_InitFifo(uint32_t index);

int32_t G8RTOS_ReadFifo(uint32_t index);

int32_t G8RTOS_WriteFifo(uint32_t index, int32_t data);


#endif /* G8RTOS_FIFO_H_ */
