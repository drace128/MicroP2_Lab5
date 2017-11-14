
//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include "msp.h"
#include "BSP.h"
#include "G8RTOS.h"
#include <stdio.h>
#include <stdbool.h>
#include "Game.h"
#include "cc3100_usage.h"
#include "driverlib.h"
playerType player1, player2;
void main(void)
{
    G8RTOS_InitSemaphore(receiveDataSem, 0);
    player1 = Host;
     player2 = Client;
    // sl_Select()
   //  initCC3100(player1);        //initialize the host
    // initCC3100(player2);        //initialize the client

    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer
    G8RTOS_Init();


      G8RTOS_AddThread(&IdleThread, 255, "idle");
      G8RTOS_AddAPeriodicEvent(&LCD_tap, 5, PORT4_IRQn);

    //initCC3100(player1);
    //  G8RTOS_AddThread(&CreateGame, 1, "idle");
    // isHost= true;

     initCC3100(player2);
     G8RTOS_AddThread(&JoinGame, 1,"sfsf");
   //  isHost= false;

      G8RTOS_Launch();

    //G8RTOS_AddThread(&CreateGame, 1, "start");
   //CreateGame();
 //   G8RTOS_AddThread(&IdleThread, 255, "idle");
   // G8RTOS_AddThread(&CreateGame, 1, "idle");
   // G8RTOS_AddThread(&JoinGame, 1,"sfsf");
   // G8RTOS_AddThread(&ReceiveDataFromHost,1,"sfsf");
   // G8RTOS_AddAPeriodicEvent(&LCD_tap, 5, PORT4_IRQn);

    //G8RTOS_AddThread(&DrawObjects, 1, "draw");
    //G8RTOS_AddThread(&GenerateBall, 1, "genBall");
  //  G8RTOS_AddThread(&displayScore, 1, "genBall");
   // G8RTOS_AddThread(&ReadJoystickClient, 1, "joystick");
    while(1);
}



//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************
/*
#include "msp.h"
#include "BSP.h"
#include "G8RTOS.h"
#include <stdio.h>
#include <stdbool.h>
#include "Game.h"
#include "cc3100_usage.h"
#include "driverlib.h"

bool flag;
playerType player1, player2;
uint8_t buffer[100];

void runServer()
{
    //while(1)

        //try receiving from the client
        while(ReceiveData(buffer, sizeof(buffer)) < 0);
        //printf("received data: %s\n", buffer);
        LCD_Text(140, 120, buffer, LCD_WHITE);
        _u32 temp_IP;

        temp_IP = ((buffer[3]& 0x000000ff) << 24 )|((buffer[2]& 0x000000ff) << 16)|((buffer[1]& 0x000000ff) << 8)|((buffer[0]& 0x000000ff) );
        temp_IP = temp_IP;
          buffer[0] = 'A';
          buffer[1] ='B';
          buffer[2] = 'C';
          buffer[3] = 'D';

        SendData( buffer, temp_IP, sizeof(buffer));
}

void runClient(char *str)
{
    //try receiving from the host
    //int i = 0;
    //while(*str != 0)
    //{
        //buffer[i] = *str++;
        //i++;
    //}
    _u32 ClientIP = getLocalIP();
    buffer[0] = (ClientIP & 0x000000ff);
    buffer[1] = (ClientIP & 0x0000ff00) >> 8;
    buffer[2] = (ClientIP & 0x00ff0000) >> 16;
    buffer[3] = (ClientIP & 0xff000000) >> 24;
    printf("data being sent: %d\n", buffer);
    SendData(buffer, HOST_IP_ADDR, sizeof(buffer));
}

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer
    G8RTOS_Init();

    player1 = Host;
    player2 = Client;

    initCC3100(player1);        //initialize the host
   // initCC3100(player2);        //initialize the client

    runServer();
   // runClient("Hello World");
    while(1);
}



*/













//G8RTOS_InitFifo(0);
//G8RTOS_InitFifo(1);

//G8RTOS_AddThread(&StartGame, 50, "BEGIN");
//G8RTOS_AddAPeriodicEvent(&AperiodicTap, 5, PORT4_IRQn);


//G8RTOS_InitSemaphore(&sensorI2C, 1);
//G8RTOS_InitSemaphore(&ledI2C, 1);

//G8RTOS_AddThread(&task3, 100, "task3");















//G8RTOS_InitFifo(0);
//G8RTOS_InitFifo(1);

//G8RTOS_AddThread(&StartGame, 50, "BEGIN");
//G8RTOS_AddAPeriodicEvent(&AperiodicTap, 5, PORT4_IRQn);


//G8RTOS_InitSemaphore(&sensorI2C, 1);
//G8RTOS_InitSemaphore(&ledI2C, 1);

//G8RTOS_AddThread(&task3, 100, "task3");
