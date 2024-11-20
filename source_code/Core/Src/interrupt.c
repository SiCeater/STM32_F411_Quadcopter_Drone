#include "interrupt.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  // if (htim == &htim7) // 400Hz
  // {
  //   control();
  // }
  // if (htim == &htim6) // 1Hz
  // {
  //   // self_test_gyro();
  //   // self_test_accel();
  //   IMU_get_datas();
  //   // IMU_get_datas_DMA();
  //   print_imu_datas();
  // }
}
