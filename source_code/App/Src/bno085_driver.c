#include "bno085_driver.h"
#include <math.h>


//Global Variables
uint8_t shtpHeader[4]; //Each packet has a header of 4 bytes
uint8_t shtpData[MAX_PACKET_SIZE];
uint8_t sequenceNumber[6] = {0, 0, 0, 0, 0, 0}; //There are 6 com channels. Each channel has its own seqnum
uint8_t commandSequenceNumber = 0;			//Commands have a seqNum as well. These are inside command packet, the header uses its own seqNum per channel
uint32_t metaData[MAX_METADATA_SIZE];			//There is more than 10 words in a metadata record but we'll stop at Q point 3

//These are the raw sensor values pulled from the user requested Input Report
uint16_t rawAccelX, rawAccelY, rawAccelZ, accelAccuracy;
uint16_t rawLinAccelX, rawLinAccelY, rawLinAccelZ, accelLinAccuracy;
uint16_t rawGyroX, rawGyroY, rawGyroZ, gyroAccuracy;
uint16_t rawMagX, rawMagY, rawMagZ, magAccuracy;
uint16_t rawQuatI, rawQuatJ, rawQuatK, rawQuatReal, rawQuatRadianAccuracy, quatAccuracy;
uint16_t stepCount;
uint32_t timeStamp;
uint8_t stabilityClassifier;
uint8_t activityClassifier;
uint8_t *_activityConfidences; //Array that store the confidences of the 9 possible activities
uint8_t calibrationStatus;	 //Byte R0 of ME Calibration Response

//These Q values are defined in the datasheet but can also be obtained by querying the meta data records
//See the read metadata example for more info
int16_t rotationVector_Q1 = 14;
int16_t accelerometer_Q1 = 8;
int16_t linear_accelerometer_Q1 = 8;
int16_t gyro_Q1 = 9;
int16_t magnetometer_Q1 = 4;


void BNO085_GPIO_SPI_Initialization(void)
{
	LL_SPI_InitTypeDef SPI_InitStruct = {0};
	
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	/**SPI2 GPIO Configuration
	PB13   ------> SPI2_SCK
	PB14   ------> SPI2_MISO
	PB15   ------> SPI2_MOSI
	*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_13|LL_GPIO_PIN_14|LL_GPIO_PIN_15;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
	SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
	SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
	SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
	SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
	SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
	SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV16;
	SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
	SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
	SPI_InitStruct.CRCPoly = 10;
	LL_SPI_Init(BNO085_SPI_CHANNEL, &SPI_InitStruct);
	LL_SPI_SetStandard(BNO085_SPI_CHANNEL, LL_SPI_PROTOCOL_MOTOROLA);
	
	/**BNO085 GPIO Control Configuration
	 * PB12 ------> BNO085_CS (output)
	 * PA8  ------> BNO085_PS0/WAKE (output)
	 * PC9  ------> BNO085_RST (output)
	 * PC8  ------> BNO085_INT (input)
	 */
	/**/
	LL_GPIO_ResetOutputPin(BNO085_RST_PORT, BNO085_RST_PIN);
	LL_GPIO_ResetOutputPin(BNO085_SPI_CS_PORT, BNO085_SPI_CS_PIN);
	LL_GPIO_ResetOutputPin(BNO085_PS0_WAKE_PORT, BNO085_PS0_WAKE_PIN);
	
	/**/
	GPIO_InitStruct.Pin = BNO085_SPI_CS_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(BNO085_SPI_CS_PORT, &GPIO_InitStruct);
	
	/**/
	GPIO_InitStruct.Pin = BNO085_RST_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(BNO085_RST_PORT, &GPIO_InitStruct);
	
	/**/
	GPIO_InitStruct.Pin = BNO085_PS0_WAKE_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(BNO085_PS0_WAKE_PORT, &GPIO_InitStruct);
	
	/**/
	GPIO_InitStruct.Pin = BNO085_INT_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	LL_GPIO_Init(BNO085_INT_PORT, &GPIO_InitStruct);

	LL_SPI_Enable(BNO085_SPI_CHANNEL);

	CHIP_DESELECT(BNO085);
	WAKE_HIGH();
	RESET_HIGH();
}

