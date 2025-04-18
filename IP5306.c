#include "BitOps.h"
#include "IP5306.h"

#define I2C_READ_TIMEOUT_MS 5
#define I2C_WRITE_WAIT_MS 5

#define REG_SYS_CTL0_ADDR 0x00
#define REG_SYS_CTL1_ADDR 0x01
#define REG_SYS_CTL2_ADDR 0x02

#define REG_CHARGER_CTL0_ADDR 0x20
#define REG_CHARGER_CTL1_ADDR 0x21
#define REG_CHARGER_CTL2_ADDR 0x22
#define REG_CHARGER_CTL3_ADDR 0x23
#define REG_CHG_DIG_CTL0_ADDR 0x24

#define REG_READ0_ADDR 0x70
#define REG_READ1_ADDR 0x71
#define REG_READ2_ADDR 0x72
#define REG_READ3_ADDR 0x77

#define KEY_SHORT_PRESS_MS 30 // If the button is pressed for longer than 30ms but less than 2s, it is a short press.
#define KEY_LONG_PRESS_MS 2000 // If the button is pressed for longer than 2 seconds, it is a long press

#define MIN_STATE_CHANGE_PERIOD_MS 1000


bool IP5306_Init(struct IP5306_Platform *platform) {
    platform->setKeyGpioMode(IP5306_GpioMode_FloatingInput);

    platform->state = IP5306_State_Unknown;
    platform->lastStateChangeCycleTime = platform->invalidCycleTimeValue;

    return true;
}

void IP5306_Step(struct IP5306_Platform *platform, uint32_t cycleTime) {
    enum IP5306_State prevState = platform->state;

    // Check that enough time has passed since the last state change to avoid confusion press with double press
    bool stateChanging = (platform->state == IP5306_State_WakingUp || platform->state == IP5306_State_ShuttingDown) &&
        (platform->lastStateChangeCycleTime != platform->invalidCycleTimeValue &&
        platform->getTimeDiffMs(cycleTime, platform->lastStateChangeCycleTime) < MIN_STATE_CHANGE_PERIOD_MS + 500);
    
    if (!stateChanging) {
        // Update state based on IRQ pin
        int irq = platform->getIrqGpioPin();
        if (irq) {
            platform->state = IP5306_State_Working;
        } else {
            platform->state = IP5306_State_Sleep;
        }

        // Update state based on status register
        // struct IP5306_Status status;
        // if (IP5306_ReadStatus(platform, &status, IP5306_READ0_BIT)) {
        //     if (status.read0RegData == IP5306_SLEEPING_ANY_REG_VALUE) {
        //         platform->state = IP5306_State_Sleep;
        //     } else {
        //         platform->state = IP5306_State_Working;
        //     }
        // }

        platform->lastStateChangeCycleTime = platform->invalidCycleTimeValue;
    }

    if (platform->state != prevState) {
        platform->debugPrint("IP5306: State changed from %d to %d\r\n", prevState, platform->state);
    }
}

enum IP5306_State IP5306_GetState(struct IP5306_Platform *platform) {
    return platform->state;
}

bool IP5306_IsWorkingState(struct IP5306_Platform *platform) {
    return platform->state == IP5306_State_Working;
}

bool IP5306_WakeUp(struct IP5306_Platform *platform) {
    if (platform->state != IP5306_State_Sleep) {
        return false;
    }

    // A short press will turn on the power indicator and boost output.
    platform->setKeyGpioMode(IP5306_GpioMode_PushPullOutput);
    platform->setKeyGpioPin(0);
    platform->delayMs(4 * KEY_SHORT_PRESS_MS);
    platform->setKeyGpioMode(IP5306_GpioMode_FloatingInput);

    platform->state = IP5306_State_WakingUp;
    platform->lastStateChangeCycleTime = platform->getCycleTime();

    platform->debugPrint("IP5306: Waking up key sent\r\n");

    return true;
}

