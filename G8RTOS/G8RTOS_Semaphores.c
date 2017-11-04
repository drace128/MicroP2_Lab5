/*
 * G8RTOS_Semaphores.c
 */

/*********************************************** Dependencies and Externs *************************************************************/

#include <stdint.h>
#include <driverlib.h>
#include "msp.h"
#include "G8RTOS_Semaphores.h"
#include "G8RTOS_CriticalSection.h"
#include "G8RTOS_Scheduler.h"
#include "G8RTOS_Structures.h"

#define startContextSwitch() (SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk)


/*********************************************** Dependencies and Externs *************************************************************/


/*********************************************** Public Functions *********************************************************************/

/*
 * Initializes a semaphore to a given value
 * Param "s": Pointer to semaphore
 * Param "value": Value to initialize semaphore to
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_InitSemaphore(semaphore_t *s, int32_t value)
{
	/* Implement this */
    uint32_t state;
    state = StartCriticalSection();
    (*s) = value;
    EndCriticalSection(state);

}

/*
 * Waits for a semaphore to be available (value greater than 0)
 * 	- Decrements semaphore when available
 * 	- Spinlocks to wait for semaphore
 * Param "s": Pointer to semaphore to wait on
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_WaitSemaphore(semaphore_t *s)
{
	/* Implement this */

    uint32_t state;
    state = StartCriticalSection();
    (*s) = (*s) - 1;
    if((*s) < 0)
    {
        CurrentlyRunningThread->blocked = s;
        startContextSwitch();
    }
    EndCriticalSection(state);
}

/*
 * Signals the completion of the usage of a semaphore
 * 	- Increments the semaphore value by 1
 * Param "s": Pointer to semaphore to be signaled
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_SignalSemaphore(semaphore_t *s)
{
	/* Implement this */
    uint32_t state;
    tcb_t *ptr;
    state = StartCriticalSection();
    (*s) = (*s) + 1;
    if((*s) <= 0)
    {
        ptr = (tcb_t *)&CurrentlyRunningThread->next;
        while(ptr->blocked != s)
        {
            ptr = ptr->next;
        }
        ptr->blocked = 0;
    }
    EndCriticalSection(state);
}

/*********************************************** Public Functions *********************************************************************/


