/*
 * game.c
 *
 *  Created on: Nov 7, 2017
 *      Author: brons
 */
#include "msp.h"
#include "game.h"
#include "BSP.h"
#include "G8RTOS.h"
#include "time.h"
#include "stdbool.h"
#include "cc3100_usage.h"
#include "leds.h"
#define winnigScore 100

int16_t xCoord, yCoord;
GeneralPlayerInfo_t host, client;
PrevPlayer_t oldhost, oldclient;
Ball_t balls[MAX_NUM_OF_BALLS];
uint16_t numberOfBalls = 0;
PrevBall_t oldballs[MAX_NUM_OF_BALLS];
int hostScore, clientScore;
int clientScoreChanged;
int HostScoreChanged;
int gb= false;
int dob= false;
int mb= false;
int rj= false;
int ub= false;
int up= false;
int ClientChoosed=0;
int HostChoosed=0;
uint16_t global_x;
uint16_t global_y;
uint16_t LED_HOST=0;
uint16_t LED_CLIENT=0;
int listen_to_restart_flag =0;
int restart=0;
bool isHost = true;

GameState_t game;
SpecificPlayerInfo_t player1;
uint8_t buffer[100];

void fillBuffer(uint8_t * buffer, uint8_t numOfBytes, int data, int index)
{
    if(numOfBytes == 1)
    {
        buffer[index] = data;
    }
    else if(numOfBytes == 2)
    {
        buffer[index+1] = data >> 8;
        buffer[index] = data;
    }
    else if(numOfBytes == 3)
    {
        buffer[index] = data >> 16;
        buffer[index+1] = data >> 8;
        buffer[index+2] = data;
    }
    else if(numOfBytes == 4)
    {
        buffer[index] = data >> 24;
        buffer[index+1] = data >> 16;
        buffer[index+2] = data >> 8;
        buffer[index+3] = data;
    }
}


inline int16_t emptyBuffer_16(uint8_t * buffer, int index)
{
    int16_t receivedData = (buffer[index+1]&0x00FF << 8)| buffer[index]&0x00FF ;
    return receivedData;
}



//The JoinGame thread is like CreateGame for the client

void JoinGame()
{

    isHost = false;
    host.color = PLAYER_RED;
    host.currentCenter = PADDLE_X_CENTER;
    host.position = TOP;
    client.color = PLAYER_BLUE;
    client.currentCenter = PADDLE_X_CENTER;
    client.position = BOTTOM;
    oldhost.Center = host.currentCenter;
    oldclient.Center = client.currentCenter;
    LCD_Clear(LCD_BLACK);
    LCD_DrawRectangle(37, 39, ARENA_MIN_Y, ARENA_MAX_Y, LCD_WHITE);
    LCD_DrawRectangle(ARENA_MIN_X, ARENA_MAX_X, ARENA_MIN_Y, ARENA_MAX_Y, LCD_GREEN);
    LCD_DrawRectangle(281, 283, ARENA_MIN_Y, ARENA_MAX_Y, LCD_WHITE);
    LCD_Text(10, 4, "00", LCD_RED);
    LCD_Text(10, 220, "00", LCD_BLUE);
    LCD_DrawRectangle(128, 128+PADDLE_LEN, ARENA_MIN_Y, ARENA_MIN_Y+PADDLE_WID, PLAYER_RED);
    LCD_DrawRectangle(128, 128+PADDLE_LEN, ARENA_MAX_Y-PADDLE_WID, ARENA_MAX_Y, PLAYER_BLUE);

    /* player1.IP_address = getLocalIP();
    player1.displacement = 0;
    player1.ready = false;
    player1.joined = false;
    player1.acknowledge = false;

    fillBuffer(buffer, 4, player1.IP_address, 0);
    fillBuffer(buffer, 2, player1.displacement, 4);
    fillBuffer(buffer, 1, player1.ready, 6);
    fillBuffer(buffer, 1, player1.joined, 7);
    fillBuffer(buffer, 1, player1.acknowledge, 8);
     */
    // SendData(buffer, HOST_IP_ADDR, sizeof(buffer));

    //   while(ReceiveData(buffer, sizeof(buffer)) < 0);
    //show connection with an LED...

    //  G8RTOS_AddThread(&ReadJoystickClient, 1, "clientJoy");
    // G8RTOS_AddThread(&SendDataToHost, 1, "dataToHost");
    G8RTOS_AddThread(&ReceiveDataFromHost, 1, "dataFromHost");
    G8RTOS_AddThread(&DrawObjects, 1, "drawObjects");
    //G8RTOS_AddThread(&MoveLEDs, 1, "LEDs");

    //   G8RTOS_AddThread(&IdleThread, 255, "idle");

    G8RTOS_KillSelf();
    while(1);
}

