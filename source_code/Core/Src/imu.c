#include "imu.h"

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if ((hspi == &hspi1) && (DMA_transfer_flag))
  {
    IMU_deactivate();
    DMA_transfer_flag = 0;
    AccData[0] = (((int16_t)imu_spi_miso_buffer[0]) << 8) | imu_spi_miso_buffer[1];
    AccData[1] = (((int16_t)imu_spi_miso_buffer[2]) << 8) | imu_spi_miso_buffer[3];
    AccData[2] = (((int16_t)imu_spi_miso_buffer[4]) << 8) | imu_spi_miso_buffer[5];
    GyroData[0] = (((int16_t)imu_spi_miso_buffer[6]) << 8) | imu_spi_miso_buffer[7];
    GyroData[1] = (((int16_t)imu_spi_miso_buffer[8]) << 8) | imu_spi_miso_buffer[9];
    GyroData[2] = (((int16_t)imu_spi_miso_buffer[10]) << 8) | imu_spi_miso_buffer[11];
  }
}

void IMU_get_datas_DMA()
{
  uint8_t addr = MPUREG_FIFO_R_W | READ;

  IMU_activate();
  DMA_transfer_flag = 1;
  HAL_SPI_Transmit_DMA(&hspi1, &addr, 1);
  HAL_SPI_Receive_DMA(&hspi1, imu_spi_miso_buffer, 12);
}

void IMU_get_datas()
{
  MPU_read_register(MPUREG_FIFO_R_W, imu_spi_miso_buffer, 12);

  AccData[0] = (((int16_t)imu_spi_miso_buffer[0]) << 8) | ((int16_t)imu_spi_miso_buffer[1]);
  AccData[1] = (((int16_t)imu_spi_miso_buffer[2]) << 8) | ((int16_t)imu_spi_miso_buffer[3]);
  AccData[2] = (((int16_t)imu_spi_miso_buffer[4]) << 8) | ((int16_t)imu_spi_miso_buffer[5]);
  GyroData[0] = (((int16_t)imu_spi_miso_buffer[6]) << 8) | ((int16_t)imu_spi_miso_buffer[7]);
  GyroData[1] = (((int16_t)imu_spi_miso_buffer[8]) << 8) | ((int16_t)imu_spi_miso_buffer[9]);
  GyroData[2] = (((int16_t)imu_spi_miso_buffer[10]) << 8) | ((int16_t)imu_spi_miso_buffer[11]);

  int16_t magx = (((int16_t)imu_spi_miso_buffer[15]) << 8) | imu_spi_miso_buffer[14];
  int16_t magy = (((int16_t)imu_spi_miso_buffer[17]) << 8) | imu_spi_miso_buffer[16];
  int16_t magz = (((int16_t)imu_spi_miso_buffer[19]) << 8) | imu_spi_miso_buffer[18];

  MagData[0] = (int16_t)((float)magx * ((float)(mag_adjust[0] - 128) / 256.0f + 1.0f));
  MagData[1] = (int16_t)((float)magy * ((float)(mag_adjust[1] - 128) / 256.0f + 1.0f));
  MagData[2] = (int16_t)((float)magz * ((float)(mag_adjust[2] - 128) / 256.0f + 1.0f));
}

