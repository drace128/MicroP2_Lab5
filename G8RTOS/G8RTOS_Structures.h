/*
 * G8RTOS_Structure.h
 *
 *  Created on: Jan 12, 2017
 *      Author: Raz Aloni
 */

#ifndef G8RTOS_STRUCTURES_H_
#define G8RTOS_STRUCTURES_H_

#include "G8RTOS.h"
#include "G8RTOS_Semaphores.h"
#include "stdbool.h"

/*********************************************** Data Structure Definitions ***********************************************************/

typedef uint32_t threadID_t;

/*
 *  Thread Control Block:
 *      - Every thread has a Thread Control Block
 *      - The Thread Control Block holds information about the Thread Such as the Stack Pointer, Priority Level, and Blocked Status
 *      - For Lab 2 the TCB will only hold the Stack Pointer, next TCB and the previous TCB (for Round Robin Scheduling)
 */
struct tcb {
    int32_t *sp;
    struct tcb *next;
    struct tcb *previous;
    semaphore_t *blocked;
    uint32_t sleepCNT;
    bool isAsleep;

    uint8_t priority;
    threadID_t ID;
    bool alive;
    char threadName[MAX_NAME_LENGTH];
};
typedef struct tcb tcb_t;

/*
 *  Periodic Thread:
 *      - Every Periodic thread has a Periodic Event struct
 *      - Information held contains a period, execution time, pointer to the next and previous periodic event
 */
struct p_event {
    void (*Handler)(void);
    uint32_t period;
    uint32_t executeTime;
    struct p_event *previous;
    struct p_event *next;
};
typedef struct p_event p_event_t;
/*********************************************** Data Structure Definitions ***********************************************************/


/*********************************************** Public Variables *********************************************************************/

tcb_t * CurrentlyRunningThread;

/*********************************************** Public Variables *********************************************************************/




#endif /* G8RTOS_STRUCTURES_H_ */