int BNO085_Initialization(void)
{
	BNO085_GPIO_SPI_Initialization();
	
	//printf("Checking BNO085...");
	
	CHIP_DESELECT(BNO085);
	
	//Configure the BNO085 for SPI communication
	WAKE_HIGH();	//Before boot up the PS0/WAK pin must be high to enter SPI mode
	RESET_LOW();	//Reset BNO085
	LL_mDelay(200);	//Min length not specified in datasheet?
	RESET_HIGH();	//Bring out of reset
	
	BNO085_waitForSPI(); //Wait until INT pin goes low.
	
	//At system startup, the hub must send its full advertisement message (see 5.2 and 5.3) to the
	//host. It must not send any other data until this step is complete.
	//When BNO085 first boots it broadcasts big startup packet
	//Read it and dump it
	BNO085_waitForSPI(); //Wait for assertion of INT before reading advert message.
	BNO085_receivePacket();
	
	//The BNO085 will then transmit an unsolicited Initialize Response (see 6.4.5.2)
	//Read it and dump it
	BNO085_waitForSPI();  //Wait for assertion of INT before reading Init response
	BNO085_receivePacket();
	
	//Check communication with device
	shtpData[0] = SHTP_REPORT_PRODUCT_ID_REQUEST; //Request the product ID and reset info
	shtpData[1] = 0;						 //Reserved
	
	//Transmit packet on channel 2, 2 bytes
	BNO085_sendPacket(CHANNEL_CONTROL, 2);
	
	//Now we wait for response
	BNO085_waitForSPI();
	if (BNO085_receivePacket() == 1)
	{
		//printf("header: %d %d %d %d\n", shtpHeader[0], shtpHeader[1], shtpHeader[2], shtpHeader[3]); // debug uart 2
		if (shtpData[0] == SHTP_REPORT_PRODUCT_ID_RESPONSE)
		{
			//printf("BNO085 who_am_i = 0x%02x...ok\n\n", shtpData[0]); debug uart 2
			return (0);
		}// Sensor OK
	}
	
	//printf("BNO085 Not OK: 0x%02x Should be 0x%02x\n", shtpData[0], SHTP_REPORT_PRODUCT_ID_RESPONSE); // debug uart 2
	return (1); //Something went wrong
}

unsigned char SPI2_SendByte(unsigned char data)
{
	while(LL_SPI_IsActiveFlag_TXE(BNO085_SPI_CHANNEL)==RESET);
	LL_SPI_TransmitData8(BNO085_SPI_CHANNEL, data);
	
	while(LL_SPI_IsActiveFlag_RXNE(BNO085_SPI_CHANNEL)==RESET);
	return LL_SPI_ReceiveData8(BNO085_SPI_CHANNEL);
}


//////////////////////////////////////////////////////////////////////////
//init
//////////////////////////////////////////////////////////////////////////

//Updates the latest variables if possible
//Returns false if new readings are not available
int BNO085_dataAvailable(void)
{
	//If we have an interrupt pin connection available, check if data is available.
	//If int pin is NULL, then we'll rely on BNO085_receivePacket() to timeout
	//See issue 13: https://github.com/sparkfun/SparkFun_BNO085_Arduino_Library/issues/13
	if (LL_GPIO_IsInputPinSet(BNO085_INT_PORT, BNO085_INT_PIN) == 1)
		return (0);

	if (BNO085_receivePacket() == 1)
	{
		//Check to see if this packet is a sensor reporting its data to us
		if (shtpHeader[2] == CHANNEL_REPORTS && shtpData[0] == SHTP_REPORT_BASE_TIMESTAMP)
		{
			BNO085_parseInputReport(); //This will update the rawAccelX, etc variables depending on which feature report is found
			return (1);
		}
		else if (shtpHeader[2] == CHANNEL_CONTROL)
		{
			BNO085_parseCommandReport(); //This will update responses to commands, calibrationStatus, etc.
			return (1);
		}
	}
	return (0);
}

//This function pulls the data from the command response report

//Unit responds with packet that contains the following:
//shtpHeader[0:3]: First, a 4 byte header
//shtpData[0]: The Report ID
//shtpData[1]: Sequence number (See 6.5.18.2)
//shtpData[2]: Command
//shtpData[3]: Command Sequence Number
//shtpData[4]: Response Sequence Number
//shtpData[5 + 0]: R0
//shtpData[5 + 1]: R1
//shtpData[5 + 2]: R2
//shtpData[5 + 3]: R3
//shtpData[5 + 4]: R4
//shtpData[5 + 5]: R5
//shtpData[5 + 6]: R6
//shtpData[5 + 7]: R7
//shtpData[5 + 8]: R8
void BNO085_parseCommandReport(void)
{
	if (shtpData[0] == SHTP_REPORT_COMMAND_RESPONSE)
	{
		//The BNO085 responds with this report to command requests. It's up to use to remember which command we issued.
		uint8_t command = shtpData[2]; //This is the Command byte of the response

		if (command == COMMAND_ME_CALIBRATE)
		{
			calibrationStatus = shtpData[5]; //R0 - Status (0 = success, non-zero = fail)
		}
	}
	else
	{
		//This sensor report ID is unhandled.
		//See reference manual to add additional feature reports as needed
	}

	//TODO additional feature reports may be strung together. Parse them all.
}

//This function pulls the data from the input report
//The input reports vary in length so this function stores the various 16-bit values as globals

