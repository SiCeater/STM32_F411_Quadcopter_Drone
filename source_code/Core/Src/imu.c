#include "imu.h"

uint8_t shtpData[MAX_PACKET_SIZE]; // Buffer de réception SHTP
int16_t receivedDataLength;
uint8_t shtpHeader[4];

/**
 * @brief Initialise le bus SPI et désactive la communication
 */
void IMU_SPIBus_init(void)
{
    LL_SPI_Enable(BNO085_SPI_CHANNEL);
    CHIP_DESELECT(BNO085);
    RESET_HIGH();
    if(debug>=1)
        print_to_console("\nSPI 1 initialisé\n", strlen("\nSPI 1 initialisé\n"));
}

/**
 * @brief Envoie et reçoit un octet via SPI
 */
uint8_t BNO085_SPI_bus_read_write_8(uint8_t data)
{
    while (!LL_SPI_IsActiveFlag_TXE(BNO085_SPI_CHANNEL));
    LL_SPI_TransmitData8(BNO085_SPI_CHANNEL, data);

    while (!LL_SPI_IsActiveFlag_RXNE(BNO085_SPI_CHANNEL));
    return LL_SPI_ReceiveData8(BNO085_SPI_CHANNEL);
}

/**
 * @brief Attend que le BNO085 soit prêt à répondre (INT bas)
 */
uint8_t BNO085_wait_response(void)
{
    for (uint32_t counter = 0; counter < 100; counter++) 
    {
        if (LL_GPIO_IsInputPinSet(BNO085_INT_PORT, BNO085_INT_PIN) == 0)
        {
            if(debug>= 2)
                print_to_console("Données disponibles\n", strlen("Données disponibles\n"));
            return 1;
        }
        if (debug >= 3)
        {
            char buffer_debug[50];
            sprintf(buffer_debug, "SPI Wait %ld\n", counter);
            print_to_console(buffer_debug, strlen(buffer_debug));
        }
        LL_mDelay(2);
    }
    if(debug>= 2)
        print_to_console("BNO085 timeout: Pas de réponse\n", strlen("BNO085 timeout: Pas de réponse\n"));
    return 0;
}



/**
 * @brief Lit la réponse du BNO085
 */
uint16_t BNO085_read_response(void)
{
    memset(shtpData, 0, MAX_PACKET_SIZE); // on remet le tableau a 0

    if (LL_GPIO_IsInputPinSet(BNO085_INT_PORT, BNO085_INT_PIN) == 1)
    {
        if (debug >= 3)
            print_to_console("Aucun signal du BNO085\n", strlen("Aucun signal du BNO085\n"));
        return 0;
    }

    CHIP_SELECT(BNO085);

    // Lecture du header SHTP
    for (int i = 0; i < 4; i++)
        shtpHeader[i] = BNO085_SPI_bus_read_write_8(0);

    receivedDataLength = ((uint16_t)shtpHeader[1] << 8 | shtpHeader[0]) & 0x7FFF;
    if (receivedDataLength < 4)
    {
        CHIP_DESELECT(BNO085);
        if (debug >= 3)
            print_to_console("Réponse vide ou erronée\n", strlen("Réponse vide ou erronée\n"));
        return 0;
    }

    receivedDataLength -= 4;  

    for (uint16_t i = 0; i < receivedDataLength; i++)
    {
        if (i < MAX_PACKET_SIZE)
            shtpData[i] = BNO085_SPI_bus_read_write_8(0xFF);
    }

    CHIP_DESELECT(BNO085);
    
    if (debug >= 2)
    {

        char buffer_debug[100];
        sprintf(buffer_debug, "length: %d\n", receivedDataLength);
        print_to_console(buffer_debug, strlen(buffer_debug));

        for (uint16_t i = 0; i < 4; i++)
        {
            sprintf(buffer_debug, "%X ", shtpHeader[i]);
            print_to_console(buffer_debug, strlen(buffer_debug));
        }
        
        for (uint16_t i = 0; i < receivedDataLength; i++)
        {
            sprintf(buffer_debug, "%X ", shtpData[i]);
            print_to_console(buffer_debug, strlen(buffer_debug));
        }
        print_to_console("\n",1);
    }
    return receivedDataLength;
}

