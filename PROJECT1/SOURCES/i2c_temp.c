/****************************************************************************************************************
* Authors : Vishal Vishnani
* Date : 10/06/2017
* 
* File : i2c_temp.c
* Description : Header file for i2c Temperature
*               -temp_write_pointer_register()
*               -temp_configure_conv_rate()
*               -temp_configure_shutdown_mode()
*               -temp_read_write_configuration_register()
*               -read_temperature()
*****************************************************************************************************************/


#include "i2c_temp.h"

  
/*Used to write pointer register of Temperature Sensor*/
void temp_write_pointer_reg(int32_t file,uint8_t register_type){
  uint8_t write_value=0;
  write_value=register_type;

  if(write(file,&write_value,1)!=1){
    printf("\nERROR: Write in write_pointer_reg\n");
  }
}


/*temp_write_pointer_reg() - Used to configure conversion rate of Temperature Sensor*/
uint16_t temp_configure_conv_rate(int32_t file, uint8_t command_reg, uint16_t write_value,uint8_t set_value){
  uint16_t read_value=read_register_halfword_temp(file,command_reg);
  write_register_halfword_temp(file,command_reg,write_value,set_value);
  read_value=read_register_halfword_temp(file,command_reg);

  return read_value;
}


/*Used to enable/disable shutdown of Temperature Sensor*/
uint16_t temp_configure_shutdown_mode(int32_t file,uint8_t command_reg, uint16_t mode_value,uint8_t set_value){
  uint16_t read_value=read_register_halfword_temp(file,command_reg);
  write_register_halfword_temp(file,command_reg,mode_value,set_value);
  read_value=read_register_halfword_temp(file,command_reg);
  return read_value;

}


/*Used to modify configuration of Temperature Sensor*/
uint16_t temp_read_write_configuration_register(int32_t file,uint8_t op, uint16_t write_value,uint8_t set_value){

  uint16_t read_value=read_register_halfword_temp(file,(uint8_t)CONFIG_REG);

  //read operation
  if(op==0){
    printf("\nRead value = %x\n",read_value);
  }
  else{
    write_register_halfword_temp(file,(uint8_t) CONFIG_REG, write_value,set_value); 
    read_value=read_register_halfword_temp(file,(uint8_t)CONFIG_REG);
      
  }
  return read_value;
}


/*Used to read ADC values of Temperature Sensor*/
int16_t read_temperature(int32_t file, uint8_t command_reg){
  temp_write_pointer_reg(file,command_reg);

  uint8_t read_buffer[2]={'\0'};

  if(read(file,read_buffer,2)!=2){
    printf("\nERROR: Read Register\n");
  }

  int16_t temp_read_value = read_buffer[0]<<4 | read_buffer[1]>>4;

  return temp_read_value;

}



