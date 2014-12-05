#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "led.h"
#define pi 3.1415626535

POS LOC::data_access()
{
	coordinate A1;
	coordinate B1;
	coordinate C1;
	A1.x = A.x;
	A1.y = A.y;
	B1.x = B.x;
	B1.y = B.y;
	C1.x = C.x;
	C1.y = C.y;
	double data_receive[3];//data_receive[0]记录X,data_receive[1]记录Y,data_receive[2]记录旋转角度,旋转角度顺时针为正，逆时针为负
	/*确定参考点坐标*/
	coordinate A0;
	coordinate B0;
	coordinate C0;
	coordinate centre0;
	centre0.x = length/2;
	centre0.y = width/2;
	double contour;
	contour = sqrt((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y));
	A0.x = centre0.x - 0.5  * contour;
	A0.y = centre0.y - ((sqrt(3.))/6.) * contour;
	B0.x = centre0.x;
	B0.y = centre0.y + ((sqrt(3.))/3.) * contour;
	C0.x = centre0.x + 0.5 * contour;
	C0.y = centre0.y - ((sqrt(3.))/6.) * contour;
	/*确定三点中心*/
	coordinate centre1;
	centre1.x = (A1.x + B1.x + C1.x)/3;
	centre1.y = (A1.y + B1.y + C1.y)/3;
	/*确定X和Y*/
	data_receive[0] = centre1.x - centre0.x;
	data_receive[1] = centre1.y - centre0.y;
	/*确定消除平移后的三角形坐标*/
	coordinate A2;
	coordinate B2;
	coordinate C2;
	A2.x = A1.x - data_receive[0];
	A2.y = A1.y - data_receive[1];
	B2.x = B1.x - data_receive[0];
	B2.y = B1.y - data_receive[1];
	C2.x = C1.x - data_receive[0];
	C2.y = C1.y - data_receive[1];
	/*通过斜率计算得到旋转角度*/
	double k1;
	double k2;
	double k3;
	if(centre1.x != centre0.x)
		k3 = (centre1.y - centre0.y)/(centre1.x - centre0.x);
	double degree_temp;
	double degree = 0;//degree代表小车偏离正北多少度，顺时针为正，逆时针为负
	if(num==1)
	{
		if(centre0.x != A2.x)
		{
			k1 = (centre0.y - A2.y)/(centre0.x - A2.x);
			degree = 90-atan(-k1)*180./pi;
			
			if(A2.x > B0.x)
				degree = degree + 180;
		}
		else degree = 0; 
		if(centre1.x != A1.x)
		{
			k2 = (centre1.y - A1.y)/(centre1.x - A1.x);
			if(centre1.x != centre0.x)
				degree_temp = atan((k2-k3)/(1+k2*k3));
			else degree_temp = pi/2 - atan(k2);
		}
		else
		{
			if(centre1.x != centre0.x)
				degree_temp = pi/2 - atan(k3);
			else degree_temp = 0;
		}		
	}
	if(num==2)
	{
		if(centre0.x != B2.x)
		{
			k1 = (centre0.y - B2.y)/(centre0.x - B2.x);
			degree = 90-atan(-k1)*180./pi;
			if(B2.x > B0.x)
				degree = degree + 180;
		}
		else degree = 0; 
		if(centre1.x != B1.x)
		{
			k2 = (centre1.y - B1.y)/(centre1.x - B1.x);
			if(centre1.x != centre0.x)
				degree_temp = atan((k2-k3)/(1+k2*k3));
			else degree_temp = pi/2 - atan(k2);
		}
		else
		{
			if(centre1.x != centre0.x)
				degree_temp = pi/2 - atan(k3);
			else degree_temp = 0;
		}
	}
	if(num==3)
	{
		if(centre0.x != C2.x)
		{
			k1 = (centre0.y - C2.y)/(centre0.x - C2.x);
			degree = 90-atan(-k1)*180./pi;
			if(C2.x > B0.x)
				degree = degree + 180;
		}
		else degree = 0;
		if(centre1.x != C1.x)
		{
			k2 = (centre1.y - C1.y)/(centre1.x - C1.x);
			if(centre1.x != centre0.x)
				degree_temp = atan((k2-k3)/(1+k2*k3));
			else degree_temp = pi/2 - atan(k2);
		}
		else
		{
			if(centre1.x != centre0.x)
				degree_temp = pi/2 - atan(k3);
			else degree_temp = 0;
		}
	}
	if(condition == 1)
		degree=degree+180.;
	if(degree > 360)
		degree -= 360;
	data_receive[2] = degree;
	double L = sqrt((centre1.x - centre0.x)*(centre1.x - centre0.x)+(centre1.y - centre0.y)*(centre1.y - centre0.y));
	data_receive[1] = L*fabs(cos(degree_temp))*((centre0.y - centre1.y)/fabs(centre0.y - centre1.y));
	data_receive[0] = L*fabs(sin(degree_temp))*((centre0.y - centre1.y)/fabs(centre0.y - centre1.y));
	POS position;
	position.positionx=data_receive[0];
	position.positiony=data_receive[1];
	position.positiondegree=data_receive[2];
	return position;
}