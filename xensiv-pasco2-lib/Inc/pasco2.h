/*
 * pasco2.h
 *
 * Driver interface for Infineon XENSIV PAS CO2 sensor.
 *
 *  Created on: Nov 10, 2024
 *      Author: GeeberuBasav
 */

#ifndef INC_PASCO2_H_
#define INC_PASCO2_H_


#include "xensiv_pasco2_regs.h"
#include "stm32f4xx_hal.h"

typedef struct{
	I2C_HandleTypeDef * I2CHandle;
	uint16_t co2ppm;
	uint8_t addr;
}PASCO2;
/** I2C address of the XENSIV™ PASCO2 sensor */
#define XENSIV_PASCO2_DEV_ADDR 0x28

#define XENSIV_PASCO2_COMM_TEST_VAL             (0xA5U)

#define XENSIV_PASCO2_SOFT_RESET_DELAY_MS       (2000U)

#define XENSIV_PASCO2_REG_PROD_ID_VAL			      (0x4FU)

/** Minimum allowed measurement rate */
#define XENSIV_PASCO2_MEAS_RATE_MIN             (5U)

/** Maximum allowed measurement rate */
#define XENSIV_PASCO2_MEAS_RATE_MAX             (4095U)

#define XENSIV_PASCO2_MEAS_RATE_FCS 10




/********************************* Type definitions **************************************/

/** Enum defining the different device commands */
typedef enum
{
    XENSIV_PASCO2_CMD_SOFT_RESET = 0xA3U,               /**< Soft reset the sensor */
    XENSIV_PASCO2_CMD_RESET_ABOC = 0xBCU,               /**< Resets the ABOC context */
    XENSIV_PASCO2_CMD_SAVE_FCS_CALIB_OFFSET = 0xCFU,    /**< Saves the force calibration offset into the non volatile memory */
    XENSIV_PASCO2_CMD_RESET_FCS = 0xFCU,                /**< Resets the forced calibration correction factor */
} xensiv_pasco2_cmd_t;

/** Enum defining the different device operating modes */
typedef enum
{
    XENSIV_PASCO2_OP_MODE_IDLE = 0U,                    /**< The device does not perform any CO2 concentration measurement */
    XENSIV_PASCO2_OP_MODE_SINGLE = 1U,                  /**< The device triggers a single measurement sequence. At the end of the measurement sequence, the device automatically goes back to idle mode. */
    XENSIV_PASCO2_OP_MODE_CONTINUOUS = 2U               /**< The device periodically triggers a CO2 concentration measurement sequence.
                                                             Once a measurement sequence is completed, the device goes back to an inactive state and wakes
                                                             up automatically for the next measurement sequence. The measurement period can be programmed from 5 seconds to 4095 seconds. */
} xensiv_pasco2_op_mode_t;

/** Enum defining the different device baseline offset compensation (BOC) modes */
typedef enum
{
    XENSIV_PASCO2_BOC_CFG_DISABLE = 0U,                 /**< No offset compensation occurs */
    XENSIV_PASCO2_BOC_CFG_AUTOMATIC = 1U,               /**< The offset is periodically updated at each BOC computation */
    XENSIV_PASCO2_BOC_CFG_FORCED = 2U                   /**< Forced compensation */
} xensiv_pasco2_boc_cfg_t;

/** Enum defining the PWM mode configuration */
typedef enum
{
    XENSIV_PASCO2_PWM_MODE_SINGLE_PULSE = 0U,           /**< PWM single-pulse */
    XENSIV_PASCO2_PWM_MODE_TRAIN_PULSE = 1U             /**< PWM pulse-train mode */
} xensiv_pasco2_pwm_mode_t;

/** Enum defining different interrupt active levels */
typedef enum
{
    XENSIV_PASCO2_INTERRUPT_TYPE_LOW_ACTIVE = 0U,       /**< Pin INT is configured as push-pull and is active LOW */
    XENSIV_PASCO2_INTERRUPT_TYPE_HIGH_ACTIVE = 1U       /**< Pin INT is configured as push-pull and is active HIGH */
} xensiv_pasco2_interrupt_type_t;

