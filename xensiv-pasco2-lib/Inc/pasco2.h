/*
 * pasco2.h
 *
 *  Created on: Nov 10, 2024
 *      Author: GeeberuBasav
 */

#ifndef INC_PASCO2_H_
#define INC_PASCO2_H_

/**
 * @file pasco2.h
 * @brief Infineon XENSIV PASCO2 CO2 sensor driver interface.
 *
 * This header defines the PASCO2 device context and the sensor API for use
 * with an STM32 HAL-based I2C bus. The driver supports device commands,
 * measurement configuration, interrupt configuration, and read/write access
 * to the sensor registers.
 */

#include "xensiv_pasco2_regs.h"
#include "stm32f4xx_hal.h"

/**
 * @brief XENSIV PASCO2 device context.
 *
 * The application must provide a valid I2C handle and may use the default
 * 7-bit I2C device address defined by XENSIV_PASCO2_DEV_ADDR.
 */
typedef struct
{
    I2C_HandleTypeDef *I2CHandle; /**< HAL I2C handle used for sensor communication */
    uint8_t addr;                 /**< 7-bit I2C device address */
} PASCO2;

/** I2C address of the XENSIV™ PASCO2 sensor */
#define XENSIV_PASCO2_DEV_ADDR 0x28

/** Communication test value returned by the sensor during a device check. */
#define XENSIV_PASCO2_COMM_TEST_VAL             (0xA5U)

/** Delay after issuing a soft reset command, in milliseconds. */
#define XENSIV_PASCO2_SOFT_RESET_DELAY_MS       (2000U)

/** Expected product ID value for the XENSIV PASCO2 sensor. */
#define XENSIV_PASCO2_REG_PROD_ID_VAL           (0x4FU)

/** Minimum allowed measurement rate in seconds for continuous mode. */
#define XENSIV_PASCO2_MEAS_RATE_MIN             (5U)

/** Maximum allowed measurement rate in seconds for continuous mode. */
#define XENSIV_PASCO2_MEAS_RATE_MAX             (4095U)




/********************************* Type definitions **************************************/

/**
 * @brief Device commands supported by the XENSIV PASCO2 sensor.
 *
 * Commands are written to the sensor command register to perform
 * operations such as reset, forced calibration, and offset context reset.
 */
typedef enum
{
    XENSIV_PASCO2_CMD_SOFT_RESET = 0xA3U,               /**< Soft reset the sensor */
    XENSIV_PASCO2_CMD_RESET_ABOC = 0xBCU,               /**< Resets the ABOC context */
    XENSIV_PASCO2_CMD_SAVE_FCS_CALIB_OFFSET = 0xCFU,    /**< Saves the force calibration offset into the non volatile memory */
    XENSIV_PASCO2_CMD_RESET_FCS = 0xFCU,                /**< Resets the forced calibration correction factor */
} xensiv_pasco2_cmd_t;

/**
 * @brief PASCO2 device operating modes.
 *
 * - IDLE: sensor remains inactive.
 * - SINGLE: one-shot measurement followed by return to IDLE.
 * - CONTINUOUS: periodic measurements with programmable interval.
 */
typedef enum
{
    XENSIV_PASCO2_OP_MODE_IDLE = 0U,                    /**< The device does not perform any CO2 concentration measurement */
    XENSIV_PASCO2_OP_MODE_SINGLE = 1U,                  /**< The device triggers a single measurement sequence. At the end of the measurement sequence, the device automatically goes back to idle mode. */
    XENSIV_PASCO2_OP_MODE_CONTINUOUS = 2U               /**< The device periodically triggers a CO2 concentration measurement sequence.
                                                             Once a measurement sequence is completed, the device goes back to an inactive state and wakes
                                                             up automatically for the next measurement sequence. The measurement period can be programmed from 5 seconds to 4095 seconds. */
} xensiv_pasco2_op_mode_t;

/**
 * @brief Baseline offset compensation (BOC) modes for the PASCO2 sensor.
 *
 * BOC can be disabled, performed automatically, or forced by command.
 */
