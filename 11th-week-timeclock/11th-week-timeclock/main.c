/*
 * 11th-week-timeclock.c
 *
 * Created: 2019-05-15 오전 10:05:34
 * Author : hyeon
 */ 
/////////////////define include, global variable////////////
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
unsigned timeNum = 0, timeIterruptSet = 0;
unsigned char FND_SEGNP[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x27,0x7F,0x6F};//숫자
unsigned char FND_SEGWP[10] = {0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0xA7,0xFF,0xEF};//점있는숫자
unsigned char FND_SEGPOS[4] = {0x01,0x02,0x04,0x08};//세그먼트 자리
unsigned int exp10[4] = {1,10,100,1000};
#define STATE_INIT 0
#define STATE_TIMER_RUNNING 1
#define STATE_TIMER_PAUSE 2
unsigned int state = STATE_INIT;
//////////////////////

void printSeg(int num, int whatSegment){
	//자리수 지정
	PORTA = FND_SEGPOS[whatSegment];
	PORTE = 0x04;
	PORTE = 0x00;
	//세그먼트 숫자출력
	PORTA = FND_SEGNP[num];
	PORTE = 0x08;
	PORTE = 0x00;
	
}

void initPort(){
	DDRA = 0xFF;
	DDRF = 0xF0;
	DDRG = 0x00;
	DDRE = 0x0C;
	PORTE = 0x04;
	PORTA = 0x0F;
}

void initInterrupt(){
	//timer interrupt 
	//normal mode(0), no prescaling
	//when timer being overflow, interrupt exc
	TCCR0 = 0x01;//nomal
	TCNT0 = 0x00;
	TIMSK = 0x01;//overflow 활성화
	TIFR = 0xff;//초기화 임의 값을 넣으면 하드웨어적으로 초기화
	//external interrupt
	
	//int4 버튼 falling edge활성화
	EICRB  &= ~(1<<ISC40);
	EICRB  |= 1<<ISC41;
	//int4 입력 활성화
	EIMSK |= 1<<INT4;    
	
	//int5 버튼 falling edge활성화
	EICRB  &= ~(1<<ISC50);	//0으로 설정
	EICRB  |= 1<<ISC51;		//1로 설정
	//int5 입력 활성화
	EIMSK |= 1<<INT5;
	
}
ISR(INT4_vect)
{
	
	if(state == STATE_INIT || state == STATE_TIMER_PAUSE){
		state = STATE_TIMER_RUNNING;
	}else if(state == STATE_TIMER_RUNNING){
		state = STATE_TIMER_PAUSE;
	}
	
	
	
}
ISR(INT5_vect)
{
	if(state == STATE_TIMER_PAUSE){
		timeNum = 0;
		state = STATE_INIT;
	}else if(state == STATE_TIMER_RUNNING){
		timeNum = 0;
	}
	
}

ISR(TIMER0_OVF_vect)
{
	timeIterruptSet++;
	if(timeIterruptSet > 625 && state == STATE_TIMER_RUNNING){
		timeNum++;
		timeIterruptSet = 0;
	}
}


int main(void)
{
	
	initPort();
	initInterrupt();

	sei();      //Global Interrupt Enable
	
	while (1){
		
		//segment print
		for(int i = 0; i < 4; i++){
			printSeg((timeNum/exp10[i])%10,3-i);
		}
		//초기화
		if(timeNum == 10000) timeNum = 0;
		
	}
	
}