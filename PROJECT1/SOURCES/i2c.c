#include "i2c.h"
#include "i2c_temp.h"
#include "i2c_light.h"



int32_t setup_i2c(int8_t filename[],uint8_t slave_address){
  int32_t file=open(filename,O_RDWR);

  if(file<0){
    printf("\nERROR: Open file\n");
    exit(1);
  }

  if(ioctl(file, I2C_SLAVE, slave_address)<0){
    printf("\nERROR: Slave Address Resolution\n");
    exit(1);
  }

  return file;
}


uint16_t read_register_halfword_temp(int32_t file,uint8_t command_reg){

  if(ioctl(file, I2C_SLAVE, (uint8_t)DEV_ADDR_TEMP)<0){
    printf("\nERROR: Slave Address Resolution\n");
    exit(1);
  }

  temp_write_pointer_reg(file,command_reg);

  uint8_t read_buffer[2]={'\0'};

  if(read(file,read_buffer,2)!=2){
    printf("\nERROR: Read Register\n");
  }

  uint16_t read_value=read_buffer[0] | read_buffer[1] << 8;

  return read_value;
}



void write_register_halfword_temp(int32_t file, uint8_t command_reg, uint16_t write_value,uint8_t set_value){

  if(ioctl(file, I2C_SLAVE, (uint8_t)DEV_ADDR_TEMP)<0){
    printf("\nERROR: Slave Address Resolution\n");
    exit(1);
  }

  uint16_t read_value=read_register_halfword_temp(file,(uint8_t)CONFIG_REG);

  uint16_t comp_write_value=~write_value;
  read_value=read_value & comp_write_value;

  if(set_value==1){
    write_value=write_value | read_value;
  }
  else{
    write_value=read_value;
  }


  uint8_t write_buffer[3]={'\0'};
  write_buffer[0]=command_reg;
  write_buffer[1]=(uint8_t)(write_value & 0xff);
  write_buffer[2]=(uint8_t)((write_value & 0xff00)>>8);

  if(write(file,write_buffer,3)!=3){
    printf("\nERROR: Write Register\n");
  }

}

void write_reg_byte_light(int32_t file, uint8_t command_reg, uint8_t write_value){

  if(ioctl(file, I2C_SLAVE, (uint8_t)DEV_ADDR_LIGHT)<0){
    printf("\nERROR: Slave Address Resolution\n");
    exit(1);
  }

  uint8_t write_buffer[BUFFER_SIZE];
  memset(write_buffer,'\0',sizeof(write_buffer));
  write_buffer[0]=command_reg;
  write_buffer[1]=write_value;
  if(write(file,write_buffer,2)!=2){
    printf("\nERROR: Write1\n");
  }
}


uint8_t read_reg_byte_light(int32_t file,uint8_t command_reg){

  if(ioctl(file, I2C_SLAVE, (uint8_t)DEV_ADDR_LIGHT)<0){
    printf("\nERROR: Slave Address Resolution\n");
    exit(1);
  }


  if(write(file,&command_reg,1)!=1){
    printf("\nERROR: Write2\n");
  }
  uint8_t read_value;
  if(read(file,&read_value,1)!=1){
    printf("\nERROR: Write2\n");
  }
  return read_value;
}




