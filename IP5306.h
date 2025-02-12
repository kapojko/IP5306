#ifndef IP5306_H
#define IP5306_H

#include <stdint.h>
#include <stdbool.h>

#define IP5306_I2C_ADDR (0xea >> 1)

#define IP5306_SYS_CTL0_BIT 00001
#define IP5306_SYS_CTL1_BIT 00002
#define IP5306_SYS_CTL2_BIT 00004
#define IP5306_SYS_CTL_ALL_BITS 00007

#define IP5306_CHARGER_CTL0_BIT 00010
#define IP5306_CHARGER_CTL1_BIT 00020
#define IP5306_CHARGER_CTL2_BIT 00040
#define IP5306_CHARGER_CTL3_BIT 00100
#define IP5306_CHG_DIG_CTL0_BIT 00200
#define IP5306_CHARGER_CTL_ALL_BITS 00370

#define IP5306_READ0_BIT 00400
#define IP5306_READ1_BIT 01000
#define IP5306_READ2_BIT 02000
#define IP5306_READ3_BIT 04000
#define IP5306_READ_ALL_BITS 07400

enum IP5306_DisableBoostControl {
    IP5306_DisableBoostControl_LongPress = 1,
    IP5306_DisableBoostControl_ShortPressTwice = 0
};

enum IP5306_SwitchWLEDControl {
    IP5306_SwitchWLEDControl_ShortPressTwice = 1,
    IP5306_SwitchWLEDControl_LongPress = 0
};

enum IP5306_LightLoadShutdownTime {
    IP5306_LightLoadShutdownTime_64S = 03,
    IP5306_LightLoadShutdownTime_16S = 02,
    IP5306_LightLoadShutdownTime_32S = 01,
    IP5306_LightLoadShutdownTime_8S = 00
};

struct IP5306_SystemControl {
    // SYS_CTL0
    bool boostEnable; // Note: After disable, IP5306 cannot automatically shut down under light load. You need to press the key to send a double-click pulse signal to shut down and enter sleep mode; default 1
    bool chargerEnable; // Note: After fully charging, stop charging without unplugging the input. Enable - disable - enable to restart charging; default 1
    bool autoPowerOn; // Automatic power-on function enabled when load is inserted; default 1
    bool outputNormallyOpen; // BOOST output normally open f unction; default 1
    bool keyShutdownEnable; // Key shutdown enable; default 0
    uint8_t sysCtl0RegData; // Raw register data

    // SYS_CTL1
    enum IP5306_DisableBoostControl disableBoostControl; // Disable boost control signal selection; default 0
    enum IP5306_SwitchWLEDControl switchWLEDControl; // Switch WLED flashlight control signal selection; default 0
    bool shortPressSwitchBoostEnable; // Short press switch boost; default 0
    bool enableBoostAfterVINUnplug; // After VIN is unplugged, should Boost be enabled?; default 1
    bool batlow3V0ShutdownEnable; // Batlow 3.0V low power shutdown enable; default 1
    uint8_t sysCtl1RegData; // Raw register data

    // SYS_CTL2
    enum IP5306_LightLoadShutdownTime lightLoadShutdownTime; // Light load shutdown time setting; default ??
    uint8_t sysCtl2RegData; // Raw register data
};

// NOTE: Corresponding to charging cut-off voltage of 4.2V/4.3V/4.35V/4.4V respectively
// It is recommended to use 01 or 00 gear
enum IP5306_ChargerFullStop {
    IP5306_ChargerFullStop_4V2 = 03, // 4.2/4.305/4.35/4.395
    IP5306_ChargerFullStop_4V185 = 02, // 4.185/4.29/4.335/4.38
    IP5306_ChargerFullStop_4V17 = 01, // 4.17/4.275/4.32/4.365
    IP5306_ChargerFullStop_4V14 = 00 // 4.14/4.26/4.305/4.35
};

// NOTE: IP5306 full charge detection first detects the current and then the battery voltage
enum IP5306_EndCurrentDetection {
    IP5306_EndCurrentDetection_600mA = 03,
    IP5306_EndCurrentDetection_500mA = 02,
    IP5306_EndCurrentDetection_400mA = 01,
    IP5306_EndCurrentDetection_200mA = 00
};

