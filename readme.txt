{\rtf1\ansi\ansicpg1252\cocoartf1343\cocoasubrtf140
{\fonttbl\f0\fswiss\fcharset0 Helvetica;}
{\colortbl;\red255\green255\blue255;\red42\green47\blue60;\red244\green245\blue246;}
\margl1440\margr1440\vieww10800\viewh8400\viewkind0
\pard\tx720\tx1440\tx2160\tx2880\tx3600\tx4320\tx5040\tx5760\tx6480\tx7200\tx7920\tx8640\pardirnatural

\f0\fs24 \cf0 For running Raspberry Pi Code, Go into the Rpi folder\
	1. You must first successfully install the wiringPi libraries to do so follow instructions on\
	{\field{\*\fldinst{HYPERLINK "https://projects.drogon.net/raspberry-pi/wiringpi/download-and-install/"}}{\fldrslt https://projects.drogon.net/raspberry-pi/wiringpi/download-and-install/}}\
	or if you cd into Rpi/wiringPi, then execute ./build this should be enough.\
\
	2. execute chmod +x run.sh\
	3. execute ./run.sh\
		(This Compiles the code with the make file, runs ./Setup.php and runs ./slave.x)\
\
For The Gumstix code\
\
	1. execute Make in ul folder and km folder\
	2. move compiled code into gumstix\
	3. execute \cf2 \cb3 \expnd0\expndtw0\kerning0
\outl0\strokewidth0 \strokec2 mknod /dev/dig c 61 0\
	4. execute insmod dig.ko (runs motors)\
	5. ./ul (runs ti watch)}