bool IP5306_Shutdown(struct IP5306_Platform *platform) {
    if (platform->state != IP5306_State_Working) {
        return false;
    }

    // Pressing the button twice within 1 second will turn off the boost output, power display and lighting LED.
    platform->setKeyGpioMode(IP5306_GpioMode_PushPullOutput);
    platform->setKeyGpioPin(0);
    platform->delayMs(4 * KEY_SHORT_PRESS_MS); // safety margin
    platform->setKeyGpioMode(IP5306_GpioMode_FloatingInput);

    platform->delayMs(100);

    platform->setKeyGpioMode(IP5306_GpioMode_PushPullOutput);
    platform->setKeyGpioPin(0);
    platform->delayMs(4 * KEY_SHORT_PRESS_MS); // safety margin
    platform->setKeyGpioMode(IP5306_GpioMode_FloatingInput);

    platform->state = IP5306_State_ShuttingDown;
    platform->lastStateChangeCycleTime = platform->getCycleTime();

    platform->debugPrint("IP5306: Shutdown key sent\r\n");

    return true;
}

bool IP5306_ReadSystemControl(struct IP5306_Platform *platform, struct IP5306_SystemControl *systemControl, unsigned int regBits) {
    int ret;
    uint8_t data;

    // Read SYS_CTL0 register
    if (regBits & IP5306_SYS_CTL0_BIT) {
        ret = platform->i2cReadReg(IP5306_I2C_ADDR, REG_SYS_CTL0_ADDR, &data, 1, I2C_READ_TIMEOUT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to read SYS_CTL0 register: %d\r\n", -ret);
            return false;
        }

        systemControl->boostEnable = BITOPS_GET_BIT(data, 5);
        systemControl->chargerEnable = BITOPS_GET_BIT(data, 4);
        systemControl->autoPowerOn = BITOPS_GET_BIT(data, 2);
        systemControl->outputNormallyOpen = BITOPS_GET_BIT(data, 1);
        systemControl->keyShutdownEnable = BITOPS_GET_BIT(data, 0);

        systemControl->sysCtl0RegData = data;
    }

    // Read SYS_CTL1 register
    if (regBits & IP5306_SYS_CTL1_BIT) {
        ret = platform->i2cReadReg(IP5306_I2C_ADDR, REG_SYS_CTL1_ADDR, &data, 1, I2C_READ_TIMEOUT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to read SYS_CTL1 register: %d\r\n", -ret);
            return false;
        }

        systemControl->disableBoostControl = (enum IP5306_DisableBoostControl)BITOPS_GET_BIT(data, 7);
        systemControl->switchWLEDControl = (enum IP5306_SwitchWLEDControl)BITOPS_GET_BIT(data, 6);
        systemControl->shortPressSwitchBoostEnable = BITOPS_GET_BIT(data, 5);
        systemControl->enableBoostAfterVINUnplug = BITOPS_GET_BIT(data, 2);
        systemControl->batlow3V0ShutdownEnable = BITOPS_GET_BIT(data, 0);

        systemControl->sysCtl1RegData = data;
    }

    // Read SYS_CTL2 register
    if (regBits & IP5306_SYS_CTL2_BIT) {
        ret = platform->i2cReadReg(IP5306_I2C_ADDR, REG_SYS_CTL2_ADDR, &data, 1, I2C_READ_TIMEOUT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to read SYS_CTL2 register: %d\r\n", -ret);
            return false;
        }

        systemControl->lightLoadShutdownTime = (enum IP5306_LightLoadShutdownTime)BITOPS_GET_BITS(data, 2, 2);

        systemControl->sysCtl2RegData = data;
    }

    return true;
}

