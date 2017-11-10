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

int16_t xCoord, yCoord;
GeneralPlayerInfo_t host, client;
PrevPlayer_t oldhost, oldclient;
Ball_t balls[MAX_NUM_OF_BALLS];
uint16_t numberOfBalls = 0;
PrevBall_t oldballs[MAX_NUM_OF_BALLS];
int hostScore, clientScore;
bool scoreChanged;

void GenerateBall()
{
    while(1)
    {
        if(numberOfBalls < MAX_NUM_OF_BALLS)
        {
            G8RTOS_AddThread(&MoveBall, 1, "MoveBall");
            numberOfBalls++;
        }
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
        switch(checkForCollision(&balls[i]))
        {
        case 1:
            directionX = 1;
            break;
        case 2:
            directionX = 0;
            break;
        case 3:
           // directionY = 1;
            if (touched){
            clientScore++;
            scoreChanged = true;
            }
            balls[i].alive = false;
            numberOfBalls--;
            G8RTOS_KillSelf();
            break;
        case 4:
           // directionY = 0;
            if (touched){
           hostScore++;
            scoreChanged = true;
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

        }

        if(directionX == 1){balls[i].currentCenterX++;}
        else if(directionX == 0){balls[i].currentCenterX--;}

        if(directionY == 1){balls[i].currentCenterY++;}
        else{balls[i].currentCenterY--;}

        G8RTOS_OS_Sleep(481);
    }
}

void ReadJoystickClient()
{
    while(1)
    {
        GetJoystickCoordinates(&xCoord, &yCoord);
        if(xCoord > 1000){
            if (client.currentCenter > 72){
            host.currentCenter--;
            client.currentCenter--;     //temporarily used to mirror paddle
        }
        }
        if(xCoord < -1000){
            if (client.currentCenter < (320-72)){
            host.currentCenter++;
            client.currentCenter++;     //temporarily used to mirror paddle
        }
        }
        G8RTOS_OS_Sleep(379);
    }
}

void DrawObjects()
{
    while(1)
    {
        for(int i = 0; i < MAX_NUM_OF_BALLS; i++)
        {
            if(balls[i].alive)
            {
                UpdateBallOnScreen(&oldballs[i], &balls[i], balls[i].color);
            }
        }
        UpdatePlayerOnScreen(&oldhost, &host);
        UpdatePlayerOnScreen(&oldclient, &client);
        displayScore();
        G8RTOS_OS_Sleep(19);
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

    LCD_DrawRectangle(37, 39, ARENA_MIN_Y, ARENA_MAX_Y, LCD_WHITE);
    LCD_DrawRectangle(ARENA_MIN_X, ARENA_MAX_X, ARENA_MIN_Y, ARENA_MAX_Y, LCD_GREEN);
    LCD_DrawRectangle(281, 283, ARENA_MIN_Y, ARENA_MAX_Y, LCD_WHITE);
    LCD_DrawRectangle(128, 128+PADDLE_LEN, ARENA_MIN_Y, ARENA_MIN_Y+PADDLE_WID, PLAYER_RED);
    LCD_DrawRectangle(128, 128+PADDLE_LEN, ARENA_MAX_Y-PADDLE_WID, ARENA_MAX_Y, PLAYER_BLUE);

    LCD_Text(10, 0, "00", LCD_RED);
    LCD_Text(10, 220, "00", LCD_BLUE);
    hostScore =0;
    clientScore=0;
    scoreChanged= false;

//    G8RTOS_AddThread(&DrawObjects, 1, "draw");
//    G8RTOS_AddThread(&ReadJoystickClient, 1, "joystick");
//    G8RTOS_AddThread(&IdleThread, 255, "idle");
//    G8RTOS_AddThread(&GenerateBall, 1, "genBall");

//    G8RTOS_KillSelf();
//    while(1);
}

void UpdatePlayerOnScreen(PrevPlayer_t * prevPlayerIn, GeneralPlayerInfo_t * outPlayer)
{
    if(outPlayer->currentCenter != prevPlayerIn->Center   )
    {

        if (outPlayer->currentCenter > prevPlayerIn->Center){       //moving right
            if (outPlayer->position == BOTTOM){
                LCD_DrawRectangle(outPlayer->currentCenter-(PADDLE_LEN_D2+1), outPlayer->currentCenter-PADDLE_LEN_D2, 0, PADDLE_WID, LCD_GREEN);
                LCD_DrawRectangle(outPlayer->currentCenter+(PADDLE_LEN_D2-1), outPlayer->currentCenter+PADDLE_LEN_D2, 0, PADDLE_WID, PLAYER_RED);
            }else
            {
                LCD_DrawRectangle(outPlayer->currentCenter-(PADDLE_LEN_D2+1), outPlayer->currentCenter-PADDLE_LEN_D2, 240-PADDLE_WID, 240, LCD_GREEN);
                LCD_DrawRectangle(outPlayer->currentCenter+(PADDLE_LEN_D2-1), outPlayer->currentCenter+PADDLE_LEN_D2, 240-PADDLE_WID, 240, PLAYER_BLUE);
            }

        }
        else        //moving left
        {
            if (outPlayer->position == BOTTOM){
                LCD_DrawRectangle(outPlayer->currentCenter+PADDLE_LEN_D2, outPlayer->currentCenter+(PADDLE_LEN_D2+1), 0, PADDLE_WID, LCD_GREEN);
                LCD_DrawRectangle(outPlayer->currentCenter-PADDLE_LEN_D2, outPlayer->currentCenter-(PADDLE_LEN_D2-1), 0, PADDLE_WID, PLAYER_RED);
            }else
            {
                LCD_DrawRectangle(outPlayer->currentCenter+PADDLE_LEN_D2, outPlayer->currentCenter+(PADDLE_LEN_D2+1), 240-PADDLE_WID, 240, LCD_GREEN);
                LCD_DrawRectangle(outPlayer->currentCenter-PADDLE_LEN_D2, outPlayer->currentCenter-(PADDLE_LEN_D2-1), 240-PADDLE_WID, 240, PLAYER_BLUE);
            }
        }
        prevPlayerIn->Center = outPlayer->currentCenter;
    }
}

void UpdateBallOnScreen(PrevBall_t * previousBall, Ball_t * currentBall, uint16_t outColor)
{
    LCD_DrawRectangle(previousBall->CenterX-4, previousBall->CenterX+4, previousBall->CenterY-4, previousBall->CenterY+4, LCD_GREEN);
    LCD_DrawRectangle(currentBall->currentCenterX-2, currentBall->currentCenterX+2, currentBall->currentCenterY-2, currentBall->currentCenterY+2, outColor);
    previousBall->CenterX = currentBall->currentCenterX;
    previousBall->CenterY = currentBall->currentCenterY;
}

void IdleThread()
{
    while(1);
}

int checkForCollision(Ball_t* ball){

    if (ball->currentCenterX == 42)     return 1;
    if (ball->currentCenterX == 320-42) return 2;
    if (ball->currentCenterY == 0)      return 3;
    if (ball->currentCenterY == 240)    return 4;

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
    uint8_t clientScoreString[2];


        if (scoreChanged){

            hostScoreString[1] = hostScore %10 + '0';
            hostScoreString[0] = hostScore /10 + '0';
            clientScoreString[1] = clientScore %10 + '0';
            clientScoreString[0] = clientScore /10 + '0';
            LCD_DrawRectangle(10, 30,0,30, LCD_BLACK);
            LCD_DrawRectangle(10, 30,220,240, LCD_BLACK);

            LCD_Text(10, 220, clientScoreString, LCD_BLUE);
            LCD_Text(10, 0, hostScoreString, LCD_RED);

            scoreChanged = false;

        }





}
