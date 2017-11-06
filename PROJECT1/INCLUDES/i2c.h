#ifndef I2C_H
#define I2C_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>


int32_t setup_i2c(int8_t filename[],uint8_t slave_address);


uint16_t read_register_halfword_temp(int32_t file,uint8_t command_reg);
void write_register_halfword_temp(int32_t file, uint8_t command_reg, uint16_t write_value);

uint8_t read_reg_byte_light(int32_t file,uint8_t command_reg);
void write_reg_byte_light(int32_t file, uint8_t command_reg, uint8_t write_value);


#endif
