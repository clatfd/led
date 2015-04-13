
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
vector<RotatedRect>	rectProfile;
VideoCapture cap(CAMERAID);
int cPointID[3];

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

bool Checkvalidlocation(Point2f center){
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
	if(h>=127&&h<=185)	//green
		return 1;
	else if(h>185&&h<=250)//blue
		return 3;
	else if(h>339||h<=21)//red
		return 2;
	/*
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
	*/
	else 
		return -1;
}

bool Checkpoints(Mat frame){
	Mat imghsv;
	cvtColor (frame,imghsv,CV_BGR2HSV);
	Mat mv[3];
	split(imghsv,mv);
	#ifdef DEMO 
	imshow("S",mv[1]);
	imwrite("imgS.jpg",mv[1]);
	//imshow("H",mv[0]);
	//imwrite("imgH.jpg",mv[0]);
	//imshow("V",mv[2]);
	//imwrite("imgV.jpg",mv[2]);
	#endif
	Mat Sthres;
	threshold(mv[1],Sthres,80,255,CV_THRESH_BINARY);
	#ifdef DEMO 
	imshow("S",Sthres);
	imwrite("imgthres.jpg",Sthres);
	waitKey(0);
	#endif
 	Mat imgSCanny;

//	findContours(img_erode, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	findContours(Sthres, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	/*
	Mat cc = frame;
	for (int j = 0; j < contours.size(); j++){
		Scalar color(rand()&255, rand()&255, rand()&255);
		drawContours(cc, contours, j, color);
	}
	imshow("contours", cc);
	imwrite("contours.jpg",cc);
	*/
	vector<int> validcontour;
	int validcenternum=0;
	for(unsigned int i=0;i<contours.size();i++){
		//cout<<contourArea(contours[i])<<endl;
		if(contourArea(contours[i])>1000){
			validcontour.push_back(i);
			Point2f pointcur;
			pointcur.x=(int)minAreaRect(contours[i]).center.x;
			pointcur.y=(int)minAreaRect(contours[i]).center.y;
			if(Checkvalidlocation(pointcur)){
				if(validcenternum==3){
					cout<<"more than 3 led."<<endl;
					return 0;
				}
				rectProfile.push_back(minAreaRect(contours[i]));
				circlecenter[validcenternum]=pointcur;
				validcenternum++;
			}

		}
	}
	#ifdef DEMO 
	Mat ccc = frame;
	for (int j = 0; j < validcenternum; j++){
		Scalar color(rand()&255, rand()&255, rand()&255);
		drawContours(ccc, contours, validcontour[j], color);
	}
	imshow("validcontours", ccc);
	imwrite("validcontours.jpg",ccc);
	#endif
	if(validcontour.size()!=3){
		cout<<"Err..Find "<<contours.size()<<" led!"<<endl;
		return 0;
	}
	int t;
	for(t=0;t<3;t++){
		cPointID[t]=SelHSVcoltype(imghsv.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[0]*2,imghsv.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[1]);  
		cout<<"ID:"<<cPointID[t]<<"    ";
		cout<<(double)imghsv.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[0]*2<<",";
		cout<<(double)imghsv.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[1]<<",";
		cout<<(double)imghsv.at<Vec3b>(circlecenter[t].y,circlecenter[t].x)[2]<<endl;
		if(cPointID[t]==-1){
			cout<<"err_pic_color"<<endl;
			return 0;
		}
	}
	return 1;
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

void Turnleft(int time){
	cout<<"turning left.."<<endl;
	#ifdef _WIN32
	#else
	//	ofstream fsetdir;
	//	fsetdir.open("/usr/share/nginx/www/led/setdir.txt"); 
	//	fsetdir<<"l"<<endl;
	#endif
}

void Turnright(int time){
	cout<<"turning right.."<<endl;
	#ifdef _WIN32
	#else
	
	#endif
}

void Straightforward(){
	cout<<"Going forward.."<<endl;
	#ifdef _WIN32
	#else
	
	#endif
}

void Straightbackward(){
	cout<<"Going backward.."<<endl;
	#ifdef _WIN32
	#else
	
	#endif
}

int main(){
	Initled();
	if(!Initcamera())
		return 0;
	Mat img;
	while(1){
		while(1){
			//Capturephoto();
			img=imread("cpic.jpg");		//pic captured
			//img=imread("yt2.jpg");	//test
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
		//log location to file
		locout<<circlecenter[0].x<<","<<circlecenter[0].y<<endl;
		locout<<circlecenter[1].x<<","<<circlecenter[1].y<<endl;
		locout<<circlecenter[2].x<<","<<circlecenter[2].y<<endl;
		locout<<location.condition<<endl;
		locout<<location.num<<endl;
		*/
		LOC location=Examinepoint(ledattr[cPointID[0]],ledattr[cPointID[1]],ledattr[cPointID[2]]);
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

/*	auto control
		if(position.positiondegree>15&&position.positiondegree<180)
			Turnleft(position.positiondegree);
		else if(position.positiondegree<345&&position.positiondegree>180)
			Turnright(position.positiondegree);
		else{
			Straightforward();
		}
*/
		Point circlecenterdraw[3];
		for(int i=0;i<3;i++){
			circlecenterdraw[i].x=circlecenter[i].x;
			circlecenterdraw[i].y=circlecenter[i].y;
			char locnumpic[3];
			sprintf(locnumpic,"P%d",i);
			putText(img,locnumpic, circlecenterdraw[i],FONT_HERSHEY_SIMPLEX,0.4,Scalar(255,0,0));
			if(location.num==i+1)
				circle(img, circlecenterdraw[i], 3, Scalar(255,0,255), 3); 
			else
				circle(img, circlecenterdraw[i], 3, Scalar(255,0,0), 3); 
		}
		line(img,Point(W/4,0), Point(0,H/4),Scalar(0,0,255));
		line(img,Point(3*W/4,0),Point(W,H/4),Scalar(0,0,255));
		line(img,Point(W/4,H),Point(0,3*H/4),Scalar(0,0,255));
		line(img,Point(3*W/4,H),Point(W,3*H/4),Scalar(0,0,255));
		#ifdef DEMO 
		imshow("led",img);
		#endif
		imwrite("Result.jpg",img);
		//waitKey(0);
		//destroyAllWindows();
		#ifdef _WIN32
			Sleep(500);
		#else
			usleep(15000*1000);  
		#endif
	}
}