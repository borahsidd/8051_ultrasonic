#include <reg51.h>
#include <intrins.h> 
#define dataport P2

sfr16 DPTR=0x82;

sbit rs=P2^0;
sbit rw=P2^1;
sbit e=P2^2;

sbit trig=P3^3;
sbit echo=P3^2;

unsigned int range=0;

int x;

//////////////////////////////////Delay program
void delay(unsigned int msec)
{
  int i,j;
  for(i=0;i<msec;i++)
	{
  for(j=0;j<1275;j++);
	}
}
/////////////////////////////////// Function for creating delay of 10 Microseconds
void delay10us()		
{
_nop_(); _nop_(); _nop_(); _nop_(); _nop_();
_nop_(); _nop_(); _nop_(); _nop_(); _nop_();
}

////////////////////////////////////Function to interface ultrasonic Sensor
unsigned char ultrasonic()         
{
	unsigned char dataD;
	trig=1;				//These three line is to generate ultrasonic sound of 40 Khz
	delay10us();
	trig=0;
	
	while(INT0==0);  	//////// Wait untill INT0 pin (echo pin) high
	while(INT0==1);		//////// Wait untill INT0 pin (echo pin) low
	DPH=TH0;					/////////Store TH0 value to DPH
	DPL=TL0;					/////////Store TL0 value to DPL
	TH0=TL0=0xff;			
	
	dataD=DPTR/59;			////////// 59 from calculation
	return dataD;
}

////////////////////////////// Function to send command to LCD
void lcd_cmd(unsigned char item) 
{ 
	////////////////////////////Sending upper nibble of command
	unsigned char a;
	a=item&0xf0;
	dataport=a;
  rs=0;
  rw=0;
  e=1;
  delay(1);
  e=0;
	////////////////////////////Sending lower nibble of command
	a=(item<<4)&0xf0;
	dataport=a;
	rs=0;
  rw=0;
  e=1;
  delay(1);
  e=0;
}
//////////////////////////////// Function to send data to LCD

void lcd_data(unsigned char item) 
{ 
	////////////////////////////Sending upper nibble of data_
  unsigned char a;
	a=item&0xf0;
	dataport=a;
  rs=1;
  rw=0;
  e=1;
  delay(1);
  e=0;
	////////////////////////////Sending lower nibble of data_
	a=(item<<4)& 0xf0;
	dataport=a;
	rs=1;
  rw=0;
  e=1;
  delay(1);
  e=0;  
}
//////////////////////////////// Function to send more than one character/data
void message(unsigned char *s)
{
	while(*s)
		lcd_data(*s++);
}
////////////////////////////////Function to ininitialize LCD

void lcdinit()
{
	lcd_cmd(0x33); // These two commands (0x33 and ox32) are used in 4 bit mode of LCD 
	lcd_cmd(0x32);
	lcd_cmd(0x28); //4 bit mode of LCD
	lcd_cmd(0x06); //display from left to right
	lcd_cmd(0x0C); //display ON, cursor hide
	lcd_cmd(0x01); //clear LCD
}

///////////////////////////////////////Main program
void main()
{
  lcdinit();
	lcd_cmd(0x80);
	message("Initialising....");
	delay(100);	
//////////////////////////////////////Code for ultrasonic sensor
	trig=0;
	TMOD=0x09;		//Timer 0 ; Mode 1 i.e 16 bit Timer
	TH0=TL0=0;		//Load the TH0 and TL0 with 0
	TR0=1;				// Start the Timer 0
	echo=1;
  while(1)
  {
	lcdinit();
  lcd_cmd(0x80);
  message(" Distance is= ");
  range=ultrasonic();
	lcd_cmd(0xc4);
	lcd_data((range/100)+48);
	lcd_data(((range%100)/10)+48);
	lcd_data(((range%100)%10)+48);
	lcd_cmd(0xc7);
	message("cm");delay(100);
  }
}