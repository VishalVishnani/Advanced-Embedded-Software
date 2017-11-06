#include "i2c_light.h"
#include "i2c.h"


uint8_t light_write_read_control_register(int32_t file,uint8_t write_value,uint8_t op){
  uint8_t ret=0;
  if(op==1){
    write_reg_byte_light(file,(uint8_t) COMMAND_CONTROL_REG,write_value);
  }
  else{
    ret=read_reg_byte_light(file,(uint8_t) COMMAND_CONTROL_REG);
    printf("\nControl Register Value = %x\n",ret);
  }
  return ret;

}


uint8_t light_configure_integration_time(int32_t file, uint8_t write_value){
  write_reg_byte_light(file,(uint8_t) COMMAND_TIMING_REG,write_value);

  //read timing register
  uint8_t ret=read_reg_byte_light(file,(uint8_t) COMMAND_TIMING_REG);
  printf("\nTiming Register = %x\n",ret);

  return ret;
}

uint8_t light_read_identification_register(int32_t file){
  uint8_t ret=read_reg_byte_light(file,(uint8_t) IDENTIFICATION_REG);
  printf("\nIdentification Register = %x\n",ret);
  return ret;
}

uint8_t light_enable_disable_interrupt_control_register(int32_t file,uint8_t write_value){
   write_reg_byte_light(file,(uint8_t) COMMAND_INT_REG, write_value);

  //read interrupt control register
  uint8_t ret=read_reg_byte_light(file,(uint8_t) COMMAND_INT_REG);
  printf("\nInterrupt Control Register = %x\n",ret);

  return ret;
}


uint16_t light_read_sensor_data_value(int32_t file){
  //read ADC0 low
  uint8_t ret=read_reg_byte_light(file,(uint8_t) COMMAND_ADC0_LOW);

  uint8_t ret1=0;
  if(read(file,&ret1,1)!=1){
    printf("\nERROR: Write2\n");
  }

  uint16_t sensor_data= (ret1<<8)|ret;
  return sensor_data;
}

