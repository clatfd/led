led location v2.0

Capture picture of led lights on the ceiling to determine where the camera equipped toy car is.

By Chen Li, Wang Dongyi

2014.11

using opencv 2.4.9

Linux using raspbian on raspberry

##File Tree

|___main.cpp		main program

 |_led.h			define classes

 |_ledattr.cpp	determine deviation location and degree based on targeted three nodes

 |_testpic		pictures used for test.

 |_ledserver		show location of car in webpage (place under server root directory)

	 |_index.html	page content

	 |_serverdata.txt location info(json format) generated by main program

	 |_jquery.js	supporting script

	 |_map.jpg		Arrange led lights like that

##compile under linux with opencv and wiringPi

gcc main.cpp ledattr.cpp -o maindemo -ggdb `pkg-config opencv --cflags --libs` -lm  -lstdc++ -lwiringPi


##linux installing opencv

sudo apt-get install opencv-doc libcv4 libhighgui4 libcvaux4 libcv-dev libcvaux-dev libhighgui-dev libgtk2.0-dev

sudo apt-get install build-essential libgtk2.0-dev libavcodec-dev libavformat-dev libjpeg62-dev libtiff4-dev cmake libswscale-dev libjasper-dev

sudo apt-get install libopencv-dev

##install wiringPi

Follow the instructions on wiringpi.com