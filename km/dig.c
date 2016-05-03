#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/ctype.h>
#include <asm/hardware.h>
#include <asm/gpio.h>
#include <asm/arch/pxa-regs.h>
#include <asm/uaccess.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <asm/system.h> /* cli(), *_flags */



MODULE_LICENSE("Dual BSD/GPL");

static int dig_init(void);
static int dig_exit(void);

static ssize_t dig_open(struct inode *inode, struct file *filp);
static ssize_t dig_release(struct inode *inode, struct file *filp);
//static ssize_t dig_read(struct file *filp,char *buf, size_t count, loff_t *f_pos);
static ssize_t dig_write(struct file *filp,const char *buf, size_t count, loff_t *f_pos);
static void digfunc(unsigned long);
static unsigned bite = 128;
static uint8_t capacity = 128;
module_param(capacity, uint, S_IRUGO);
module_param(bite, uint, S_IRUGO);

bool FLAG=0;
static void output(int);
int degree_step(int , int);
static void relative_angle(int);
void one(uint8_t frame);
void two(uint8_t frame);
void three(uint8_t frame);
void four(uint8_t frame);
void five(uint8_t frame);
void six(uint8_t frame);
void seven(uint8_t frame);
void eight(uint8_t frame);
void nine(uint8_t frame);
void zero(uint8_t frame);
void BU(uint8_t frame);
void U(uint8_t frame);
void down_TI();
void up_TI();
void left();
void right();
void calibrate();
void intialize();
void digtime(int m, int frame);
static int dig_len;
void start(int digit);
module_init(dig_init);
module_exit(dig_exit);
int pins[2] = {28,29};
uint gum_control[24]={118,117,16,17,101,47,46,43,45,9,30,113,11,13,14,19,31,27,27,27,100,100,100,100}; ///last 3 0s are gpio27 and gpio 100 which r dummy
// int std_stpr[4] = {0,650,1250,1880} ;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////      pins 11,13,14,19  will be going to raspberry for remaing 2 frames i.e 12 motors       ///////////////////////////////
/////////////////////////////////////////////////////////  pin 31 for led frame 0 ///////////////////////////////////////////////////////////////////////
int pattern=0;
char pattern_1[128]={'0'};
char *dig_buffer;
int  digit=0;
int sequence =0;
uint flag=0;
int stepper=0;
int angle=0;
int temp_stpr = 0;
int8_t time=0;
int counter=0;
static int i =0 ,j = 0;
static int dig_major = 61;
struct timer_list digtimer;
struct file_operations dig_fops = 
{
	
	open: dig_open,
	release: dig_release,
	write: dig_write
};


typedef struct motor
{	
	int pre_degree;
	int cur_degree;
	int step;
	uint control ;              
	int8_t rsbry_control;   ///////   4 control pins to raspberry ////////////////
	/////////4 control pins for leds should be defined in raspberry ...logic already developed////////////////////////////////////////////// 
};
struct motor mot[3][8];

unsigned long J , T =0;
int hour , min, K ;
hour = 9;
min = 58;

int msb_hour,lsb_hour;
int msb_min,lsb_min;
static int dig_init(void)
{	
	int result;
	int8_t l=0,k=0;
	result = register_chrdev(dig_major, "dig", &dig_fops);		/*registering character device*/
	if (result < 0)
	{
		printk(KERN_ALERT,"dig: cannot obtain major number %d\n", dig_major);
		return result;
	}
	printk("dig: Inserting module!\n");
	pxa_gpio_mode( pins[0] | GPIO_OUT);
	pxa_gpio_mode( pins[1] | GPIO_OUT);
	for (i=0;i<8;i++)
	for ( j = 0; j < 3; j++)
	{		
			mot[j][i].control = k;
			mot[j][i].pre_degree = 0;
			pxa_gpio_mode(gum_control[k] | GPIO_OUT);
			pxa_gpio_set_value(gum_control[k], 0);
			//printk("\n mot[%d][%d].control = %d is %d",j,i,k,pxa_gpio_get_value(gum_control[k]));
			k++;
			dig_buffer = kmalloc(capacity, GFP_KERNEL);
			memset(dig_buffer, '\0', capacity);
			dig_len=0;

			if (i>3)
			{
				mot[j][i].rsbry_control = l++; 
				// printk("\n\n /////motor[%d][%d] ka rsbry_control = %d //////////\n" ,j,i,mot[j][i].rsbry_control);
			                  ///combination for control singnals on raspberry 
				/* code */
			}

		/* code */
	}

			J = jiffies/HZ;
		
		
	
	init_timer(&digtimer);
	digtimer.data = 0;
	 digtimer.function = digfunc;
calibrate();
			digtimer.expires = jiffies + HZ;	
			add_timer(&digtimer);	
	//intialize();
	return result;
}

