/****************************************************************************************
* Authors : Vishal Vishnani
* Date : 10/06/2017
* 
* File : i2c.h
* Description : Header file for i2c libraries
*               -setup_i2c()
*               -read_register_halfword_temp()
*               -read_register_byte_light()
*               -write_register_halfword_()
*               -write_register_byte_light()
******************************************************************************************/


#ifndef I2C_H
#define I2C_H

/*INCLUDES*/
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>


/******************************************************************************************
* setup_i2c() - It is used to setup a particular i2c
*           
*
* int8_t filename - Filename to be open for i2c
* uint8_t slave_address - Device address which will be using i2c
*     
* int32_t(return)- Returns the file
*******************************************************************************************/
int32_t setup_i2c(int8_t filename[],uint8_t slave_address);



/******************************************************************************************
* read_register_halfword() - Used to read 16 bits at specific address/register in file
*           
* int32_t file - File to read register 
* uint8_t command_reg - Register/Address to be read
*        
* uint16_t (return)- Value read is returned
*******************************************************************************************/
uint16_t read_register_halfword_temp(int32_t file,uint8_t command_reg);



/******************************************************************************************
* write_register_halfword() - Used to write 16 bits at specific address/register in file
*           
* int32_t file - File to write register 
* uint8_t command_reg - Register/Address to  write value
* uint16_t write_value - Value to write
*******************************************************************************************/
void write_register_halfword_temp(int32_t file, uint8_t command_reg, uint16_t write_value,uint8_t set_value);



/******************************************************************************************
* read_register_byte() - Used to read 8 bits at specific address/register in file
*           
* int32_t file - File to read register 
* uint8_t command_reg - Register/Address to be read
*        
* uint16_t (return)- Value read is returned
*******************************************************************************************/
uint8_t read_reg_byte_light(int32_t file,uint8_t command_reg);



/******************************************************************************************
* write_register_byte() - Used to read 8 bits at specific address/register in file
*           
* int32_t file - File to write register 
* uint8_t command_reg - Register/Address to be written
* uint8_t write_value - value to write       
*******************************************************************************************/
void write_reg_byte_light(int32_t file, uint8_t command_reg, uint8_t write_value);


#endif