typedef enum
{
    XENSIV_PASCO2_BOC_CFG_DISABLE = 0U,                 /**< No offset compensation occurs */
    XENSIV_PASCO2_BOC_CFG_AUTOMATIC = 1U,               /**< The offset is periodically updated at each BOC computation */
    XENSIV_PASCO2_BOC_CFG_FORCED = 2U                   /**< Forced compensation */
} xensiv_pasco2_boc_cfg_t;

/**
 * @brief PWM output mode selection.
 *
 * The PASCO2 sensor can generate either a single PWM pulse or a pulse train
 * on the PWM output pin when PWM output is enabled.
 */
typedef enum
{
    XENSIV_PASCO2_PWM_MODE_SINGLE_PULSE = 0U,           /**< PWM single-pulse */
    XENSIV_PASCO2_PWM_MODE_TRAIN_PULSE = 1U             /**< PWM pulse-train mode */
} xensiv_pasco2_pwm_mode_t;

/**
 * @brief Interrupt active-level configuration for the PASCO2 INT pin.
 *
 * Select whether INT is active-high or active-low when used as the sensor
 * interrupt output.
 */
typedef enum
{
    XENSIV_PASCO2_INTERRUPT_TYPE_LOW_ACTIVE = 0U,       /**< Pin INT is configured as push-pull and is active LOW */
    XENSIV_PASCO2_INTERRUPT_TYPE_HIGH_ACTIVE = 1U       /**< Pin INT is configured as push-pull and is active HIGH */
} xensiv_pasco2_interrupt_type_t;

/**
 * @brief Interrupt function selection for the PASCO2 INT pin.
 *
 * The INT pin can signal no event, alarm threshold violations, data-ready,
 * busy state, or early measurement start notification in continuous mode.
 */
typedef enum
{
    XENSIV_PASCO2_INTERRUPT_FUNCTION_NONE = 0U,         /**< Pin INT is inactive */
    XENSIV_PASCO2_INTERRUPT_FUNCTION_ALARM = 1U,        /**< Pin INT is configured as the alarm threshold violation notification pin */
    XENSIV_PASCO2_INTERRUPT_FUNCTION_DRDY = 2U,         /**< Pin INT is configured as the data ready notification pin */
    XENSIV_PASCO2_INTERRUPT_FUNCTION_BUSY = 3U,         /**< Pin INT is configured as the sensor busy notification pin */
    XENSIV_PASCO2_INTERRUPT_FUNCTION_EARLY = 4U         /**< Pin INT is configured as the early measurement start notification pin
                                                             @note This function is available only in continuous mode */
} xensiv_pasco2_interrupt_function_t;

/**
 * @brief Alarm direction used for INT pin threshold violation reporting.
 *
 * - HIGH_TO_LOW: indicates a falling CO2 concentration event.
 * - LOW_TO_HIGH: indicates a rising CO2 concentration event.
 */
typedef enum
{
    XENSIV_PASCO2_ALARM_TYPE_HIGH_TO_LOW = 0U,          /**< CO2 ppm value falling below the alarm threshold */
    XENSIV_PASCO2_ALARM_TYPE_LOW_TO_HIGH = 1U           /**< CO2 ppm value rising above the alarm threshold */
} xensiv_pasco2_alarm_type_t;

/**
 * @brief Product and firmware revision information returned by the PROD_ID register.
 *
 * The sensor reports an 8-bit identification value that includes product and
 * revision fields.
 */
typedef union
{
  struct
  {
    uint32_t rev:5;                                     /*!< Product and firmware revision */
    uint32_t prod:3;                                    /*!< Product type */
  } b;                                                  /*!< Structure used for bit  access */
  uint8_t u;                                            /*!< Type used for byte access */
} xensiv_pasco2_id_t;

/**
 * @brief Status register representation for the PASCO2 sensor.
 *
 * The status register provides error flags, power and temperature range status,
 * PWM output status, and sensor readiness information.
 */