bool IP5306_WriteSystemControl(struct IP5306_Platform *platform, struct IP5306_SystemControl *systemControl, unsigned int regBits) {
    int ret;
    uint8_t data;

    if (regBits & IP5306_SYS_CTL0_BIT) {
        data = systemControl->sysCtl0RegData;
        BITOPS_SET_BIT(&data, 5, systemControl->boostEnable);
        BITOPS_SET_BIT(&data, 4, systemControl->chargerEnable);
        BITOPS_SET_BIT(&data, 2, systemControl->autoPowerOn);
        BITOPS_SET_BIT(&data, 1, systemControl->outputNormallyOpen);
        BITOPS_SET_BIT(&data, 0, systemControl->keyShutdownEnable);

        ret = platform->i2cWriteReg(IP5306_I2C_ADDR, REG_SYS_CTL0_ADDR, &data, 1, I2C_WRITE_WAIT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to write SYS_CTL0 register: %d\r\n", -ret);
            return false;
        }

        systemControl->sysCtl0RegData = data;
    }

    if (regBits & IP5306_SYS_CTL1_BIT) {
        data = systemControl->sysCtl1RegData;
        BITOPS_SET_BIT(&data, 7, systemControl->disableBoostControl);
        BITOPS_SET_BIT(&data, 6, systemControl->switchWLEDControl);
        BITOPS_SET_BIT(&data, 5, systemControl->shortPressSwitchBoostEnable);
        BITOPS_SET_BIT(&data, 2, systemControl->enableBoostAfterVINUnplug);
        BITOPS_SET_BIT(&data, 0, systemControl->batlow3V0ShutdownEnable);

        ret = platform->i2cWriteReg(IP5306_I2C_ADDR, REG_SYS_CTL1_ADDR, &data, 1, I2C_WRITE_WAIT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to write SYS_CTL1 register: %d\r\n", -ret);
            return false;
        }

        systemControl->sysCtl1RegData = data;
    }

    if (regBits & IP5306_SYS_CTL2_BIT) {
        data = systemControl->sysCtl2RegData;
        BITOPS_SET_BITS(&data, 2, 2, systemControl->lightLoadShutdownTime);

        ret = platform->i2cWriteReg(IP5306_I2C_ADDR, REG_SYS_CTL2_ADDR, &data, 1, I2C_WRITE_WAIT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to write SYS_CTL2 register: %d\r\n", -ret);
            return false;
        }

        systemControl->sysCtl2RegData = data;
    }

    return true;
}

bool IP5306_ReadChargerControl(struct IP5306_Platform *platform, struct IP5306_ChargerControl *chargerControl, unsigned int regBits) {
    int ret;
    uint8_t data;

    // Read Charger_CTL0 register
    if (regBits & IP5306_CHARGER_CTL0_BIT) {
        ret = platform->i2cReadReg(IP5306_I2C_ADDR, REG_CHARGER_CTL0_ADDR, &data, 1, I2C_READ_TIMEOUT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to read CHARGER_CTL0 register: %d\r\n", -ret);
            return false;
        }

        chargerControl->chargerFullStop = (enum IP5306_ChargerFullStop)BITOPS_GET_BITS(data, 0, 2);

        chargerControl->chargerCtl0RegData = data;
    }

    // Read Charger_CTL1 register
    if (regBits & IP5306_CHARGER_CTL1_BIT) {
        ret = platform->i2cReadReg(IP5306_I2C_ADDR, REG_CHARGER_CTL1_ADDR, &data, 1, I2C_READ_TIMEOUT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to read CHARGER_CTL1 register: %d\r\n", -ret);
            return false;
        }

        chargerControl->endCurrentDetection = (enum IP5306_EndCurrentDetection)BITOPS_GET_BITS(data, 6, 2);
        chargerControl->chargingUndervoltageLoop = (enum IP5306_ChargingUndervoltageLoop)BITOPS_GET_BITS(data, 2, 3);

        chargerControl->chargerCtl1RegData = data;
    }

    // Read Charger_CTL2 register
    if (regBits & IP5306_CHARGER_CTL2_BIT) {
        ret = platform->i2cReadReg(IP5306_I2C_ADDR, REG_CHARGER_CTL2_ADDR, &data, 1, I2C_READ_TIMEOUT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to read CHARGER_CTL2 register: %d\r\n", -ret);
            return false;
        }

        chargerControl->batteryVoltage = (enum IP5306_BatteryVoltage)BITOPS_GET_BITS(data, 2, 2);
        chargerControl->constantVoltageCharging = (enum IP5306_ConstantVoltageCharging)BITOPS_GET_BITS(data, 0, 2);

        chargerControl->chargerCtl2RegData = data;
    }

    // Read Charger_CTL3 register
    if (regBits & IP5306_CHARGER_CTL3_BIT) {
        ret = platform->i2cReadReg(IP5306_I2C_ADDR, REG_CHARGER_CTL3_ADDR, &data, 1, I2C_READ_TIMEOUT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to read CHARGER_CTL3 register: %d\r\n", -ret);
            return false;
        }

        chargerControl->chargingCurrentLoop = (enum IP5306_ChargingCurrentLoop)BITOPS_GET_BIT(data, 5);

        chargerControl->chargerCtl3RegData = data;
    }

    // Read CHG_DIG_CTL0 register
    if (regBits & IP5306_CHG_DIG_CTL0_BIT) {
        ret = platform->i2cReadReg(IP5306_I2C_ADDR, REG_CHG_DIG_CTL0_ADDR, &data, 1, I2C_READ_TIMEOUT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to read CHG_DIG_CTL0 register: %d\r\n", -ret);
            return false;
        }

        // Read polynomial coefficient bits
        int b0 = BITOPS_GET_BIT(data, 0);
        int b1 = BITOPS_GET_BIT(data, 1);
        int b2 = BITOPS_GET_BIT(data, 2);
        int b3 = BITOPS_GET_BIT(data, 3);
        int b4 = BITOPS_GET_BIT(data, 4);

        // Calculate current in mA
        chargerControl->chargingCurrent = 50 + b0 * 100 + b1 * 200 + b2 * 400 + b3 * 800 + b4 * 1600;

        chargerControl->chgDigCtl0RegData = data;
    }

    return true;
}

