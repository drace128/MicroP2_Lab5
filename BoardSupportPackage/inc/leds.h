/*
 * leds.h
 *
 *  Created on: Sep 6, 2017
 *      Author: brons
 */

#ifndef LEDS_H_
#define LEDS_H_

#define RED 0x60
#define GREEN 0x61
#define BLUE 0x62

#define LS0 0x06
#define LS1 0x07
#define LS2 0x08
#define LS3 0x09

extern void init_GPIO(void);
extern void init_I2C(void);
extern void startBit(void);
extern void stopBit(void);
extern void I2C_write(uint8_t slave_address, uint8_t register_address, uint8_t data);
extern void initI2C_ledsOFF(void);
extern void ledsOFF(uint8_t color);
extern void write_leds(uint8_t color, uint16_t leds);
extern uint8_t generate(uint8_t ls);

#endif /* LEDS_H_ */
