/* Accident Detection and Alerting System
   Vibration sensor is used for accident detection
	 When accident occurs, 
	    SMS will be sent with a google map link that is attached with current location co-ordinates */


#include<reg51.h>

#define LCD P2

sbit rs = P2^0;
sbit en = P2^1;
sbit vib = P1^0;

void lcd_init();
void display(unsigned char *s);
void lcd_cmd(unsigned char x);
void lcd_data(unsigned char y);
void ini();
void send(char *z);
void serial(unsigned char s);
char rx();

unsigned int c = 1,a,k,i,j;
unsigned int count = 0;
unsigned char gps_location[26]="",lat[12]="",lon[12]="";

void ini(){
	TMOD = 0x20;
	TH1 = 0xfd;
	SCON = 0x50;
	TR1 = 1;
}

void delay(unsigned int x){
	unsigned int i,j;
	for(i = 0;i<x;i++){
		for(j = 0;j<1275;j++);
	}
}

void serial(unsigned char s){
	SBUF = s;
	while (TI==0);
	TI=0;
}

void send(char *z){
	unsigned int k;
	for(k = 0; z[k]!=0;k++){
		serial(z[k]);
	}
}

char rx(){
	while(RI==0);
	a = SBUF;
	RI=0;
	return a;
}

void lcd_init(){
	lcd_cmd(0x02);
	delay(5);
	lcd_cmd(0x28);
	delay(5);
	lcd_cmd(0x0c);
	delay(5);
}

void display(unsigned char *s){
	while(*s){
		lcd_data(*s++);
	}
}

void lcd_data(unsigned char y){
	LCD = y & (0xf0);
	rs = 1;
	en = 1;
	delay(5);
	en = 0;
	LCD = ((y<<4)&(0xf0));
	rs = 1;
	en = 1;
	delay(5);
	en = 0;
}

void lcd_cmd(unsigned char x){
	LCD = x & (0xf0);
	rs = 0;
	en = 1;
	delay(5);
	en = 0;
	LCD = ((x<<4)&(0xf0));
	rs = 0;
	en = 1;
	delay(5);
	en = 0;
}

void main(){
	lcd_init();
	lcd_cmd(0x01);
	delay(200);
	lcd_cmd(0x80);
	display("WELCOME");
	delay(200);
	send("AT\n");
	delay(200);
	send("ATE0\n");
	delay(200);
	send("AT+CSQ \n");
	delay(200);
	send("AT+CMGF=1\r\n");
	delay(200);
	send("AT+CMGD=1\r\n");
	delay(200);
	while(1){
		lcd_cmd(0x01);
		delay(100);
		lcd_cmd(0x80);
		display("Getting");
		lcd_cmd(0xc0);
		display("Location...");
		delay(100);
		while(rx()!='C');
		for(count=0;count<13;count++){
			rx();
		}
		for(count=0;count<24;count++){
			gps_location[count]=rx();
		}
		for(count=0;count<11;count++){
			lat[count]=gps_location[count];
		}
		for(count=0;count<11;count++){
			lon[count]=gps_location[count+12];
		}
		lcd_cmd(0x01);
		delay(100);
		lcd_cmd(0x80);
		display("Location Found");
		delay(100);
		lcd_cmd(0x01);
		delay(100);
		lcd_cmd(0x80);
		display("Latitude:");
		lcd_cmd(0xc0);
		display(lat);
		delay(100);
		lcd_cmd(0x01);
		delay(100);
		lcd_cmd(0x80);
		display("Longitude:");
		lcd_cmd(0xc0);
		display(lon);
		delay(100);
		lcd_cmd(0x01);
		if(vib == 1 && c == 1){
			send("AT+CMGF=1\n");
			delay(200);
			send("AT+CMGS=\"number\"\n");
			delay(200);
			send("Accident Occured");
			send("https://maps.google.com/maps/place/");
			serial(lat[0]);
			serial(lat[1]);
			send("%%20");
			for(i = 2;i<11;i++){
				serial(lat[i]);
			}
			send(",");
			serial(lon[0]);
			serial(lon[1]);
			send("%%20");
			for(i = 2;i<11;i++){
				serial(lon[i]);
			}
			serial(0x1a);
			delay(200);
			display("Message sent");
			c = 2;
		}
		else if(vib== 0 && c == 2){
			c = 1;
		}
	}
}