void ReceiveDataFromHost()
{
    _u32 ClientIP = getLocalIP();
    buffer[0] = (ClientIP & 0x000000ff);
    buffer[1] = (ClientIP & 0x0000ff00) >> 8;
    buffer[2] = (ClientIP & 0x00ff0000) >> 16;
    buffer[3] = (ClientIP & 0xff000000) >> 24;
    //  printf("data being sent: %d\n", buffer);
    SendData(buffer, HOST_IP_ADDR, sizeof(buffer));

    while(1)
    {
     //  while(buffer[0]== buffer[2]){
        while(ReceiveData(buffer, 20) < 0);
        {


            //G8RTOS_SignalSemaphore(something);
            //G8RTOS_WaitSemaphore(something);

           // G8RTOS_OS_Sleep(20);




       }
        G8RTOS_OS_Sleep(5);
        SendData( buffer, HOST_IP_ADDR, 20);
        // G8RTOS_WaitSemaphore(receiveDataSem);

        client.currentCenter= emptyBuffer_16(buffer, 0);
        host.currentCenter=client.currentCenter;

            int j = 2;
                for (int i = 0 ; i < MAX_NUM_OF_BALLS ; i++ ){
                     balls[i].currentCenterX= emptyBuffer_16(buffer, j);
                    j+=2;
                   balls[i].currentCenterY =emptyBuffer_16(buffer, j);
                   j+=2;

                    balls[i].alive= 1;
                    j++;

                }

        // G8RTOS_SignalSemaphore(receiveDataSem);
        // host.position = buffer[2];
        //   printf("currentCenter: %d\n",  buffer[0]);
        // printf("position: %d\n", buffer[2]);
        //manipulate the received data

        if(game.gameDone)
        {
            G8RTOS_AddThread(&EndOfGameClient, 1, "endall");
        }

        G8RTOS_OS_Sleep(5);
    }
}

<<<<<<< HEAD
void SendDataToClient(){

    while(ReceiveData(buffer, sizeof(buffer)) < 0){
        G8RTOS_OS_Sleep(10);

    }
    //printf("received data: %s\n", buffer);
    // LCD_Text(140, 120, buffer, LCD_WHITE);
    _u32 temp_IP;

    temp_IP = ((buffer[3]& 0x000000ff) << 24 )|((buffer[2]& 0x000000ff) << 16)|((buffer[1]& 0x000000ff) << 8)|((buffer[0]& 0x000000ff) );
    temp_IP = temp_IP;
    while(1){

        // fillBuffer(buffer, 4, player1.IP_address, 0);
        //  SpecificPlayerInfo_t player;
        //  GeneralPlayerInfo_t players[MAX_NUM_OF_PLAYERS];


        //game.players[0].currentCenter = host.currentCenter;
        // fillBuffer(buffer, 2, 8, 0);

      //  game.players[0].position = host.position;
        // fillBuffer(buffer, 2,9, 2);
        //buffer[0]= host.currentCenter;
        //fillBuffer(buffer,  2, host.currentCenter, 0);
        buffer[1] = host.currentCenter >> 8;
        buffer[0] = host.currentCenter;

        buffer[3] = balls[0].currentCenterX >> 8;
        buffer[2] = balls[0].currentCenterX ;

        buffer[5] =  balls[0].currentCenterY >> 8;
        buffer[4] =  balls[0].currentCenterY;

        buffer[7] =  balls[0].alive >> 8;

        buffer[9] = balls[1].currentCenterX >> 8;
        buffer[10] = balls[1].currentCenterX ;

        buffer[12] =  balls[1].currentCenterY >> 8;
        buffer[11] =  balls[1].currentCenterY;

       buffer[13] =  balls[1].alive >> 8;



       /* int j = 2;
        for (int i = 0 ; i < MAX_NUM_OF_BALLS ; i++ ){
            fillBuffer(buffer,  2, balls[i].currentCenterX, j);
            j+=2;
            fillBuffer(buffer,  2,balls[i].currentCenterY, j);
            j+=2;
           fillBuffer(buffer,  1, balls[i].alive, j);
           j++;*/

      //  buffer[1]= host.position;
       // int j = 2;
        /*  for (int i = 0 ; i < MAX_NUM_OF_BALLS ; i++ ){
            buffer[j] = balls[i].currentCenterX;
            j++;
            buffer[j] = balls[i].currentCenterY;
            j++;
            buffer[j] = balls[i].alive;
            j++;
            buffer[j] = balls[i].color;
            j++;

        }*/

        SendData( buffer, temp_IP, 20);
        G8RTOS_OS_Sleep(5);
        while(ReceiveData(buffer, 20)) ;
        //  game.players[0].color = host.color;


        // game.players[1].currentCenter = client.currentCenter;
        // game.players[1].position = client.position;
        //  game.players[1].color = client.color;

        //  &buffer = &game.player;

        //  fillBuffer(game, 4, player1.IP_address, 0);

        G8RTOS_OS_Sleep(5);

        // fillBuffer(buffer, 4, player1.IP_address, 0);
        //  Ball_t balls[MAX_NUM_OF_BALLS];
        //  for (8){

        //  }
        //  uint16_t numberOfBalls;
        //   fillBuffer(buffer, 4, player1.IP_address, 0);
        //   bool winner;
        //   fillBuffer(buffer, 4, player1.IP_address, 0);
        //   bool gameDone;
        //   fillBuffer(buffer, 4, player1.IP_address, 0);
        //    uint8_t LEDScores[2];
        //   fillBuffer(buffer, 4, player1.IP_address, 0);
        //    uint8_t overallScores[2];
        //   fillBuffer(buffer, 4, player1.IP_address, 0);
        //    fillBuffer(buffer, 4, player1.IP_address, 0);

    }

}