//Unit responds with packet that contains the following:
//shtpHeader[0:3]: First, a 4 byte header
//shtpData[0:4]: Then a 5 byte timestamp of microsecond clicks since reading was taken
//shtpData[5 + 0]: Then a feature report ID (0x01 for Accel, 0x05 for Rotation Vector)
//shtpData[5 + 1]: Sequence number (See 6.5.18.2)
//shtpData[5 + 2]: Status
//shtpData[3]: Delay
//shtpData[4:5]: i/accel x/gyro x/etc
//shtpData[6:7]: j/accel y/gyro y/etc
//shtpData[8:9]: k/accel z/gyro z/etc
//shtpData[10:11]: real/gyro temp/etc
//shtpData[12:13]: Accuracy estimate
void BNO085_parseInputReport(void)
{
	//Calculate the number of data bytes in this packet
	int16_t dataLength = ((uint16_t)shtpHeader[1] << 8 | shtpHeader[0]);
	dataLength &= ~(1 << 15); //Clear the MSbit. This bit indicates if this package is a continuation of the last.
	//Ignore it for now. TODO catch this as an error and exit

	dataLength -= 4; //Remove the header bytes from the data count

	timeStamp = ((uint32_t)shtpData[4] << (8 * 3)) | (shtpData[3] << (8 * 2)) | (shtpData[2] << (8 * 1)) | (shtpData[1] << (8 * 0));

	uint8_t status = shtpData[7] & 0x03; //Get status bits
	uint16_t data1 = (uint16_t)shtpData[10] << 8 | shtpData[9];
	uint16_t data2 = (uint16_t)shtpData[12] << 8 | shtpData[11];
	uint16_t data3 = (uint16_t)shtpData[14] << 8 | shtpData[13];
	uint16_t data4 = 0;
	uint16_t data5 = 0;

	if (dataLength > 14)
	{
		data4 = (uint16_t)shtpData[16] << 8 | shtpData[15];
	}
	if (dataLength > 16)
	{
		data5 = (uint16_t)shtpData[18] << 8 | shtpData[17];
	}

	//Store these generic values to their proper global variable
	switch(shtpData[5])
	{
		case SENSOR_REPORTID_ACCELEROMETER:
		{
			accelAccuracy = status;
			rawAccelX = data1;
			rawAccelY = data2;
			rawAccelZ = data3;
			break;
		}
		case SENSOR_REPORTID_LINEAR_ACCELERATION:
		{
			accelLinAccuracy = status;
			rawLinAccelX = data1;
			rawLinAccelY = data2;
			rawLinAccelZ = data3;
			break;
		}
		case SENSOR_REPORTID_GYROSCOPE:
		{
			gyroAccuracy = status;
			rawGyroX = data1;
			rawGyroY = data2;
			rawGyroZ = data3;
			break;
		}
		case SENSOR_REPORTID_MAGNETIC_FIELD:
		{
			magAccuracy = status;
			rawMagX = data1;
			rawMagY = data2;
			rawMagZ = data3;
			break;
		}
		case SENSOR_REPORTID_ROTATION_VECTOR:
		case SENSOR_REPORTID_GAME_ROTATION_VECTOR:
		{
			quatAccuracy = status;
			rawQuatI = data1;
			rawQuatJ = data2;
			rawQuatK = data3;
			rawQuatReal = data4;
			rawQuatRadianAccuracy = data5; //Only available on rotation vector, not game rot vector
			break;
		}
		case SENSOR_REPORTID_STEP_COUNTER:
		{
			stepCount = data3; //Bytes 8/9
			break;
		}
		case SENSOR_REPORTID_STABILITY_CLASSIFIER:
		{
			stabilityClassifier = shtpData[5 + 4]; //Byte 4 only
			break;
		}
		case SENSOR_REPORTID_PERSONAL_ACTIVITY_CLASSIFIER:
		{
			activityClassifier = shtpData[5 + 5]; //Most likely state

			//Load activity classification confidences into the array
			for (uint8_t x = 0; x < 9; x++)					   //Hardcoded to max of 9. TODO - bring in array size
				_activityConfidences[x] = shtpData[11 + x]; //5 bytes of timestamp, byte 6 is first confidence byte
			break;
		}
		case SHTP_REPORT_COMMAND_RESPONSE:
		{
			//printf("!");
			//The BNO085 responds with this report to command requests. It's up to use to remember which command we issued.
			uint8_t command = shtpData[5 + 2]; //This is the Command byte of the response

			if (command == COMMAND_ME_CALIBRATE)
			{
				//printf("ME Cal report found!");
				calibrationStatus = shtpData[5 + 5]; //R0 - Status (0 = success, non-zero = fail)
			}
			break;
		}
		default:
		{
			//This sensor report ID is unhandled.
			//See reference manual to add additional feature reports as needed
		}
	}

	//TODO additional feature reports may be strung together. Parse them all.
}

//Return the rotation vector quaternion I
float BNO085_getQuatI()
{
	return BNO085_qToFloat(rawQuatI, rotationVector_Q1);
}

//Return the rotation vector quaternion J
float BNO085_getQuatJ()
{
	return BNO085_qToFloat(rawQuatJ, rotationVector_Q1);
}

