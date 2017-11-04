/*
 * G8RTOS_Scheduler.c
 */

/*********************************************** Dependencies and Externs *************************************************************/

#include <stdint.h>
#include <driverlib.h>
#include <BSP.h>
#include "msp.h"
#include "G8RTOS_Scheduler.h"
#include "G8RTOS_Structures.h"
#define startContextSwitch() (SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk)

/*
 * G8RTOS_Start exists in asm
 */
extern void G8RTOS_Start();
extern uint8_t StartCriticalSection();
extern void EndCriticalSection(uint8_t state);

/* System Core Clock From system_msp432p401r.c */
extern uint32_t SystemCoreClock;

/*
 * Pointer to the currently running Thread Control Block
 */
extern tcb_t * CurrentlyRunningThread;

/*********************************************** Dependencies and Externs *************************************************************/


/*********************************************** Defines ******************************************************************************/

/* Status Register with the Thumb-bit Set */
#define THUMBBIT 0x01000000

/*********************************************** Defines ******************************************************************************/


/*********************************************** Data Structures Used *****************************************************************/

/* Thread Control Blocks
 *	- An array of thread control blocks to hold pertinent information for each thread
 */
static tcb_t threadControlBlocks[MAX_THREADS];

/* Periodic Events
 *  - An array of periodic events to hold pertinent information for each periodic event
 */
static p_event_t periodicEvents[MAX_P_EVENTS];

/* Thread Stacks
 *	- An array of arrays that will act as invdividual stacks for each thread
 */
static int32_t threadStacks[MAX_THREADS][STACKSIZE];


/*********************************************** Data Structures Used *****************************************************************/


/*********************************************** Private Variables ********************************************************************/

/*
 * Current Number of Threads currently in the scheduler
 */
static uint32_t NumberOfThreads;

/*
 * Current Number of Periodic Events currently in the scheduler
 */
static uint32_t NumberOfPerEvents;

static uint16_t IDCounter;


/*********************************************** Private Variables ********************************************************************/


/*********************************************** Private Functions ********************************************************************/

/*
 * Initializes the Systick and Systick Interrupt
 * The Systick interrupt will be responsible for starting a context switch between threads
 * Param "numCycles": Number of cycles for each systick interrupt
 */
static void InitSysTick(uint32_t numCycles)
{
	/* Implement this */
    MAP_SysTick_enableModule();
    //MAP_SysTick_setPeriod((ClockSys_GetSysFreq()/numCycles));
    MAP_SysTick_setPeriod(1500);
    MAP_SysTick_enableInterrupt();
}

/*
 * Chooses the next thread to run.
 * Lab 3 Scheduling Algorithm:
 * 	- Choose the next running thread to be the current like in Round Robin
 * 	- However, now we check if the thread is asleep or blocked
 * 	- if so, then continue iterating through threads until 1 is either not asleep or not blocked.
 */
void G8RTOS_Scheduler()
{
	/* Implement This */
    //CurrentlyRunningThread = CurrentlyRunningThread->next;
    tcb_t *tempNextThread, *threadToRun;
    uint16_t currentMaxPriority = 256;
    tempNextThread = CurrentlyRunningThread;
    do
    {
        tempNextThread = tempNextThread->next;
        if(!tempNextThread->isAsleep && !tempNextThread->blocked)
        {
            if(tempNextThread->priority < currentMaxPriority)
            {
                currentMaxPriority = tempNextThread->priority;
                threadToRun = tempNextThread;
            }
        }
    }while(CurrentlyRunningThread != tempNextThread);
    CurrentlyRunningThread = threadToRun;
}

/*
 * SysTick Handler
 * Increments the system time and handles both Periodic and Aperiodic Threads
 * Iterates through every periodic event checking if execute time == SystemTime
 * If so, then update the execution time and run the periodic event
 * Also runs through every aperiodic thread checking if they are asleep
 * If so, check if sleepCNT == SystemTime {wake it up}
 * Lastly, start a context switch
 */
