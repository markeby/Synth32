//#######################################################################
// Module:     ADS1115.h
// Descrption: Interface constants for analog to digital converter
// Creator:    markeby
// Date:       2/8/2026
//#######################################################################
#pragma once

#include <Wire.h>
#include <stdint.h>

// I2C addresses. You have to connect the address pin with the corresponding data line
#define ADS1115_I2C_ADDR_GND            0b1001000
#define ADS1115_I2C_ADDR_VDD            0b1001001
#define ADS1115_I2C_ADDR_SDA            0b1001010
#define ADS1115_I2C_ADDR_SCL            0b1001011

// This application is only using single ended signal measurement, which half the scale from ground to +VIN
#define ADS1115_MIN_MEASUREMENT_VAL     0x0000
#define ADS1115_MAX_MEASUREMENT_VAL     0x7FFF

// Register addresses
#define ADS1115_CONVERSION_REG_ADDR     0b00
#define ADS1115_CONFIG_REG_ADDR         0b01
#define ADS1115_LOW_TRESH_REG_ADDR      0b10
#define ADS1115_HIGH_TRESH_REG_ADDR     0b11

// Register defaults
#define ADS1115_CONVERSION_REG_DEF      0x0000
#define ADS1115_CONFIG_REG_DEF          0x8583
#define ADS1115_LOW_TRESH_REG_DEF       0x8000
#define ADS1115_HIGH_TRESH_REG_DEF      0x7FFF

// Config register flag/data positions
#define ADS1115_OS_FLAG_POS             15
#define ADS1115_MUX2_DAT_POS            14
#define ADS1115_MUX1_DAT_POS            13
#define ADS1115_MUX0_DAT_POS            12
#define ADS1115_PGA2_DAT_POS            11
#define ADS1115_PGA1_DAT_POS            10
#define ADS1115_PGA0_DAT_POS            9
#define ADS1115_MODE_FLAG_POS           8
#define ADS1115_DR2_DAT_POS             7
#define ADS1115_DR1_DAT_POS             6
#define ADS1115_DR0_DAT_POS             5
#define ADS1115_COMP_MODE_FLAG_POS      4
#define ADS1115_COMP_POL_FLAG_POS       3
#define ADS1115_COMP_LAT_FLAG_POS       2
#define ADS1115_COMP_QUE1_DAT_POS       1
#define ADS1115_COMP_QUE0_DAT_POS       0

// Operational status macros
#define ADS1115_OS_START_SINGLE         0b1
#define ADS1115_OS_ONGOING_CONV         0b0
#define ADS1115_OS_NO_CONV              0b1

// Input multiplexer macros
#define ADS1115_MUX_AIN0_AIN1           0b000
#define ADS1115_MUX_AIN0_AIN3           0b001
#define ADS1115_MUX_AIN1_AIN3           0b010
#define ADS1115_MUX_AIN2_AIN3           0b011
#define ADS1115_MUX_AIN0_GND            0b100
#define ADS1115_MUX_AIN1_GND            0b101
#define ADS1115_MUX_AIN2_GND            0b110
#define ADS1115_MUX_AIN3_GND            0b111

// Programmable gain amplifier macros
#define ADS1115_PGA_6_144               0b000
#define ADS1115_PGA_4_096               0b001
#define ADS1115_PGA_2_048               0b010
#define ADS1115_PGA_1_024               0b011
#define ADS1115_PGA_0_512               0b100
#define ADS1115_PGA_0_256               0b101

// Device operation mode macros
#define ADS1115_MODE_CONTINOUS          0
#define ADS1115_MODE_SINGLE             1

// Data rate macros
#define ADS1115_DR_8_SPS                0b000
#define ADS1115_DR_16_SPS               0b001
#define ADS1115_DR_32_SPS               0b010
#define ADS1115_DR_64_SPS               0b011
#define ADS1115_DR_128_SPS              0b100
#define ADS1115_DR_250_SPS              0b101
#define ADS1115_DR_475_SPS              0b110
#define ADS1115_DR_860_SPS              0b111

// Comparator mode macros
#define ADS1115_COMP_MODE_TRADITIONAL   0
#define ADS1115_COMP_MODE_WINDOW        1

// Comparator polarity macros
#define ADS1115_COMP_POL_LOW            0
#define ADS1115_COMP_POL_HIGH           1

// Latching comparator macros
#define ADS1115_COMP_LAT_NO_LATCH       0
#define ADS1115_COMP_LAT_LATCH          1

// Comparator queue and disable macros
#define ADS1115_COMP_QUE_ONE_CONV       0b00
#define ADS1115_COMP_QUE_TWO_CONV       0b01
#define ADS1115_COMP_QUE_FOUR_CONV      0b10
#define ADS1115_COMP_QUE_DISABLE        0b11

// Value multipliers
#define ADS1115_PGA_6_144_MULT          0.1875
#define ADS1115_PGA_4_096_MULT          0.125
#define ADS1115_PGA_2_048_MULT          0.625
#define ADS1115_PGA_1_024_MULT          0.3125
#define ADS1115_PGA_0_512_MULT          0.15625
#define ADS1115_PGA_0_256_MULT          0.78125