//Return the rotation vector quaternion K
float BNO085_getQuatK()
{
	return BNO085_qToFloat(rawQuatK, rotationVector_Q1);
}

//Return the rotation vector quaternion Real
float BNO085_getQuatReal()
{
	return BNO085_qToFloat(rawQuatReal, rotationVector_Q1);
}

//Return the rotation vector accuracy
float BNO085_getQuatRadianAccuracy()
{
	return BNO085_qToFloat(rawQuatRadianAccuracy, rotationVector_Q1);
}

//Return the acceleration component
uint8_t BNO085_getQuatAccuracy()
{
	return (quatAccuracy);
}

//Return the acceleration component
float BNO085_getAccelX()
{
	return BNO085_qToFloat(rawAccelX, accelerometer_Q1);
}

//Return the acceleration component
float BNO085_getAccelY()
{
	return BNO085_qToFloat(rawAccelY, accelerometer_Q1);
}

//Return the acceleration component
float BNO085_getAccelZ()
{
	return BNO085_qToFloat(rawAccelZ, accelerometer_Q1);
}

//Return the acceleration component
uint8_t BNO085_getAccelAccuracy()
{
	return (accelAccuracy);
}

// linear acceleration, i.e. minus gravity

//Return the acceleration component
float BNO085_getLinAccelX()
{
	return BNO085_qToFloat(rawLinAccelX, linear_accelerometer_Q1);
}

//Return the acceleration component
float BNO085_getLinAccelY()
{
	return BNO085_qToFloat(rawLinAccelY, linear_accelerometer_Q1);
}

//Return the acceleration component
float BNO085_getLinAccelZ()
{
	return BNO085_qToFloat(rawLinAccelZ, linear_accelerometer_Q1);
}

//Return the acceleration component
uint8_t BNO085_getLinAccelAccuracy()
{
	return (accelLinAccuracy);
}

//Return the gyro component
float BNO085_getGyroX()
{
	return BNO085_qToFloat(rawGyroX, gyro_Q1);
}

//Return the gyro component
float BNO085_getGyroY()
{
	return BNO085_qToFloat(rawGyroY, gyro_Q1);
}

//Return the gyro component
float BNO085_getGyroZ()
{
	return BNO085_qToFloat(rawGyroZ, gyro_Q1);
}

//Return the gyro component
uint8_t BNO085_getGyroAccuracy()
{
	return (gyroAccuracy);
}

//Return the magnetometer component
float BNO085_getMagX()
{
	return BNO085_qToFloat(rawMagX, magnetometer_Q1);
}

//Return the magnetometer component
float BNO085_getMagY()
{
	return BNO085_qToFloat(rawMagY, magnetometer_Q1);
}

//Return the magnetometer component
float BNO085_getMagZ()
{
	return BNO085_qToFloat(rawMagZ, magnetometer_Q1);
}

//Return the mag component
uint8_t BNO085_getMagAccuracy()
{
	return (magAccuracy);
}

//Return the step count
uint16_t BNO085_getStepCount()
{
	return (stepCount);
}

//Return the stability classifier
uint8_t BNO085_getStabilityClassifier()
{
	return (stabilityClassifier);
}

//Return the activity classifier
uint8_t BNO085_getActivityClassifier()
{
	return (activityClassifier);
}

//Return the time stamp
uint32_t BNO085_getTimeStamp()
{
	return (timeStamp);
}

//Given a record ID, read the Q1 value from the metaData record in the FRS (ya, it's complicated)
//Q1 is used for all sensor data calculations
int16_t BNO085_getQ1(uint16_t recordID)
{
	//Q1 is always the lower 16 bits of word 7
	return BNO085_readFRSword(recordID, 7) & 0xFFFF; //Get word 7, lower 16 bits
}

//Given a record ID, read the Q2 value from the metaData record in the FRS
//Q2 is used in sensor bias
int16_t BNO085_getQ2(uint16_t recordID)
{
	//Q2 is always the upper 16 bits of word 7
	return BNO085_readFRSword(recordID, 7) >> 16; //Get word 7, upper 16 bits
}

//Given a record ID, read the Q3 value from the metaData record in the FRS
//Q3 is used in sensor change sensitivity
int16_t BNO085_getQ3(uint16_t recordID)
{
	//Q3 is always the upper 16 bits of word 8
	return BNO085_readFRSword(recordID, 8) >> 16; //Get word 8, upper 16 bits
}

//Given a record ID, read the resolution value from the metaData record in the FRS for a given sensor
float BNO085_getResolution(uint16_t recordID)
{
	//The resolution Q value are 'the same as those used in the sensor's input report'
	//This should be Q1.
	int16_t Q = BNO085_getQ1(recordID);

	//Resolution is always word 2
	uint32_t value = BNO085_readFRSword(recordID, 2); //Get word 2

	return BNO085_qToFloat(value, Q);
}

