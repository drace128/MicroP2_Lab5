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
            if (client.currentCenter < 320-72){
            host.currentCenter++;
            client.currentCenter++;     //temporarily used to mirror paddle
        }
        }
        G8RTOS_OS_Sleep(100);
    }
}

void DrawObjects()
{
    LCD_DrawRectangle(37, 39, 0, 240, LCD_WHITE);
    LCD_DrawRectangle(40, 279, 0, 240, LCD_GREEN);
    LCD_DrawRectangle(280, 282, 0, 240, LCD_WHITE);
    LCD_DrawRectangle(128, 128+PADDLE_LEN, 10, 10+PADDLE_WID, PLAYER_BLUE);
    LCD_DrawRectangle(128, 128+PADDLE_LEN, 230-PADDLE_WID, 230, PLAYER_RED);

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
    host.position = BOTTOM;
    client.color = PLAYER_BLUE;
    client.currentCenter = PADDLE_X_CENTER;
    client.position = TOP;

}

void UpdatePlayerOnScreen(PrevPlayer_t * prevPlayerIn, GeneralPlayerInfo_t * outPlayer)
{
    if(outPlayer->currentCenter != prevPlayerIn->Center   )
    {

        if (outPlayer->currentCenter > prevPlayerIn->Center){
            if (outPlayer->position == BOTTOM){
                LCD_DrawRectangle(outPlayer->currentCenter-33, outPlayer->currentCenter-32, 10, 10+PADDLE_WID, LCD_GREEN);
                LCD_DrawRectangle(outPlayer->currentCenter+32, outPlayer->currentCenter+33, 10, 10+PADDLE_WID, PLAYER_BLUE);
            }else
            {
                LCD_DrawRectangle(outPlayer->currentCenter-33, outPlayer->currentCenter-32, 230-PADDLE_WID, 230, LCD_GREEN);
                LCD_DrawRectangle(outPlayer->currentCenter+32, outPlayer->currentCenter+33, 230-PADDLE_WID, 230, PLAYER_RED);
            }

        }
        else
        {
            if (outPlayer->position == BOTTOM){
                LCD_DrawRectangle(outPlayer->currentCenter+32, outPlayer->currentCenter+33, 10, 10+PADDLE_WID, LCD_GREEN);
                LCD_DrawRectangle(outPlayer->currentCenter-33, outPlayer->currentCenter-32, 10, 10+PADDLE_WID, PLAYER_BLUE);
            }else
            {
                LCD_DrawRectangle(outPlayer->currentCenter+32, outPlayer->currentCenter+33, 230-PADDLE_WID, 230, LCD_GREEN);
                LCD_DrawRectangle(outPlayer->currentCenter-33, outPlayer->currentCenter-32, 230-PADDLE_WID, 230, PLAYER_RED);
            }
        }
        prevPlayerIn->Center = outPlayer->currentCenter;
    }
}

void IdleThread()
{
    while(1);
}
