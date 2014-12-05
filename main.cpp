
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<vector>
#include <fstream> 
//if windows
#ifdef _WIN32
#include <windows.h>
//if linux
#else
#include <unistd.h>
#include<wiringPi.h>
#endif
#include "opencv2/opencv.hpp"
#include "led.h"

#define W 400
#define H 346
#define THRESH 30
#define CAMERAID 0
#define LENGTHINVISION 200
#define DEMO		//comment to hide display; uncomment to show ,press ESC to capture

using namespace cv;
using namespace std;

ofstream locout("loc.txt");	

LED ledattr[7];

Point2f circlecenter[3];
vector< vector<Point> >	contours;
VideoCapture cap(CAMERAID);

bool Initcamera(){
	if(!cap.isOpened())
	{
		cout<<"Can not open camera"<<endl;
		return 0;
	}
	return 1;
}

void Initled(){
	ledattr[1].Initledattr(0,0,0,0,0);
	ledattr[2].Initledattr(0,1,0,1,0);
	ledattr[3].Initledattr(1,1,1,0.5,0.87);
	ledattr[4].Initledattr(2,0,0,0,1.73);
	ledattr[5].Initledattr(2,1,0,1,1.73);
	ledattr[6].Initledattr(3,0,1,0.5,2.60);
}

void LED::Initledattr(int trow,int tcol,bool tparity,double tx,double ty){
	row=trow;
	col=tcol;
	parity=tparity;
	x=tx*LENGTHINVISION;
	y=ty*LENGTHINVISION;
}

void Capturephoto(){
	Mat frame;
#ifdef DEMO 
	while(1){
		cap>>frame;
		imshow("video",frame);  
		char c=cvWaitKey(33);  
		//press ESC to capture
		if(c==27)
			break;  
	}
#else
	#ifdef _WIN32
		Sleep(500);
	#else
		usleep(500*1000);  
	#endif
	cap>>frame;
#endif
	Mat img2;
    resize(frame,img2,Size(461,346),0,0,CV_INTER_LINEAR);
	img2 = img2(Range(0,346),Range(30,430));
	imwrite("cpic.jpg",img2);
}

bool Checkvalidlocation(Point2f center,int currentid){
	int x=(int)center.x;
	int y=(int)center.y;
	if(2*H*x+W*y<H*W/2){
		cout<<"("<<x<<","<<y<<") is in top left corner. Invalid!"<<endl;
		return 0;
	}
	if(2*H*x-W*y>3*H*W/2){
		cout<<"("<<x<<","<<y<<") is in top right corner. Invalid!"<<endl;
		return 0;
	}
	if(2*H*x-W*y<-H*W/2){
		cout<<"("<<x<<","<<y<<") is in bottom left corner. Invalid!"<<endl;
		return 0;
	}
	if(2*H*x+W*y>5*H*W/2){
		cout<<"("<<x<<","<<y<<") is in bottom right corner. Invalid!"<<endl;
		return 0;
	}
	
	return 1;
}

