#include <reg52.h>
#define control P0
#define keypad P1 //keypad
#define segment P2 //segment output
#define KEYPAD_NO_NEW_DATA '-'
sbit ca1 = P0^0; //segment controls
sbit ca2 = P0^1;
sbit ca3 = P0^2;
sbit ca4 = P0^3;
sbit r1 = P1^0;
sbit r2 = P1^1;
sbit r3 = P1^2;
sbit r4 = P1^3;
sbit c1 = P1^4;
sbit c2 = P1^5;
sbit c3 = P1^6;
sbit c4 = P1^7;
sbit freq = P3^7; //frequency output
sbit sw=P3^2;
bit result;
unsigned int fq = 0;
unsigned int start_value;
unsigned int default_overflow_count;
char hexvalue[] = {0x0c0, 0x0f9, 0x0a4, 0x0b0, 0x99, 0x92, 0x82, 0x0f8, 0x80, 0x90, 0x88, 0x83, 0x0c6, 0x0a1, 0x86, 0x8e};
char digit,i;
char resarray[] = {0, 0, 0, 0};
char myres[]={0,0,0,0};
char controlvalue[] = {0x0f8, 0x0f4, 0x0f2, 0x0f1};
unsigned int overflow_count = 100;
static char Last_Valid_Key_G = KEYPAD_NO_NEW_DATA;
char pKey;

void debounce1()
{
		unsigned char i = 0;
	  while(	sw == 0 );		
		for( i = 255; i > 0; i--)
			if ( sw == 0 )
					i = 255;	
}

void debounce()
{
	int i = 100;
	int j = 1000;

	for(;i>0;i--)
		for(;j>0;j--)
			;
}
char KeyPad_Scan()
{
	static char Old_Key;
	char Key = KEYPAD_NO_NEW_DATA;

	r1 = 0;
			 if(c1 == 0) {Key = 1;}
	else if(c2 == 0) {Key = 2;}
	else if(c3 == 0) {Key = 3;}
	else if(c4 == 0) {Key = 10;}//'A'
	r1 = 1;

	r2 = 0;
			 if(c1 == 0) {Key = 4;}
	else if(c2 == 0) {Key = 5;}
	else if(c3 == 0) {Key = 6;}
	else if(c4 == 0) {Key = 11;}//'B'
	r2 = 1;

	r3 = 0;
			 if(c1 == 0) {Key = 7;}
	else if(c2 == 0) {Key = 8;}
	else if(c3 == 0) {Key = 9;}
	else if(c4 == 0) {Key = 12;}//'C'
	r3 = 1;

	r4 = 0;
			 if(c1 == 0) {Key = 14;}//'*'
	else if(c2 == 0) {Key = 0;}
	else if(c3 == 0) {Key = 15;}//'#'
	else if(c4 == 0) {Key = 13;}//'D'
	r4 = 1;

	debounce();

	if(Key == KEYPAD_NO_NEW_DATA)
	{
		Old_Key = KEYPAD_NO_NEW_DATA;
		Last_Valid_Key_G = KEYPAD_NO_NEW_DATA;
		return 0;
	}

	if(Key == Old_Key)
	{
		if(Key != Last_Valid_Key_G)
		{
			pKey = Key;
			Last_Valid_Key_G = Key;
			return 1;
		}
	}

	Old_Key = Key;
	return 0;
}

void delay_ms(unsigned int ms_Count)
{
    unsigned int i, j;
    for (i = 0; i < ms_Count; i++)
    {
        for (j = 0; j < 100; j++);
    }
}

void EX0_INT(void) interrupt 0
{
	
    unsigned long clock;
	  float count;
	  debounce1();
		fq = fq*2;
		clock = 11059200.0/12.0;
	  if(fq<500){
		count = (clock/fq)+100 ;}
		else if (fq>4000)
		{count = (clock/fq)-10 ;
		}
		else{
			count = (clock/fq)+30;
		}
		default_overflow_count = count / 0xFFFF;
		start_value = 0xFFFF - ((long)count % 0xFFFF);
		TL1 = start_value & 0xFF;
		TH1 = (start_value >> 8) & 0xFF;
		ET1 = 1;
		TR1 = 1;
}



void Timer1_INT (void) interrupt 3
{
			TR1=0;
			freq = ~freq;
			TL1 = start_value & 0xFF;
			TH1 = (start_value >> 8) & 0xFF;	
		  TR1 = 1;
}
void Timer0_INT (void) interrupt 1
{   // timer 0 overflow every 50 msec
	  if(overflow_count == 0)
      {	  TR0 = 0; overflow_count = 100;}
		else
			overflow_count--;
}
void main()
{
	keypad = 0x0f0;
	r1=1;
	r2=1;
	r3=1;
	r4=1;
	segment = 0x00;
	digit = 0;
	EX0 = 1;
	IT0 = 1;
	EA = 1;
	TR0 = 0;
	TR1 = 0;
	TMOD=0x11;
	while(1)
	{
		result = 0;
		while(!result)
		{
			control = controlvalue[0];
			segment = hexvalue[resarray[0]];
			delay_ms(1);
			control = controlvalue[1];
			segment = hexvalue[resarray[1]];
			delay_ms(1);
			control = controlvalue[2];
			segment = hexvalue[resarray[2]];
			delay_ms(1);
			control = controlvalue[3];
			segment = hexvalue[resarray[3]];
			delay_ms(1);

			result = KeyPad_Scan();
		}

		  ET0 = 1;
		  TL0 = 0x0;
	    TH0 = 0x4C;
      TR0 = 1;
		if(digit==3){
		fq=myres[0]*1000+myres[1]*100+myres[2]*10+myres[3];
		}
		for(i=3; i>0; i--)
		{
			resarray[i] = resarray[i-1];
		}

		if(digit == 4)
		{
			digit = 0;
			resarray[0] = 0;
			resarray[1] = 0;
			resarray[2] = 0;
			resarray[3] = 0;
		}
		resarray[0] = pKey;
		myres[digit]=pKey;
		digit++;

	}
}