=======
GameState_t game;
SpecificPlayerInfo_t player1;
uint8_t buffer[100];

void fillBuffer(uint8_t * buffer, uint8_t numOfBytes, int data, int index)
{
    if(numOfBytes == 1)
    {
        buffer[index] = data;
    }
    else if(numOfBytes == 2)
    {
        buffer[index] = data >> 8;
        buffer[index+1] = data;
    }
    else if(numOfBytes == 3)
    {
        buffer[index] = data >> 16;
        buffer[index+1] = data >> 8;
        buffer[index+2] = data;
    }
    else if(numOfBytes == 4)
    {
        buffer[index] = data >> 24;
        buffer[index+1] = data >> 16;
        buffer[index+2] = data >> 8;
        buffer[index+3] = data;
    }
}

//The JoinGame thread is like CreateGame for the client

void JoinGame()
{
    player1.IP_address = getLocalIP();
    player1.displacement = 0;
    player1.ready = false;
    player1.joined = false;
    player1.acknowledge = false;

    fillBuffer(buffer, 4, player1.IP_address, 0);
    fillBuffer(buffer, 2, player1.displacement, 4);
    fillBuffer(buffer, 1, player1.ready, 6);
    fillBuffer(buffer, 1, player1.joined, 7);
    fillBuffer(buffer, 1, player1.acknowledge, 8);

    SendData(buffer, HOST_IP_ADDR, sizeof(buffer));

    while(ReceiveData(buffer, sizeof(buffer)) < 0);
    //show connection with an LED...

    G8RTOS_AddThread(&ReadJoystickClient, 1, "clientJoy");
    G8RTOS_AddThread(&SendDataToHost, 1, "dataToHost");
    G8RTOS_AddThread(&ReceiveDataFromHost, 1, "dataFromHost");
    G8RTOS_AddThread(&DrawObjects, 1, "drawObjects");
    //G8RTOS_AddThread(&MoveLEDs, 1, "LEDs");
    G8RTOS_AddThread(&IdleThread, 255, "idle");

    G8RTOS_KillSelf();
}

void ReceiveDataFromHost()
{
    while(1)
    {
        while(ReceiveData(buffer, sizeof(buffer)) < 0);
        {
            //G8RTOS_SignalSemaphore(something);
            //G8RTOS_WaitSemaphore(something);
            G8RTOS_OS_Sleep(1);
        }
        //manipulate the received data

        if(game.gameDone)
        {
            G8RTOS_AddThread(&EndOfGameClient, 1, "endall");
        }

        G8RTOS_OS_Sleep(5);
    }
}

>>>>>>> 0f61f3c5ed6bab13b57f0372e10606d11ffcb2d3
void SendDataToHost()
{
    while(1)
    {
<<<<<<< HEAD
       // fillBuffer(buffer, 4, player1.IP_address, 0);
=======
        fillBuffer(buffer, 4, player1.IP_address, 0);
>>>>>>> 0f61f3c5ed6bab13b57f0372e10606d11ffcb2d3
        fillBuffer(buffer, 2, player1.displacement, 4);
        fillBuffer(buffer, 1, player1.ready, 6);
        fillBuffer(buffer, 1, player1.joined, 7);
        fillBuffer(buffer, 1, player1.acknowledge, 8);
        SendData(buffer, HOST_IP_ADDR, sizeof(buffer));
        G8RTOS_OS_Sleep(2);
    }
}
<<<<<<< HEAD