bool IP5306_WriteChargerControl(struct IP5306_Platform *platform, struct IP5306_ChargerControl *chargerControl, unsigned int regBits) {
    int ret;
    uint8_t data;

    if (regBits & IP5306_CHARGER_CTL0_BIT) {
        data = chargerControl->chargerCtl0RegData;
        BITOPS_SET_BITS(&data, 0, 2, (uint8_t)chargerControl->chargerFullStop);

        ret = platform->i2cWriteReg(IP5306_I2C_ADDR, REG_CHARGER_CTL0_ADDR, &data, 1, I2C_WRITE_WAIT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to write CHARGER_CTL0 register: %d\r\n", -ret);
            return false;
        }

        chargerControl->chargerCtl0RegData = data;
    }

    if (regBits & IP5306_CHARGER_CTL1_BIT) {
        data = chargerControl->chargerCtl1RegData;
        BITOPS_SET_BITS(&data, 6, 2, (uint8_t)chargerControl->endCurrentDetection);
        BITOPS_SET_BITS(&data, 2, 3, (uint8_t)chargerControl->chargingUndervoltageLoop);

        ret = platform->i2cWriteReg(IP5306_I2C_ADDR, REG_CHARGER_CTL1_ADDR, &data, 1, I2C_WRITE_WAIT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to write CHARGER_CTL1 register: %d\r\n", -ret);
            return false;
        }

        chargerControl->chargerCtl1RegData = data;
    }

    if (regBits & IP5306_CHARGER_CTL2_BIT) {
        data = chargerControl->chargerCtl2RegData;
        BITOPS_SET_BITS(&data, 2, 2, (uint8_t)chargerControl->batteryVoltage);
        BITOPS_SET_BITS(&data, 0, 2, (uint8_t)chargerControl->constantVoltageCharging);

        ret = platform->i2cWriteReg(IP5306_I2C_ADDR, REG_CHARGER_CTL2_ADDR, &data, 1, I2C_WRITE_WAIT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to write CHARGER_CTL2 register: %d\r\n", -ret);
            return false;
        }

        chargerControl->chargerCtl2RegData = data;
    }

    if (regBits & IP5306_CHARGER_CTL3_BIT) {
        data = chargerControl->chargerCtl3RegData;
        BITOPS_SET_BIT(&data, 5, (uint8_t)chargerControl->chargingCurrentLoop);

        ret = platform->i2cWriteReg(IP5306_I2C_ADDR, REG_CHARGER_CTL3_ADDR, &data, 1, I2C_WRITE_WAIT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to write CHARGER_CTL3 register: %d\r\n", -ret);
            return false;
        }

        chargerControl->chargerCtl3RegData = data;
    }

    if (regBits & IP5306_CHG_DIG_CTL0_BIT) {
        // Calculate polynomial coefficients from current value
        int current = chargerControl->chargingCurrent - 50;
        int b0, b1, b2, b3, b4;

        if (current >= 1600) {
            b4 = 1;
            current -= 1600;
        } else {
            b4 = 0;
        }

        if (current >= 800) {
            b3 = 1;
            current -= 800;
        } else {
            b3 = 0;
        }

        if (current >= 400) {
            b2 = 1;
            current -= 400;
        } else {
            b2 = 0;
        }

        if (current >= 200) {
            b1 = 1;
            current -= 200;
        } else {
            b1 = 0;
        }

        if (current >= 100) {
            b0 = 1;
            current -= 100;
        } else {
            b0 = 0;
        }

        // Write data
        data = chargerControl->chgDigCtl0RegData;
        BITOPS_SET_BIT(&data, 0, b0);
        BITOPS_SET_BIT(&data, 1, b1);
        BITOPS_SET_BIT(&data, 2, b2);
        BITOPS_SET_BIT(&data, 3, b3);
        BITOPS_SET_BIT(&data, 4, b4);

        ret = platform->i2cWriteReg(IP5306_I2C_ADDR, REG_CHG_DIG_CTL0_ADDR, &data, 1, I2C_WRITE_WAIT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to write CHG_DIG_CTL0 register: %d\r\n", -ret);
            return false;
        }

        chargerControl->chgDigCtl0RegData = data;
    }

    return true;
}

