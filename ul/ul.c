#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyACM0"
char Input;
char START_AP[] = { 0xFF, 0x07, 0x03 };
char ASK_ACC_DATA[] = { 0xFF, 0x08, 0x07, 0x00, 0x00, 0x00, 0x00 };
FILE *pFile;
int main(int argc, char **argv) 
{

	int fd, fp, res;
	struct termios options;
	char buf[255];
	char acc[256] = "";

	char line[256];
	int ii,  count = 0;
	int i =0, count1 =0, count2 =0, count3 = 0, count4 =0, count5=0 , flag1 = 0, flag2 = 0,flag3 = 0,flag4 = 0,flag5 = 0;
	fd = open(MODEMDEVICE, O_RDWR );
	if (fd < 0) 
	{
		printf(" MODEMDEVICE " );
		return -1;
	}

	/* Check to see if the mytimer successfully has mknod run
	   Assumes that mytimer is tied to /dev/mytimer */

	fp=open("/dev/dig", O_RDWR);
	 // pFile = fopen("/dev/dig", "r+");
	// if (pFile==NULL) {
	// 	fputs("dig module isn't loaded\n",stderr);
	// 	return -1;
	// }

	bzero(&options, sizeof(options));
	options.c_cflag = CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;

	options.c_lflag = 0;
	options.c_cc[VTIME] = 0; /* inter-character timer unused */
	options.c_cc[VMIN] = 3;

	tcgetattr(fd, &options);
	cfsetispeed(&options, BAUDRATE);
	cfsetospeed(&options, BAUDRATE);
	cfmakeraw(&options);
	tcsetattr(fd, TCSANOW, &options);
	tcflush(fd, TCIOFLUSH);

	if(write(fd, START_AP, sizeof(START_AP))==-1)
	{
		printf("write() failed \n");
		return -1;
	}

	res = read(fd, buf, 3);
	if(res == -1)
	{
		printf("read() failed");
	}

	printf("Reached here \n");

	while(1)
	{
		if(write(fd, ASK_ACC_DATA, sizeof(ASK_ACC_DATA))==-1) 
		{
			printf("write() failed\n");
			return -1;
		}
		res = read(fd, buf, 7); 

		if(res==-1) 
		{
			printf("read() failed\n");
			exit(-1);
		}

		if(buf[3]==1) 
		{

			if((flag1 > 0) && (flag2 > 0) && (flag3 > 0) && (flag4 > 0))
			{
				flag1 = 0;
				flag2 = 0;
				flag3 = 0;
				flag4 = 0;
				flag5 = 0;
				printf("Entered here\n");
			}
			// for down 
			for(i=0; i < 500 ; i ++)
			{
				if((buf[4] > 240) && ((buf[5] > 195) && (buf[5] < 210)) && (buf[6] > 230))
				{
					count1++;
				}
			}
			
			if(count1 > 495 && flag1 <1 )
			{	Input ='1';
				printf("\nputiing %c\n",Input);
				// fputs("1", pFile);       
				write(fp, &Input,sizeof(Input));
				flag1++;
				count1 = 0;
				count2 = 0;
				count3 = 0;
				count4 = 0;
			}
			
			// for down
			for(i=0; i < 500 ; i ++)
			{
				if((buf[4] <10 ) && ((buf[5] >35) && (buf[5] < 55)) && (buf[6] > 230))
				{
					count2++;
				}
			}
			
			if(count2 >495 && flag2 <1 )
			{
				
				Input ='2';
				printf("\nputiing %c\n",Input);
				// fputs("1", pFile);       
				write(fp, &Input,sizeof(Input));
				flag2++;
				count1 = 0;
				count2 = 0;
				count3 = 0;
				count4 = 0;
			}

			// for right
			for(i=0; i < 500 ; i ++)
			{
				if(((buf[4] > 195) &&(buf[4] < 210)) && (buf[5] >240) && ((buf[6] > 205) && (buf[6] < 225)))
				{
					count3++;
				}
			}
			
			if(count3 > 495 && flag3 <1 )
			{
				Input ='3';
				printf("\nputiing %c\n",Input);
				// fputs("1", pFile);       
				write(fp, &Input,sizeof(Input));
				flag3++;
				count1 = 0;
				count2 = 0;
				count3 = 0;
				count4 = 0;
			}

			// for right down
			for(i=0; i < 500 ; i ++)
			{
				if((buf[4] > 240) && (buf[5] >240) && (buf[6] > 15 && buf[6] < 35))
				{
					count4++;
				}
			}
			
			if(count4 > 495 && flag4 <1 )
			{
				Input ='4';
				printf("\nputiing %c\n",Input);
				// fputs("1", pFile);       
				write(fp, &Input,sizeof(Input));
				flag4++;
				count1 = 0;
				count2 = 0;
				count3 = 0;
				count4 = 0;
			}
		}
		
		if (buf[3]==18)		// for PPT
		{
			count5++;
			if(count5 > 10)
			{
				Input ='5';
				printf("\nputiing %c\n",Input);
				// fputs("1", pFile);       
				write(fp, &Input,sizeof(Input));		
				flag5++;
				count5 = 0;
			}
		}
	}


	fclose(pFile);
	return 0;
}
