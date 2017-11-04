/*
 * leds.c
 *
 *  Created on: Sep 6, 2017
 *      Author: brons
 */

#include "msp.h"
#include <stdbool.h>
#include "leds.h"
#include <driverlib.h>
#include <stdint.h>


//UCMODEx bits must be set to 11.
//SDA uses P3.6
//SCL uses P3.7
//UCTXSTP stop
//UCTXSTT start

void init_GPIO()
{
    P3DIR |= BIT7 | BIT6;
    //P3REN |= BIT7 | BIT6;
    P3OUT = 0x0;
    P3SEL0 |= BIT7 | BIT6;
}

void init_I2C()
{
    //while(UCB2CTLW0 & UCTXSTP);             //wait for stop condition
    UCB2CTLW0 = UCSWRST;         //enable software reset to configure the I2C module

    UCB2CTLW0 |= UCMST | UCMODE_3 | UCSYNC | UCSSEL_3;          //I2C, Master, Synchronous
    UCB2BRW = 50;
    UCB2IE |= UCTXIE0 | UCNACKIE | UCSTPIE;                //TX interupt enable and Nack interupt enable



    UCB2CTLW0 &= ~UCSWRST;        //disable software reset
//
//    CS->KEY = 0x695A; // unlock CS registers
//    CS->CTL0 = 0; // reset DCO settings
//    CS->CTL0 = CS_CTL0_DCORSEL_5; // select DCO 5 (48MHz)
//    CS->CTL1 = CS_CTL1_SELA__REFOCLK | CS_CTL1_SELS__DCOCLK | CS_CTL1_SELM__DCOCLK; // ACLK = REFOCLK, SMCLK = MCLK = DCOCLK
//    CS->KEY = 0; // lock CS registers

    __enable_interrupt();
}

void startBit()
{
    UCB2CTLW0 |= UCTXSTT | UCTR;              //send start bit and enable transmitter
}

void stopBit()
{
    UCB2CTLW0 |= UCTXSTP | UCTR;          //send stop bit and enable transmitter
}

void I2C_write(uint8_t slave_address, uint8_t register_address, uint8_t data)
{
    UCB2I2CSA = slave_address;

    //while((UCB2STATW & UCBBUSY) != 0);
    startBit();
    while((UCB2IFG & UCTXIFG0) == 0);
    UCB2TXBUF = register_address;
    //while((UCB2STATW & UCBBUSY) != 0);

    while((UCB2IFG & UCTXIFG0) == 0);

    UCB2TXBUF = register_address;
    while((UCB2IFG & UCTXIFG0) == 0);

    UCB2TXBUF = data;
    while((UCB2IFG & UCTXIFG0) == 0);
    //while((UCB2STATW & UCBBUSY) != 0);


    stopBit();
}

void initI2C_ledsOFF(void)
{
    init_GPIO();
    init_I2C();
    ledsOFF(RED);
    ledsOFF(BLUE);
    ledsOFF(GREEN);
}

void ledsOFF(uint8_t color)
{
    I2C_write(color, LS0, 0x00);
    I2C_write(color, LS1, 0x00);
    I2C_write(color, LS2, 0x00);
    I2C_write(color, LS3, 0x00);

    I2C_write(color, LS0, 0x00);
    I2C_write(color, LS1, 0x00);
    I2C_write(color, LS2, 0x00);
    I2C_write(color, LS3, 0x00);

    I2C_write(color, LS0, 0x00);
    I2C_write(color, LS1, 0x00);
    I2C_write(color, LS2, 0x00);
    I2C_write(color, LS3, 0x00);
}

void write_leds(uint8_t color, uint16_t leds)
{
    uint8_t reg;
    uint8_t data;
    uint8_t temp;
    if((leds & 0x000f) != 0)
    {
        reg = LS0;
        temp = (leds & 0x000f);
        data = generate(temp);
        I2C_write(color, reg, data);
    }
    if((leds & 0x00f0) != 0)
    {
        reg = LS1;
        temp = ((leds & 0x00f0) >> 4);
        data = generate(temp);
        I2C_write(color, reg, data);
    }
    if((leds & 0x0f00) != 0)
    {
        reg = LS2;
        temp = ((leds & 0x0f00) >> 8);
        data = generate(temp);
        I2C_write(color, reg, data);
    }
    if((leds & 0xf000) != 0)
    {
        reg = LS3;
        temp = ((leds & 0xf000) >> 12);
        data = generate(temp);
        I2C_write(color, reg, data);
    }
}

uint8_t generate(uint8_t ls)
{
    if(ls == 0x00){return 0x00;}
    else if(ls == 0x01){return 0x01;}
    else if(ls == 0x02){return 0x04;}
    else if(ls == 0x03){return 0x05;}
    else if(ls == 0x04){return 0x10;}
    else if(ls == 0x05){return 0x11;}
    else if(ls == 0x06){return 0x14;}
    else if(ls == 0x07){return 0x15;}
    else if(ls == 0x08){return 0x40;}
    else if(ls == 0x09){return 0x41;}
    else if(ls == 0x0a){return 0x44;}
    else if(ls == 0x0b){return 0x45;}
    else if(ls == 0x0c){return 0x50;}
    else if(ls == 0x0d){return 0x51;}
    else if(ls == 0x0e){return 0x54;}
    else if(ls == 0x0f){return 0x55;}
    else return 0;
}