void BNO085_SetFeature(uint8_t sensorID, uint32_t reportInterval)
{
    uint8_t setFeatureCommand[20] = {0};

    setFeatureCommand[0] = 0x14;
    setFeatureCommand[1] = 0x00;
    setFeatureCommand[2] = 0x02;
    setFeatureCommand[3] = 0x00;

    setFeatureCommand[4] = 0xFD; // Report ID for Set Feature
    setFeatureCommand[5] = sensorID; // Sensor ID
    setFeatureCommand[6] = 0x00; // Feature flags (default)
    
    // Change Sensitivity (absolute/relative) - Default to 0
    setFeatureCommand[7] = 0x00;
    setFeatureCommand[8] = 0x00;

    // Report Interval (Little Endian)
    setFeatureCommand[9] = (uint8_t)(reportInterval & 0xFF);
    setFeatureCommand[10] = (uint8_t)((reportInterval >> 8) & 0xFF);
    setFeatureCommand[11] = (uint8_t)((reportInterval >> 16) & 0xFF);
    setFeatureCommand[12] = (uint8_t)((reportInterval >> 24) & 0xFF);

    // Batch Interval (Set to 0 for real-time data)
    setFeatureCommand[13] = 0x00;
    setFeatureCommand[14] = 0x00;
    setFeatureCommand[15] = 0x00;
    setFeatureCommand[16] = 0x00;

    // Sensor-Specific Configuration - Default to 0
    setFeatureCommand[17] = 0x00;
    setFeatureCommand[18] = 0x00;
    setFeatureCommand[19] = 0x00;
    setFeatureCommand[20] = 0x00;

    // Envoi de la commande via SPI
    CHIP_SELECT(BNO085);
    for (uint8_t i = 0; i < 17; i++)
    {
        BNO085_SPI_bus_read_write_8(setFeatureCommand[i]);
    }
    CHIP_DESELECT(BNO085);

    if (debug >= 2)
    {
        print_to_console("command sent\n", strlen("command sent\n"));
    }
    BNO085_wait_response();
    BNO085_read_response(); // on lit la reponse ack du capteur apres la trame de set
}

/**
 * @brief Analyse les trames SPI reçues après l'initialisation
 */
void BNO085_Analyze_Trame()
{
    char buffer_debug[256];

    // Affichage de la version firmware
    snprintf(buffer_debug, sizeof(buffer_debug), "Firmware Version : %c.%c.%c\n",
             shtpData[13-4], shtpData[15-4], shtpData[17-4]);
    print_to_console(buffer_debug, strlen(buffer_debug));

    // Décodage des noms des services ASCII
    snprintf(buffer_debug, sizeof(buffer_debug),
             "- Protocole : %.*s\n- Control : %.*s\n- Executable : %.*s\n"
             "- Sensor Hub : %.*s\n- Input Normal : %.*s\n- Input Wake : %.*s\n"
             "- Input Gyro Rv : %.*s\n",
             4, &shtpData[33], 7, &shtpData[43], 10, &shtpData[59],
             10, &shtpData[90], 11, &shtpData[118], 9, &shtpData[135], 11, &shtpData[150]);
    print_to_console(buffer_debug, strlen(buffer_debug));
}

/**
 * @brief  Envoie un Get Feature (ReportID=0xFE) pour connaître
 *         l'intervalle courant d'un sensorID donné.
 * @param  sensorID  Identifiant du capteur virtuel (ex: 0x05, 0x2A, etc.)
 * @param  interval  Pointeur pour stocker la période trouvée (us)
 * @return 0 si erreur, 1 si OK
 */
uint8_t BNO085_GetFeature(uint8_t sensorID, uint32_t *interval)
{
    // Paquet minimal pour Get Feature
    // (SHTP payload: [0xFE, sensorID, ...])
    // On va construire un petit buffer SHTP style. Cf doc SH-2 sect. 6.5.3
    uint8_t getFeatureCmd[6] = {0};

    // Première partie d'en-tête local (pas l'entête SHTP complet, tu peux faire comme pour SetFeature)
    getFeatureCmd[0] = 0x06; // En-tête local minimal: length LSB = 6
    getFeatureCmd[1] = 0x00; // length MSB
    getFeatureCmd[2] = 0x02; // channel = 2 (Sensor Hub Control), à ajuster si besoin
    getFeatureCmd[3] = 0x00; // seqnum local ?

    // Charge utile
    getFeatureCmd[4] = 0xFE; // Report ID = Get Feature
    getFeatureCmd[5] = sensorID;

    // On émet la commande sur SPI
    CHIP_SELECT(BNO085);
    for(int i=0; i<6; i++)
    {
        BNO085_SPI_bus_read_write_8(getFeatureCmd[i]);
    }
    CHIP_DESELECT(BNO085);

    if (debug >= 2)
        print_to_console("GetFeature cmd sent\n", strlen("GetFeature cmd sent\n"));

    if(!BNO085_wait_response()) // si pas de reponse
    {
        if (debug >= 2)
            print_to_console("Timeout en GetFeature\n", strlen("Timeout en GetFeature\n"));
        return 0;
    }
    uint16_t rlen = BNO085_read_response(); // si reponse vide
    if(rlen < 1) 
    {
        if (debug >= 2)
            print_to_console("Pas de reponse a GetFeature\n", strlen("Pas de reponse a GetFeature\n"));
        return 0;
    }

    // Parcourir shtpData[] pour trouver un "Get Feature Response" (report ID=0xFC).
    // Format: byte0=0xFC, byte1= sensorID, byte2= FeatureFlags, byte3-4=ChangeSensitivity
    //         byte5-8=ReportInterval, byte9-12=BatchInterval, byte13-16=SensorSpecific
    //
    // -> On regarde si shtpData[0]==0xFC et shtpData[1]==sensorID
    // -> On récupère shtpData[5..8] pour l'interval
    // Il est possible qu'il y ait plusieurs paquets. On fait simple.

    // Cherchons un 0xFC dans shtpData
    // toutes les trame de retour de get feature ont la meme forme et la meme taille (17 octets)
    
    for(int i=0; i<rlen-1; i++)
    {
        if((shtpData[i] == 0xFC) && (shtpData[i+1] == sensorID))
        {
            // On lit l'interval
            uint32_t val =
                (uint32_t)shtpData[i+5] |
                ((uint32_t)shtpData[i+6]<<8) |
                ((uint32_t)shtpData[i+7]<<16) |
                ((uint32_t)shtpData[i+8]<<24);
            *interval = val;

            if(debug >= 2) {
                char dbgBuf[80];
                sprintf(dbgBuf, "[GetFeature] Sensor=0x%02X, interval=%lu us\n", sensorID, val);
                print_to_console(dbgBuf, strlen(dbgBuf));
            }
            return 1;
        }
    }

    // Si on arrive ici, on n'a pas trouvé la réponse attendue
    print_to_console("GetFeature: pas de reponse 0xFC ou sensorID mismatch\n", 53);
    return 0;
}