void intialize()
{   	printk("\nsequence = %d\n",sequence);
	int k=0;
	pxa_gpio_set_value(28, 0);		/*setting the value of respective pin as high or low*/
	pxa_gpio_set_value(29, 0); 
		//printk("\n a = b =0 \n");
	for (k=0;k<24;k++)
			pxa_gpio_set_value(gum_control[k], 1);
	if(sequence==0)
		{
			sequence++;
			two(1);
			four(2);

			digtimer.expires = jiffies + HZ;	
			add_timer(&digtimer);

		}
	 if(sequence==1)
	{
		sequence++;
		BU(1);
			digtimer.expires = jiffies + HZ;	
			add_timer(&digtimer);

	}
	else if(sequence==2)
	{	
		sequence++;
		two(0);
		zero(1);
		one(2);
		four(3);
			digtimer.expires = jiffies + HZ;	
			add_timer(&digtimer);
	}
	else if(sequence<20)
	{	sequence++;
		//printk("pattern = %d\n",pattern);
		if(pattern)
		{start(pattern);
			digtimer.expires = jiffies + HZ;	
			add_timer(&digtimer);
		}
		else intialize(); 
	}
	 if(sequence==20)
	{	
		 sequence=21;
		calibrate();
			digtimer.expires = jiffies + HZ;	
			add_timer(&digtimer);		
	}


}

void start(int digit)
{
	switch(digit)
	{
		// case 0: 
		// {
		// 	printk("\n stwich 0");
		// 	pxa_gpio_set_value(28, 0);		setting the value of respective pin as high or low
		// 	pxa_gpio_set_value(29, 0); 
		// 	break;
		// }
		case 1:
		{	
			down_TI();
			break;
		}
		case 2: {
			up_TI();
			break;

		}
		case 3:{
			right();
			//printk("\n enttering right");
			break;
		}

		case 4:
		{
			left();
			//printk("\n enttering left");
			break;

		}
		case 5:{       

			if ( (J-T) > 59)
			{   
			    min = min + (J-T)/60;
    			T= J;
			}

			if(min > 59)
			{
    			K = min/60;
    			hour = hour + K;
    			min = min%60;
			}

			if(hour > 23)
			{
				 		hour = 0;
			}   
			printk("Hour: %d Min : %d", hour, min);

			J = jiffies/HZ;
			printk("\n\ntime : %lu\t jiffies = %lu\n" , J++,jiffies);
			
			msb_hour=hour/10;
			lsb_hour=hour%10;
			msb_min=min/10;	
			lsb_min=min%10;/////////////////bhau
			digtime(msb_hour , 0);
			digtime(lsb_hour , 1);
			digtime(msb_min , 2);
			digtime(lsb_min , 3); 
			break;
		}
	}
}

void digtime(int m , int frame)
{

	printk("\nin digtime\n");
	switch(m)
	{	
		case 0: zero(frame); break;
		case 1: one(frame);break;
		case 2: two(frame);break;
		case 3: three(frame);break;
		case 4: four(frame);break;
		case 5: five(frame);break;
		case 6: six(frame);break;
		case 7: seven(frame);break;
		case 8: eight(frame);break;
		case 9: nine(frame);break;

	}
}

