#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <fcntl.h>

#define DEV_ADDR (0x48)
#define I2C_NUM (2)
#define BUFFER_SIZE (2)
#define TEMP_REG (0x00)
#define CONFIG_REG (0x01)

#define EXTENDED_MODE13 ((0x1)<<12)
#define EXTENDED_MODE12 ((0x0)<<12)
#define SHUTDOWN_ENABLE (0x1)
#define SHUTDOWN_DISABLE (0x0)
#define THERMOSTAT_COMPARATOR ((0x0)<<1)
#define THERMOSTAT_INTERRUPT ((0x1)<<1)
#define POLARITY_LOW ((0x0)<<2)
#define POLARITY_HIGH ((0x1)<<2)
#define FAULTS_ONE ((0x00)<<3)
#define FAULTS_TWO ((0x01)<<3)
#define FAULTS_FOUR ((0x2)<<3)
#define FAULTS_SIX ((0x3)<<3)
#define ONE_SHOT_ENABLE ((0x1)<<7)
#define ONE_SHOT_DISABLE ((0x1)<<7)
#define CONV_RATE_1BY4 ((0x00)<<14)
#define CONV_RATE_ONE ((0x01)<<14)
#define CONV_RATE_FOUR ((0x2)<<14)
#define CONV_RATE_EIGHT ((0x03)<<14)

uint8_t read_buffer[3]={'\0'};
uint8_t write_buffer[3]={'\0'};


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


  
void write_pointer_reg(int32_t file,uint8_t register_type){
  uint8_t write_value=0;
  write_value=register_type;

  if(write(file,&write_value,1)!=1){
    printf("\nERROR: Write in write_pointer_reg\n");
  }
}



uint16_t read_register_halfword(int32_t file,uint8_t command_reg){
  write_pointer_reg(file,command_reg);

  uint8_t read_buffer[2]={'\0'};
 
  if(read(file,read_buffer,2)!=2){
    printf("\nERROR: Read Register\n");
  }

  uint16_t read_value=read_buffer[0] | read_buffer[1] << 8;
 
  return read_value;   
}



void write_register_halfword(int32_t file, uint8_t command_reg, uint16_t write_value){

  uint16_t read_value=read_register_halfword(file,(uint8_t)CONFIG_REG);
 
  uint16_t comp_write_value=~write_value;
  read_value=read_value & comp_write_value;
  write_value=write_value | read_value;

  uint8_t write_buffer[3]={'\0'};
  write_buffer[0]=command_reg;
  write_buffer[1]=(uint8_t)(write_value & 0xff);
  write_buffer[2]=(uint8_t)((write_value & 0xff00)>>8);

  if(write(file,write_buffer,3)!=3){
    printf("\nERROR: Write Register\n");
  }
   
}



void configure_conv_rate(int32_t file, uint8_t command_reg, uint16_t write_value){
  uint16_t read_value=read_register_halfword(file,command_reg);
  write_register_halfword(file,command_reg,write_value);
  
}



void configure_shutdown_mode(int32_t file,uint8_t command_reg, uint16_t mode_value){
  uint16_t read_value=read_register_halfword(file,command_reg);
  write_register_halfword(file,command_reg,mode_value);
}



void read_write_configuration_register(int32_t file,uint8_t op, uint16_t write_value){

  uint16_t read_value=read_register_halfword(file,(uint8_t)CONFIG_REG);

  //read operation
  if(op==0){
    printf("\nRead value = %x\n",read_value);
  }
  else{
    write_register_halfword(file,(uint8_t) CONFIG_REG, write_value);       
  }
}



int16_t read_temperature(int32_t file, uint8_t command_reg){
  write_pointer_reg(file,command_reg);

  uint8_t read_buffer[2]={'\0'};

  if(read(file,read_buffer,2)!=2){
    printf("\nERROR: Read Register\n");
  }

  int16_t temp_read_value = read_buffer[0]<<4 | read_buffer[1]>>4;

  return temp_read_value;

}



int main(){

  int8_t filename[20];
  int32_t file;
  snprintf(filename,19,"/dev/i2c-%d",I2C_NUM);
  file=setup_i2c(filename);

  //read config reg
  uint16_t read_value = read_register_halfword(file, (uint8_t)CONFIG_REG);

  printf("\nRead value = %x\n",read_value); 

  //set conv rate
  configure_conv_rate(file, (uint8_t)CONFIG_REG, (uint16_t)CONV_RATE_EIGHT);
  read_value = read_register_halfword(file, (uint8_t)CONFIG_REG);
  printf("\nRead value = %x\n",read_value);

  //enable shutdown mode
  configure_shutdown_mode(file,(uint8_t)CONFIG_REG,(uint16_t)SHUTDOWN_DISABLE);
  read_value = read_register_halfword(file, (uint8_t)CONFIG_REG);
  printf("\nRead value = %x\n",read_value);


  //temperature read value
  int16_t temp_value=read_temperature(file,(uint8_t)TEMP_REG);
  printf("\nTemperature value = %x\n",temp_value);

  //read/write configuration register
  uint16_t write_value=POLARITY_HIGH;
  read_write_configuration_register(file,1,write_value);

  read_write_configuration_register(file,0,write_value);
  close(file);

  return 0;

}