void SysTick_Handler()
{
	/* Implement this */
    SystemTime++;
    int i;

/*Handling Periodic Threads*/
    p_event_t *pptr;
    pptr = &periodicEvents[0];
    for(i = 0; i < NumberOfPerEvents; i++)
    {
        if(pptr->executeTime == SystemTime)
        {
            pptr->executeTime = pptr->period + SystemTime;
            (*pptr->Handler)();
        }
        else
        {
            pptr = pptr->next;
        }
    }

/*Handling Sleep Counts on Non-Periodic Threads*/
    tcb_t *ptr;
    ptr = CurrentlyRunningThread;
    for(i = 0; i < NumberOfThreads; i++)
    {
        if(ptr->isAsleep)
        {
            if(ptr->sleepCNT == SystemTime)
            {
                ptr->isAsleep = false;
            }
            else
            {
                ptr = ptr->next;
            }
        }
        else
        {
            ptr = ptr->next;
        }
    }
    startContextSwitch();
}

/*********************************************** Private Functions ********************************************************************/


/*********************************************** Public Variables *********************************************************************/

/* Holds the current time for the whole System */
uint32_t SystemTime;

/*********************************************** Public Variables *********************************************************************/


/*********************************************** Public Functions *********************************************************************/

/*
 * Sets variables to an initial state (system time and number of threads)
 * Enables board for highest speed clock and disables watchdog
 */
void G8RTOS_Init()
{
	/* Implement this */
    uint32_t newVTORTable = 0x20000000;
    memcpy((uint32_t *)newVTORTable, (uint32_t *)SCB->VTOR, 57*4);  // 57 interrupt vectors to copy
    SCB->VTOR = newVTORTable;

    SystemTime = 0;
    NumberOfThreads = 0;
    IDCounter = 0;
    BSP_InitBoard();
}

/*
 * Starts G8RTOS Scheduler
 * 	- Initializes the Systick
 * 	- Sets Context to first thread
 * Returns: Error Code for starting scheduler. This will only return if the scheduler fails
 */
int G8RTOS_Launch()
{
    CurrentlyRunningThread = &threadControlBlocks[0];
    G8RTOS_Scheduler();
    InitSysTick(0);

    NVIC_SetPriority(PendSV_IRQn, OSINT_PRIORITY);
    NVIC_SetPriority(SysTick_IRQn, OSINT_PRIORITY);

    G8RTOS_Start();
    return 0;
}

/* initializes the stack for a given thread with
 * default values for the registers
 * Parameter: "threadNum" the specific thread to initialize the stack for
 */
void initStack(uint8_t threadNum)
{
    threadStacks[threadNum][STACKSIZE-1] = THUMBBIT;            //Status Register
  //threadStacks[threadNum][STACKSIZE-2] = FUNCTION_POINTER;    //Program Counter
    threadStacks[threadNum][STACKSIZE-3] = 0x14141414;          //Link Register
    threadStacks[threadNum][STACKSIZE-4] = 0x12121212;
    threadStacks[threadNum][STACKSIZE-5] = 0x33333333;
    threadStacks[threadNum][STACKSIZE-6] = 0x22222222;
    threadStacks[threadNum][STACKSIZE-7] = 0x11111111;
    threadStacks[threadNum][STACKSIZE-8] = 0x00000000;
    threadStacks[threadNum][STACKSIZE-9] = 0x11111111;
    threadStacks[threadNum][STACKSIZE-10] = 0x10101010;
    threadStacks[threadNum][STACKSIZE-11] = 0x99999999;
    threadStacks[threadNum][STACKSIZE-12] = 0x88888888;
    threadStacks[threadNum][STACKSIZE-13] = 0x77777777;
    threadStacks[threadNum][STACKSIZE-14] = 0x66666666;
    threadStacks[threadNum][STACKSIZE-15] = 0x55555555;
    threadStacks[threadNum][STACKSIZE-16] = 0x44444444;
}