static ssize_t dig_write(struct file *filp,const char *buf, size_t count, loff_t *f_pos)
{
	
	if (*f_pos >= capacity)
	{
	
		return -ENOSPC;
	}

	/* do not eat more than a bite */
	if (count > bite) count = bite;

	/* do not go over the end */
	if (count > capacity - *f_pos)
		count = capacity - *f_pos;

	memset(dig_buffer, '\0' , capacity);
	if (copy_from_user(dig_buffer+*f_pos, buf, count))
	{
		return -EFAULT;
	}
       
       strcpy(pattern_1,dig_buffer);
	  	pattern= pattern_1[0] -'0';
	  	printk("\npattern = %d",pattern);

	return count;
}



static int dig_exit(void)
{
	unregister_chrdev(dig_major, "dig");
	del_timer(&digtimer);
	kfree(dig_buffer);
	return 0;	
}

static ssize_t dig_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t dig_release(struct inode *inode, struct file *filp)
{
	return 0;
}


static void digfunc(unsigned long data)
{	//printk("\n mai hoon na\n" );
	int8_t temp,k;
	for (i=0;i<8;i++)
	for ( j = 0; j < 3; j++)
	{	
			if(mot[j][i].step ==0) 
			{   flag++;
				// printk("\n motor done  = %d %d at flag   = %d\n" ,j,i,flag);

				if(i<4)
				{
					
					k=mot[j][i].control;
					pxa_gpio_set_value(gum_control[k],0);
					 
					
			    }
			    else 
			    {	temp=mot[j][i].rsbry_control;
			    	pxa_gpio_set_value(gum_control[12],temp%2);
			    	temp>>=1;
			    	pxa_gpio_set_value(gum_control[13],temp%2);
			    	temp>>=1;
			    	pxa_gpio_set_value(gum_control[14],temp%2);
		    		temp>>=1;
			    	pxa_gpio_set_value(gum_control[15],temp%2);
			    	// printk("\n\n motor[%d][%d] done at control signal = %d%d%d%d and rasp = %d \n" ,
			    	// 	j,i,pxa_gpio_get_value(gum_control[15]),pxa_gpio_get_value(gum_control[14]),pxa_gpio_get_value(gum_control[13]),
			    	// 	pxa_gpio_get_value(gum_control[12]),mot[j][i].rsbry_control);
			    }
			
			}
			mot[j][i].step--;
		/* code */
	}
	if(flag<24)
	{	FLAG=1;
		output(time);
			if (time==4)time=0;
				else time++;
						digtimer.expires = jiffies + HZ/1000;		//setting the value of timer as 1000Hz
						add_timer(&digtimer);	
	}
	else{
			flag=0;
			FLAG=0;
			printk("\n////////////////////////////////  all done   //////////////////////////\n");
			 intialize();
			// pxa_gpio_set_value(28, 0);		//setting the value of respective pin as high or low
			// pxa_gpio_set_value(29, 0);
			// pxa_gpio_set_value(113, 0); 
			// pxa_gpio_set_value(31, 0);
			// //degree_step(angle);
			// digtimer.expires = jiffies + 4*HZ;		//setting the value of timer as per the value of a
			// add_timer(&digtimer);
			// pxa_gpio_set_value(101, 0);


	}
}

int  degree_step(int degree, int tot_deg)
{	
	switch((degree - tot_deg))
	{
		case 0: stepper=0;
				break;

		case 45: stepper=325;
				 break;
		case -315: stepper=325;
				 break;
		case 90: stepper=650;
				 break;
		case -270: stepper=650;
				   break;
		case 135: stepper=950;
					break;
		 case -225: stepper=950;
					break;
		case 180: stepper=1250;
				  break;
		case -180 : stepper=1250;
					break;
		case 225: stepper=1565;
				   break;
		case -135 : stepper=1565;
					break;
		case 270: stepper = 1880;
				  break;
		case -90 : stepper=1880;
					break;		
		case 315: stepper= 2195;
				 break;
		case -45: stepper = 2195;
				break;		
				
	}

// printk("\ndegree = %d \t relative stepper = %d \t \n\n\n" , degree, stepper);

 return stepper;
} 

