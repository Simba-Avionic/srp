/**
 * @file i2cdriver.cpp
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "i2cdriver.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/types.h>
#include <linux/i2c.h>
inline __s32 I2C::i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data) {
    struct i2c_smbus_ioctl_data args;
    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;
    return ioctl(file, I2C_SMBUS, &args);
}
inline __s32 I2C::i2c_smbus_read_byte_data(int file, __u8 command) {
    union i2c_smbus_data data;
    if (i2c_smbus_access(file, I2C_SMBUS_READ, command, I2C_SMBUS_BYTE_DATA, &data)) {
        return -1;
    }
    return 0xFF & data.byte;
}
 inline __s32 I2C::i2c_smbus_write_byte_data(int file, __u8 command, __u8 value) {
    union i2c_smbus_data data;
    data.byte = value;

    if (i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_BYTE_DATA, &data)) {
        return -1;
    }
    return 0;
}


int I2C::init(const char * path) {
    this->file = open(path, O_RDWR);
    if (this->file < 0) {
        return -1;
    }
    return 0;
}
uint8_t I2C::read(uint8_t address, uint8_t reg) {
    if (address != this->slaveAddress) {
        if (ioctl(file,I2C_SLAVE,address)<0) {
        return -1;
    }
    this->slaveAddress = address;
    }
    uint8_t data;
    data = i2c_smbus_read_byte_data(this->file, reg);
    if (data < 0) {
        return -2;
    }
    return data;
}
int I2C::write(uint8_t address, uint8_t reg, uint8_t data) {
    if (address != this->slaveAddress){
        if(ioctl(file,I2C_SLAVE,address)<0){
        return -1;
    }
    this->slaveAddress = address;
    }
    if (i2c_smbus_write_byte_data(this->file, reg, data) < 0) {
        return -2;
    }
    return 0;
}