/*
 * Adds threads to G8RTOS Scheduler
 * 	- Checks if there are still available threads to insert to scheduler
 * 	- Initializes the thread control block for the provided thread
 * 	- Initializes the stack for the provided thread to hold a "fake context"
 * 	- Sets stack tcb stack pointer to top of thread stack
 * 	- Sets up the next and previous tcb pointers in a round robin fashion
 * Param "threadToAdd": Void-Void Function to add as preemptable main thread
 * Returns: Error code for adding threads
 */
int G8RTOS_AddThread(void (*threadToAdd)(void), uint8_t priority, char *str)
{
    uint32_t state;
    state = StartCriticalSection();

    if(NumberOfThreads >= MAX_THREADS)
    {
        EndCriticalSection(state);
        return -1;
    }

    int i = 0;
    while(i < MAX_THREADS)
    {
        if(!threadControlBlocks[i].alive)
        {
            break;
        }
        i++;
        if(i == MAX_THREADS)
        {
            EndCriticalSection(state);
            return -1;
        }
    }

    if(NumberOfThreads == 0)      //first thread to add
    {
        threadControlBlocks[0].next = &threadControlBlocks[0];
        threadControlBlocks[0].previous = &threadControlBlocks[0];
    }
    else
    {
        threadControlBlocks[i].next = &threadControlBlocks[0];
        threadControlBlocks[i].previous = &threadControlBlocks[i-1];
        threadControlBlocks[i-1].next = &threadControlBlocks[i];
    }

    threadControlBlocks[i].sp = &threadStacks[i][STACKSIZE-16];
    threadControlBlocks[i].blocked = 0;
    threadControlBlocks[i].sleepCNT = 0;
    threadControlBlocks[i].isAsleep = false;

    IDCounter = IDCounter << 16;
    threadControlBlocks[i].ID = IDCounter | i;

    threadControlBlocks[i].priority = priority;
    threadControlBlocks[i].alive = true;
    initStack(i);
    threadStacks[i][STACKSIZE-2] = (uint32_t)threadToAdd;

    int k = 0;
    while(*str != 0)
    {
        threadControlBlocks[i].threadName[k] = *str++;
        k++;
    }

    NumberOfThreads++;
    IDCounter++;
    EndCriticalSection(state);
    return 0;
}

/*
 * Allows a thread to wait a prescribed amount of time.
 *  - Initialize CurrentlyRunningThread's sleep count
 *  - Put thread to sleep
 *  - Yield control of this thread to allow other threads to run(begin context switch)
 * Param "duration": time in milliseconds to sleep
 * Returns: Error code for sleeping thread
 */
int32_t G8RTOS_OS_Sleep(uint32_t duration)
{
    CurrentlyRunningThread->sleepCNT = duration + SystemTime;
    CurrentlyRunningThread->isAsleep = true;
    startContextSwitch();
    return 0;
}


/* Get the current thread's ID
 * Returns: threadID_t ID
 */
threadID_t G8RTOS_GetThreadID()
{
    return CurrentlyRunningThread->ID;
}


int32_t G8RTOS_KillThread(threadID_t threadID)
{
    uint32_t state;
    state = StartCriticalSection();

    if(NumberOfThreads == 1)
    {
        EndCriticalSection(state);
        return -1;
    }

    tcb_t *ptr_prev, *ptr_next;
    tcb_t *ptr = &threadControlBlocks[0];
    int i;
    for(i = 0; i < NumberOfThreads; i++)
    {
        if(ptr->ID == threadID)
        {
            break;
        }
        else
        {
            ptr = ptr->next;
        }
        if((i+1) == NumberOfThreads)
        {
            EndCriticalSection(state);
            return -1;
        }
    }
    ptr_prev = ptr->previous;
    ptr_next = ptr->next;

    ptr->alive = false;
    ptr_prev->next = ptr->next;
    ptr_next->previous = ptr->previous;

    if(ptr == CurrentlyRunningThread)
    {
        startContextSwitch();
    }

    NumberOfThreads--;
    EndCriticalSection(state);
    return 0;
}