/**
 * @brief Vérifie si les 2 capteurs virtuels (0x2A et 0x05) sont actifs,
 *        les active le cas échéant, puis re-vérifie leur activation.
 */
void BNO085_CheckAndEnableSensors(void)
{
    char dbgBuf[100];
    uint32_t intervalCheck = 0;

    if(debug >= 1)
        print_to_console("\n--- Vérif capteurs GyroIntRV & RotationVec ---\n", strlen("\n--- Vérif capteurs GyroIntRV & RotationVec ---\n"));

    // 1) Vérifier le Gyro-Integrated Rotation Vector
    if(BNO085_GetFeature(SENSORID_GYRO_INTEGRATED, &intervalCheck))
    {
        if(intervalCheck == 0) {
            // Non activé, on l'active
            if(debug >= 1) {
                print_to_console("GyroIntegratedRV inactif, on l'active...\n", strlen("GyroIntegratedRV inactif, on l'active...\n"));
            }
            BNO085_SetFeature(SENSORID_GYRO_INTEGRATED, INTERVAL_GYRO_INTEGRATED);
            // Relire pour vérifier
            intervalCheck = 0;
            BNO085_GetFeature(SENSORID_GYRO_INTEGRATED, &intervalCheck);

            if(intervalCheck == 0) {
                if(debug >= 1)
                    print_to_console("Echec activation GyroIntegratedRV\n", strlen("Echec activation GyroIntegratedRV\n"));
            } else {
                if(debug >= 1)
                    print_to_console("GyroIntegratedRV activé OK\n", strlen("GyroIntegratedRV activé OK\n"));
            }
        } else {
            // Déjà actif
            if(debug >= 1)
            {
                sprintf(dbgBuf, "GyroIntegratedRV déjà actif à %lu us\n", intervalCheck);
                print_to_console(dbgBuf, strlen(dbgBuf));
            }
        }
    }

    // 2) Vérifier le Rotation Vector
    if(BNO085_GetFeature(SENSORID_ROTATION_VECTOR, &intervalCheck))
    {
        if(intervalCheck == 0) {
            // Non activé => on l'active
            if(debug >= 1) {
                print_to_console("RotationVector inactif, on l'active...\n", strlen("RotationVector inactif, on l'active...\n"));
            }
            BNO085_SetFeature(SENSORID_ROTATION_VECTOR, INTERVAL_ROTATION_VECTOR);
            // Relire pour vérifier
            intervalCheck = 0;
            BNO085_GetFeature(SENSORID_ROTATION_VECTOR, &intervalCheck);

            if(intervalCheck == 0) {
                if(debug >= 1)
                    print_to_console("Echec activation RotationVector\n", strlen("Echec activation RotationVector\n"));
            } else {
                if(debug >= 1)
                    print_to_console("RotationVector activé OK\n", strlen("RotationVector activé OK\n"));
            }
        } else {
            // Déjà actif
            if(debug >= 1)
            {
                sprintf(dbgBuf, "RotationVector déjà actif à %lu us\n", intervalCheck);
                print_to_console(dbgBuf, strlen(dbgBuf));
            }
        }
    }

    print_to_console("--- Fin vérif capteurs ---\n", strlen("--- Fin vérif capteurs ---\n"));
}

/**
 * @brief Initialise le BNO085 et effectue une analyse des trames SPI après l'init
 */
void IMU_Init()
{
    IMU_SPIBus_init();
    RESET_LOW();
    LL_mDelay(10);
    RESET_HIGH();
    LL_mDelay(500);

    if (!BNO085_wait_response())
    {
        if(debug >= 1)
            print_to_console("Erreur: BNO085 ne répond pas au démarrage\n", strlen("Erreur: BNO085 ne répond pas au démarrage\n"));
        return;
    }

    BNO085_read_response();
    if(debug >= 1)
        print_to_console("BNO085 prêt\n", strlen("BNO085 prêt\n"));

    // Analyse des trames SPI reçues
    BNO085_Analyze_Trame();

    BNO085_wait_response();
    BNO085_read_response();
    BNO085_wait_response();
    BNO085_read_response();
    // ces deux dernieres lectures car le BNO repond 2 fois apres un reset
}