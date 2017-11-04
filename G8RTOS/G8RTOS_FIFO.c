/*
 * G8RTOS_FIFO.c
 *
 *  Created on: Sep 30, 2017
 *      Author: brons
 */

#include "G8RTOS_FIFO.h"

extern fifo_t fifo[MAX_FIFOS];

void initRMSBuf()
{
    RMSbuffer.buf[0] = 0;
    RMSbuffer.buf[1] = 0;
    RMSbuffer.buf[2] = 0;
    RMSbuffer.buf[3] = 0;
    RMSbuffer.head = &RMSbuffer.buf[0];
    RMSbuffer.tail = &RMSbuffer.buf[0];
}

void G8RTOS_InitFifo(uint32_t index)
{
    fifo[index].tail = &fifo[index].buffer[0];
    fifo[index].head = &fifo[index].buffer[0];
    G8RTOS_InitSemaphore(fifo[index].currentSize, 0);
    G8RTOS_InitSemaphore(fifo[index].mutex, 1);
    fifo[index].lostDataCount = 0;
}

int32_t G8RTOS_ReadFifo(uint32_t index)
{
    G8RTOS_WaitSemaphore(fifo[index].mutex);
    G8RTOS_WaitSemaphore(fifo[index].currentSize);

    int32_t data = *(fifo[index].head);
    if(fifo[index].head == &fifo[index].buffer[FIFOSIZE-1])
    {
        fifo[index].head = &fifo[index].buffer[0];
    }
    else
    {
        fifo[index].head++;
    }

    G8RTOS_SignalSemaphore(fifo[index].mutex);
    return data;
}

int32_t G8RTOS_WriteFifo(uint32_t index, int32_t data)
{
    if(fifo[index].currentSize == FIFOSIZE-1)
    {
        fifo[index].lostDataCount++;
        return -1;
    }

    *(fifo[index].tail) = data;

    if(fifo[index].tail == &fifo[index].buffer[FIFOSIZE-1])
    {
        fifo[index].tail = &fifo[index].buffer[0];
    }
    else
    {
        fifo[index].tail++;
    }

    G8RTOS_SignalSemaphore(fifo[index].currentSize);
    return 0;
}
