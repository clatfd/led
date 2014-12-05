class LED{
public:
	void Initledattr(int row,int col,bool parity,double x,double y);
	int row;
	int col;
	bool parity;	//row num   0-even 1-odd
	double x;
	double y;
};

class POS{
public:
	double positionx;	//deviation pixel from center
	double positiony;
	double positiondegree;
};

struct coordinate{
	double x;
	double y;
};

class LOC{
public:
	POS data_access();
	double length;	//image size
	double width;
	coordinate A;
	coordinate B;
	coordinate C;
	int condition;	//0,1		special node in north(0)/south(1)
	int num;		//1,2,3		node A(1)/B(2)/C(3) in north/south
	double gridx;	// real center location of three leds in room
	double gridy;
	double realx;	//real location now, after adjusting with deviation(position.positionx)
	double realy;
};