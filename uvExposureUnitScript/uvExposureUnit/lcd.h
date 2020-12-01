/*
 * lcd.h
 *
 * Created: 10/28/2020 11:42:40 PM
 *  Author: TeyTea
 */ 


#ifndef LCD_H_
#define LCD_H_

#ifndef LIBDEP_H_
#include "libDep.h"
#endif

#define lcdCursorOn lcdCmd(14)
#define lcdCursorOff lcdCmd(12)
#define lcdOn lcdCmd(14)
#define lcdOff lcdCmd(8)
#define lcdClear lcdCmd(1)
#define lcdReturnHome lcdCmd(2)
#define lcdScrollLeft lcdCmd(24)
#define lcdScrollRight lcdCmd(28)

#ifdef lcd_custom
uint8_t earphone[8] = {0x01, 0x03, 0x05, 0x09, 0x09, 0x0B, 0x1B, 0x18 };
uint8_t battery[8] = {0x04, 0x1F, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F };
uint8_t plug[8] = {0x0A, 0x0A, 0x1F, 0x11, 0x11, 0x0E, 0x04, 0x04 };
uint8_t line[8] = {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 };
#endif

uint8_t row_address[] = {128, 192, 148, 212};
uint8_t ARR_LcdLineNumAddress_U8[]={0x80,0xc0,0x90,0xd0};

void map(uint8_t data){
	writePin(lcdD4,isBitSet(data, 0));
	writePin(lcdD5,isBitSet(data, 1));
	writePin(lcdD6,isBitSet(data, 2));
	writePin(lcdD7,isBitSet(data, 3));
}
void send(bool op){
	writePin(lcdRs,op);
	writePin(lcdEn,1);
	_delay_ms(2);
	writePin(lcdEn,0);
}
void lcdCmd(uint8_t cmd){
	map(cmd>>4);
	send(false);
	map(cmd & 0xf);
	send(false);
}

void initLcd(){
	writePinDirection(lcdRs,1);
	writePinDirection(lcdEn,1);
	writePinDirection(lcdD4,1);
	writePinDirection(lcdD5,1);
	writePinDirection(lcdD6,1);
	writePinDirection(lcdD7,1);
	lcdCmd(2);	_delay_ms(5);		//set home entry
	lcdCmd(40);  _delay_ms(5);		//set to 4bit
	lcdCmd(14);	_delay_ms(5);		//display on
	lcdCmd(128); 	_delay_ms(5);		//set display at row 1 col 1
	lcdCmd(12);	_delay_ms(5);		//cursor off
	lcdClear;
}


void lcdChrCp(uint8_t data){
	map(data>>4);
	send(true);
	map(data & 0xf);
	send(true);
}

void lcdChr(uint8_t row, uint8_t column, uint8_t ascii_data){
	lcdCmd(row_address[row - 1] + (column - 1));
	lcdChrCp(ascii_data);
}

void lcdOutCp(const char *str){
	//while(*str)lcdChrCp();
	for (uint8_t i = 0; str[i] != 0; i++)lcdChrCp(str[i]);
}

void lcdOut(uint8_t row, uint8_t column, const char *str){
	lcdCmd(row_address[row - 1] + (column - 1));
	lcdOutCp(str);
}

void lcdCustomChr (uint8_t loc, uint8_t row, uint8_t column, unsigned char *msg){
	if(loc<8){
		lcdCmd(0x40 + (loc*8));	/* Command 0x40 and onwards forces the device to point CGRAM address */
		for(uint8_t i=0;i<8;i++)	/* Write 8 byte for generation of 1 character */
		lcdChrCp(msg[i]);
	}
	lcdCmd(row_address[row - 1] + (column - 1));
	lcdChrCp(loc);
}


#endif /* LCD_H_ */