// NOTE: When charging, the IC will detect the output voltage of VOUT.
// Automatically adjust the charging current when the VOUT voltage is greater than the set value
// When the current is less than the set value, it will automatically charge at the maximum current.
// Automatically reduce the charging current to maintain this voltage;
// If the customer requires that the output end be sampled while charging and discharging
// The load current at the output end of the resistor detection charging and discharging state is greater than
// When the current is 100mA, the undervoltage loop can be set to the highest level, giving priority to external loads.
// Load charging
enum IP5306_ChargingUndervoltageLoop {
    IP5306_ChargingUndervoltageLoop_4V8 = 07,
    IP5306_ChargingUndervoltageLoop_4V75 = 06,
    IP5306_ChargingUndervoltageLoop_4V7 = 05,
    IP5306_ChargingUndervoltageLoop_4V65 = 04,
    IP5306_ChargingUndervoltageLoop_4V6 = 03,
    IP5306_ChargingUndervoltageLoop_4V55 = 02,
    IP5306_ChargingUndervoltageLoop_4V5 = 01,
    IP5306_ChargingUndervoltageLoop_4V45 = 00
};

enum IP5306_BatteryVoltage {
    IP5306_BatteryVoltage_4V4 = 03,
    IP5306_BatteryVoltage_4V35 = 02,
    IP5306_BatteryVoltage_4V3 = 01,
    IP5306_BatteryVoltage_4V2 = 00
};

enum IP5306_ConstantVoltageCharging {
    IP5306_ConstantVoltageCharging_Press42mV = 03,
    IP5306_ConstantVoltageCharging_Press28mV = 02,
    IP5306_ConstantVoltageCharging_Press14mV = 01,
    IP5306_ConstantVoltageCharging_NoPressure = 00
};

enum IP5306_ChargingCurrentLoop {
    IP5306_ChargingCurrentLoop_VIN_CC = 1,
    IP5306_ChargingCurrentLoop_BAT_CC = 0
};

// Note: 4.30V/4.35V/4.4V recommends increasing voltage by 14mV;
// 4.2V recommends adding 28mV;
struct IP5306_ChargerControl {
    // CHARGER_CTL0
    enum IP5306_ChargerFullStop chargerFullStop; // Charging full stop setting; default 02
    uint8_t chargerCtl0RegData; // Raw register data

    // CHARGER_CTL1
    enum IP5306_EndCurrentDetection endCurrentDetection; // Battery end charging current detection; default 01
    enum IP5306_ChargingUndervoltageLoop chargingUndervoltageLoop; // Charging undervoltage loop setting (the voltage at the output t erminal VOUT during charging); default 05
    uint8_t chargerCtl1RegData; // Raw register data

    // CHARGER_CTL2
    enum IP5306_BatteryVoltage batteryVoltage; // Battery voltage setting; default 00
    enum IP5306_ConstantVoltageCharging constantVoltageCharging; // Constant voltage charging voltage boost setting; default 01
    uint8_t chargerCtl2RegData; // Raw register data

    // CHARGER_CTL3
    enum IP5306_ChargingCurrentLoop chargingCurrentLoop; // Charging constant current loop selection; default 1
    uint8_t chargerCtl3RegData; // Raw register data

    // CHG_DIG_CTL0
    int chargingCurrent; // Charger (VIN end) current setting (mA); default ??
    uint8_t chgDigCtl0RegData; // Raw register data
};

struct IP5306_Status {
    // READ0
    bool chargingOn; // Charging enable flag
    uint8_t read0RegData; // Raw register data

    // READ1
    bool fullyCharged; // Full flag
    uint8_t read1RegData; // Raw register data

    // READ2
    bool lightLoad; // Output light load flag (Heavy load/Light load)
    uint8_t read2RegData; // Raw register data

    // READ3
    bool doubleClick; // KEY button double click symbol, write 1 to clear
    bool longPress; // KEY button long press symbol, write 1 to clear
    bool shortPress; // KEY button short press symbol, write 1 to clear
    uint8_t read3RegData; // Raw register data
};

struct IP5306_Platform {
    int (*i2cWriteReg)(uint8_t addr7bit, uint8_t regNum, const uint8_t *data, uint8_t length, uint8_t wait);
    int (*i2cReadReg)(uint8_t addr7bit, uint8_t regNum, uint8_t *data, uint8_t length, int timeout);

    void (*debugPrint)(const char *fmt, ...);
};

void IP5306_Init(struct IP5306_Platform *platform);

bool IP5306_ReadSystemControl(struct IP5306_SystemControl *systemControl, unsigned int regBits);
bool IP5306_WriteSystemControl(struct IP5306_SystemControl *systemControl, unsigned int regBits);

bool IP5306_ReadChargerControl(struct IP5306_ChargerControl *chargerControl, unsigned int regBits);
bool IP5306_WriteChargerControl(struct IP5306_ChargerControl *chargerControl, unsigned int regBits);

bool IP5306_ReadStatus(struct IP5306_Status *status, unsigned int regBits);
bool IP5306_WriteStatus(struct IP5306_Status *status);


#endif // IP5306_H
