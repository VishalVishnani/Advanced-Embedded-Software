#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


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


#define RETURN_ZERO (0)
#define RETURN_POWER_UP (0x33)
#define RETURN_INTEGRATION_TIME (0x1)
#define RETURN_IDENTIFICATION_REGISTER (0x50)
#define RETURN_INTERRUPT_CONTROL_REGISTER (0x14)

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


uint8_t write_read_control_register(int32_t file,uint8_t write_value,uint8_t op){
  uint8_t ret=0;
  if(op==1){
    write_reg_byte(file,(uint8_t) COMMAND_CONTROL_REG,write_value);
  }
  else{
    ret=read_reg_byte(file,(uint8_t) COMMAND_CONTROL_REG);
  }
  return ret;
}


uint8_t configure_integration_time(int32_t file, uint8_t write_value){
  write_reg_byte(file,(uint8_t) COMMAND_TIMING_REG,write_value);

  //read timing register
  uint8_t ret=read_reg_byte(file,(uint8_t) COMMAND_TIMING_REG);

  return ret;
}


uint8_t read_identification_register(int32_t file){
  uint8_t ret=read_reg_byte(file,(uint8_t) IDENTIFICATION_REG);

  return ret;
}


uint8_t enable_disable_interrupt_control_register(int32_t file,uint8_t write_value){
   write_reg_byte(file,(uint8_t) COMMAND_INT_REG, write_value);

  //read interrupt control register
  uint8_t ret=read_reg_byte(file,(uint8_t) COMMAND_INT_REG);

  return ret;
}


uint16_t read_sensor_data_value(int32_t file){
  //read ADC0 low
  uint8_t ret=read_reg_byte(file,(uint8_t) COMMAND_ADC0_LOW);

  uint8_t ret1=0;
  if(read(file,&ret1,1)!=1){
    printf("\nERROR: Write2\n");
  }

  uint16_t sensor_data= (ret1<<8)|ret;
  return sensor_data;
}

int main(){

  int8_t filename[20];
  int32_t file;
  snprintf(filename,19,"/dev/i2c-%d",I2C_NUM);
  file=setup_i2c(filename);

  uint8_t ret=write_read_control_register(file,(uint8_t)POWER_UP,1);
  ret=write_read_control_register(file,(uint8_t)POWER_UP,0);

  if(ret==RETURN_POWER_UP){
    printf("\nTEST1: READ/WRITE CONTROL REGISTER PASSED\n");
  }
  else{
    printf("\nTEST1: READ/WRITE CONTROL REGISTER FAILED\n");
  }


  ret=configure_integration_time(file, (uint8_t)TIMING_REG);

  if(ret==RETURN_INTEGRATION_TIME){
    printf("\nTEST2: CONFIGURE INTEGRATION REGISTER PASSED\n");
  }
  else{
    printf("\nTEST2: CONFIGURE INTEGRATION REGISTER FAILED\n");
  }



  ret=read_identification_register(file);

  if(ret==RETURN_IDENTIFICATION_REGISTER){
    printf("\nTEST3: IDENTIFICATION REGISTER PASSED\n");
  }
  else{
    printf("\nTEST3: IDENTIFICATION REGISTER FAILED\n");
  }


  ret=enable_disable_interrupt_control_register(file,(uint8_t)INTR_VALUE);
  if(ret==RETURN_INTERRUPT_CONTROL_REGISTER){
    printf("\nTEST4: ENABLE/DISABLE INTERRUPT CONTROL REGISTER PASSED\n");
  }
  else{
    printf("\nTEST4: ENABLE/DISABLE INTERRUPT CONTROL REGISTER FAILED\n");
  }


  close(file);

  return 0;
}
