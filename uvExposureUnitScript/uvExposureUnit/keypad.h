/*
 * keypad.h
 *
 * Created: 10/28/2020 10:07:34 PM
 *  Author: TeyTea
 */ 


#ifndef KEYPAD_H_
#define KEYPAD_H_

#ifndef LIBDEP_H_
#define LIBDEP_H_
#include "libDep.h"
#endif

ioPin row1, row2, row3, row4, col1, col2, col3, col4;

void initKeypad(ioPin r1, ioPin r2, ioPin r3, ioPin r4, ioPin co1, ioPin co2, ioPin co3, ioPin co4){
	row1=r1; row2=r2; row3=r3; row4=r4,col1=co1;col2=co2;col3=co3;col4=co4;
	writePinDirection(row1,1);writePinDirection(row2,1);writePinDirection(row3,1);writePinDirection(row4,1);
	writePinDirection(col1,0);writePinDirection(col2,0);writePinDirection(col3,0);writePinDirection(col4,0);
}

uint8_t scanKey(){
	while(readPin(col1) || readPin(col2) || readPin(col3) || readPin(col4));
	writePin(row1,1);writePin(row2,0);writePin(row3,0);writePin(row4,0);_delay_ms(50);
	if(readPin(col1))return '1';
	else if(readPin(col2))return '2';
	else if(readPin(col3))return '3';
	else if(readPin(col4))return 'A';
	writePin(row1,0);writePin(row2,1);writePin(row3,0);writePin(row4,0);_delay_ms(50);
	if(readPin(col1))return '4';
	else if(readPin(col2))return '5';
	else if(readPin(col3))return '6';
	else if(readPin(col4))return 'B';
	writePin(row1,0);writePin(row2,0);writePin(row3,1);writePin(row4,0);_delay_ms(50);
	if(readPin(col1))return '7';
	else if(readPin(col2))return '8';
	else if(readPin(col3))return '9';
	else if(readPin(col4))return 'C';
	writePin(row1,0);writePin(row2,0);writePin(row3,0);writePin(row4,1);_delay_ms(50);
	if(readPin(col1))return '*';
	else if(readPin(col2))return '0';
	else if(readPin(col3))return '#';
	else if(readPin(col4))return 'D';
	return 0;
}

#endif /* KEYPAD_H_ */