typedef union
{
  struct
  {
    uint32_t :3;
    uint32_t iccerr:1;                                  /*!< Communication error notification bit.
                                                             Indicates whether an invalid command has been received by the serial communication interface */
    uint32_t orvs:1;                                    /*!< Out-of-range VDD12V error bit */
    uint32_t ortmp:1;                                   /*!< Out-of-range temperature error bit */
    uint32_t pwm_dis_st:1;                              /*!< PWM_DIS pin status */
    uint32_t sen_rdy:1;                                 /*!< Sensor ready bit */
  } b;                                                  /*!< Structure used for bit  access */
  uint8_t u;                                            /*!< Type used for byte access */
} xensiv_pasco2_status_t;

/**
 * @brief Measurement configuration register fields for the PASCO2 sensor.
 *
 * This register controls operating mode, baseline offset compensation,
 * PWM output mode, and PWM enable state.
 */
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

/**
 * @brief Interrupt configuration register fields for the PASCO2 sensor.
 *
 * This register configures threshold alarm behavior, INT pin function, and
 * active level polarity.
 */
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

/**
 * @brief Measurement status register fields for the PASCO2 sensor.
 *
 * The MEAS_STS register indicates whether measurements are ready, if an alarm
 * threshold was violated, and whether the INT pin is currently asserted.
 */
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

/**
 * @brief GPIO configuration structure for the PASCO2 INT pin.
 *
 * This structure is used to configure the external GPIO pin and interrupt
 * settings that receive the sensor's INT output.
 */
typedef struct
{
    GPIO_TypeDef *portName;       /**< GPIO port for the INT pin */
    GPIO_InitTypeDef *gpioInit;    /**< Pointer to HAL GPIO initialization data */
    uint8_t IRQ_Num;              /**< External interrupt number for the INT pin */
    uint8_t IRQ_Priority;         /**< Interrupt priority for the INT pin */
} PASCO2_INT_PIN_GPIO_cfg_t;