static void output (int time)
{
	switch(time)
		{
		case 0:	pxa_gpio_set_value(28, 1);		/*setting the value of respective pin as high or low*/
			pxa_gpio_set_value(29, 1);

			break;
		case 1: pxa_gpio_set_value(28, 1);		/*setting the value of respective pin as high or low*/
			pxa_gpio_set_value(29, 0);
;		
			break;
		case 2: pxa_gpio_set_value(28, 0);		/*setting the value of respective pin as high or low*/ 
			pxa_gpio_set_value(29, 0); 

			break;
		case 3: pxa_gpio_set_value(28, 0);		/*setting the value of respective pin as high or low*/
			pxa_gpio_set_value(29, 1); 

			break;
	
		
		}		
}



void one(uint8_t frame)
{	
	printk("\nOne\n");
	int m ;                                 //dam: m is the 1st column in that frame and m is 2nd column in that frame
	switch(frame)
	{
		case 0: m=0; break;
		case 1: m=2; break;
		case 2: m=4; break;
		case 3: m=6; break;
	}

	mot[0][m].cur_degree = 0;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 45;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 90;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;

		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );

	mot[0][++m].cur_degree = 180;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 180;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 90;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
}

void two(uint8_t frame)
{
	printk("\nTwo\n");
		int m ;                                 //dam: m is the 1st column in that frame and m is 2nd column in that frame
	switch(frame)
	{
		case 0: m=0; break;
		case 1: m=2; break;
		case 2: m=4; break;
		case 3: m=6; break;
	}

	mot[0][m].cur_degree = 90;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 180;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 90;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
	mot[0][++m].cur_degree = 180;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 270;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 270;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
}

void three(uint8_t frame)
{
	printk("\nThree\n");
		int m ;                                 //dam: m is the 1st column in that frame and m is 2nd column in that frame
	switch (frame){
		case 0: m=0; break;
		case 1: m=2; break;
		case 2: m=4; break;
		case 3: m=6; break;
	}

	mot[0][m].cur_degree = 90;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 90;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree= 90;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
	mot[0][++m].cur_degree = 180;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 180;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 0;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
}

void four(uint8_t frame)
{
	printk("\nFour\n");
		int m ;                                 //dam: m is the 1st column in that frame and m is 2nd column in that frame
	switch (frame){
		case 0: m=0; break;
		case 1: m=2; break;
		case 2: m=4; break;
		case 3: m=6; break;
	}

	mot[0][m].cur_degree = 180;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 90;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 225;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
	mot[0][++m].cur_degree = 180;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 0;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 0;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
	}

void five(uint8_t frame)
{
	printk("\n Five\n");
	int m ;                                 //dam: m is the 1st column in that frame and m is 2nd column in that frame
	switch (frame){
		case 0: m=0; break;
		case 1: m=2; break;
		case 2: m=4; break;
		case 3: m=6; break;
	}

	mot[0][m].cur_degree = 90;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 0;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 90;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
	mot[0][++m].cur_degree = 270;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 270;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 0;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
}

void six(uint8_t frame)
{
	printk("\n Six\n");
		int m ;                                 //dam: m is the 1st column in that frame and m is 2nd column in that frame
	switch (frame){
		case 0: m=0; break;
		case 1: m=2; break;
		case 2: m=4; break;
		case 3: m=6; break;
	}

	mot[0][m].cur_degree = 180;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 180;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 90;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
	mot[0][++m].cur_degree = 270;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 0;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 0;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
}

void seven(uint8_t frame)
{
	printk("\n Seven\n");
	int m ;                                 //dam: m is the 1st column in that frame and m is 2nd column in that frame
	switch (frame){
		case 0: m=0; break;
		case 1: m=2; break;
		case 2: m=4; break;
		case 3: m=6; break;
	}

	mot[0][m].cur_degree = 90;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 90;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 225;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
	mot[0][++m].cur_degree = 180;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 90;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 0;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
}

void eight(uint8_t frame)
{
	uint8_t temp;
	printk("\n Eight\n");
	// flag_for_8=1;
int m ;                                 //dam: m is the 1st column in that frame and m is 2nd column in that frame
	switch (frame){
		case 0: m=0; break;
		case 1: m=2; break;
		case 2: m=4; break;
		case 3: m=6; break;
	}

	mot[0][m].cur_degree = 90;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 90;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 90;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
	 // printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );

	mot[0][++m].cur_degree = 180;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 180;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 180;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
	 // printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );

		    

}


