#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <string.h>


/*
 * react
 * Parameter : the input signal
 * Description : reacts by turning gpio pins on or off depending on the input signal recieved from gumstix.
 */
void react(char key[5])
{

	 	printf("Reading =\t%s\n",key);	
		int i=1;
		if(strcmp (key,"0000\0") !=0){
			printf("DO Nothing\n");
		}
		else if(strcmp (key,"0001\0") !=0){
			digitalWrite(11, 0); //gpio 17
		        printf("Turned pin 11 Off\n");
		}
		else if(strcmp (key,"0010\0") !=0 ){
			digitalWrite(13, 0); //gpio 27
			printf("Turned pin 13 Off\n");
		}
		else if( strcmp (key,"0011\0") !=0){
			digitalWrite(15, 0); //gpio 22
			printf("Turned pin 15 Off\n");
		}
		else if( strcmp (key,"0100\0") !=0){ //gpio 10
			digitalWrite(19, 0);
		        printf("Turned pin 19 Off\n");
		}
		else if( strcmp (key,"0101\0") !=0){ //gpio 9
			digitalWrite(21, 0);
			printf("Turned pin 21 Off\n");
		}
		else if( strcmp (key,"0110\0") !=0){ //gpio 11
			digitalWrite(23, 0);
			printf("Turned pin 23 Off\n");
		}
		else if(strcmp (key,"0111\0") !=0){ //5
			digitalWrite(29, 0);
			printf("Turned pin 29 Off\n");
		}
		else if( strcmp (key,"1000\0") !=0){ //6
			digitalWrite(31, 0);
			printf("Turned pin 31 Off\n");
		}
		else if( strcmp (key,"1001\0") !=0){ //13
			digitalWrite(33, 0);
			printf("Turned pin 33 Off\n");
		}
		else if( strcmp (key,"1010\0") !=0){ //19
			digitalWrite(35, 0);
			printf("Turned pin 35 Off\n");
		}
		else if( strcmp (key,"1011\0") !=0){ //26
			digitalWrite(37, 0);
		 	printf("Turned pin 37 Off\n");
		}
		else if(strcmp (key,"1100\0") !=0){ //21
			digitalWrite(40, 0);
			printf("Turned pin 40 Off\n");
		}
		else if( strcmp (key,"1101\0") !=0){ // DO NOTHING
			//	printf("Turned Pin 7 OFF\n");
			// digitalWrite(7, 0);
			printf("13 Do Nothing\n");
		}
		else if(strcmp (key,"1110\0") !=0){ // all low
			printf("All Pins OFF\n");
			for(i=1; i <41; i++)
			{
				digitalWrite(i, 0);
			}
		}
		else if( strcmp (key,"1111\0") !=0){ // all high
			printf("All Pins ON\n");
			for(i=1; i <41; i++)
			{	
				digitalWrite(i, 1);
			}
		}

	
}

int main(int argc, char *argv[]){
	
	char inco[5];

        //Sets up the wiring pi api to use the physical gpio pin numbers as identifiers
		wiringPiSetupPhys();
	
	/* Prints out all values of input pins as the code sees it and may not be accurate
	int k =0;
	for(k; k<41;k++)
	{
		int f =digitalRead(k);
		printf("%d = %d\n",k,f);
	}
	*/
	

    // Loops at a 1 millisecond interval , reading the input pins for signals sent from the Gumstix
	while(1)
	{
		strcpy(inco,"0000\0");

		if(digitalRead(26))
		{
			inco[0] ='1';
		}

		if(digitalRead(27))
		{
			inco[1] ='1';
		}

		if(digitalRead(28))
		{
			inco[2] ='1';
		}

		if(digitalRead(29))	
		{
			inco[3] ='1';
		}
		//printf("Reading =\t%s\n",inco);
		
		react(inco);
		usleep(1);  				// CHANGE THIS VALUE FOR DIFFRENT SAMPLING TIMES
	}

	
	return 0;


}