bool IP5306_ReadStatus(struct IP5306_Platform *platform, struct IP5306_Status *status, unsigned int regBits) {
    int ret;
    uint8_t data;

    if (regBits & IP5306_READ0_BIT) {
        ret = platform->i2cReadReg(IP5306_I2C_ADDR, REG_READ0_ADDR, &data, 1, I2C_READ_TIMEOUT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to read READ0 register: %d\r\n", -ret);
            return false;
        }

        status->chargingOn = BITOPS_GET_BIT(data, 3);

        status->read0RegData = data;
    }

    if (regBits & IP5306_READ1_BIT) {
        ret = platform->i2cReadReg(IP5306_I2C_ADDR, REG_READ1_ADDR, &data, 1, I2C_READ_TIMEOUT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to read READ1 register: %d\r\n", -ret);
            return false;
        }

        status->fullyCharged = BITOPS_GET_BIT(data, 3);

        status->read1RegData = data;
    }

    if (regBits & IP5306_READ2_BIT) {
        ret = platform->i2cReadReg(IP5306_I2C_ADDR, REG_READ2_ADDR, &data, 1, I2C_READ_TIMEOUT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to read READ2 register: %d\r\n", -ret);
            return false;
        }

        status->lightLoad = BITOPS_GET_BIT(data, 2);

        status->read2RegData = data;
    }

    if (regBits & IP5306_READ3_BIT) {
        ret = platform->i2cReadReg(IP5306_I2C_ADDR, REG_READ3_ADDR, &data, 1, I2C_READ_TIMEOUT_MS);
        if (ret < 0) {
            platform->debugPrint("IP5306: Failed to read READ3 register: %d\r\n", -ret);
            return false;
        }

        status->doubleClick = BITOPS_GET_BIT(data, 2);
        status->longPress = BITOPS_GET_BIT(data, 1);
        status->shortPress = BITOPS_GET_BIT(data, 0);

        status->read3RegData = data;
    }

    return true;
}

bool IP5306_WriteStatus(struct IP5306_Platform *platform, struct IP5306_Status *status) {
    int ret;
    uint8_t data;

    // Prepare data (READ3 register only)
    data = status->read3RegData;
    if (status->doubleClick) {
        BITOPS_SET_BIT(&data, 2, 1); // Clear double click flag
    }

    if (status->longPress) {
        BITOPS_SET_BIT(&data, 1, 1); // Clear long press flag
    }

    if (status->shortPress) {
        BITOPS_SET_BIT(&data, 0, 1); // Clear short press flag
    }

    // Write data (READ3 register only)
    ret = platform->i2cWriteReg(IP5306_I2C_ADDR, REG_READ3_ADDR, &data, 1, I2C_WRITE_WAIT_MS);
    if (ret < 0) {
        platform->debugPrint("IP5306: Failed to write READ3 register: %d\r\n", -ret);
        return false;
    }

    status->read3RegData = data;

    return true;
}