//Given a record ID, read the range value from the metaData record in the FRS for a given sensor
float BNO085_getRange(uint16_t recordID)
{
	//The resolution Q value are 'the same as those used in the sensor's input report'
	//This should be Q1.
	int16_t Q = BNO085_getQ1(recordID);

	//Range is always word 1
	uint32_t value = BNO085_readFRSword(recordID, 1); //Get word 1

	return BNO085_qToFloat(value, Q);
}

//Given a record ID and a word number, look up the word data
//Helpful for pulling out a Q value, range, etc.
//Use readFRSdata for pulling out multi-word objects for a sensor (Vendor data for example)
uint32_t BNO085_readFRSword(uint16_t recordID, uint8_t wordNumber)
{
	if (BNO085_readFRSdata(recordID, wordNumber, 1) == 1) //Get word number, just one word in length from FRS
		return (metaData[0]);						  //Return this one word

	return (0); //Error
}

//Ask the sensor for data from the Flash Record System
//See 6.3.6 page 40, FRS Read Request
void BNO085_frsReadRequest(uint16_t recordID, uint16_t readOffset, uint16_t blockSize)
{
	shtpData[0] = SHTP_REPORT_FRS_READ_REQUEST; //FRS Read Request
	shtpData[1] = 0;							//Reserved
	shtpData[2] = (readOffset >> 0) & 0xFF;		//Read Offset LSB
	shtpData[3] = (readOffset >> 8) & 0xFF;		//Read Offset MSB
	shtpData[4] = (recordID >> 0) & 0xFF;		//FRS Type LSB
	shtpData[5] = (recordID >> 8) & 0xFF;		//FRS Type MSB
	shtpData[6] = (blockSize >> 0) & 0xFF;		//Block size LSB
	shtpData[7] = (blockSize >> 8) & 0xFF;		//Block size MSB

	//Transmit packet on channel 2, 8 bytes
	BNO085_sendPacket(CHANNEL_CONTROL, 8);
}

//Given a sensor or record ID, and a given start/stop bytes, read the data from the Flash Record System (FRS) for this sensor
//Returns true if metaData array is loaded successfully
//Returns false if failure
int BNO085_readFRSdata(uint16_t recordID, uint8_t startLocation, uint8_t wordsToRead)
{
	uint8_t spot = 0;

	//First we send a Flash Record System (FRS) request
	BNO085_frsReadRequest(recordID, startLocation, wordsToRead); //From startLocation of record, read a # of words

	//Read bytes until FRS reports that the read is complete
	while (1)
	{
		//Now we wait for response
		while (1)
		{
			uint8_t counter = 0;
			while (BNO085_receivePacket() == 0)
			{
				if (counter++ > 100)
					return (0); //Give up
				LL_mDelay(1);
			}

			//We have the packet, inspect it for the right contents
			//See page 40. Report ID should be 0xF3 and the FRS types should match the thing we requested
			if (shtpData[0] == SHTP_REPORT_FRS_READ_RESPONSE)
				if (((uint16_t)shtpData[13] << 8 | shtpData[12]) == recordID)
					break; //This packet is one we are looking for
		}

		uint8_t dataLength = shtpData[1] >> 4;
		uint8_t frsStatus = shtpData[1] & 0x0F;

		uint32_t data0 = (uint32_t)shtpData[7] << 24 | (uint32_t)shtpData[6] << 16 | (uint32_t)shtpData[5] << 8 | (uint32_t)shtpData[4];
		uint32_t data1 = (uint32_t)shtpData[11] << 24 | (uint32_t)shtpData[10] << 16 | (uint32_t)shtpData[9] << 8 | (uint32_t)shtpData[8];

		//Record these words to the metaData array
		if (dataLength > 0)
		{
			metaData[spot++] = data0;
		}
		if (dataLength > 1)
		{
			metaData[spot++] = data1;
		}

		if (spot >= MAX_METADATA_SIZE)
		{
			//printf("metaData array over run. Returning."); debug uart 2
			return (1); //We have run out of space in our array. Bail.
		}

		if (frsStatus == 3 || frsStatus == 6 || frsStatus == 7)
		{
			return (1); //FRS status is read completed! We're done!
		}
	}
}

//Send command to reset IC
//Read all advertisement packets from sensor
//The sensor has been seen to reset twice if we attempt too much too quickly.
//This seems to work reliably.
void BNO085_softReset(void)
{
	shtpData[0] = 1; //Reset

	//Attempt to start communication with sensor
	BNO085_sendPacket(CHANNEL_EXECUTABLE, 1); //Transmit packet on channel 1, 1 byte

	//Read all incoming data and flush it
	LL_mDelay(50);
	while (BNO085_receivePacket() == 1);
	LL_mDelay(50);
	while (BNO085_receivePacket() == 1);
}