void IMU_init()
{
  uint8_t old_data = 0;

  HAL_Delay(100);

  if (MPU_check() == -1)
  {
    // boucle infinie ici
    while (1)
    {
    };
  }

  do
  {
    MPU_read_register(MPUREG_GYRO_CONFIG, &old_data, 1);
    MPU_write_register(MPUREG_GYRO_CONFIG, old_data | 0b00000001);
    MPU_read_register(MPUREG_GYRO_CONFIG, &old_data, 1);
    if (debug)
    {
      print_to_console("\nGYRO_CONFIG configuration", 26);
      print_imu_register(old_data);
    }
  } while (old_data != 0b00000001);
  if (debug)
    print_to_console("\nGYRO_CONFIG configured", 23);

  do
  {
    MPU_read_register(MPUREG_ACCEL_CONFIG_2, &old_data, 1);
    MPU_write_register(MPUREG_ACCEL_CONFIG_2, old_data | 0b00001000);
    MPU_read_register(MPUREG_ACCEL_CONFIG_2, &old_data, 1);
    if (debug)
    {
      print_to_console("\nACCEL_CONFIG_2 configuration", 29);
      print_imu_register(old_data);
    }
    old_data = old_data & (uint8_t)0b00001111;
  } while (old_data != 0b00001000);
  if (debug)
    print_to_console("\nACCEL_CONFIG_2 configured", 26);

  do
  {
    MPU_read_register(MPUREG_FIFO_EN, &old_data, 1);
    MPU_write_register(MPUREG_FIFO_EN, old_data | 0b01111000);
    MPU_read_register(MPUREG_FIFO_EN, &old_data, 1);
    if (debug)
    {
      print_to_console("\nFIFO_EN configuration", 22);
      print_imu_register(old_data);
    }
  } while (old_data != 0b01111000);
  if (debug)
    print_to_console("\nFIFO_EN configured", 19);

  do
  {
    MPU_read_register(MPUREG_INT_PIN_CFG, &old_data, 1);
    MPU_write_register(MPUREG_INT_PIN_CFG, old_data | 0b00110000);
    MPU_read_register(MPUREG_INT_PIN_CFG, &old_data, 1);
    if (debug)
    {
      print_to_console("\nINT_PIN_CFG configuration", 26);
      print_imu_register(old_data);
    }
    old_data = old_data & (uint8_t)0b11111110;
  } while (old_data != 0b00110000);
  if (debug)
    print_to_console("\nINT_PIN_CFG configured", 23);

  do
  {
    MPU_read_register(MPUREG_INT_ENABLE, &old_data, 1);
    MPU_write_register(MPUREG_INT_ENABLE, old_data | 0b00000001);
    MPU_read_register(MPUREG_INT_ENABLE, &old_data, 1);
    if (debug)
    {
      print_to_console("\nINT_ENABLE configuration", 25);
      print_imu_register(old_data);
    }
    old_data = old_data & (uint8_t)0b01011001;
  } while (old_data != 0b000000001);
  if (debug)
    print_to_console("\nINT_ENABLE configured", 22);

  do
  {
    MPU_read_register(MPUREG_PWR_MGMT_1, &old_data, 1);
    MPU_write_register(MPUREG_PWR_MGMT_1, old_data | 0b00000001);
    MPU_read_register(MPUREG_PWR_MGMT_1, &old_data, 1);
    if (debug)
    {
      print_to_console("\nPWR_MGMT_1 configuration", 25);
      print_imu_register(old_data);
    }
  } while (old_data != 0b00000001);
  if (debug)
    print_to_console("\nPWR_MGMT_1 configured", 22);

  do
  {
    MPU_read_register(MPUREG_USER_CTRL, &old_data, 1);
    MPU_write_register(MPUREG_USER_CTRL, old_data | 0b01000000);
    MPU_read_register(MPUREG_USER_CTRL, &old_data, 1);
    if (debug)
    {
      print_to_console("\nUSER_CTRL configuration", 24);
      print_imu_register(old_data);
    }
    old_data = old_data & (uint8_t)0b01110111;
  } while (old_data != 0b010000000);
  if (debug)
    print_to_console("\nUSER_CTRL configured", 21);

  if (debug)
    print_to_console("\nSPI MPU               : initialized", 36);
}

int MPU_check()
{
  uint8_t RxData = 0xFF;
  MPU_read_register(MPUREG_WHOAMI, &RxData, 1);
  if (RxData == 0x75) // retour attendu 0x75
  {
    // indiquer physiquement une reussite (buzzer)
    if (debug)
      print_to_console("\nSPI MPU               : detected", 33);
    return 0;
  }
  else
  {
    // indiquer physiquement une erreur (buzzer)
    if (debug)
    {
      print_to_console("\nSPI MPU               : error, no response", 43);
      print_imu_register(RxData);
    }
    return -1;
  }
}

int MAG_check()
{
  uint8_t RxData = 0xFF;
  MAG_read_register(MAGREG_WHOAMI, &RxData, 1);
  if (RxData == 0x00) // retour attendu ???
  {
    // ici un indicateur physique ex buzzer
    if (debug)
      print_to_console("\nSPI MAG               : detected", 33);
    return 0;
  }
  else
  {
    // indiquer physiquement une erreur (buzzer)
    if (debug)
    {
      print_to_console("\nSPI MAG               : error, no response", 43);
      print_imu_register(RxData);
    }
    return -1;
  }
}

