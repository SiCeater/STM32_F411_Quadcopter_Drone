# STM32 F411 Quadcopter Drone

> Projet de drone quadcopter basé sur le microcontrôleur STM32F411 avec les drivers STM LL (Low Layer)

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-STM32F411-green.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32f411.html)
[![Language](https://img.shields.io/badge/language-C-orange.svg)](https://en.wikipedia.org/wiki/C_(programming_language))

## Sommaire

- [À propos](#-à-propos)
- [Fonctionnalités](#-fonctionnalités)
- [Architecture matérielle](#-architecture-matérielle)
- [Prérequis](#-prérequis)
- [Installation](#-installation)
- [Structure du projet](#-structure-du-projet)
- [Utilisation](#-utilisation)
- [Configuration](#-configuration)
- [Développement](#-développement)
- [Contribution](#-contribution)
- [Licence](#-licence)

## À propos

Ce projet implémente un système de contrôle de vol pour un drone quadcopter utilisant le microcontrôleur **STM32F411** (sur bord générique type blackpill ). Le firmware gère la stabilisation, le contrôle moteur et la communication basique avec un protocole UART simple en utilisant un couple de modules TX/RX HC-05.

### Points clés
- Contrôle temps réel avec STM32F411 (100 MHz)
- Algorithmes PID pour la stabilisation
- Communication sans fil intégrée

## Fonctionnalités

### Contrôle de vol (pas encore implementé)
- **Stabilisation multi-axes** : Roll, Pitch, Yaw 
- **Contrôleurs PID** tunables pour chaque axe
- **Modes de vol** : Acro, Angle, Horizon
- **Failsafe** et protection contre les crashs

### Capteur
- **IMU** (Inertial Measurement Unit) : accéléromètre + gyroscope + magnétomètre
  la fusion accel/gyro/mag est gérée en interne par le BNO085 avec un algoritme avancé (type IEKF) les quaterions sont directement calculés et disponnibles dans des registres.

### Communication
- **Protocole radio ** pour télécommande
- **Configuration** via port USB (OTG/DFU) ou ST-Link V2/V3
- **Debug** en temps réel

### Sécurité
- Désarmement d'urgence en cas de perte liaison
- Limites de débattement configurables

## Architecture matérielle

### Composants principaux

| Composant | Modèle recommandé | Fonction |
|-----------|-------------------|----------|
| Microcontrôleur | STM32F411CEU6 | Cœur du système |
| IMU | BNO085 | Mesure d'attitude |
| ESC | 4x BLHeli_S | Contrôle moteurs |
| Moteurs | 4x Brushless 2212-920KV | Propulsion |
| Récepteur | HC-05 | Réception radio |
| Batterie | LiPo 4S 3200mAh | Alimentation |

### Schéma de connexion
```raw
STM32F411
    ├── SPI1 → IMU (BNO085)
    ├── USART1 → Récepteur radio
    ├── USART2 → Debug/Config
    ├── TIM1 → PWM Moteur 1-4
```

## Prérequis

### Logiciels requis
- **VSCode et extension STM32CubeIDE**
- **STM32CubeMX** pour la configuration des périphériques
- **STM32CubeProgrammer (optionnel)** pour le flash
- **Git** pour le versioning

### Matériel requis
- **Carte de développement** STM32F411 (Black Pill ou équivalent)
- **Programmateur** ST-Link V2/V3 (optinnel)
- **Câbles** USB et de connexion (si pas de ST-Link pour le flash en USB/DFU)
- **Oscilloscope** ou analyseur logique (recommandé pour debug)

## Installation

### 1. Cloner le repository
```bash
git clone https://github.com/SiCeater/STM32_F411_Quadcopter_Drone.git
cd STM32_F411_Quadcopter_Drone
```

### 2. Ouvrir le projet
```bash
# Avec VSCode
# File → Open folder → Sélectionner le dossier source_code/
```

### 3. Compiler le firmware
```bash
# Dans VSCode, la petite roue crantée en bas a gauche
```

### 4. Flasher le microcontrôleur
```bash
# Connecter le ST-Link au STM32F411
# Run → Debug (F11) ou Run (Ctrl+F11)
# Ou utiliser le script "flash_DFU.sh" (si pas de ST-Link), pour flasher en USB/DFU, enfoncer le bouton "BOOT" avant de brancher le cable USB pour mettre le F411 en mode DFU puis lancer le script
```

### 5. Vérification
- Connexion série (configurer la bonne vitesse en bps, voir le fichier .ioc ou uart.c et ne pas oublier d'activer le debug : debug = 1 dans global.c)
- Vérifier les messages de boot

## Structure du projet

```raw
STM32_F411_Quadcopter_Drone/
│
├── source_code/              # Code source du firmware
│   ├── Core/                 # Fichiers HAL et startup
│   │   ├── Src/             # Fichiers sources (.c)
│   │   └── Inc/             # Fichiers headers (.h)
│   ├── App/                 # Fichiers drivers materiels et middleware
│   │   ├── Src/             # Fichiers sources (.c)
│   │   └── Inc/             # Fichiers headers (.h)
│   ├── Drivers/             # Drivers STM32 HAL
│   └── *.ioc                # Fichier de config STM32CubeMX
│
├── documentation/           # Documentation du projet
│   ├── schemas/            # Schémas électroniques
│   ├── datasheets/         # Datasheets composants
│
└── README.md               # Ce fichier
```

2. **Vérification des moteurs**

### Limites de sécurité
```c
#define MAX_THROTTLE       2000  // µs
#define MIN_THROTTLE       1000  // µs
#define MAX_ANGLE          30    // degrés
#define BATTERY_MIN_VOLTAGE 12.8 // volts (4S LiPo)
```

## Développement

### Debug

```c
// Activer mode debug dans config.h
#define DEBUG_MODE 1

// Utiliser les macros de debug (decrite dans debug.c)
DEBUG_PRINT("IMU: roll=%.2f pitch=%.2f yaw=%.2f", roll, pitch, yaw);
```

## Contribution

Les contributions sont les bienvenues ce projet n'est qu'une ébauche, pour contribuer :

1. **Fork** le projet
2. Créer une **branche** pour votre feature (`git checkout -b feature/AmazingFeature`)
3. **Commiter** vos changements (`git commit -m 'Add some AmazingFeature'`)
4. **Pusher** vers la branche (`git push origin feature/AmazingFeature`)
5. Ouvrir une **Pull Request**

### Guidelines
- Utiliser uniquement les drivers matériels LL fournis par STM
- Code en anglais, commentaires en français acceptés
- Respecter le style de code existant
- Tester avant de soumettre
- Documenter les nouvelles fonctionnalités

## Licence

Ce projet est sous licence MIT

## Contact

**SiCeater** - [@SiCeater](https://github.com/SiCeater)

Lien du projet : [https://github.com/SiCeater/STM32_F411_Quadcopter_Drone](https://github.com/SiCeater/STM32_F411_Quadcopter_Drone)

---

Si ce projet vous aide, n'hésitez pas à lui donner une étoile !
