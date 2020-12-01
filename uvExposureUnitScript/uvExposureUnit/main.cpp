/*
* uvExposureUnit.cpp
*
* Created: 29/10/2020 6:09:09 pm
* Author : TemitopeOluwatosin
*/

#define lcdRs a0
#define lcdEn a1
#define lcdD4 a2
#define lcdD5 a3
#define lcdD6 a4
#define lcdD7 a5
#define F_CPU 8000000

#include <avr/io.h>
#include "lcd.h"
#include "keypad.h"
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdlib.h>

uint8_t myKey;
void welcomeUser();
void startUVLight();
void stopUVLight();
void setTimer();
void displayTimer();
uint8_t readEEPROM();
void pauseUVLight();

static uint8_t hour=0, min=0, sec=0, hourCnt=0, minCnt=0, secCnt=0, hourAdd[4], minAdd[4], secAdd[4], setTime, col, i, status=0;
bool arrStatus;
static uint16_t cnt=0, cntDelay=0;
char out[4], keepKey[5];

int main(void){
	initLcd();
	initKeypad(d0,d1,d2,d3,d4,d5,d6,d7);
	setTime = readEEPROM();
	TCCR0|=_BV(CS00)|_BV(CS01);
	TIMSK|=(1 << TOIE0);
	TCNT0=131;
	writePinDirection(c0,1);
	writePinDirection(b0,1);
	PORTC=0;
	PORTB=0;
	lcdOut(1,2,"FIELDBASE SER.");
	lcdOut(2,7,"LTD.");
	_delay_sec(2);
	lcdClear;
	while (1) {
		setTimer();
		displayTimer();
	}
}

void setTimer(){
	while (setTime){
		status=0;
		lcdOut(1,1,"SET TIMER:");
		lcdOut(2,10,"0:00:00");
		lcdOut(2,10,"");
		col=10;
		sei();
		while(setTime){
			myKey = scanKey();
			arrStatus=true;
			if(myKey>='0' && myKey<='9' && col<17){
				if((col==12 || col==15) && myKey>'5')continue;
				lcdChr(2,col,myKey);
				keepKey[col-10]=myKey;
				col++;
				if(col==11 || col==14){
					keepKey[col-10]=myKey;
					col+=1;
					lcdOut(2,col,"");
				}
				else if(col==17){
					col=10;
					lcdOut(2,col,"");
				}
				_delay_ms(100);
			}
			else if(myKey=='#'){
				for (i=0;i<7;i++){
					if(keepKey[i]==0){
						arrStatus=false;
						continue;
					}
				}
				if(arrStatus==true){
					hour = keepKey[0]-48;
					min = (((keepKey[2]-48)*10)+(keepKey[3]-48));
					sec = (((keepKey[5]-48)*10)+(keepKey[6]-48));
					eeprom_write_byte(hourAdd,hour);
					eeprom_write_byte(minAdd,min);
					eeprom_write_byte(secAdd,sec);
					_delay_ms(500);
					cli();
					lcdCursorOff;
					lcdClear;
					lcdOut(1,1,"SAVING.");
					for (i=0;i<7;i++){
						_delay_ms(100);
						lcdChrCp('.');
					}
					lcdClear;
					lcdOut(1,1,"SAVED");
					_delay_ms(500);
					setTime=0;
				}
			}
		}
	}
}

void displayTimer(){
	top:
	if(status==0){
		lcdOut(1,1,"TIMER: ");
		lcdChr(1,8,hour+48);
		lcdChr(1,9,':');
		itoa(min,out,10);
		if(min<10){
			lcdChrCp('0');
			lcdChrCp(min+48);
		}
		else lcdOut(1,10,out);
		lcdChr(1,12,':');
		itoa(sec,out,10);
		if(sec<10){
			lcdChrCp('0');
			lcdChrCp(sec+48);
		}
		else lcdOut(1,13,out);
	}
	else if(status==1){
		myKey = scanKey();
		lcdOut(1,1,"START: ");
		lcdOut(2,1,"STOP: ");
		lcdChr(2,8,hourCnt+48);
		lcdChr(2,9,':');
		itoa(minCnt,out,10);
		if(minCnt<10){
			lcdChrCp('0');
			lcdChrCp(minCnt+48);
		}
		else lcdOut(2,10,out);
		lcdChr(2,12,':');
		itoa(secCnt,out,10);
		if(secCnt<10){
			lcdChrCp('0');
			lcdChrCp(secCnt+48);
		}
		else lcdOut(2,13,out);
		if(myKey == 'A')startUVLight();
		else if(myKey == 'B')pauseUVLight();
		else if((hourCnt==0 && minCnt==0 && secCnt==0) || myKey == 'C'){
			cli();
			writePin(b0,0);
			lcdClear;
			if(hourCnt==0 && minCnt==0 && secCnt==0){
				status=2;
				cntDelay=0;
				lcdOut(1,1,"DONE: EXPOSURING");
			}
			else status=0;
			hourCnt=0; minCnt=0; secCnt=0;
			goto top;
		}
	}
	while(status==2){
		if(cntDelay==0){
			for (i=0; i<80; i++){
				writePin(c0,1); _delay_ms(5);
				writePin(c0,0); _delay_ms(15);
			}
		}
		cntDelay++;
		if(cntDelay==20)cntDelay=0;
		myKey = scanKey();
		if (myKey=='D'){
			status=0;
			lcdClear;
			goto top;
		}
	}
	while(status==0){
		myKey = scanKey();
		if(myKey=='A'){
			status=1;
			hourCnt = hour;
			minCnt = min;
			secCnt = sec;
			sei();
			writePin(b0,1);
		}
		if(myKey=='*'){
			setTime=1;
			status=1;
			for(i=0;i<7;i++)keepKey[i]=0;
			lcdClear;
		}
	}
}

void startUVLight(){
	sei();
	writePin(b0,1);
}

void pauseUVLight(){
	cli();
	writePin(b0,0);
}

uint8_t readEEPROM(){
	hour = eeprom_read_byte(hourAdd);
	min = eeprom_read_byte(minAdd);
	sec = eeprom_read_byte(secAdd);
	if(hour>99 || min > 60 || sec>60)return 1;
	else return 0;
}

ISR(TIMER0_OVF_vect){
	cnt++;
	if(cnt==500){
		if(status==0)lcdCursorOn;
	}
	else if(cnt==1000){
		if(status==0)lcdCursorOff;
		else if(status==1){
			if((minCnt==0 && secCnt==0) && hourCnt>0){
				minCnt=60;
				hourCnt-=1;
			}
			if(secCnt==0 && minCnt>0){
				secCnt=60;
				minCnt-=1;
			}
			if(secCnt>0)secCnt-=1;
		}
		cnt=0;
	}
	TCNT0 = 131;
}