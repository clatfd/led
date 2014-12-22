#include <stdio.h>
#include <iostream>
#include <wiringPi.h>
#include <fstream>
using namespace std;

ifstream fmodein;
ofstream fclear;

//front left motor
#define FRONTLEFTMOTORFORPIN 0
#define FRONTLEFTMOTORBACKPIN 2
//front right motor
#define FRONTRIGHTMOTORFORPIN 8
#define FRONTRIGHTMOTORBACKPIN 9
//behind left motor
#define BEHINDLEFTMOTORFORPIN 10
#define BEHINDLEFTMOTORBACKPIN 11
//behind right motor
#define BEHINDRIGHTMOTORFORPIN 5
#define BEHINDRIGHTMOTORBACKPIN 4

#define MOTORSTEPTIME 300

void initwire(){
	wiringPiSetup();
	pinMode(FRONTLEFTMOTORFORPIN,OUTPUT);
	pinMode(FRONTLEFTMOTORBACKPIN,OUTPUT);
	pinMode(FRONTRIGHTMOTORFORPIN,OUTPUT);
	pinMode(FRONTRIGHTMOTORBACKPIN,OUTPUT);
	pinMode(BEHINDLEFTMOTORFORPIN,OUTPUT);
	pinMode(BEHINDLEFTMOTORBACKPIN,OUTPUT);
	pinMode(BEHINDRIGHTMOTORFORPIN,OUTPUT);
	pinMode(BEHINDRIGHTMOTORBACKPIN,OUTPUT);
}

void Turnleft(){
	cout<<"turning left.."<<endl;
	
	digitalWrite(FRONTLEFTMOTORFORPIN,HIGH);	
	digitalWrite(FRONTLEFTMOTORBACKPIN,LOW);
	digitalWrite(FRONTRIGHTMOTORFORPIN,LOW);
	digitalWrite(FRONTRIGHTMOTORBACKPIN,LOW);
	digitalWrite(BEHINDLEFTMOTORFORPIN,HIGH);	
	digitalWrite(BEHINDLEFTMOTORBACKPIN,LOW);
	digitalWrite(BEHINDRIGHTMOTORFORPIN,LOW);
	digitalWrite(BEHINDRIGHTMOTORBACKPIN,LOW);
	delay(MOTORSTEPTIME);
	digitalWrite(FRONTLEFTMOTORFORPIN,LOW);
	digitalWrite(BEHINDLEFTMOTORFORPIN,LOW);
}

void Turnright(){
	cout<<"turning right.."<<endl;
	
	digitalWrite(FRONTLEFTMOTORFORPIN,LOW);	
	digitalWrite(FRONTLEFTMOTORBACKPIN,LOW);
	digitalWrite(FRONTRIGHTMOTORFORPIN,HIGH);
	digitalWrite(FRONTRIGHTMOTORBACKPIN,LOW);
	digitalWrite(BEHINDLEFTMOTORFORPIN,LOW);	
	digitalWrite(BEHINDLEFTMOTORBACKPIN,LOW);
	digitalWrite(BEHINDRIGHTMOTORFORPIN,HIGH);
	digitalWrite(BEHINDRIGHTMOTORBACKPIN,LOW);
	delay(MOTORSTEPTIME);
	digitalWrite(FRONTRIGHTMOTORFORPIN,LOW);
	digitalWrite(BEHINDRIGHTMOTORFORPIN,LOW);
}

void Straightforward(){
	cout<<"Going forward.."<<endl;
	
	digitalWrite(FRONTLEFTMOTORFORPIN,HIGH);	
	digitalWrite(FRONTLEFTMOTORBACKPIN,LOW);
	digitalWrite(FRONTRIGHTMOTORFORPIN,HIGH);
	digitalWrite(FRONTRIGHTMOTORBACKPIN,LOW);
	digitalWrite(BEHINDLEFTMOTORFORPIN,HIGH);	
	digitalWrite(BEHINDLEFTMOTORBACKPIN,LOW);
	digitalWrite(BEHINDRIGHTMOTORFORPIN,HIGH);
	digitalWrite(BEHINDRIGHTMOTORBACKPIN,LOW);
	delay(MOTORSTEPTIME);
	digitalWrite(FRONTLEFTMOTORFORPIN,LOW);
	digitalWrite(FRONTRIGHTMOTORFORPIN,LOW);
	digitalWrite(BEHINDLEFTMOTORFORPIN,LOW);
	digitalWrite(BEHINDRIGHTMOTORFORPIN,LOW);
}

void Straightbackward(){
	cout<<"Going backward.."<<endl;
	
	digitalWrite(FRONTLEFTMOTORFORPIN,LOW);	
	digitalWrite(FRONTLEFTMOTORBACKPIN,HIGH);
	digitalWrite(FRONTRIGHTMOTORFORPIN,LOW);
	digitalWrite(FRONTRIGHTMOTORBACKPIN,HIGH);
	digitalWrite(BEHINDLEFTMOTORFORPIN,LOW);	
	digitalWrite(BEHINDLEFTMOTORBACKPIN,HIGH);
	digitalWrite(BEHINDRIGHTMOTORFORPIN,LOW);
	digitalWrite(BEHINDRIGHTMOTORBACKPIN,HIGH);
	delay(MOTORSTEPTIME);
	digitalWrite(FRONTLEFTMOTORBACKPIN,LOW);
	digitalWrite(FRONTRIGHTMOTORBACKPIN,LOW);
	digitalWrite(BEHINDLEFTMOTORBACKPIN,LOW);
	digitalWrite(BEHINDRIGHTMOTORBACKPIN,LOW);
}

int main(){
	initwire();
//	while(1)
//	Straightforward();
//	Straightbackward();
//	Turnright();
//	Turnleft();
	while(1){
		fmodein.open("/usr/share/nginx/www/led/setdir.txt"); 
		char inputmode;
		fmodein>>inputmode;
		if(inputmode!='s'){
			switch(inputmode){
				case 'l':Turnleft();break;
				case 'r':Turnright();break;
				case 'u':Straightforward();break;
				case 'd':Straightbackward();break;
				default:break;
			}
			fmodein.close();
			fclear.open("/usr/share/nginx/www/led/setdir.txt"); 
			fclear<<"s"<<endl;
			fclear.close();
		}
		else
			fmodein.close();
	}
}