//Get the reason for the last reset
//1 = POR, 2 = Internal reset, 3 = Watchdog, 4 = External reset, 5 = Other
uint8_t BNO085_resetReason()
{
	shtpData[0] = SHTP_REPORT_PRODUCT_ID_REQUEST; //Request the product ID and reset info
	shtpData[1] = 0;							  //Reserved

	//Transmit packet on channel 2, 2 bytes
	BNO085_sendPacket(CHANNEL_CONTROL, 2);

	//Now we wait for response
	if (BNO085_receivePacket() == 1)
	{
		if (shtpData[0] == SHTP_REPORT_PRODUCT_ID_RESPONSE)
		{
			return (shtpData[1]);
		}
	}

	return (0);
}

//Given a register value and a Q point, convert to float
//See https://en.wikipedia.org/wiki/Q_(number_format)
float BNO085_qToFloat(int16_t fixedPointValue, uint8_t qPoint)
{
	return fixedPointValue * powf(2, qPoint * -1);
}

//Sends the packet to enable the rotation vector
void BNO085_enableRotationVector(uint16_t timeBetweenReports)
{
	BNO085_setFeatureCommand(SENSOR_REPORTID_ROTATION_VECTOR, timeBetweenReports, 0);
}

//Sends the packet to enable the rotation vector
void BNO085_enableGameRotationVector(uint16_t timeBetweenReports)
{
	BNO085_setFeatureCommand(SENSOR_REPORTID_GAME_ROTATION_VECTOR, timeBetweenReports, 0);
}

//Sends the packet to enable the accelerometer
void BNO085_enableAccelerometer(uint16_t timeBetweenReports)
{
	BNO085_setFeatureCommand(SENSOR_REPORTID_ACCELEROMETER, timeBetweenReports, 0);
}

//Sends the packet to enable the accelerometer
void BNO085_enableLinearAccelerometer(uint16_t timeBetweenReports)
{
	BNO085_setFeatureCommand(SENSOR_REPORTID_LINEAR_ACCELERATION, timeBetweenReports, 0);
}

//Sends the packet to enable the gyro
void BNO085_enableGyro(uint16_t timeBetweenReports)
{
	BNO085_setFeatureCommand(SENSOR_REPORTID_GYROSCOPE, timeBetweenReports, 0);
}

//Sends the packet to enable the magnetometer
void BNO085_enableMagnetometer(uint16_t timeBetweenReports)
{
	BNO085_setFeatureCommand(SENSOR_REPORTID_MAGNETIC_FIELD, timeBetweenReports, 0);
}

//Sends the packet to enable the step counter
void BNO085_enableStepCounter(uint16_t timeBetweenReports)
{
	BNO085_setFeatureCommand(SENSOR_REPORTID_STEP_COUNTER, timeBetweenReports, 0);
}

//Sends the packet to enable the Stability Classifier
void BNO085_enableStabilityClassifier(uint16_t timeBetweenReports)
{
	BNO085_setFeatureCommand(SENSOR_REPORTID_STABILITY_CLASSIFIER, timeBetweenReports, 0);
}

//Sends the commands to begin calibration of the accelerometer
void BNO085_calibrateAccelerometer()
{
	BNO085_sendCalibrateCommand(CALIBRATE_ACCEL);
}

//Sends the commands to begin calibration of the gyro
void BNO085_calibrateGyro()
{
	BNO085_sendCalibrateCommand(CALIBRATE_GYRO);
}

//Sends the commands to begin calibration of the magnetometer
void BNO085_calibrateMagnetometer()
{
	BNO085_sendCalibrateCommand(CALIBRATE_MAG);
}

//Sends the commands to begin calibration of the planar accelerometer
void BNO085_calibratePlanarAccelerometer()
{
	BNO085_sendCalibrateCommand(CALIBRATE_PLANAR_ACCEL);
}

//See 2.2 of the Calibration Procedure document 1000-4044
void BNO085_calibrateAll()
{
	BNO085_sendCalibrateCommand(CALIBRATE_ACCEL_GYRO_MAG);
}

void BNO085_endCalibration()
{
	BNO085_sendCalibrateCommand(CALIBRATE_STOP); //Disables all calibrations
}

//See page 51 of reference manual - ME Calibration Response
//Byte 5 is parsed during the readPacket and stored in calibrationStatus
int BNO085_calibrationComplete()
{
	if (calibrationStatus == 0)
		return (1);
	return (0);
}