void EndOfGameClient()
{

=======

void EndOfGameClient()
{

>>>>>>> 0f61f3c5ed6bab13b57f0372e10606d11ffcb2d3
}

void GenerateBall()
{
    while(1)
    {
        gb = 1;
        if(numberOfBalls < MAX_NUM_OF_BALLS)
        {
            G8RTOS_AddThread(&MoveBall, 1, "MoveBall");
            numberOfBalls++;
        }
        gb = 0;
        G8RTOS_OS_Sleep(37443);      /*sleep proportional to numOfBalls later*/
    }
}

void MoveBall()
{
    bool touched =false;
    uint8_t randomX = 0;
    uint8_t randomY = 0;
    uint8_t directionX = 0;          //right = 1, left = 0, none = 2
    uint8_t directionY = 0;          //down = 1, up = 0
    int i;

    for(i = 0; i < MAX_NUM_OF_BALLS; i++)
    {
        if(!balls[i].alive)
        {
            balls[i].alive = true;
            balls[i].color = LCD_WHITE;
            break;
        }
    }

    randomX = (xCoord & 0x003f) + 128;
    randomY = (yCoord & 0x003f) + 128;

    directionX = (xCoord & 0x11);
    directionY = (yCoord & 0x11);

    //    randomX = 160;
    //    randomY = 160;
    //    directionX = 1;
    //    directionY = 0;

    balls[i].currentCenterX = randomX;
    balls[i].currentCenterY = randomY;

    oldballs[i].CenterX = randomX;
    oldballs[i].CenterY = randomY;

    while(1)
    {
        mb= 1;
        switch(checkForCollision(&balls[i]))
        {
        case 1:
            directionX = 1;
            break;
        case 2:
            directionX = 0;
            break;
        case 4:
            // directionY = 1;
            if (touched){
                clientScore++;
                clientScoreChanged = true;
            }
            balls[i].alive = false;
            numberOfBalls--;
            //  LCD_DrawRectangle(balls[i].currentCenterX-2, balls[i].currentCenterX+2,0, 2, LCD_GREEN);

            G8RTOS_KillSelf();
            break;
        case 3:
            // directionY = 0;
            if (touched){
                hostScore++;
                HostScoreChanged = true;
            }
            balls[i].alive = false;
            numberOfBalls--;
            G8RTOS_KillSelf();



            break;
        case 5:
            directionX = 2;
            directionY = 1;
            balls[i].color = LCD_RED;
            touched = true;
            break;
        case 6:
            directionX = 2;
            directionY = 0;
            balls[i].color = LCD_BLUE;
            touched = true;
            break;
        case 7:
            directionX = 1;
            directionY = 1;
            balls[i].color = LCD_RED;
            touched = true;
            break;
        case 8:
            directionX = 1;
            directionY = 0;
            balls[i].color = LCD_BLUE;
            touched = true;
            break;
        case 9:
            directionX = 0;
            directionY = 1;
            balls[i].color = LCD_RED;
            touched = true;
            break;
        case 10:
            directionX = 0;
            directionY = 0;
            balls[i].color = LCD_BLUE;
            touched = true;
            break;
        case 11:

            balls[i].color = LCD_GREEN;

            break;
        case 12:

            balls[i].color = LCD_RED;
            balls[i].RED_flag = true;

            break;
        case 13:

            if (touched){


                hostScore++;
                HostScoreChanged = true;

            }
            balls[i].alive = false;
            numberOfBalls--;
            G8RTOS_KillSelf();
            break;




        case 14:

            balls[i].color = LCD_BLUE;
            balls[i].GREEN_flag = true;

            break;

        case 15:

            if (touched){

                clientScore++;
                clientScoreChanged = true;

            }
            balls[i].alive = false;
            numberOfBalls--;
            G8RTOS_KillSelf();
            break;
        }



        if(directionX == 1){balls[i].currentCenterX++;}
        else if(directionX == 0){balls[i].currentCenterX--;}

        if(directionY == 1){balls[i].currentCenterY++;}
        else{balls[i].currentCenterY--;}
        mb= 0;
        G8RTOS_OS_Sleep(481);
    }
}

void ReadJoystickClient()
{
    while(1)
    {
        rj =1;
        GetJoystickCoordinates(&xCoord, &yCoord);
        if(xCoord > 1000){
            if (client.currentCenter > 72){
                host.currentCenter-=2;
                client.currentCenter-=2;     //temporarily used to mirror paddle
            }
        }
        if(xCoord < -1000){
            if (client.currentCenter < (320-72)){
                host.currentCenter+=2;
                client.currentCenter+=2;     //temporarily used to mirror paddle
            }
        }
        rj = 0;
        G8RTOS_OS_Sleep(401);
    }
}

void DrawObjects()
{
    int i = 0;
    while(1)
    {

        //while(i < 4)
        //{
        if(balls[i].alive )
        {
            UpdateBallOnScreen(&oldballs[i], &balls[i], balls[i].color);
        }


        //}
        if(i != 6) UpdatePlayerOnScreen(&oldhost, &host);
        if(i != 6)  UpdatePlayerOnScreen(&oldclient, &client);
        if(i == 6) displayScore();
        dob =0;
        i++;
        if (i == 8) i =0;
        G8RTOS_OS_Sleep(21);
    }
}

void CreateGame()
{
    isHost = true;
    host.color = PLAYER_RED;
    host.currentCenter = PADDLE_X_CENTER;
    host.position = TOP;
    client.color = PLAYER_BLUE;
    client.currentCenter = PADDLE_X_CENTER;
    client.position = BOTTOM;
    oldhost.Center = host.currentCenter;
    oldclient.Center = client.currentCenter;
    LCD_Clear(LCD_BLACK);
    LCD_DrawRectangle(37, 39, ARENA_MIN_Y, ARENA_MAX_Y, LCD_WHITE);
    LCD_DrawRectangle(ARENA_MIN_X, ARENA_MAX_X, ARENA_MIN_Y, ARENA_MAX_Y, LCD_GREEN);
    LCD_DrawRectangle(281, 283, ARENA_MIN_Y, ARENA_MAX_Y, LCD_WHITE);
    //   LCD_DrawRectangle(128, 128+PADDLE_LEN, ARENA_MIN_Y, ARENA_MIN_Y+PADDLE_WID, PLAYER_RED);
    //  LCD_DrawRectangle(128, 128+PADDLE_LEN, ARENA_MAX_Y-PADDLE_WID, ARENA_MAX_Y, PLAYER_BLUE);

    LCD_Text(10, 4, "00", LCD_RED);
    LCD_Text(10, 220, "00", LCD_BLUE);

    LCD_DrawRectangle(100, 220, 50, 90, LCD_RED);
    LCD_Text(145,65, "Host", LCD_WHITE);
    LCD_DrawRectangle(100, 220, 130, 170, LCD_BLUE);
    LCD_Text(140,145, "Client", LCD_WHITE);
    hostScore =0;
    clientScore=0;
    clientScoreChanged= false;
    HostScoreChanged= false;
    init_GPIO();
    init_I2C();
    initI2C_ledsOFF();

    while(1){

        if (HostChoosed==1){

            LCD_DrawRectangle(90, 230, 40,180, LCD_GREEN);
            LCD_Text(160,120, "3", LCD_RED);
            G8RTOS_OS_Sleep(30000);
            LCD_DrawRectangle(90, 230, 40,180, LCD_GREEN);
            LCD_Text(160,120, "2", LCD_RED);
            G8RTOS_OS_Sleep(30000);
            LCD_DrawRectangle(90, 230, 40,180, LCD_GREEN);
            LCD_Text(160,120, "1", LCD_RED);
            G8RTOS_OS_Sleep(30000);
            LCD_DrawRectangle(90, 230, 40,180, LCD_GREEN);
            LCD_Text(155,120, "GO!", LCD_RED);
            G8RTOS_OS_Sleep(30000);
            LCD_DrawRectangle(90, 230, 40,180, LCD_GREEN);

            LCD_DrawRectangle(128, 128+PADDLE_LEN, ARENA_MIN_Y, ARENA_MIN_Y+PADDLE_WID, PLAYER_RED);
            LCD_DrawRectangle(128, 128+PADDLE_LEN, ARENA_MAX_Y-PADDLE_WID, ARENA_MAX_Y, PLAYER_BLUE);
            G8RTOS_AddThread(&DrawObjects, 1, "draw");
            G8RTOS_AddThread(&GenerateBall, 1, "genBall");

            G8RTOS_AddThread(&ReadJoystickClient, 1, "joystick");
            G8RTOS_AddThread(&SendDataToClient,1,"sendingdatatoclient");
            G8RTOS_KillSelf();


        }

        if (ClientChoosed==1){

            LCD_DrawRectangle(90, 230, 40,180, LCD_GREEN);
            LCD_Text(160,120, "3", LCD_BLUE);
            G8RTOS_OS_Sleep(30000);
            LCD_DrawRectangle(90, 230, 40,180, LCD_GREEN);
            LCD_Text(160,120, "2", LCD_BLUE);
            G8RTOS_OS_Sleep(30000);
            LCD_DrawRectangle(90, 230, 40,180, LCD_GREEN);
            LCD_Text(160,120, "1", LCD_BLUE);
            G8RTOS_OS_Sleep(30000);
            LCD_DrawRectangle(90, 230, 40,180, LCD_GREEN);
            LCD_Text(155,120, "GO!", LCD_BLUE);
            G8RTOS_OS_Sleep(30000);
            LCD_DrawRectangle(90, 230, 40,180, LCD_GREEN);
            LCD_DrawRectangle(90, 230, 40,180, LCD_GREEN);
            LCD_DrawRectangle(128, 128+PADDLE_LEN, ARENA_MIN_Y, ARENA_MIN_Y+PADDLE_WID, PLAYER_RED);
            LCD_DrawRectangle(128, 128+PADDLE_LEN, ARENA_MAX_Y-PADDLE_WID, ARENA_MAX_Y, PLAYER_BLUE);
            G8RTOS_AddThread(&DrawObjects, 1, "draw");
            G8RTOS_AddThread(&GenerateBall, 1, "genBall");
            G8RTOS_AddThread(&SendDataToClient,1,"sendingdatatoclient");
            G8RTOS_AddThread(&ReadJoystickClient, 1, "joystick");
            G8RTOS_KillSelf();

        }

    };

    //    G8RTOS_AddThread(&DrawObjects, 1, "draw");
    //    G8RTOS_AddThread(&ReadJoystickClient, 1, "joystick");
    //    G8RTOS_AddThread(&IdleThread, 255, "idle");
    //    G8RTOS_AddThread(&GenerateBall, 1, "genBall");

    //    G8RTOS_KillSelf();
    //    while(1);
}

void UpdatePlayerOnScreen(PrevPlayer_t * prevPlayerIn, GeneralPlayerInfo_t * outPlayer)
{
    if (isHost==true){
        if(outPlayer->currentCenter != prevPlayerIn->Center   )
        {

            if (outPlayer->currentCenter > prevPlayerIn->Center){       //moving right
                if (outPlayer->position == BOTTOM){
                    LCD_DrawRectangle(outPlayer->currentCenter-(PADDLE_LEN_D2+4), outPlayer->currentCenter-PADDLE_LEN_D2, 0, PADDLE_WID, LCD_GREEN);
                    LCD_DrawRectangle(outPlayer->currentCenter+(PADDLE_LEN_D2-4), outPlayer->currentCenter+PADDLE_LEN_D2, 0, PADDLE_WID, PLAYER_RED);
                }else
                {
                    LCD_DrawRectangle(outPlayer->currentCenter-(PADDLE_LEN_D2+4), outPlayer->currentCenter-PADDLE_LEN_D2, 240-PADDLE_WID, 240, LCD_GREEN);
                    LCD_DrawRectangle(outPlayer->currentCenter+(PADDLE_LEN_D2-4), outPlayer->currentCenter+PADDLE_LEN_D2, 240-PADDLE_WID, 240, PLAYER_BLUE);
                }

            }
            else        //moving left
            {
                if (outPlayer->position == BOTTOM){
                    LCD_DrawRectangle(outPlayer->currentCenter+PADDLE_LEN_D2, outPlayer->currentCenter+(PADDLE_LEN_D2+4), 0, PADDLE_WID, LCD_GREEN);
                    LCD_DrawRectangle(outPlayer->currentCenter-PADDLE_LEN_D2, outPlayer->currentCenter-(PADDLE_LEN_D2-4), 0, PADDLE_WID, PLAYER_RED);
                }else
                {
                    LCD_DrawRectangle(outPlayer->currentCenter+PADDLE_LEN_D2, outPlayer->currentCenter+(PADDLE_LEN_D2+4), 240-PADDLE_WID, 240, LCD_GREEN);
                    LCD_DrawRectangle(outPlayer->currentCenter-PADDLE_LEN_D2, outPlayer->currentCenter-(PADDLE_LEN_D2-4), 240-PADDLE_WID, 240, PLAYER_BLUE);
                }
            }
            prevPlayerIn->Center = outPlayer->currentCenter;
        }
    }else
    {

        int prevPos = prevPlayerIn->Center;
        int xPos = outPlayer->currentCenter;


        if(prevPos != xPos   )
        {

            if (xPos> prevPlayerIn->Center){       //moving right
                if (outPlayer->position == BOTTOM){
                    LCD_DrawRectangle(prevPos-(PADDLE_LEN_D2+4), prevPos-PADDLE_LEN_D2, 0, PADDLE_WID, LCD_GREEN);
                    LCD_DrawRectangle(prevPos+(PADDLE_LEN_D2-4), prevPos+PADDLE_LEN_D2, 0, PADDLE_WID, PLAYER_RED);
                }else
                {
                    LCD_DrawRectangle(prevPos-(PADDLE_LEN_D2+4), prevPos-PADDLE_LEN_D2, 240-PADDLE_WID, 240, LCD_GREEN);
                    LCD_DrawRectangle(prevPos+(PADDLE_LEN_D2-4), prevPos+PADDLE_LEN_D2, 240-PADDLE_WID, 240, PLAYER_BLUE);
                }
                prevPos+=4;
            }
            else        //moving left
            {
                if (outPlayer->position == BOTTOM){
                    LCD_DrawRectangle(prevPos+PADDLE_LEN_D2, prevPos+(PADDLE_LEN_D2+4), 0, PADDLE_WID, LCD_GREEN);
                    LCD_DrawRectangle(prevPos-PADDLE_LEN_D2, prevPos-(PADDLE_LEN_D2-4), 0, PADDLE_WID, PLAYER_RED);
                }else
                {
                    LCD_DrawRectangle(prevPos+PADDLE_LEN_D2, prevPos+(PADDLE_LEN_D2+4), 240-PADDLE_WID, 240, LCD_GREEN);
                    LCD_DrawRectangle(prevPos-PADDLE_LEN_D2, prevPos-(PADDLE_LEN_D2-4), 240-PADDLE_WID, 240, PLAYER_BLUE);
                }
                prevPos-=4;
            }
            prevPlayerIn->Center = prevPos;
        }
    }

}

void UpdateBallOnScreen(PrevBall_t * previousBall, Ball_t * currentBall, uint16_t outColor)
{
    ub = 1;
    if (previousBall->CenterY ==7)previousBall->CenterY =8;
    if (previousBall->CenterY ==233)previousBall->CenterY =232;
    if (previousBall->CenterX ==42)previousBall->CenterX =43;
    if (previousBall->CenterX ==320-42)previousBall->CenterX =320-43;

    if(previousBall->CenterX !=  currentBall->currentCenterX || previousBall->CenterY !=  currentBall->currentCenterY){
        if (currentBall->RED_flag){
            LCD_DrawRectangle(previousBall->CenterX-6, previousBall->CenterX+6,5,6, LCD_GREEN);
            LCD_DrawRectangle(previousBall->CenterX-4, previousBall->CenterX+4,0,4, LCD_RED);
            currentBall->RED_flag= false;
        } else

            if (currentBall->GREEN_flag){
                LCD_DrawRectangle(previousBall->CenterX-6, previousBall->CenterX+6,234,235, LCD_GREEN);
                LCD_DrawRectangle(previousBall->CenterX-4, previousBall->CenterX+4,236,240, LCD_BLUE);
                currentBall->GREEN_flag= false;
            }
            else {
                {
                    LCD_DrawRectangle(previousBall->CenterX-3, previousBall->CenterX+3, previousBall->CenterY-3, previousBall->CenterY+3, LCD_GREEN);

                }

                LCD_DrawRectangle(currentBall->currentCenterX-2, currentBall->currentCenterX+2, currentBall->currentCenterY-2, currentBall->currentCenterY+2, outColor);
            }
    }

    if(previousBall->CenterX !=  currentBall->currentCenterX )
        previousBall->CenterX = currentBall->currentCenterX;
    if(previousBall->CenterY !=  currentBall->currentCenterY  )
        previousBall->CenterY = currentBall->currentCenterY;


    ub = 0;
}

void IdleThread()
{
    while(1);
}

int checkForCollision(Ball_t* ball){

    if (ball->currentCenterX == 42)     return 1;
    if (ball->currentCenterX == 320-42) return 2;
    if (ball->currentCenterY == -8)      return 3;
    if ((ball->currentCenterY == -7 || ball->currentCenterY == 240+5)  && ((host.currentCenter - ball->currentCenterX > 33) || (host.currentCenter - ball->currentCenterX < -33)) )     return 11;
    if((ball->currentCenterY <= 2) && (ball->currentCenterY >-4)  && (ball->currentCenterX - host.currentCenter < 32) && (host.currentCenter - ball->currentCenterX) < 32)  return 12;
    if(ball->currentCenterY == 240+6   && ((ball->currentCenterX - host.currentCenter < 32) && (host.currentCenter - ball->currentCenterX < 32)))  return 15;

    if(ball->currentCenterY == -4   && ((ball->currentCenterX - host.currentCenter < 32) && (host.currentCenter - ball->currentCenterX < 32)))  return 13;

    if((ball->currentCenterY >= 240-2 )&& (ball->currentCenterY < 240+6) && (ball->currentCenterX - host.currentCenter < 32) && (host.currentCenter - ball->currentCenterX < 32))  return 14;

    if (ball->currentCenterY == 247)    return 4;

    if (ball->currentCenterY == 7 && ((host.currentCenter - ball->currentCenterX < 11) && (host.currentCenter - ball->currentCenterX > -11)))  return 5;
    if (ball->currentCenterY == 233 && ((client.currentCenter - ball->currentCenterX < 11) && (client.currentCenter - ball->currentCenterX > -11)))    return 6;

    if (ball->currentCenterY == 7 && ((ball->currentCenterX - host.currentCenter <= 33) && (ball->currentCenterX - host.currentCenter >= 11)))  return 7;
    if (ball->currentCenterY == 233 && ((ball->currentCenterX - host.currentCenter <= 33) && (ball->currentCenterX - host.currentCenter >= 11)))    return 8;

    if (ball->currentCenterY == 7 && ((host.currentCenter - ball->currentCenterX <= 33) && (host.currentCenter - ball->currentCenterX >= 11)))  return 9;
    if (ball->currentCenterY == 233 && ((client.currentCenter - ball->currentCenterX <= 33) && (client.currentCenter - ball->currentCenterX >= 11)))    return 10;
    //if (ball->currentCenterY == 72)     return 1;
    //if (ball->currentCenterY == 72)     return 1;
    return 0;
}

void displayScore(){

    uint8_t hostScoreString[2];
    uint8_t clientScoreString[3];


    if (clientScoreChanged){


        clientScoreString[1] = clientScore %10 + '0';
        clientScoreString[0] = clientScore /10 + '0';
        clientScoreString[2] = 0;
        LCD_DrawRectangle(10, 25,4,19, LCD_BLACK);
        LED_CLIENT = (LED_CLIENT << 1) | 0x0001;
        write_leds(RED,LED_CLIENT );
        LCD_Text(10, 4, clientScoreString, LCD_RED);

        clientScoreChanged = false;
        if (clientScore == winnigScore)  G8RTOS_AddThread(&EndOfGameHost, 1, "EndOfGameHost");
    }



    if (HostScoreChanged){

        hostScoreString[1] = hostScore %10 + '0';
        hostScoreString[0] = hostScore /10 + '0';

        LCD_DrawRectangle(10, 25,220,235, LCD_BLACK);

        LED_HOST = (LED_HOST << 1) | 0x0001;
        write_leds(BLUE,LED_HOST );
        LCD_Text(10, 220, hostScoreString, LCD_BLUE);
        //LCD_DrawRectangle(10, 25,220,235, LCD_GREEN);

        // LCD_Text(10, 220, clientScoreString, LCD_BLUE);
        HostScoreChanged = false;
        if (hostScore == winnigScore)  G8RTOS_AddThread(&EndOfGameHost, 1, "EndOfGameHost");
    }


}


void  LCD_tap()
{
    Point xy=TP_ReadXY() ;
    global_x=xy.x;
    global_y=xy.y;
    P4IE = 0;
    if(listen_to_restart_flag==0){
        if (global_x > 100 && global_x < 220 && global_y > 50 && global_y < 90){ listen_to_restart_flag = 2;HostChoosed++;}
        if (global_x > 100 && global_x < 220 && global_y > 130 && global_y < 170) {listen_to_restart_flag = 2;ClientChoosed++;}

    }


    if(listen_to_restart_flag==1)
    {
        if (global_x > 100 && global_x < 220 && global_y > 130 && global_y < 170){
            restart=1;
            listen_to_restart_flag =0;
            HostChoosed=0;
            ClientChoosed=0;
        }
    }



    P4IFG &= ~BIT0;
    P4IFG &= ~BIT0;
    P4IE = 1;

}

void EndOfGameHost(){
    G8RTOS_KillAllOtherThreads();
    for (int i = 0; i < MAX_NUM_OF_BALLS; i++){
        balls[i].alive = false;
    }

    numberOfBalls=0;
    G8RTOS_AddThread(&IdleThread, 255, "idle");
    if (clientScore == winnigScore){
        LCD_Clear(LCD_RED);
        LCD_Text(70,65, "The winner is : Client", LCD_BLACK);
    }

    if (hostScore == winnigScore){
        LCD_Clear(LCD_BLUE);
        LCD_Text(70,65, "The winner is : Host", LCD_WHITE);
    }



    LCD_DrawRectangle(100, 220, 130, 170, LCD_BLACK);
    LCD_Text(120,145, "Play again!", LCD_WHITE);
    listen_to_restart_flag = 1;
    restart = 0;
    while(1){

        if (restart==1){
            restart=0;
            HostChoosed=0;
            ClientChoosed=0;
            hostScore =0;
            clientScore =0;
            LED_HOST =0;
            LED_CLIENT=0;
            G8RTOS_AddThread(&CreateGame, 1, "CreateGame");
            G8RTOS_KillSelf();
        }
    }

}

