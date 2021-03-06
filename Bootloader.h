/*******************************************************************************
* Copyright (C) 2021 Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************
*/

#include "mbed.h"
#include "USBSerial.h"

#define AES_NONCE_SIZE 11
#define AES_AUTH_SIZE 16
#define MAX_PAGE_SIZE 8192
#define CHECKBYTES_SIZE 16

typedef enum {
    SUCCESS = 0x0,
    ERR_UNAVAIL_CMD = 0x01,
    ERR_UNAVAIL_FUNC = 0x02,
    ERR_DATA_FORMAT = 0x03,
    ERR_INPUT_VALUE = 0x04,
    ERR_BTLDR_TRY_AGAIN_OR_INVALID_MODE = 0x05,
    ERR_BTLDR_GENERAL = 0x80,
    ERR_BTLDR_CHECKSUM = 0x81,
    ERR_BTLDR_AUTH = 0x82,
    ERR_BTLDR_INVALID_APP = 0x83,
    ERR_TRY_AGAIN = 0xFE,
    ERR_UNKNOWN = 0xFF
} sh_err_t;

typedef enum {
    APPLICATION_MODE,
    RESET = 0x02,
    BOOTLOADER_MODE = 0x08
} sh_opmode_t;

typedef struct {
    int major;
    int minor;
    int rev;
} sh_version_t;

typedef struct {
    uint8_t major;
    uint8_t minor;
    uint8_t rev;
} bl_version_t;

// The bootloader class is responsible for driving the I2C bus, MFIO, and RSTN properly to interface with the Sensor Hub's bootloader.  The commands implemented can be found in the User Guide and Quick Start Guide for your MAX32664 variant.  This is only an implementation of the bootloader-related commands.  

class Bootloader {
public:
    Bootloader(I2C* i2c, PinName mfio, PinName rstn);
    sh_err_t enter_bootloader();
    sh_err_t exit_bootloader();
    sh_err_t get_bootloader_version(bl_version_t* out);
    sh_err_t get_sh_version(sh_version_t* out);
    sh_err_t get_operating_mode(sh_opmode_t* out);
    void reset();
    sh_err_t get_page_size(int* out);
    sh_err_t set_num_pages(int num_pages);
    sh_err_t set_iv(char* iv_bytes);
    sh_err_t set_auth(char* auth_bytes);
    sh_err_t erase();
    sh_err_t flash(char* page);
protected:
private:
    I2C* i2c;
    DigitalOut mfio = DigitalOut(P5_4); // Specifying a default value since there is no empty constructor for DigitalOut
    DigitalOut rstn = DigitalOut(P5_6);
    int sh_addr = 0xAA; //8-bit I2C slave address for Sensor Hub
    USBSerial* debug;
};