int32_t G8RTOS_KillSelf()
{
    uint32_t state;
    state = StartCriticalSection();

    if(NumberOfThreads == 1)
    {
        EndCriticalSection(state);
        return -1;
    }

    tcb_t *ptr_prev, *ptr_next;
    tcb_t *ptr = CurrentlyRunningThread;

    //G8RTOS_Scheduler();

    ptr_prev = ptr->previous;
    ptr_next = ptr->next;

    ptr->alive = false;
    ptr_prev->next = ptr->next;
    ptr_next->previous = ptr->previous;

    startContextSwitch();

    NumberOfThreads--;
    EndCriticalSection(state);
    //return 0;
    while(1);
}



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

int32_t G8RTOS_AddPeriodicEvent(void (*eventToAdd)(void), uint32_t period)
{
    if(NumberOfPerEvents >= MAX_P_EVENTS)
    {
        return 0;
    }

    if(NumberOfPerEvents == 0)
    {
        periodicEvents[0].Handler = eventToAdd;
        periodicEvents[0].period = period;
        periodicEvents[0].executeTime = period + SystemTime;
        periodicEvents[0].previous = &periodicEvents[0];
        periodicEvents[0].next = &periodicEvents[0];
    }
    else
    {
        periodicEvents[NumberOfPerEvents].Handler = eventToAdd;
        periodicEvents[NumberOfPerEvents].period = period;
        periodicEvents[NumberOfPerEvents].executeTime = period + SystemTime + 1;
        periodicEvents[NumberOfPerEvents].previous = &periodicEvents[NumberOfPerEvents-1];
        periodicEvents[NumberOfPerEvents].next = &periodicEvents[0];
        periodicEvents[NumberOfPerEvents-1].next = &periodicEvents[NumberOfPerEvents];
    }

    NumberOfPerEvents++;
    return 0;
}



int32_t G8RTOS_AddAPeriodicEvent(void (*AthreadToAdd)(void), uint8_t priority, IRQn_Type IRQn)
{
    int32_t state;
    state = StartCriticalSection();
    if(IRQn <= PSS_IRQn || IRQn >= PORT6_IRQn){return -1;}
    if(priority > 6){return -1;}

    NVIC_SetVector(IRQn, AthreadToAdd);
    NVIC_SetPriority(IRQn, priority);
    NVIC_EnableIRQ(IRQn);
    EndCriticalSection(state);
    return 0;
}

/*
* Currently running thread kills all other threads
* Returns: Error Code for Removing Threads
*/
int32_t G8RTOS_KillAllOtherThreads()
{
    /* Critical Section */
    int i;
    int32_t PrimaskState = StartCriticalSection();

    /* Check if this is last thread (we cannot have all threads killed in this scheduler */
    if (NumberOfThreads == 1)
    {
        EndCriticalSection(PrimaskState);
        return -1;
    }

    tcb_t * threadPtr = CurrentlyRunningThread;
    for(i = 1; i < NumberOfThreads; i++)
    {
        /* Set alive bit of next TCB to false*/
        threadPtr->next->alive= false;

        threadPtr = threadPtr->next;
    }

    /* Update Number of Threads */
    NumberOfThreads = 1;

    /* Set currently running thread's next TCB to itself */
    CurrentlyRunningThread->next = CurrentlyRunningThread;

    /* Set currently running thread's previous TCB to itself */
    CurrentlyRunningThread->previous = CurrentlyRunningThread;

    EndCriticalSection(PrimaskState);
    return -1;
}



/*********************************************** Public Functions *********************************************************************/
