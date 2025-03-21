#ifndef __IMU_H__
#define __IMU_H__

#include "global.h"
#include "spi.h"
#include "debug.h"
#include "string.h"
#include "stdio.h"

/**
 * @brief Definition for connected to SPI1 (APB2 PCLK = 100MHz)
 */
#define BNO085_SPI_CHANNEL		SPI1

#define BNO085_SPI_SCLK_PIN		LL_GPIO_PIN_5
#define BNO085_SPI_SCLK_PORT	GPIOA
#define BNO085_SPI_SCLK_CLK		LL_AHB1_GRP1_PERIPH_GPIOA

#define BNO085_SPI_MISO_PIN		LL_GPIO_PIN_6
#define BNO085_SPI_MISO_PORT	GPIOA
#define BNO085_SPI_MISO_CLK		LL_AHB1_GRP1_PERIPH_GPIOA

#define BNO085_SPI_MOSI_PIN		LL_GPIO_PIN_7
#define BNO085_SPI_MOSI_PORT	GPIOA
#define BNO085_SPI_MOSI_CLK		LL_AHB1_GRP1_PERIPH_GPIOA

#define BNO085_SPI_CS_PIN		LL_GPIO_PIN_4
#define BNO085_SPI_CS_PORT		GPIOA
#define BNO085_SPI_CS_CLK		LL_AHB1_GRP1_PERIPH_GPIOA

#define BNO085_RST_PIN			LL_GPIO_PIN_0
#define BNO085_RST_PORT			GPIOB
#define BNO085_RST_CLK			LL_AHB1_GRP1_PERIPH_GPIOB

#define BNO085_INT_PIN			LL_GPIO_PIN_1
#define BNO085_INT_PORT			GPIOB
#define BNO085_INT_CLK			LL_AHB1_GRP1_PERIPH_GPIOB

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define CHIP_SELECT(BNO085)		LL_GPIO_ResetOutputPin(BNO085_SPI_CS_PORT, BNO085_SPI_CS_PIN)
#define CHIP_DESELECT(BNO085)	LL_GPIO_SetOutputPin(BNO085_SPI_CS_PORT, BNO085_SPI_CS_PIN)

#define RESET_HIGH()			LL_GPIO_SetOutputPin(BNO085_RST_PORT, BNO085_RST_PIN)
#define RESET_LOW()				LL_GPIO_ResetOutputPin(BNO085_RST_PORT, BNO085_RST_PIN)
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define SHTP_REPORT_PRODUCT_ID_RESPONSE 0xF8
#define SHTP_REPORT_PRODUCT_ID_REQUEST 0xF9
#define MAX_PACKET_SIZE 1000
#define SHTP_REPORT_GET_FEATURE 0xFE
#define SHTP_CHANNEL_CONTROL 2  // Canal utilisé pour les commandes
#define CHANNEL_EXECUTABLE 1
// IDs des deux capteurs virtuels
#define SENSORID_ROTATION_VECTOR        0x05  // Rotation Vector classique
#define SENSORID_GYRO_INTEGRATED       0x2A  // Gyro-Integrated Rotation Vector

// Périodes souhaitées (en microsecondes).
// 1000 us => 1 kHz, 2500 us => 400 Hz, etc.
#define INTERVAL_GYRO_INTEGRATED       1000   // 1 kHz
#define INTERVAL_ROTATION_VECTOR       2500   // 400 Hz

void IMU_SPIBus_init(void);
uint8_t BNO085_SPI_bus_read_write_8(uint8_t data);
uint8_t BNO085_wait_response(void);
uint16_t BNO085_read_response(void);
void BNO085_SetFeature(uint8_t sensorID, uint32_t reportInterval);
void BNO085_Analyze_Trame();
uint8_t BNO085_GetFeature(uint8_t sensorID, uint32_t *interval);
void BNO085_CheckAndEnableSensors(void);
void IMU_Init();

#endif