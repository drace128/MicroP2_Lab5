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
#include "leds.h"
#define winnigScore 16
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
        G8RTOS_OS_Sleep(20);
    }
}

void CreateGame()
{

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
    up = 1;
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
    up = 0;
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