/** Enum defining different pin interrupt functions */
typedef enum
{
    XENSIV_PASCO2_INTERRUPT_FUNCTION_NONE = 0U,         /**< Pin INT is inactive */
    XENSIV_PASCO2_INTERRUPT_FUNCTION_ALARM = 1U,        /**< Pin INT is configured as the alarm threshold violation notification pin */
    XENSIV_PASCO2_INTERRUPT_FUNCTION_DRDY = 2U,         /**< Pin INT is configured as the data ready notification pin */
    XENSIV_PASCO2_INTERRUPT_FUNCTION_BUSY = 3U,         /**< Pin INT is configured as the sensor busy notification pin */
    XENSIV_PASCO2_INTERRUPT_FUNCTION_EARLY = 4U         /**< Pin INT is configured as the early measurement start notification pin
                                                             @note This function is available only in continuous mode */
} xensiv_pasco2_interrupt_function_t;

/** Enum defining whether an alarm is issued in the case of a lower or higher threshold violation */
typedef enum
{
    XENSIV_PASCO2_ALARM_TYPE_HIGH_TO_LOW = 0U,          /**< CO2 ppm value falling below the alarm threshold */
    XENSIV_PASCO2_ALARM_TYPE_LOW_TO_HIGH = 1U           /**< CO2 ppm value rising above the alarm threshold */
} xensiv_pasco2_alarm_type_t;

/** Structure of the sensor's product and revision ID register (PROD_ID) */
typedef union
{
  struct
  {
    uint32_t rev:5;                                     /*!< Product and firmware revision */
    uint32_t prod:3;                                    /*!< Product type */
  } b;                                                  /*!< Structure used for bit  access */
  uint8_t u;                                            /*!< Type used for byte access */
} xensiv_pasco2_id_t;

/** Structure of the sensor's status register (SENS_STS) */
typedef union
{
  struct
  {
    uint32_t :3;
    uint32_t iccerr:1;                                  /*!< Communication error notification bit.
                                                             Indicates whether an invalid command has been received by the serial communication interface*/
    uint32_t orvs:1;                                    /*!< Out-of-range VDD12V error bit */
    uint32_t ortmp:1;                                   /*!< Out-of-range temperature error bit */
    uint32_t pwm_dis_st:1;                              /*!< PWM_DIS pin status */
    uint32_t sen_rdy:1;                                 /*!< Sensor ready bit */
  } b;                                                  /*!< Structure used for bit  access */
  uint8_t u;                                            /*!< Type used for byte access */
} xensiv_pasco2_status_t;

/** Structure of the sensor's measurement configuration register (MEAS_CFG) */
typedef union
{
  struct
  {
    uint32_t op_mode:2;                                 /*!< @ref xensiv_pasco2_op_mode_t */
    uint32_t boc_cfg:2;                                 /*!< @ref xensiv_pasco2_boc_cfg_t */
    uint32_t pwm_mode:1;                                /*!< @ref xensiv_pasco2_pwm_mode_t */
    uint32_t pwm_outen:1;                               /*!< PWM output software enable bit */
    uint32_t :2;
  } b;                                                  /*!< Structure used for bit  access */
  uint8_t u;                                            /*!< Type used for byte access */
} xensiv_pasco2_measurement_config_t;

/** Structure of the sensor's interrupt configuration register (INT_CFG) */
typedef union
{
  struct
  {
    uint32_t alarm_typ:1;                               /*!< @ref xensiv_pasco2_alarm_type_t */
    uint32_t int_func:3;                                /*!< @ref xensiv_pasco2_interrupt_function_t */
    uint32_t int_typ:1;                                 /*!< @ref xensiv_pasco2_interrupt_type_t */
    uint32_t :3;
  } b;                                                  /*!< Structure used for bit access */
  uint8_t u;                                            /*!< Type used for byte access */
} xensiv_pasco2_interrupt_config_t;

/** Structure of the sensor's measurement status register (MEAS_STS) */
typedef union
{
  struct
  {
    uint32_t :2;
    uint32_t alarm:1;                                   /*!< Set at the end of every measurement sequence if a threshold violation occurs */
    uint32_t int_sts:1;                                 /*!< Indicates whether the INT pin has been latched to active state (if alarm or data is ready) */
    uint32_t drdy:1;                                    /*!< Indicates whether new data is available */
    uint32_t :3;
  } b;                                                  /*!< Structure used for bit  access */
  uint8_t u;                                            /*!< Type used for byte access */
} xensiv_pasco2_meas_status_t;


/** Initialize PASCO2 device context for I2C communication. */
HAL_StatusTypeDef PASCO2_I2C_Init(PASCO2 *dev,I2C_HandleTypeDef *i2cHandler);

