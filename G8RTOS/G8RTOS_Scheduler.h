/*
 * G8RTOS_Scheduler.h
 */

#ifndef G8RTOS_SCHEDULER_H_
#define G8RTOS_SCHEDULER_H_

#include "msp.h"
/*********************************************** Sizes and Limits *********************************************************************/
#define MAX_THREADS 25
#define MAX_P_EVENTS 6
#define STACKSIZE 512
#define OSINT_PRIORITY 7
#define MAX_NAME_LENGTH 16

/*********************************************** Sizes and Limits *********************************************************************/

/*********************************************** Public Variables *********************************************************************/

/* Holds the current time for the whole System */
extern uint32_t SystemTime;

/*********************************************** Public Variables *********************************************************************/


/*********************************************** Public Functions *********************************************************************/

/*
 * Initializes variables and hardware for G8RTOS usage
 */
void G8RTOS_Init();

/*
 * Starts G8RTOS Scheduler
 * 	- Initializes Systick Timer
 * 	- Sets Context to first thread
 * Returns: Error Code for starting scheduler. This will only return if the scheduler fails
 */
int32_t G8RTOS_Launch();

/*
 * Adds threads to G8RTOS Scheduler
 * 	- Checks if there are stil available threads to insert to scheduler
 * 	- Initializes the thread control block for the provided thread
 * 	- Initializes the stack for the provided thread
 * 	- Sets up the next and previous tcb pointers in a round robin fashion
 * Param "threadToAdd": Void-Void Function to add as preemptable main thread
 * Returns: Error code for adding threads
 */
int32_t G8RTOS_AddThread(void (*threadToAdd)(void), uint8_t priority, char *str);

/*
 * Allows a thread to wait a prescribed amount of time.
 *  - Initialize CurrentlyRunningThread's sleep count
 *  - Put thread to sleep
 *  - Yield control of this thread to allow other threads to run(begin context switch)
 * Param "duration": time in milliseconds to sleep
 * Returns: Error code for sleeping thread
 */
int32_t G8RTOS_OS_Sleep(uint32_t duration);

/*
 * Adds Periodic Events to G8RTOS Scheduler
 *  - Checks if there are still available events to insert to scheduler
 *  - Initializes the Periodic Event list for the provided event
 *  - Sets the function pointer to the function pointer field of the struct
 *  - Sets the period for the event, and its execution time.
 *  - Sets up the next and previous pointers in a round robin fashion
 * Param "eventToAdd": Void-Void Function to add as preemptable main event
 * Returns: Error code for adding event
 */

int32_t G8RTOS_AddPeriodicEvent(void (*eventToAdd)(void), uint32_t period);


int32_t G8RTOS_AddAPeriodicEvent(void (*AthreadToAdd)(void), uint8_t priority, IRQn_Type IRQn);

int32_t G8RTOS_KillAllOtherThreads();

int32_t G8RTOS_KillSelf();

/*********************************************** Public Functions *********************************************************************/

#endif /* G8RTOS_SCHEDULER_H_ */