//Given a sensor's report ID, this tells the BNO085 to begin reporting the values
//Also sets the specific config word. Useful for personal activity classifier
void BNO085_setFeatureCommand(uint8_t reportID, uint32_t microsBetweenReports, uint32_t specificConfig)
{
	shtpData[0] = SHTP_REPORT_SET_FEATURE_COMMAND;	 //Set feature command. Reference page 55
	shtpData[1] = reportID;						 //Feature Report ID. 0x01 = Accelerometer, 0x05 = Rotation vector
	shtpData[2] = 0;							 //Feature flags
	shtpData[3] = 0;							 //Change sensitivity (LSB)
	shtpData[4] = 0;							 //Change sensitivity (MSB)
	shtpData[5] = (microsBetweenReports >> 0) & 0xFF;  //Report interval (LSB) in microseconds. 0x7A120 = 500ms
	shtpData[6] = (microsBetweenReports >> 8) & 0xFF;  //Report interval
	shtpData[7] = (microsBetweenReports >> 16) & 0xFF; //Report interval
	shtpData[8] = (microsBetweenReports >> 24) & 0xFF; //Report interval (MSB)
	shtpData[9] = 0;							 //Batch Interval (LSB)
	shtpData[10] = 0;							 //Batch Interval
	shtpData[11] = 0;							 //Batch Interval
	shtpData[12] = 0;							 //Batch Interval (MSB)
	shtpData[13] = (specificConfig >> 0) & 0xFF;	   	 //Sensor-specific config (LSB)
	shtpData[14] = (specificConfig >> 8) & 0xFF;	   	 //Sensor-specific config
	shtpData[15] = (specificConfig >> 16) & 0xFF;	 //Sensor-specific config
	shtpData[16] = (specificConfig >> 24) & 0xFF;	 //Sensor-specific config (MSB)

	//Transmit packet on channel 2, 17 bytes
	BNO085_sendPacket(CHANNEL_CONTROL, 17);
}

//Tell the sensor to do a command
//See 6.3.8 page 41, Command request
//The caller is expected to set P0 through P8 prior to calling
void BNO085_sendCommand(uint8_t command)
{
	shtpData[0] = SHTP_REPORT_COMMAND_REQUEST; //Command Request
	shtpData[1] = commandSequenceNumber++;	 //Increments automatically each function call
	shtpData[2] = command;					   //Command

	//Caller must set these
	/*shtpData[3] = 0; //P0
	shtpData[4] = 0; //P1
	shtpData[5] = 0; //P2
	shtpData[6] = 0;
	shtpData[7] = 0;
	shtpData[8] = 0;
	shtpData[9] = 0;
	shtpData[10] = 0;
	shtpData[11] = 0;*/

	//Transmit packet on channel 2, 12 bytes
	BNO085_sendPacket(CHANNEL_CONTROL, 12);
}

//This tells the BNO085 to begin calibrating
//See page 50 of reference manual and the 1000-4044 calibration doc
void BNO085_sendCalibrateCommand(uint8_t thingToCalibrate)
{
	/*shtpData[3] = 0; //P0 - Accel Cal Enable
	shtpData[4] = 0; //P1 - Gyro Cal Enable
	shtpData[5] = 0; //P2 - Mag Cal Enable
	shtpData[6] = 0; //P3 - Subcommand 0x00
	shtpData[7] = 0; //P4 - Planar Accel Cal Enable
	shtpData[8] = 0; //P5 - Reserved
	shtpData[9] = 0; //P6 - Reserved
	shtpData[10] = 0; //P7 - Reserved
	shtpData[11] = 0; //P8 - Reserved*/

	for (uint8_t x = 3; x < 12; x++) //Clear this section of the shtpData array
		shtpData[x] = 0;

	if (thingToCalibrate == CALIBRATE_ACCEL)
		shtpData[3] = 1;
	else if (thingToCalibrate == CALIBRATE_GYRO)
		shtpData[4] = 1;
	else if (thingToCalibrate == CALIBRATE_MAG)
		shtpData[5] = 1;
	else if (thingToCalibrate == CALIBRATE_PLANAR_ACCEL)
		shtpData[7] = 1;
	else if (thingToCalibrate == CALIBRATE_ACCEL_GYRO_MAG)
	{
		shtpData[3] = 1;
		shtpData[4] = 1;
		shtpData[5] = 1;
	}
	else if (thingToCalibrate == CALIBRATE_STOP)
		; //Do nothing, bytes are set to zero

	//Make the internal calStatus variable non-zero (operation failed) so that user can test while we wait
	calibrationStatus = 1;

	//Using this shtpData packet, send a command
	BNO085_sendCommand(COMMAND_ME_CALIBRATE);
}

//Request ME Calibration Status from BNO085
//See page 51 of reference manual
void BNO085_requestCalibrationStatus()
{
	/*shtpData[3] = 0; //P0 - Reserved
	shtpData[4] = 0; //P1 - Reserved
	shtpData[5] = 0; //P2 - Reserved
	shtpData[6] = 0; //P3 - 0x01 - Subcommand: Get ME Calibration
	shtpData[7] = 0; //P4 - Reserved
	shtpData[8] = 0; //P5 - Reserved
	shtpData[9] = 0; //P6 - Reserved
	shtpData[10] = 0; //P7 - Reserved
	shtpData[11] = 0; //P8 - Reserved*/

	for (uint8_t x = 3; x < 12; x++) //Clear this section of the shtpData array
		shtpData[x] = 0;

	shtpData[6] = 0x01; //P3 - 0x01 - Subcommand: Get ME Calibration

	//Using this shtpData packet, send a command
	BNO085_sendCommand(COMMAND_ME_CALIBRATE);
}