/** Check sensor communication and read product ID register. */
HAL_StatusTypeDef  PASCO2_Available(const PASCO2 *dev,uint8_t *data);

/** Read CO2 concentration (ppm) if data-ready is set. */
HAL_StatusTypeDef PASCO2_ppm(PASCO2 *dev);
/** Read CO2 concentration (ppm) using a 2-byte register read. */
HAL_StatusTypeDef PASCO2_ppm2(PASCO2 *dev);

/** Read one 8-bit register from sensor. */
HAL_StatusTypeDef PASCO2_Get_Reg(const PASCO2 *dev,uint8_t reg,uint8_t *data);

/** Write one 8-bit register to sensor. */
HAL_StatusTypeDef PASCO2_Set_Reg(const PASCO2 *dev,uint8_t reg,uint8_t *data);

/** Read two consecutive 8-bit registers from sensor. */
HAL_StatusTypeDef PASCO2_Get_Regs(const PASCO2 *dev,uint8_t reg,uint8_t *data);

/** Write two consecutive 8-bit registers to sensor. */
HAL_StatusTypeDef PASCO2_Set_Regs(const PASCO2 *dev,uint8_t reg,uint8_t *data);

/** Write a reset/control command into SENS_RST register. */
HAL_StatusTypeDef PASCO2_Reset(const PASCO2 *dev,xensiv_pasco2_cmd_t cmd);

/** Perform communication test and basic sensor readiness checks. */
HAL_StatusTypeDef PASCO2_Sensor_init(const PASCO2 *dev);

/** Read measurement configuration register. */
HAL_StatusTypeDef PASCO2_Get_Measurement_Config(const PASCO2 *dev, xensiv_pasco2_measurement_config_t *meas_config);

/** Write measurement configuration register. */
HAL_StatusTypeDef PASCO2_Set_Measurement_Config(const PASCO2 *dev, xensiv_pasco2_measurement_config_t *meas_config);

/** Set pressure reference value used for compensation. */
HAL_StatusTypeDef PASCO2_Set_Pressure_ref(const PASCO2 *dev,uint16_t *val);

/** Get pressure reference value used for compensation. */
HAL_StatusTypeDef PASCO2_Get_Pressure_ref(const PASCO2 *dev,uint16_t *val);

/** Read measurement status register. */
HAL_StatusTypeDef PASCO2_Get_MeasuremetnStatus(const PASCO2 *dev,uint8_t *val);

/** Write measurement status register (clear/latch bits). */
HAL_StatusTypeDef PASCO2_Set_MeasuremetnStatus(const PASCO2 *dev,uint8_t *val);

/** Read configured measurement period in seconds. */
HAL_StatusTypeDef PASCO2_Get_Measurement_Rate(const PASCO2 *dev, uint16_t *meas_rate);

/** Set configured measurement period in seconds. */
HAL_StatusTypeDef PASCO2_Set_Measurement_Rate(const PASCO2 *dev, uint16_t meas_rate);

/** Start continuous measurement mode with requested period. */
HAL_StatusTypeDef PASCO2_Start_ContinousMode(const PASCO2 *dev,uint16_t meas_rate);

/** Trigger one single-shot CO2 measurement sequence. */
HAL_StatusTypeDef PASCO2_Start_SingleShotMode(const PASCO2 *dev);

/** Stop measurements by forcing idle operating mode. */
HAL_StatusTypeDef PASCO2_StopMeasure(const PASCO2 *dev);

/** Clear forced-compensation context in sensor. */
HAL_StatusTypeDef PASCO2_ClearForecedCompensation(const PASCO2 *dev);

/** Execute forced compensation using known reference CO2 value. */
HAL_StatusTypeDef PASCO2_PerformForecedCompensation(const PASCO2 *dev,uint16_t val);

/** Send generic command to SENS_RST register. */
HAL_StatusTypeDef PASCO2_cmd(const PASCO2 * dev, xensiv_pasco2_cmd_t cmd);

/** Convert endianess for 16-bit register values. */
uint16_t reverse_ByteOrder(uint16_t val);

/** Set calibration reference value for forced compensation path. */
HAL_StatusTypeDef PASCO2_Set_Calibration_ref(const PASCO2 *dev,uint16_t val);
#endif /* INC_PASCO2_H_ */
