#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define DEV_ADDR (0x48)
#define I2C_NUM (2)
#define BUFFER_SIZE (2)
#define TEMP_REG (0x00)
#define CONFIG_REG (0x01)
#define EXTENDED_MODE ((0x1)<<12)
#define SHUTDOWN_MODE (0x1)
#define THERMOSTAT_MODE ((0x1)<<1)
#define POLARITY_MODE ((0x1)<<2)
#define ONE_SHOT_MODE ((0x1)<<7)
#define CONV_RATE_1BY4 ((0x00)<<14)
#define CONV_RATE_ONE ((0x01)<<14)
#define CONV_RATE_FOUR ((0x2)<<14)
#define CONV_RATE_EIGHT ((0x03)<<14)

#define RETURN_ZERO (0)
#define RETURN_CONV_RATE_8 (0xC000)
#define RETURN_SHUTDOWN_MODE (0x01)
#define RETURN_POLARITY_MODE (0x04)
#define RETURN_THERMOSTAT_MODE (0x02)


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



void write_register_halfword(int32_t file, uint8_t command_reg, uint16_t write_value,uint8_t set_value){

  uint16_t read_value=read_register_halfword(file,(uint8_t)CONFIG_REG);
 
 
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



uint16_t configure_conv_rate(int32_t file, uint8_t command_reg, uint16_t write_value,uint8_t set_value){
  uint16_t read_value=read_register_halfword(file,command_reg);
  write_register_halfword(file,command_reg,write_value,set_value);
  read_value=read_register_halfword(file,command_reg);
   
  return read_value;
}



uint16_t configure_shutdown_mode(int32_t file,uint8_t command_reg, uint16_t mode_value,uint8_t set_value){
  uint16_t read_value=read_register_halfword(file,command_reg);
  write_register_halfword(file,command_reg,mode_value,set_value);
  read_value=read_register_halfword(file,command_reg);
  return read_value;
}



uint16_t read_write_configuration_register(int32_t file,uint8_t op, uint16_t write_value,uint8_t set_value){

  uint16_t read_value=read_register_halfword(file,(uint8_t)CONFIG_REG);

  //read operation
  if(op==0){
    printf("\nRead value = %x\n",read_value);
  }
  else{
    write_register_halfword(file,(uint8_t) CONFIG_REG, write_value,set_value); 
    read_value=read_register_halfword(file,(uint8_t)CONFIG_REG);      
  }

  return read_value;
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

  int8_t sign=1;


  //set conv rate to 8
  uint16_t read_value=configure_conv_rate(file, (uint8_t)CONFIG_REG, (uint16_t)CONV_RATE_EIGHT,1);

  if((read_value & RETURN_CONV_RATE_8 )==RETURN_CONV_RATE_8){
    printf("\nTEST 1: CONFIGURE CONVERSION RATE 8 PASSED\n");
  }
  else{
    printf("\nTEST 1: CONFIGURE CONVERSION RATE 8 FAILED\n");
  }



  //enable shutdown mode
  read_value=configure_shutdown_mode(file,(uint8_t)CONFIG_REG,(uint16_t)SHUTDOWN_MODE,1);
  if((read_value & RETURN_SHUTDOWN_MODE )==RETURN_SHUTDOWN_MODE){
    printf("\nTEST 2: CONFIGURE SHUTDOWN ENABLE PASSED\n");
  }
  else{
    printf("\nTEST 2: CONFIGURE SHUTDOWN ENABLE FAILED\n");
  }



  //disable shutdown mode
  read_value=configure_shutdown_mode(file,(uint8_t)CONFIG_REG,(uint16_t)SHUTDOWN_MODE,0);
  if((read_value & RETURN_SHUTDOWN_MODE )==RETURN_ZERO){
    printf("\nTEST 3: CONFIGURE SHUTDOWN DISABLE PASSED\n");
  }
  else{
    printf("\nTEST 3: CONFIGURE SHUTDOWN DISABLE FAILED\n");
  }

  //set polarity high
  uint16_t write_value=POLARITY_MODE;
  read_value=read_write_configuration_register(file,1,write_value,1);

  if((read_value & RETURN_POLARITY_MODE )==RETURN_POLARITY_MODE){
    printf("\nTEST 4: CONFIGURE POLARITY HIGH PASSED\n");
  }
  else{
    printf("\nTEST 4: CONFIGURE POLARITY HIGH FAILED\n");
  }

  //set polarity low
  write_value=POLARITY_MODE;
  read_value=read_write_configuration_register(file,1,write_value,0);

  if((read_value & RETURN_POLARITY_MODE )==RETURN_ZERO){
    printf("\nTEST 5: CONFIGURE POLARITY HIGH PASSED\n");
  }
  else{
    printf("\nTEST 5: CONFIGURE POLARITY HIGH FAILED\n");
  }


  //configure interrupt mode
  write_value=THERMOSTAT_MODE;
  read_value=read_write_configuration_register(file,1,write_value,1);


  if((read_value & RETURN_THERMOSTAT_MODE )==RETURN_THERMOSTAT_MODE){
    printf("\nTEST 6: CONFIGURE INTERRUPT MODE PASSED\n");
  }
  else{
    printf("\nTEST 6: CONFIGURE INTERRUPT MODE FAILED\n");
  }
 
  //configure comparator mode
  write_value=THERMOSTAT_MODE;
  read_value=read_write_configuration_register(file,1,write_value,0);


  if((read_value & RETURN_THERMOSTAT_MODE )==RETURN_ZERO){
    printf("\nTEST 7: CONFIGURE COMPARATOR MODE PASSED\n");
  }
  else{
    printf("\nTEST 7: CONFIGURE COMPARATOR MODE FAILED\n");
  }


  close(file);

  return 0;

}




