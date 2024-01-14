#ifndef I2CDRIVER_H
#define I2CDRIVER_H
#include <linux/types.h>
#include <stdint.h>
class I2C{
    public:
    int init(const char * path);
    uint8_t read(uint8_t address, uint8_t reg);
    int write(uint8_t address, uint8_t reg, uint8_t data);
    private:
    int file = -1;
    uint8_t slaveAddress;
    static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data);
    static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command);
    static inline __s32 I2C::i2c_smbus_write_byte_data(int file, __u8 command, __u8 value);
};
#endif