//This tells the BNO085 to save the Dynamic Calibration Data (DCD) to flash
//See page 49 of reference manual and the 1000-4044 calibration doc
void BNO085_saveCalibration()
{
	/*shtpData[3] = 0; //P0 - Reserved
	shtpData[4] = 0; //P1 - Reserved
	shtpData[5] = 0; //P2 - Reserved
	shtpData[6] = 0; //P3 - Reserved
	shtpData[7] = 0; //P4 - Reserved
	shtpData[8] = 0; //P5 - Reserved
	shtpData[9] = 0; //P6 - Reserved
	shtpData[10] = 0; //P7 - Reserved
	shtpData[11] = 0; //P8 - Reserved*/

	for (uint8_t x = 3; x < 12; x++) //Clear this section of the shtpData array
		shtpData[x] = 0;

	//Using this shtpData packet, send a command
	BNO085_sendCommand(COMMAND_DCD); //Save DCD command
}

//Blocking wait for BNO085 to assert (pull low) the INT pin
//indicating it's ready for comm. Can take more than 104ms
//after a hardware reset
int BNO085_waitForSPI(void)
{
	for (uint32_t counter = 0; counter < 0xffffffff; counter++) //Don't got more than 255
	{
		if (LL_GPIO_IsInputPinSet(BNO085_INT_PORT, BNO085_INT_PIN) == 0)
		{
			//printf("\nData available\n");
			return (1);
		}
		//printf("SPI Wait %d\n", counter);
	}
	//printf("\nData not available\n"); debug uart 2
	return (0);
}


//Check to see if there is any new data available
//Read the contents of the incoming packet into the shtpData array
int BNO085_receivePacket(void)
{
	uint8_t incoming;

	if (LL_GPIO_IsInputPinSet(BNO085_INT_PORT, BNO085_INT_PIN) == 1)
		return (0); //Data is not available

	//Old way: if (BNO085_waitForSPI() == 0) return (0); //Something went wrong

	//Get first four bytes to find out how much data we need to read

	CHIP_SELECT(BNO085);

	//Get the first four bytes, aka the packet header
	uint8_t packetLSB = SPI2_SendByte(0);
	uint8_t packetMSB = SPI2_SendByte(0);
	uint8_t channelNumber = SPI2_SendByte(0);
	uint8_t sequenceNumber = SPI2_SendByte(0); //Not sure if we need to store this or not

	//Store the header info
	shtpHeader[0] = packetLSB;
	shtpHeader[1] = packetMSB;
	shtpHeader[2] = channelNumber;
	shtpHeader[3] = sequenceNumber;

	//Calculate the number of data bytes in this packet
	int16_t dataLength = ((uint16_t)packetMSB << 8 | packetLSB);
	dataLength &= 0x7fff; //Clear the MSbit.
	//This bit indicates if this package is a continuation of the last. Ignore it for now.
	//TODO catch this as an error and exit
	if (dataLength == 0)
	{
		//Packet is empty
		return (0); //All done
	}
	dataLength -= 4; //Remove the header bytes from the data count

	//printf("length: %d\n", dataLength);

	//Read incoming data into the shtpData array
	for (uint16_t dataSpot = 0; dataSpot < dataLength; dataSpot++)
	{
		incoming = SPI2_SendByte(0xFF);
		//printf("%d ", incoming);
		if (dataSpot < MAX_PACKET_SIZE)	//BNO085 can respond with upto 270 bytes, avoid overflow
			shtpData[dataSpot] = incoming; //Store data into the shtpData array
	}
	//printf("\n");

	CHIP_DESELECT(BNO085); //Release BNO085
	return (1); //We're done!
}


//Given the data packet, send the header then the data
//Returns false if sensor does not ACK
//TODO - Arduino has a max 32 byte send. Break sending into multi packets if needed.
int BNO085_sendPacket(uint8_t channelNumber, uint8_t dataLength)
{
	uint8_t packetLength = dataLength + 4; //Add four bytes for the header

	//Wait for BNO085 to indicate it is available for communication
	if (BNO085_waitForSPI() == 0)
		return (0); //Data is not available

	//BNO085 has max CLK of 3MHz, MSB first,
	//The BNO085 uses CPOL = 1 and CPHA = 1. This is mode3
	CHIP_SELECT(BNO085);

	//Send the 4 byte packet header
	SPI2_SendByte(packetLength & 0xFF);			//Packet length LSB
	SPI2_SendByte(packetLength >> 8);				//Packet length MSB
	SPI2_SendByte(channelNumber);					//Channel number
	SPI2_SendByte(sequenceNumber[channelNumber]++); 	//Send the sequence number, increments with each packet sent, different counter for each channel

	//Send the user's data packet
	for (uint8_t i = 0; i < dataLength; i++)
	{
		SPI2_SendByte(shtpData[i]);
	}

	CHIP_DESELECT(BNO085);

	return (1);
}
///////////////////////////////////////////////////////////////////////////