void nine(uint8_t frame)
{
int m ;                                 //dam: m is the 1st column in that frame and m is 2nd column in that frame
	switch (frame){
		case 0: m=0; break;
		case 1: m=2; break;
		case 2: m=4; break;
		case 3: m=6; break;
	}

	mot[0][m].cur_degree = 180;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 90;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 90;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		// 	 printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );

	mot[0][++m].cur_degree = 270;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 0;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 0;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		// 	 printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );

}

void zero(uint8_t frame)
{
		printk("\n Zero\n");
int m;                                 //dam: m is the 1st column in that frame and m is 2nd column in that frame
	switch (frame){
		case 0: m=0; break;
		case 1: m=2; break;
		case 2: m=4; break;
		case 3: m=6; break;
	}

	mot[0][m].cur_degree = 180;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 180;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 90;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );


	mot[0][++m].cur_degree = 270;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 0;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 0;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
}



void BU(uint8_t frame)
{
		 printk("\n BU \n");
int m=0;
			mot[0][m].cur_degree = 225;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 225;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 225;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;	

	mot[0][++m].cur_degree = 225;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 225;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 225;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;                  
	               //dam: m is the 1st column in that frame and m is 2nd column in that frame
// 	switch (frame){
// 		case 0: m=0; break;
// 		case 1: m=2; break;
// 		case 2: m=4; break;
// 		case 3: m=6; break;
// 	}

	mot[0][++m].cur_degree = 90;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 0;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 0;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );


	mot[0][++m].cur_degree = 270;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 270;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 270;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
                              //dam: m is the 1st column in that frame and m is 2nd column in that frame
	// switch (frame){
	// 	case 0: m=0; break;
	// 	case 1: m=2; break;
	// 	case 2: m=4; break;
	// 	case 3: m=6; break;
	// }

	mot[0][++m].cur_degree = 90;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 0;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 0;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );


	mot[0][++m].cur_degree = 270;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 135;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 270;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );

	 mot[0][++m].cur_degree = 90;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 0;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 0;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );


	mot[0][++m].cur_degree = 270;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 270;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 270;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;

}

void calibrate(void)
{    printk("\n  Calibrate\n");
	uint8_t m;
    for(m=0;m<8;m++)
	
	{
	mot[0][m].cur_degree = 0;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 0;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 0;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
	}
}

void down_TI(void)
{   printk("\n Down\n");
	uint8_t m;
    for(m=0;m<8;m++)
	
	{
		mot[0][m].cur_degree = 225;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 225;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 225;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
	}
}
void up_TI(void)
{   uint8_t m;
	 printk("\n Up\n");
    for(m=0;m<8;m++)
	
	{
	mot[0][m].cur_degree = 45;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 45;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 45;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
	}
}
void right(void)
{    printk("\n Right \n");
	uint8_t m;
    for(m=0;m<8;m++)
	
	{
		mot[0][m].cur_degree = 90;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 90;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 90;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
	}
}
void left(void)
{    printk("\n Left \n");
	uint8_t m;
    for(m=0;m<8;m++)
	
	{
	mot[0][m].cur_degree = 270;
	mot[0][m].step = degree_step(mot[0][m].cur_degree, mot[0][m].pre_degree);
	mot[0][m].pre_degree = mot[0][m].cur_degree;

	mot[1][m].cur_degree = 270;
	mot[1][m].step = degree_step(mot[1][m].cur_degree, mot[1][m].pre_degree);
	mot[1][m].pre_degree = mot[1][m].cur_degree;

	mot[2][m].cur_degree = 270;
	mot[2][m].step = degree_step(mot[2][m].cur_degree, mot[2][m].pre_degree);
	mot[2][m].pre_degree = mot[2][m].cur_degree;
		//  printk("\nmot[0][%d].step  = %d ", m ,mot[0][m].step );
	 // printk("\nmot[1][%d].step  = %d ", m ,mot[1][m].step );
	 // printk("\nmot[2][%d].step  = %d ", m ,mot[2][m].step );
	}
}