void self_test_gyro()
{
  char tab[4] = {0};
  uint8_t xg_st_data = 0;
  uint8_t yg_st_data = 0;
  uint8_t zg_st_data = 0;

  MPU_read_register(MPUREG_SELF_TEST_X_GYRO, &xg_st_data, 1);
  MPU_read_register(MPUREG_SELF_TEST_Y_GYRO, &yg_st_data, 1);
  MPU_read_register(MPUREG_SELF_TEST_Z_GYRO, &zg_st_data, 1);

  // ici traitement de la reponse plus indicateur physique buzzer

  if (debug)
  {
    print_to_console("\nIMU gyro self test X  : ", 25);
    sprintf(tab, "%d", xg_st_data);
    print_to_console(tab, 3);

    print_to_console("\nIMU gyro self test Y  : ", 25);
    sprintf(tab, "%d", yg_st_data);
    print_to_console(tab, 3);

    print_to_console("\nIMU gyro self test Z  : ", 25);
    sprintf(tab, "%d", zg_st_data);
    print_to_console(tab, 3);
  }
}
void self_test_accel()
{
  char tab[4] = {0};
  uint8_t xa_st_data = 0;
  uint8_t ya_st_data = 0;
  uint8_t za_st_data = 0;

  MPU_read_register(MPUREG_SELF_TEST_X_ACCEL, &xa_st_data, 1);
  MPU_read_register(MPUREG_SELF_TEST_Y_ACCEL, &ya_st_data, 1);
  MPU_read_register(MPUREG_SELF_TEST_Z_ACCEL, &za_st_data, 1);

  // ici traitement de la réponse (integrité donnée) plus indicateur physique buzzer

  if (debug)
  {
    print_to_console("\nIMU accel self test X : ", 25);
    sprintf(tab, "%d", xa_st_data);
    print_to_console(tab, 3);

    print_to_console("\nIMU accel self test Y : ", 25);
    sprintf(tab, "%d", ya_st_data);
    print_to_console(tab, 3);

    print_to_console("\nIMU accel self test Z : ", 25);
    sprintf(tab, "%d", za_st_data);
    print_to_console(tab, 3);
  }
}

void MPU_read_register(uint8_t addr, uint8_t *data, uint8_t nbbyte)
{
  addr |= READ;
  IMU_activate();
  HAL_SPI_Transmit(&hspi1, &addr, 1, 1000);
  HAL_SPI_Receive(&hspi1, data, nbbyte, 1000);
  IMU_deactivate();
}

void MPU_write_register(uint8_t addr, uint8_t data)
{
  addr |= WRITE;
  IMU_activate();
  HAL_SPI_Transmit(&hspi1, &addr, 1, 1000);
  HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
  IMU_deactivate();
}

void MAG_write_register(uint8_t addr, uint8_t data)
{
  // set slave 0 to the AK8963 and set for write
  MPU_write_register(MPUREG_I2C_SLV0_ADDR, MAGREG_I2C_ADDR);

  // set the register to the desired AK8963 sub address
  MPU_write_register(MPUREG_I2C_SLV0_REG, addr);

  // store the data for write
  MPU_write_register(MPUREG_I2C_SLV0_DO, data);

  // enable I2C and send 1 byte
  MPU_write_register(MPUREG_I2C_SLV0_CTRL, I2C_SLV0_EN | 0x01);
}

void MAG_read_register(uint8_t addr, uint8_t *data, uint8_t nbbyte)
{
  // set slave 0 to the AK8963 and set for read
  MPU_write_register(MPUREG_I2C_SLV0_ADDR, MAGREG_I2C_ADDR | I2C_READ_FLAG);

  // set the register to the desired AK8963 sub address
  MPU_write_register(MPUREG_I2C_SLV0_REG, addr);

  // enable I2C and request the bytes
  MPU_write_register(MPUREG_I2C_SLV0_CTRL, I2C_SLV0_EN | nbbyte);

  // takes some time for these registers to fill
  HAL_Delay(1);

  // read the bytes off the MPU9250 EXT_SENS_DATA registers
  MPU_read_register(MPUREG_EXT_SENS_DATA_00, data, nbbyte);
}

void IMU_activate()
{
  // HAL_GPIO_WritePin(gyro_CS_GPIO_Port, gyro_CS_Pin, GPIO_PIN_RESET);
}

void IMU_deactivate()
{
  // HAL_GPIO_WritePin(gyro_CS_GPIO_Port, gyro_CS_Pin, GPIO_PIN_SET);
}
