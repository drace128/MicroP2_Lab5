//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include "msp.h"
#include "BSP.h"
#include "G8RTOS.h"
#include <stdio.h>
#include "Game.h"

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer
    G8RTOS_Init();

    //G8RTOS_AddThread(&CreateGame, 1, "start");
   //CreateGame();
    G8RTOS_AddThread(&IdleThread, 255, "idle");
    G8RTOS_AddThread(&CreateGame, 1, "idle");
    G8RTOS_AddAPeriodicEvent(&LCD_tap, 5, PORT4_IRQn);
    G8RTOS_Launch();
    //G8RTOS_AddThread(&DrawObjects, 1, "draw");
    //G8RTOS_AddThread(&GenerateBall, 1, "genBall");
  //  G8RTOS_AddThread(&displayScore, 1, "genBall");
   // G8RTOS_AddThread(&ReadJoystickClient, 1, "joystick");


    while(1);
}

















//G8RTOS_InitFifo(0);
//G8RTOS_InitFifo(1);

//G8RTOS_AddThread(&StartGame, 50, "BEGIN");
//G8RTOS_AddAPeriodicEvent(&AperiodicTap, 5, PORT4_IRQn);


//G8RTOS_InitSemaphore(&sensorI2C, 1);
//G8RTOS_InitSemaphore(&ledI2C, 1);

//G8RTOS_AddThread(&task3, 100, "task3");
