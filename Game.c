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

int16_t xCoord, yCoord;
GeneralPlayerInfo_t host, client;
PrevPlayer_t oldhost, oldclient;

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
        G8RTOS_OS_Sleep(100);
    }
}

void DrawObjects()
{
    while(1)
    {
        UpdatePlayerOnScreen(&oldhost, &host);
        UpdatePlayerOnScreen(&oldclient, &client);
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

    G8RTOS_AddThread(&DrawObjects, 1, "draw");
    G8RTOS_AddThread(&ReadJoystickClient, 1, "joystick");
    G8RTOS_AddThread(&IdleThread, 255, "idle");

    G8RTOS_KillSelf();
    while(1);
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

void IdleThread()
{
    while(1);
}
