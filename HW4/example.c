#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");

/*Structure to store state, frequency, dutycycle*/
typedef struct packet_t{
  int state;
  int frequency;
  int duty_cycle;
}packet;

packet packet1;

/*Timer instance*/
struct timer_list my_timer;

/*GPIO pin used*/
static unsigned int gpioLED=53;

/*default led state*/
static bool ledOn=0;
static int i=0;

/*default ontime*/
static uint32_t ontime=500;

/*default offtime*/
static uint32_t offtime=500;

/*Default period*/
static uint32_t period=1000;

static uint8_t flag = 1;

/*Timer call back*/
void my_timer_callback(unsigned long data){
  int8_t ret=-1;

  i=i+1;

  //if led state on
  if(packet1.state==1){

    //for on time
    if((i%2)==0){
      printk(KERN_ALERT "TIMER EXPIRED %d ON TIME\n",i);
      ret=mod_timer(&my_timer,jiffies+msecs_to_jiffies(ontime));
      if(ret!=0){
        printk(KERN_ALERT "MOD TIMER FAILED\n");
      }

      
      ledOn=true;
    }

    //for off time
    else{
      printk(KERN_ALERT "TIMER EXPIRED %d OFF TIME\n",i);
      ret = mod_timer(&my_timer,jiffies+msecs_to_jiffies(offtime));
      if(ret!=0){
        printk(KERN_ALERT "MOD TIMER FAILED\n");
      }

      ledOn=false;
    }
  }
  else{
    ledOn=false;
    i=1;
    flag = 1 ;
  }

  //set the gpio value
  gpio_set_value(gpioLED,ledOn);

}



int simple_open (struct inode *pinode, struct file *pfile)
{
  printk(KERN_ALERT "Inside the %s function1\n",__FUNCTION__);
  return 0;
}



int simple_close (struct inode *pinode, struct file *pfile){
  printk(KERN_ALERT "Inside the %s function1\n",__FUNCTION__);
  return 0;
}



ssize_t simple_read (struct file *pfile, char __user *buffer,size_t length, loff_t *offset){
  int8_t ret=-1;

  //Copy to user the recent device parameter values
  ret=copy_to_user(buffer,&packet1,sizeof(packet1));

  printk(KERN_ALERT "Inside the %s function1\n",__FUNCTION__);
  return 0;

}



ssize_t simple_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset){

  int ret=0;
  
  //copy from user the packet structure with device parameters
  ret=copy_from_user(&packet1,buffer,length);
  if(ret!=0){
    printk(KERN_ALERT "Copy from user failed\n");
    return ENOMEM;
  }

  printk(KERN_ALERT "State= %d\n",packet1.state);
  printk(KERN_ALERT "Frequency= %d\n",packet1.frequency);
  printk(KERN_ALERT "Dutycycle= %d\n",packet1.duty_cycle);

  //period, ontime, offtime calculation
  period=packet1.frequency*1000;
  ontime=(packet1.duty_cycle * period)/100;
  offtime=period-ontime;  

  //add timer only once
  if(packet1.state==1 && flag == 1){  
    my_timer.expires=jiffies+msecs_to_jiffies(ontime);
    my_timer.data=0;
    my_timer.function=my_timer_callback;
    add_timer(&my_timer);
    flag = 0;
  }

  ledOn=(bool)packet1.state;

  //set the gpio value
  gpio_set_value(gpioLED,ledOn);

  return length;
}


//Mapping function defined in this module to file operations structure
struct file_operations simple_file_operations = {
  .owner= THIS_MODULE,
  .open= simple_open,
  .read= simple_read,
  .write= simple_write,
  .release= simple_close,
};



//Function to be executed when insert the module
int example_init(void){

  printk(KERN_ALERT "Inside the INITIALIZATION function1\n");


  //register the character driver
  register_chrdev(240, "Simple Char Drv",&simple_file_operations);
  

  //check if gpio is valid
  if(!gpio_is_valid(gpioLED)){
    printk(KERN_ALERT "INVALID GPIO\n");
    return -ENODEV;
  }

  printk(KERN_ALERT "LED INITIALIZED\n");


  ledOn=false;
  gpio_request(gpioLED,"sysfs");

  //set the direction as output
  gpio_direction_output(gpioLED,ledOn);

  //export the gpio
  gpio_export(gpioLED, false);

  //Initialize timer
  init_timer(&my_timer);

  //setup timer 
  setup_timer(&my_timer,my_timer_callback,0);

  return 0;
}

//Function to be executed wen we remove the module
void example_exit(void){
  int8_t ret=0;

  ret=del_timer(&my_timer);
  if(ret!=0){
    printk(KERN_ALERT "DELETE TIMER FAILED\n");
  }

  //off the led
  gpio_set_value(gpioLED,0);
      
  //unexport the gpio
  gpio_unexport(gpioLED);

  //free
  gpio_free(gpioLED);


  printk(KERN_ALERT "Inside the %s function\n",__FUNCTION__);

  //Unregister the char driver
  unregister_chrdev(240,"Simple Char Drv");

}

module_init(example_init);
module_exit(example_exit);

