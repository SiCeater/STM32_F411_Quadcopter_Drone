
#ifndef __IMU_H__
#define __IMU_H__

#include "global.h"
#include "spi.h"
#include "gpio.h"
#include "main.h"
#include "debug.h"

#define READ    0x80
#define WRITE   0x00

// MPU9250 registers adresses

#define MPUREG_SELF_TEST_X_GYRO     0x00
#define MPUREG_SELF_TEST_Y_GYRO     0x01
#define MPUREG_SELF_TEST_Z_GYRO     0x02

#define MPUREG_SELF_TEST_X_ACCEL    0x0D
#define MPUREG_SELF_TEST_Y_ACCEL    0x0E
#define MPUREG_SELF_TEST_Z_ACCEL    0x0F

#define MPUREG_XG_OFFSET_H          0x13
#define MPUREG_XG_OFFSET_L          0x14

#define MPUREG_YG_OFFSET_H          0x15
#define MPUREG_YG_OFFSET_L          0x16

#define MPUREG_ZG_OFFSET_H          0x17
#define MPUREG_ZG_OFFSET_L          0x18

#define MPUREG_SMPLRT_DIV           0x19 

#define MPUREG_CONFIG               0x1A
#define MPUREG_GYRO_CONFIG          0x1B
#define MPUREG_ACCEL_CONFIG_1       0x1C
#define MPUREG_ACCEL_CONFIG_2       0x1D
#define MPUREG_LP_ACCEL_ODR         0x1E
#define MPUREG_WOM_THR              0x1F

#define MPUREG_FIFO_EN              0x23

#define MPUREG_I2C_MST_CTRL         0x24
#define MPUREG_I2C_SLV0_ADDR        0x25
#define MPUREG_I2C_SLV0_REG         0x26
#define MPUREG_I2C_SLV0_CTRL        0x27
#define MPUREG_I2C_SLV1_ADDR        0x28
#define MPUREG_I2C_SLV1_REG         0x29
#define MPUREG_I2C_SLV1_CTRL        0x2A
#define MPUREG_I2C_SLV2_ADDR        0x2B
#define MPUREG_I2C_SLV2_REG         0x2C
#define MPUREG_I2C_SLV2_CTRL        0x2D
#define MPUREG_I2C_SLV3_ADDR        0x2E
#define MPUREG_I2C_SLV3_REG         0x2F
#define MPUREG_I2C_SLV3_CTRL        0x30
#define MPUREG_I2C_SLV4_ADDR        0x31
#define MPUREG_I2C_SLV4_REG         0x32
#define MPUREG_I2C_SLV4_DO          0x33
#define MPUREG_I2C_SLV4_CTRL        0x34
#define MPUREG_I2C_SLV4_DI          0x35
#define MPUREG_I2C_MST_STATUS       0x36

#define MPUREG_INT_PIN_CFG          0x37
#define MPUREG_INT_ENABLE           0x38
#define MPUREG_INT_STATUS           0x3A

#define MPUREG_ACCEL_XOUT_H         0x3B
#define MPUREG_ACCEL_XOUT_L         0x3C

#define MPUREG_ACCEL_YOUT_H         0x3D
#define MPUREG_ACCEL_YOUT_L         0x3E

#define MPUREG_ACCEL_ZOUT_H         0x3F
#define MPUREG_ACCEL_ZOUT_L         0x40

#define MPUREG_TEMP_OUT_H           0x41
#define MPUREG_TEMP_OUT_L           0x42

#define MPUREG_GYRO_XOUT_H          0x43
#define MPUREG_GYRO_XOUT_L          0x44

#define MPUREG_GYRO_YOUT_H          0x45
#define MPUREG_GYRO_YOUT_L          0x46

#define MPUREG_GYRO_ZOUT_H          0x47
#define MPUREG_GYRO_ZOUT_L          0x48

#define MPUREG_EXT_SENS_DATA_00     0x49

#define MPUREG_I2C_SLV0_DO          0x63
#define MPUREG_I2C_SLV1_DO          0x64
#define MPUREG_I2C_SLV2_DO          0x65
#define MPUREG_I2C_SLV3_DO          0x66
#define MPUREG_I2C_MST_DELAY_CTRL   0x67
#define MPUREG_SIGNAL_PATH_RESET    0x68

#define MPUREG_USER_CTRL            0x6A

#define MPUREG_PWR_MGMT_1           0x6B
#define MPUREG_PWR_MGMT_2           0x6C

#define MPUREG_BANK_SEL             0x6D
#define MPUREG_MEM_START_ADDR       0x6E
#define MPUREG_MEM_R_W              0x6F

#define MPUREG_DMP_CFG_1            0x70
#define MPUREG_DMP_CFG_2            0x71

#define MPUREG_FIFO_COUNTH          0x72
#define MPUREG_FIFO_COUNTL          0x73
#define MPUREG_FIFO_R_W             0x74

#define MPUREG_WHOAMI               0x75

#define MPUREG_XA_OFFSET_H          0x77
#define MPUREG_XA_OFFSET_L          0x78

#define MPUREG_YA_OFFSET_H          0x7A
#define MPUREG_YA_OFFSET_L          0x7B

#define MPUREG_ZA_OFFSET_H          0x7D
#define MPUREG_ZA_OFFSET_L          0x7E

// AK8963 registers addresses

#define MAGREG_I2C_ADDR             0x0C
#define MAGREG_HXL                  0x03
#define MAGREG_CNTL1                0x0A
#define MAGREG_PWR_DOWN             0x00
#define MAGREG_CNT_MEAS1            0x12
#define MAGREG_CNT_MEAS2            0x16
#define MAGREG_FUSE_ROM             0x0F
#define MAGREG_CNTL2                0x0B
#define MAGREG_RESET                0x01
#define MAGREG_ASA                  0x10
#define MAGREG_WHOAMI             0x00

// flags and set values

#define I2C_SLV0_EN 0x80
#define I2C_READ_FLAG 0x80
#define CLOCK_SEL_PLL 0x01
#define I2C_MST_EN 0x20
#define I2C_MST_CLK 0x0D
#define MAG_PWR_DOWN 0x00
#define MPU_PWR_RESET 0x80
#define MAG_RESET 0x01
#define SEN_ENABLE 0x00
#define ACCEL_FS_SEL_16G 0x18
#define GYRO_FS_SEL_250DPS 0x00
#define GYRO_FS_SEL_500DPS 0x01
#define GYRO_FS_SEL_1000DPS 0x10
#define GYRO_FS_SEL_2000DPS 0x11
#define DLPF_184 0x01
#define MAG_FUSE_ROM 0x0F

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
void IMU_get_datas_DMA();
void IMU_get_datas();
void IMU_init();
int MPU_check();
int MAG_check();
void self_test_gyro();
void self_test_accel();
void MPU_read_register(uint8_t addr , uint8_t * data, uint8_t nbbyte);
void MPU_write_register(uint8_t addr , uint8_t data);
void MAG_write_register(uint8_t addr, uint8_t data);
void MAG_read_register(uint8_t addr, uint8_t* data, uint8_t nbbyte);
void IMU_activate();
void IMU_deactivate();

#endif