#ifndef I2C_TEMP_H
#define I2C_TEMP_H

#include "i2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define DEV_ADDR_TEMP (0x48)
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

void temp_write_pointer_reg(int32_t file,uint8_t register_type);

void temp_configure_conv_rate(int32_t file, uint8_t command_reg, uint16_t write_value);

void temp_configure_shutdown_mode(int32_t file,uint8_t command_reg, uint16_t mode_value);

void temp_read_write_configuration_register(int32_t file,uint8_t op, uint16_t write_value);

int16_t read_temperature(int32_t file, uint8_t command_reg);

#endif
