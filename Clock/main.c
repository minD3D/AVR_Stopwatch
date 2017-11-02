/*
 * TIMER PROJECT
 * Created: 2017-11-01 오후 12:53:17
 * Author : MINJI KIM
 */ 
 
 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #define  F_CPU 14745600UL
 
 #include "_main.h"
 #include "_glcd.h"
 
 
 unsigned char led=0xfe;
 unsigned char cnt;
 int count, mcount, hcount;
 int sscnt, scnt,mcnt,hcnt;
 char togglemode,resetbtn,stopbtn;
 int togglecnt=0;
 //인터럽트 발생주기 1/(14.7456mhz)*1024*256=17.8ms
 //17.8ms*56=1sec

 SIGNAL(INT0_vect)
{
	togglemode=~togglemode;
}

SIGNAL(INT1_vect)
{
	resetbtn=~resetbtn;
}

SIGNAL(INT2_vect)
{
	stopbtn=~stopbtn;
}

void Interrupt_init(void){
	EICRA = 0x02;
	EIMSK = 0x07;
	DDRD = 0x00;
}

#define Timer_x  30   // 시계 중심축 좌표
#define Timer_y  60  //시계 중심축 좌표

void draw_min(int Min){
   Min = Min * 6+270;  //분 단위를 각도로 변환(1분은 6도)
   if(Min > 360)
      Min = Min -360; //Min 을 0으로 초기화 해도 될듯
   
   float rad = 3.14159 /180 * Min;  //라이안으로 변환
   float x = 15 * sin(rad) + Timer_x; 
   float y = 15 * cos(rad) + Timer_y;
   GLCD_Circle(x , y,1); // 중심축에서 분 위치 까지 라인 그리기
   GLCD_Line(Timer_x, Timer_y, x , y); // 중심축에서 분 위치 까지 라인 그리기
}
void draw_hour(float Hour,  float Min){ //시는 
   Hour = (Hour * 30) + (6*Min/60+270);  //시간 단위를 각도로 변환(1시간은 30도, 0~59분 까지 지날때 시침이 조금씩 이동함)
   if(Hour > 360)
      Hour = Hour -360;
   
   float rad = 3.14159 /180 * Hour;
   float x = 8 * sin(rad) + Timer_x; 
   float y = 8 * cos(rad) + Timer_y;
	GLCD_Circle(x , y,1); // 중심축에서 분 위치 까지 라인 그리기

	GLCD_Line(Timer_x, Timer_y, x , y); // 
	GLCD_Line(Timer_x, Timer_y, x+1 , y); // 
	GLCD_Line(Timer_x, Timer_y, x-1 , y); // 

}
void draw_second(float Second){
   Second = (Second * 6) +270;  //초 단위를 각도로 변환
   if(Second > 360)
      Second = Second -360;
   
   float rad = 3.14159 /180 * Second;
   float x = 22 * sin(rad) + Timer_x;
   float y = 22 * cos(rad) + Timer_y;
	GLCD_Circle(x , y,1); // 중심축에서 분 위치 까지 라인 그리기
	//GLCD_Line(Timer_x, Timer_y, x , y); // 
}


 void init_Timer0(void){
	 TCCR0=0x07;//TCCR0는 0번타이머이며 일반모드 1024분주
	 TCNT0=0x00;
	 SREG=0x80;
	 TIMSK=0x01;
 }
 void Port_init(void){
	 PORTA=0x00; DDRA=0xff;
	 PORTB=0xff; DDRB=0xff;
	 PORTC=0x00; DDRC=0xf0;
	 PORTD=0x80; DDRD=0x80;
	 PORTE=0x00; DDRE=0xff;
	 PORTF=0x00; DDRF=0x00;
	 
 }
 void init_devices(void){
	 
	 cli();//disable all INT
	 Port_init();
	 lcd_init();
	 sei();//re-enable

 }

 void screen_display(void){
	 lcd_clear();
	 lcd_string(0,0,"TIMER");
	 //lcd_string(1,0,"*  TIMER TEST   *");
	// lcd_string(2,0,"=================");
	 
 }

void analog_view(void){
		lcd_string(0,0,">>TIMER");
		 GLCD_Circle(30,60,25);
		 draw_second(count);
		 draw_min(mcount);
		 draw_hour(hcount,mcount);
		 lcd_xy(7,1);GLCD_2DigitDecimal(hcount);
		 lcd_string(7,4,"h");
		 lcd_xy(7,7);GLCD_2DigitDecimal(mcount);
		 lcd_string(7,10,"m");
		 lcd_xy(7,13);GLCD_2DigitDecimal(count);
		 lcd_string(7,16,"s");	
}

void clock_view(void){
		lcd_string(0,0,"  TIMER");
		lcd_string(1,0,">>STOP WATCH");	
		lcd_xy(4,0);GLCD_2DigitDecimal(hcnt);
		lcd_string(4,3,"h");
		lcd_xy(4,5);GLCD_2DigitDecimal(mcnt);
		lcd_string(4,8,"m");
		lcd_xy(4,10);GLCD_2DigitDecimal(scnt);
		lcd_string(4,13,"s");		
		lcd_xy(4,15);GLCD_2DigitDecimal(sscnt);
		lcd_string(4,18,"ss");
		lcd_string(7,0,"MENU | RESET | STOP");
		

			
}

 ISR(TIMER0_OVF_vect){
	 cnt++;
	 if(togglecnt)
		sscnt++;
	 if(sscnt==56){//1초 세팅
		scnt++;
		sscnt=0;
		  if(scnt>=59){
			  if(scnt==59)scnt=0;
			  mcnt++;
		  }
		  if(mcnt>=59){
			  if(mcnt==59)mcnt=0;
			  hcnt++;
		
		}
		
		}

	 if(cnt==56){//1초 세팅
		 count++;
		 led<<=1;
		 led|=0x01;
		 if(led==0xff)led=0xfe;
		 PORTB=led;
		 cnt=0;
		 if(count>=59){
			 if(count==59)count=0;
			 mcount++;	 
			 		   lcd_clear();	  

		 }
		 if(mcount>=59){
			 if(mcount==59)mcount=0;
			 hcount++;
			 
		 }
		   lcd_clear();	  
	  }
 }

 void resetcnt(void){
 sscnt=0;
 scnt=0;
 mcnt=0;
 hcnt=0;
  resetbtn=0;
 }
 int main(void)
 {
	 init_devices();
	 screen_display();
	 init_Timer0();
	 stopbtn=0;
	 resetbtn=0;

	 while (1)
	 {
	 Interrupt_init();
	 DDRB=0xff;
	 PORTB=0x0f;
	 togglemode=0;
	
	 while(1){
		if(togglemode)clock_view();
		else analog_view();
		if(stopbtn)togglecnt=1;
		else togglecnt=0;
		if(resetbtn)resetcnt();
	 }
	 }
 }