/**
 * @brief Initialize the PASCO2 device context for I2C communication.
 *
 * @param dev Sensor device context to initialize.
 * @param i2cHandler Pointer to the HAL I2C handle used by the sensor.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_I2C_Init(PASCO2 *dev, I2C_HandleTypeDef *i2cHandler);

/**
 * @brief Check if data is available from the PASCO2 sensor.
 *
 * @param dev Sensor device context.
 * @param data Pointer to a byte where availability status is stored.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Available(const PASCO2 *dev, uint8_t *data);

/**
 * @brief Read the latest CO2 concentration level from the sensor.
 *
 * @param dev Sensor device context.
 * @param value Pointer to a 16-bit word where CO2 ppm is returned.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Get_CO2_Levels(PASCO2 *dev, uint16_t *value);

/**
 * @brief Read a single register from the PASCO2 sensor.
 *
 * @param dev Sensor device context.
 * @param reg Register address to read.
 * @param data Pointer to the target byte for the register value.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Get_Reg(const PASCO2 *dev, uint8_t reg, uint8_t *data);

/**
 * @brief Write a single register on the PASCO2 sensor.
 *
 * @param dev Sensor device context.
 * @param reg Register address to write.
 * @param data Pointer to the byte containing the new register value.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Set_Reg(const PASCO2 *dev, uint8_t reg, uint8_t *data);

/**
 * @brief Read multiple bytes starting at the specified register.
 *
 * @param dev Sensor device context.
 * @param reg Starting register address.
 * @param data Pointer to the buffer to receive the register data.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Get_Regs(const PASCO2 *dev, uint8_t reg, uint8_t *data);

/**
 * @brief Write multiple bytes starting at the specified register.
 *
 * @param dev Sensor device context.
 * @param reg Starting register address.
 * @param data Pointer to the buffer containing data to write.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Set_Regs(const PASCO2 *dev, uint8_t reg, uint8_t *data);

/**
 * @brief Issue a device command to the PASCO2 sensor.
 *
 * @param dev Sensor device context.
 * @param cmd Command code from @ref xensiv_pasco2_cmd_t.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Reset(const PASCO2 *dev, xensiv_pasco2_cmd_t cmd);

/**
 * @brief Perform basic sensor initialization.
 *
 * @param dev Sensor device context.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Sensor_init(const PASCO2 *dev);

/**
 * @brief Read the measurement configuration register.
 *
 * @param dev Sensor device context.
 * @param meas_config Pointer to the configuration struct to populate.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Get_Measurement_Config(const PASCO2 *dev, xensiv_pasco2_measurement_config_t *meas_config);

/**
 * @brief Configure the measurement mode of the sensor.
 *
 * @param dev Sensor device context.
 * @param meas_config Pointer to the configuration struct to write.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Set_Measurement_Config(const PASCO2 *dev, xensiv_pasco2_measurement_config_t *meas_config);

/**
 * @brief Write the reference pressure value used by the sensor.
 *
 * @param dev Sensor device context.
 * @param val Pointer to the 16-bit pressure reference value.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Set_Pressure_ref(const PASCO2 *dev, uint16_t *val);

/**
 * @brief Read the sensor's reference pressure value.
 *
 * @param dev Sensor device context.
 * @param val Pointer to the 16-bit pressure value to receive.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Get_Pressure_ref(const PASCO2 *dev, uint16_t *val);

/**
 * @brief Read measurement status bits from the sensor.
 *
 * @param dev Sensor device context.
 * @param val Pointer to the status structure to populate.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Get_MeasurementStatus(const PASCO2 *dev, xensiv_pasco2_meas_status_t *val);

/**
 * @brief Write measurement status bits to the sensor.
 *
 * @param dev Sensor device context.
 * @param val Pointer to the status structure to write.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Set_MeasurementStatus(const PASCO2 *dev, xensiv_pasco2_meas_status_t *val);

/**
 * @brief Read the configured continuous measurement rate.
 *
 * @param dev Sensor device context.
 * @param meas_rate Pointer to the returned measurement interval in seconds.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Get_Measurement_Rate(const PASCO2 *dev, uint16_t *meas_rate);

/**
 * @brief Configure the continuous measurement interval.
 *
 * @param dev Sensor device context.
 * @param meas_rate Measurement interval in seconds (5 to 4095).
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Set_Measurement_Rate(const PASCO2 *dev, uint16_t meas_rate);

/**
 * @brief Start the sensor in continuous measurement mode.
 *
 * @param dev Sensor device context.
 * @param meas_rate Measurement interval in seconds (5 to 4095).
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Start_ContinousMode(const PASCO2 *dev, uint16_t meas_rate);

/**
 * @brief Start a single-shot CO2 measurement.
 *
 * @param dev Sensor device context.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_Start_SingleShotMode(const PASCO2 *dev);

/**
 * @brief Stop the ongoing measurement sequence.
 *
 * @param dev Sensor device context.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_StopMeasure(const PASCO2 *dev);

/**
 * @brief Clear forced compensation state on the sensor.
 *
 * @param dev Sensor device context.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_ClearForecedCompensation(const PASCO2 *dev);

/**
 * @brief Perform a forced compensation procedure.
 *
 * @param dev Sensor device context.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_PerformForecedCompensation(const PASCO2 *dev);

/**
 * @brief Reverse the byte order of a 16-bit value.
 *
 * @param val 16-bit value to swap.
 * @return Byte-swapped 16-bit value.
 */
uint16_t reverse_ByteOrder(uint16_t val);

/**
 * @brief Configure the INT pin GPIO and interrupt settings.
 *
 * @param dev Sensor device context.
 * @param gpio_int_cfg_s Pointer to the INT pin GPIO configuration.
 * @param pasco2_int_cfg_s Pointer to the sensor interrupt function configuration.
 * @return HAL status code, HAL_OK on success.
 */
HAL_StatusTypeDef PASCO2_int_cfg(const PASCO2 *dev, const PASCO2_INT_PIN_GPIO_cfg_t *gpio_int_cfg_s, const xensiv_pasco2_interrupt_config_t *pasco2_int_cfg_s);

#endif /* INC_PASCO2_H_ */
