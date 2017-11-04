#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <fcntl.h>


#define DEV_ADDR (0x39)
#define I2C_NUM (2)
#define BUFFER_SIZE (2)
#define COMMAND_CONTROL_REG (0x80)
#define POWER_UP (0x03)
#define POWER_DOWN (0x00)
#define COMMAND_TIMING_REG (0x81)
#define INTEGRATE_TIME (0x01)
#define GAIN (0x0)
#define TIMING_REG ((GAIN<<4)|INTEGRATE_TIME)
#define IDENTIFICATION_REG (0x8A)
#define PERSISTENCE (0x4)
#define INTR (0x01)
#define COMMAND_TH_LOW_LOWER (0x82)
#define COMMAND_TH_LOW_HIGHER (0x83)
#define COMMAND_TH_HIGH_LOWER (0x84)
#define COMMAND_TH_HIGH_HIGHER (0x85)
#define COMMAND_INT_REG (0x86)
#define LOW_TH_LOWER (0x0f)
#define LOW_TH_HIGHER (0x00)
#define HIGH_TH_LOWER (0x00)
#define HIGH_TH_HIGHER (0x08)
#define INTR_VALUE ((INTR<<4)|PERSISTENCE)
#define COMMAND_ADC0_LOW (0x8C)
#define COMMAND_ADC0_HIGH (0x8D);


int32_t setup_i2c(int8_t filename[]){
  int32_t file=open(filename,O_RDWR);
  
  if(file<0){
    printf("\nERROR: Open file\n");
    exit(1);
  }

  if(ioctl(file, I2C_SLAVE, DEV_ADDR)<0){
    printf("\nERROR: Slave Address Resolution\n");
    exit(1);
  }

  return file;

}



void write_reg_byte(int32_t file, uint8_t command_reg, uint8_t write_value){
  uint8_t write_buffer[BUFFER_SIZE];
  memset(write_buffer,'\0',sizeof(write_buffer));
  write_buffer[0]=command_reg;
  write_buffer[1]=write_value;
  if(write(file,write_buffer,2)!=2){
    printf("\nERROR: Write1\n");
  }
}



uint8_t read_reg_byte(int32_t file,uint8_t command_reg){
  
  if(write(file,&command_reg,1)!=1){
    printf("\nERROR: Write2\n");
  }
  uint8_t read_value;
  if(read(file,&read_value,1)!=1){
    printf("\nERROR: Write2\n");
  }
  return read_value;
}



int main(){

  int8_t filename[20];
  int32_t file;
  snprintf(filename,19,"/dev/i2c-%d",I2C_NUM);
  file=setup_i2c(filename);

  //write control register
  write_reg_byte(file,(uint8_t) COMMAND_CONTROL_REG, (uint8_t) POWER_UP);

  //read control register
  uint8_t ret=read_reg_byte(file,(uint8_t) COMMAND_CONTROL_REG);
  printf("\nControl Register = %x\n",ret);

  //write timing register
  write_reg_byte(file,(uint8_t) COMMAND_TIMING_REG, (uint8_t) TIMING_REG);

  
  //read timing register
  ret=read_reg_byte(file,(uint8_t) COMMAND_TIMING_REG);
  printf("\nTiming Register = %x\n",ret);
 

  //read identifiation register
  ret=read_reg_byte(file,(uint8_t) IDENTIFICATION_REG);
  printf("\nIdentification Register = %x\n",ret);



  //write low threshold lower byte register
  write_reg_byte(file,(uint8_t) COMMAND_TH_LOW_LOWER, (uint8_t)LOW_TH_LOWER);

  //write low threshold higher byte register
  write_reg_byte(file,(uint8_t) COMMAND_TH_LOW_HIGHER, (uint8_t)LOW_TH_HIGHER);

  //write high threshold lower byte register
  write_reg_byte(file,(uint8_t) COMMAND_TH_HIGH_LOWER, (uint8_t)HIGH_TH_LOWER);

  //write high threshold higher byte register
  write_reg_byte(file,(uint8_t) COMMAND_TH_HIGH_HIGHER, (uint8_t)HIGH_TH_HIGHER);

  //write interrupt control register to enable interrupts
  write_reg_byte(file,(uint8_t) COMMAND_INT_REG, (uint8_t)INTR_VALUE);

  //read interrupt control register
  ret=read_reg_byte(file,(uint8_t) COMMAND_INT_REG);
  printf("\nInterrupt Control Register = %x\n",ret);

  //read ADC0 low
  ret=read_reg_byte(file,(uint8_t) COMMAND_ADC0_LOW);

  //read ADC0 high
  uint8_t ret1=0;
  if(read(file,&ret1,1)!=1){
    printf("\nERROR: Write2\n");
  }

  uint16_t sensor_data= (ret1<<8)|ret;
  printf("\nSensor Data Register = %x\n",sensor_data);

  close(file);

  return 0;
}