bool Checkpoints(Mat frame){
	Mat image;
	cvtColor (frame,image,CV_BGR2GRAY);	
	Mat imgCanny;
	Canny(image, imgCanny, THRESH, THRESH * 3);
	findContours(imgCanny, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	#ifdef DEMO 
	imshow("canny",imgCanny);
	#endif
	//waitKey(0);
	cout<<"Find "<<contours.size()<<" led!"<<endl;
	if(contours.size()<3||contours.size()>500)
		return 0;
	//find center
	float radius[500];
	Point2f possiblecirclecenter[500];
	int validcenternum=0;
	for(unsigned int i=0;i<contours.size();i++){
		minEnclosingCircle(contours[i], possiblecirclecenter[i], radius[i]);
		if((radius[i]*radius[i]*3.14<3000)&&(radius[i]*radius[i]*3.14>300)&&(contourArea(contours[i])<3000)&&(contourArea(contours[i])>300)&&Checkvalidlocation(possiblecirclecenter[i],validcenternum)){
			circlecenter[validcenternum].x=(int)possiblecirclecenter[i].x;
			circlecenter[validcenternum].y=(int)possiblecirclecenter[i].y;
			validcenternum++;
		}
	}
	//check valid location 
	if(validcenternum==3){
		cout<<"valid"<<endl;
		return 1;
	}
	else{
		cout<<"Err:Find "<<validcenternum<<" valid led in vision."<<endl;
		return 0;
	}
}

int Selcoltype(int ori){
	/*
	if(ori>=0&&ori<80)
		return 0;
	else if(ori>=80&&ori<=165)
		return 1;
	if(ori>165&&ori<=255)
		return 2;
	*/
	if(ori<127)
		return 0;
	else 
		return 1;
}
int SelHSVcoltype(int h,int s){
	if(h>=220&&h<260)	//blue
		return 1;
	else if(h>96&&h<=144)//green
		return 2;
	else if(h>159&&h<=201)//light blue
		return 3;
	else if(h>339||h<=21)//red
		return 4;
	else if(h>279&&h<=321)//purple
		return 5;
	else if(h>39&&h<=81)//yellow
		return 6;
	else 
		return -1;
}
LOC Examinepoint(LED led1, LED led2, LED led3){
	LOC location;
	location.num=-1;
	if(led1.row==led2.row){
		location.num=3;
		if(led1.row-led3.row==1)
			location.condition=1;//South
		else if(led3.row-led1.row==1)
			location.condition=0;//North
		else	//err
			location.num=-1;
	}
	else if(led1.row==led3.row){
		location.num=2;
		if(led1.row-led2.row==1)
			location.condition=1;//South
		else if(led2.row-led1.row==1)
			location.condition=0;//North
		else	//err
			location.num=-1;
	}
	else if(led2.row==led3.row){
		location.num=1;
		if(led2.row-led1.row==1)
			location.condition=1;//South
		else if(led1.row-led2.row==1)
			location.condition=0;//North
		else	//err
			location.num=-1;
	}
	location.gridx=(led1.x+led2.x+led3.x)/3;
	location.gridy=(led1.y+led2.y+led3.y)/3;
	return location;
}

//link port 0 and 1 to relay controlled motor
void Turnleft(int time){
	cout<<"turning left.."<<endl;
	#ifdef _WIN32
	#else
	digitalWrite(1,HIGH);	
	digitalWrite(0,LOW);
	delay(500);
	digitalWrite(0,HIGH);
	#endif
}

void Turnright(int time){
	#ifdef _WIN32
	#else
	cout<<"turning right.."<<endl;
	digitalWrite(0,HIGH);
	digitalWrite(1,LOW);
	delay(500);
	digitalWrite(1,HIGH);
	#endif

}

int main(){
	Initled();
	if(!Initcamera())
		return 0;
	Mat img;
	while(1){
		while(1){
			Capturephoto();
			img=imread("cpic.jpg");		//pic captured
			//img=imread("yt4.jpg");	//test
			if(Checkpoints(img))
				break;
			else{
				cout<<"Try again"<<endl;
				#ifdef _WIN32
					Sleep(1000);
				#else
					usleep(1000*1000);  
				#endif
			}
		}
	/*	
		//use rgb to select
		int cPointR[3],cPointG[3],cPointB[3],cPointID[3];
		for(int t=0;t<3;t++){
			cPointB[t]=Selcoltype(img.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[0]);  
			cPointG[t]=Selcoltype(img.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[1]);  
			cPointR[t]=Selcoltype(img.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[2]);
			//cPointID[t]=cPointR[t]*9+cPointG[t]*3+cPointB[t];
			cPointID[t]=cPointR[t]*4+cPointG[t]*2+cPointB[t];

			cout<<"ID:"<<cPointID[t]<<"  "<<cPointR[t]<<","<<cPointG[t]<<","<<cPointB[t]<<"    ";
			cout<<(int)img.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[2]<<",";
			cout<<(int)img.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[1]<<",";
			cout<<(int)img.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[0]<<endl;
			if(cPointID[t]==-1)
				break;
		}
	*/
		//use hsv to select
		int cPointID[3];
		Mat imghsv;
		cvtColor (img,imghsv,CV_BGR2HSV);
		int t;
		for(t=0;t<3;t++){
			cPointID[t]=SelHSVcoltype(imghsv.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[0]*2,imghsv.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[1]);  
			cout<<"ID:"<<cPointID[t]<<"    ";
			cout<<(double)imghsv.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[0]*2<<",";
			cout<<(double)imghsv.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[1]<<",";
			cout<<(double)imghsv.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[2]<<endl;
			if(cPointID[t]==-1)
				break;
		}
		if(cPointID[t]==-1){
			cout<<"err_pic_color"<<endl;
			continue;
		}

		LOC location=Examinepoint(ledattr[cPointID[0]],ledattr[cPointID[1]],ledattr[cPointID[2]]);
		if(location.num==-1){
			cout<<"err_loc"<<endl;
			continue;
		}
		/*
		//log location to file
		locout<<circlecenter[0].x<<","<<circlecenter[0].y<<endl;
		locout<<circlecenter[1].x<<","<<circlecenter[1].y<<endl;
		locout<<circlecenter[2].x<<","<<circlecenter[2].y<<endl;
		locout<<location.condition<<endl;
		locout<<location.num<<endl;
		*/
		cout<<"Condition:"<<location.condition<<", Node:"<<location.num<<endl;
		location.width=H;
		location.length=W;
		location.A.x=circlecenter[0].x;
		location.A.y=H-circlecenter[0].y;
		location.B.x=circlecenter[1].x;
		location.B.y=H-circlecenter[1].y;
		location.C.x=circlecenter[2].x;
		location.C.y=H-circlecenter[2].y;

		POS position=location.data_access();
		location.realx=location.gridx+position.positionx;
		location.realy=location.gridy+position.positiony;
		cout<<"X = "<<location.realx<<",Y = "<<location.realy<<endl;
		cout<<"degree = "<<position.positiondegree<<endl;

		//server location
		#ifdef _WIN32
		ofstream serverdata("E:/server/nginx/html/led/serverdata.txt");
		#else
		ofstream serverdata("/usr/share/nginx/www/led/serverdata.txt");
		#endif
		//store location info to serverfile
		serverdata<<"{\"X\":"<<location.realx<<",\"Y\":"<<location.realy<<",\"D\":"<<position.positiondegree<<"}"<<endl;
		serverdata.close();

		#ifdef _WIN32
		#else
		//motor control
		wiringPiSetup();
		pinMode(0,OUTPUT);
		pinMode(1,OUTPUT);
		#endif

		if(position.positiondegree>15&&position.positiondegree<180)
			Turnleft(position.positiondegree);
		else if(position.positiondegree<345&&position.positiondegree>180)
			Turnright(position.positiondegree);
		else
			cout<<"Straight!";

		Point circlecenterdraw[3];
		for(int i=0;i<3;i++){
			circlecenterdraw[i].x=circlecenter[i].x;
			circlecenterdraw[i].y=circlecenter[i].y;
			if(location.num==i+1)
				circle(img, circlecenterdraw[i], 3, Scalar(255,0,255), 3); 
			else
				circle(img, circlecenterdraw[i], 3, Scalar(255,0,0), 3); 
		}
		#ifdef DEMO 
		imshow("led",img);
		#endif
		//imwrite("Result.jpg",img);
		//waitKey(0);
		//destroyAllWindows();
		#ifdef _WIN32
			Sleep(2000);
		#else
			usleep(2000*1000);  
		#endif
	}
}