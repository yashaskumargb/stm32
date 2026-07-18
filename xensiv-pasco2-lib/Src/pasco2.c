/*
 * pasco2.c
 *
 * I2C driver implementation for Infineon XENSIV PAS CO2 sensor.
 *
 *  Created on: Nov 10, 2024
 *      Author: GeeberuBasav
 */

#include "pasco2.h"

HAL_StatusTypeDef PASCO2_I2C_Init(PASCO2 *dev,I2C_HandleTypeDef *i2cHandler)
{
	if(dev==NULL || i2cHandler==NULL)
		return HAL_ERROR;
	dev->commPeripheral=(void *)i2cHandler;
	dev->addr=(uint8_t)XENSIV_PASCO2_DEV_ADDR;
	dev->co2ppm=0;
	return HAL_OK;
}
HAL_StatusTypeDef PASCO2_Get_Reg(const PASCO2 *dev,uint8_t reg,uint8_t *data)
{
	if(dev==NULL||data==NULL)
		return HAL_ERROR;
	else
		return HAL_I2C_Mem_Read(dev->commPeripheral,
								(uint8_t)((dev->addr)<<1),
								(uint8_t)reg,
								I2C_MEMADD_SIZE_8BIT,
								data,
								1,
								HAL_MAX_DELAY);
}

HAL_StatusTypeDef PASCO2_Set_Reg(const PASCO2 *dev,uint8_t reg,uint8_t *data)
{
	if(dev==NULL||data==NULL)
			return HAL_ERROR;
	else
		return HAL_I2C_Mem_Write(dev->commPeripheral,
								(uint8_t)(dev->addr<<1),
								(uint8_t)reg,
								I2C_MEMADD_SIZE_8BIT,
								data,
								1,
								HAL_MAX_DELAY);
}

HAL_StatusTypeDef PASCO2_Get_Regs(const PASCO2 *dev,uint8_t reg,uint8_t *data)
{
	if(dev==NULL||data==NULL)
		return HAL_ERROR;
	else
		return HAL_I2C_Mem_Read(dev->commPeripheral,
								(uint8_t)(dev->addr<<1),
								(uint8_t)reg,
								I2C_MEMADD_SIZE_8BIT,
								data,
								2,
								HAL_MAX_DELAY);
}

HAL_StatusTypeDef PASCO2_Set_Regs(const PASCO2 *dev,uint8_t reg,uint8_t *data)
{
	if(dev==NULL||data==NULL)
			return HAL_ERROR;
	else
		return HAL_I2C_Mem_Write(dev->commPeripheral,
								(uint8_t)(dev->addr<<1),
								(uint8_t)reg,
								I2C_MEMADD_SIZE_8BIT,
								data,
								2,
								HAL_MAX_DELAY);
}
HAL_StatusTypeDef PASCO2_Available(const PASCO2 *dev,uint8_t *data)
{
	if(dev==NULL||data==NULL)
		return HAL_ERROR;
	else
	{
		if( HAL_I2C_IsDeviceReady(dev->commPeripheral, (uint16_t)(dev->addr<<1), 3, 5)==HAL_OK)
		{
			return PASCO2_Get_Reg(dev, XENSIV_PASCO2_REG_PROD_ID, data);
		}
		else
			return HAL_ERROR;
	}

}
HAL_StatusTypeDef PASCO2_ppm(PASCO2 *dev)
{
	if(dev==NULL)
		return HAL_ERROR;
	else
	{
		xensiv_pasco2_meas_status_t meas_sts;
		if(PASCO2_Get_Reg(dev, (uint8_t)XENSIV_PASCO2_REG_MEAS_STS,&(meas_sts.u))!=HAL_OK)
			return HAL_ERROR;;
		if((meas_sts.u & XENSIV_PASCO2_REG_MEAS_STS_DRDY_MSK)!=0x00)
		{
			uint8_t temp_data;
			if(PASCO2_Get_Reg(dev, (uint8_t)XENSIV_PASCO2_REG_CO2PPM_H,&temp_data)!=HAL_OK)
				return HAL_ERROR;
			else
				dev->co2ppm=temp_data;
			if(PASCO2_Get_Reg(dev, (uint8_t)XENSIV_PASCO2_REG_CO2PPM_L,&temp_data)!=HAL_OK)
				return HAL_ERROR;
			else
			{
				dev->co2ppm=(dev->co2ppm<<8)|temp_data;
				return HAL_OK;
			}


		}
		else
			return HAL_ERROR;

	}
}
HAL_StatusTypeDef PASCO2_ppm2(PASCO2 *dev)
{
	uint16_t co2_data=0;
	xensiv_pasco2_meas_status_t meas_sts;
	if(PASCO2_Get_MeasuremetnStatus(dev, (uint8_t*)&meas_sts)!=HAL_OK)
		return HAL_ERROR;
	if(meas_sts.b.drdy==1)
	{
		if(PASCO2_Get_Regs(dev, (uint8_t)XENSIV_PASCO2_REG_CO2PPM_H, (uint8_t *)&co2_data)!=HAL_OK)
				return HAL_ERROR;
			else
			{
				//dev->co2ppm=((co2_data >> 8)&0xFF)|((co2_data & 0xFF) << 8);
				dev->co2ppm=reverse_ByteOrder(co2_data);
				return HAL_OK;
			}
	}
	else
		return HAL_ERROR;

}

HAL_StatusTypeDef PASCO2_Reset(const PASCO2 *dev,xensiv_pasco2_cmd_t cmd)
{
	if(dev==NULL)
		return HAL_ERROR;
	else
	{
		return PASCO2_Set_Reg(dev, (uint8_t)XENSIV_PASCO2_REG_SENS_RST,(uint8_t * )&cmd);
	}
	//Give a delay of 2s after SENSOR RES
}

HAL_StatusTypeDef PASCO2_Sensor_init(const PASCO2 *dev)
{
	if(dev==NULL)
		return HAL_ERROR;

	uint8_t temp_data;
	temp_data=XENSIV_PASCO2_COMM_TEST_VAL;
	if(PASCO2_Set_Reg(dev,(uint8_t)XENSIV_PASCO2_REG_SCRATCH_PAD,&temp_data)!=HAL_OK)
		return HAL_ERROR;
	if(PASCO2_Get_Reg(dev,(uint8_t)XENSIV_PASCO2_REG_SCRATCH_PAD,&temp_data)!=HAL_OK)
		return HAL_ERROR;
	if(temp_data!=XENSIV_PASCO2_COMM_TEST_VAL)
		return HAL_ERROR;
	if(PASCO2_Reset(dev,XENSIV_PASCO2_CMD_SOFT_RESET)!=HAL_OK)
		return HAL_ERROR;
	/*2s(t_SENS_RDY) delay is required after sensor reset is done*/
	HAL_Delay(XENSIV_PASCO2_SOFT_RESET_DELAY_MS);

	if(PASCO2_Get_Reg(dev,(uint8_t)XENSIV_PASCO2_REG_SENS_STS,&temp_data)!=HAL_OK)
		return HAL_ERROR;
	else
	{
		if((temp_data & XENSIV_PASCO2_REG_SENS_STS_ICCER_MSK) !=0U)
				return HAL_ERROR;

		if((temp_data &XENSIV_PASCO2_REG_SENS_STS_ORVS_MSK)!=0U)
				return HAL_ERROR;

		if((temp_data &XENSIV_PASCO2_REG_SENS_STS_SEN_RDY_MSK)==0U)
				return HAL_ERROR;
		return HAL_OK;
	}

}
HAL_StatusTypeDef PASCO2_Set_Measurement_Config(const PASCO2 *dev, xensiv_pasco2_measurement_config_t *meas_config)
{
    if(dev == NULL)
    	return HAL_ERROR;
    else
    return PASCO2_Set_Reg(dev, (uint8_t)XENSIV_PASCO2_REG_MEAS_CFG, &(meas_config->u));
}
HAL_StatusTypeDef PASCO2_Get_Measurement_Config(const PASCO2 *dev, xensiv_pasco2_measurement_config_t *meas_config)
{
    if(dev == NULL||meas_config==NULL)
    	return HAL_ERROR;
    else
    return PASCO2_Get_Reg(dev, (uint8_t)XENSIV_PASCO2_REG_MEAS_CFG, &(meas_config->u));
}

HAL_StatusTypeDef PASCO2_Set_Pressure_ref(const PASCO2 *dev,uint16_t *val)
{
	if(dev==NULL)
		return HAL_ERROR;
	*val=reverse_ByteOrder(*val);
	if(PASCO2_Set_Regs(dev,(uint8_t)XENSIV_PASCO2_REG_PRESS_REF_H,(uint8_t *)val)!=HAL_OK)
		return HAL_ERROR;
	else
		return HAL_OK;


}
HAL_StatusTypeDef PASCO2_Get_Pressure_ref(const PASCO2 *dev,uint16_t *val)
{
	if(dev==NULL)
		return HAL_ERROR;
	if(PASCO2_Get_Regs(dev, (uint8_t)XENSIV_PASCO2_REG_PRESS_REF_H,(uint8_t *)val)!=HAL_OK)
	return HAL_ERROR;
	else
	{
		*val=reverse_ByteOrder(*val);
		return HAL_OK;

	}


}

HAL_StatusTypeDef PASCO2_Get_MeasuremetnStatus(const PASCO2 *dev,uint8_t *val)
{
	return PASCO2_Get_Reg(dev, (uint8_t)XENSIV_PASCO2_REG_MEAS_STS,val);
}

HAL_StatusTypeDef PASCO2_Set_MeasuremetnStatus(const PASCO2 *dev,uint8_t *val)
{
	return PASCO2_Set_Reg(dev, (uint8_t)XENSIV_PASCO2_REG_MEAS_STS,val);
}
HAL_StatusTypeDef PASCO2_Start_SingleShotMode(const PASCO2 *dev)
{
	if(dev==NULL)
		return HAL_ERROR;
	xensiv_pasco2_measurement_config_t meas_cfg={0};
	if(PASCO2_Get_Measurement_Config(dev,&meas_cfg)!=HAL_OK)
		return HAL_ERROR;
	if(meas_cfg.b.op_mode!=XENSIV_PASCO2_OP_MODE_IDLE)
	{
		meas_cfg.b.op_mode=XENSIV_PASCO2_OP_MODE_IDLE;
		if(PASCO2_Set_Measurement_Config(dev,&meas_cfg)!=HAL_OK)
			return HAL_ERROR;
	}

	meas_cfg.b.op_mode=XENSIV_PASCO2_OP_MODE_SINGLE;
	meas_cfg.b.boc_cfg=XENSIV_PASCO2_BOC_CFG_AUTOMATIC;
	if(PASCO2_Set_Measurement_Config(dev,&meas_cfg)==HAL_OK)
		return HAL_OK;
	else
		return HAL_ERROR;


}


HAL_StatusTypeDef PASCO2_Start_ContinousMode(const PASCO2 *dev,uint16_t meas_rate)
{
	if(dev==NULL)
		return HAL_ERROR;

	 xensiv_pasco2_measurement_config_t meas_cfg={0};

	 if(PASCO2_Get_Measurement_Config(dev, &meas_cfg)!=HAL_OK)
		 return HAL_ERROR;

	 if(meas_cfg.b.op_mode!=XENSIV_PASCO2_OP_MODE_IDLE)
		 meas_cfg.b.op_mode=XENSIV_PASCO2_OP_MODE_IDLE;

	 if(PASCO2_Set_Measurement_Config(dev, &meas_cfg)!=HAL_OK)
	 		 return HAL_ERROR;
	// meas_rate=reverse_ByteOrder(meas_rate);
	 if(PASCO2_Set_Measurement_Rate(dev,meas_rate)!=HAL_OK)
		 return HAL_ERROR;

	 meas_cfg.b.op_mode=XENSIV_PASCO2_OP_MODE_CONTINUOUS;
	 meas_cfg.b.boc_cfg=XENSIV_PASCO2_BOC_CFG_AUTOMATIC;

	 if(PASCO2_Set_Measurement_Config(dev, &meas_cfg)!=HAL_OK)
		return HAL_ERROR;
	 else
		 return HAL_OK;


}

HAL_StatusTypeDef PASCO2_StopMeasure(const PASCO2 *dev)
{
	xensiv_pasco2_measurement_config_t meas_cfg={0};
	PASCO2_Get_Measurement_Config(dev, &meas_cfg);
	if(meas_cfg.b.op_mode!=XENSIV_PASCO2_OP_MODE_IDLE)
	meas_cfg.b.op_mode=XENSIV_PASCO2_OP_MODE_IDLE;
	if(PASCO2_Set_Measurement_Config(dev, &meas_cfg)==HAL_OK)
		return HAL_OK;
	else
		return HAL_ERROR;

}

HAL_StatusTypeDef PASCO2_Set_Measurement_Rate(const PASCO2 *dev, uint16_t meas_rate)
{
	meas_rate=reverse_ByteOrder(meas_rate);
	if(PASCO2_Set_Regs(dev, (uint8_t)XENSIV_PASCO2_REG_MEAS_RATE_H, (uint8_t *)&meas_rate)!=HAL_OK)
		return HAL_ERROR;
	else
		return HAL_OK;
}

HAL_StatusTypeDef PASCO2_Get_Measurement_Rate(const PASCO2 *dev, uint16_t *meas_rate)
{

	if(PASCO2_Get_Regs(dev, (uint8_t)XENSIV_PASCO2_REG_MEAS_RATE_H, (uint8_t *)meas_rate)!=HAL_OK)
		return HAL_ERROR;
	else
	{
		*meas_rate=reverse_ByteOrder(*meas_rate);
		return HAL_OK;
	}
}

HAL_StatusTypeDef PASCO2_ClearForecedCompensation(const PASCO2 *dev)
{
	if(dev==NULL)
		return HAL_ERROR;
	else
		return PASCO2_cmd(dev,XENSIV_PASCO2_CMD_RESET_FCS);


return HAL_OK;
}
HAL_StatusTypeDef PASCO2_PerformForecedCompensation(const PASCO2 *dev,uint16_t val)
{
	/*Set IDLE Mode*/
	HAL_StatusTypeDef ret;
	xensiv_pasco2_measurement_config_t measCfg;
	if(PASCO2_Get_Measurement_Config(dev, &measCfg)==HAL_OK)
	{
		measCfg.b.op_mode=XENSIV_PASCO2_OP_MODE_IDLE;
		if(PASCO2_Set_Measurement_Config(dev, &measCfg)==HAL_OK)
		{
			/*Configure Measurement rate to 10s*/
			if(PASCO2_Set_Measurement_Rate(dev, (uint16_t)XENSIV_PASCO2_MEAS_RATE_FCS)==HAL_OK)
			{
				/*Load Calibration register with compensation value*/
				if(PASCO2_Set_Calibration_ref(dev,val)==HAL_OK)
				{
					/*Set BOC mode to FCS & OPMode to Contionous Mode*/
					if(PASCO2_Get_Measurement_Config(dev, &measCfg)==HAL_OK)
					{
						measCfg.b.op_mode=XENSIV_PASCO2_OP_MODE_CONTINUOUS;
						measCfg.b.boc_cfg=XENSIV_PASCO2_BOC_CFG_FORCED;
						if(PASCO2_Set_Measurement_Config(dev, &measCfg)==HAL_OK)
						{
							/*Wait until BOC Mode switch back to ABOC Mode*/
							do
							{
								ret= PASCO2_Get_Measurement_Config(dev, &measCfg);


							}while((ret!=HAL_OK)||XENSIV_PASCO2_BOC_CFG_FORCED==measCfg.b.boc_cfg);
							if(ret==HAL_ERROR)
								return HAL_ERROR;
							/*Push to IDLE Mode*/
							measCfg.b.op_mode=XENSIV_PASCO2_OP_MODE_IDLE;
							if(PASCO2_Set_Measurement_Config(dev, &measCfg)==HAL_OK)
							{
								/*Save the calibration SENS_RST=0xCF*/
								if(PASCO2_cmd(dev, XENSIV_PASCO2_CMD_SAVE_FCS_CALIB_OFFSET)!=HAL_OK)
									return HAL_ERROR;
								else
									return HAL_OK;
							}

						}
					}
				}
			}
		}
	}
}
uint16_t reverse_ByteOrder(uint16_t val)
{
	return ((val&0xFF)<<8)|((val&0xFF00)>>8);
}

HAL_StatusTypeDef PASCO2_cmd(const PASCO2 * dev, xensiv_pasco2_cmd_t cmd)
{
	if(dev==NULL)
		return HAL_ERROR;
	return PASCO2_Set_Reg(dev, (uint8_t)XENSIV_PASCO2_REG_SENS_RST, (uint8_t*)&cmd);
}

HAL_StatusTypeDef PASCO2_Set_Calibration_ref(const PASCO2 *dev,uint16_t val)
{
	if(dev==NULL)
		return HAL_ERROR;
	val=reverse_ByteOrder(val);
	if(PASCO2_Set_Regs(dev,(uint8_t)XENSIV_PASCO2_REG_CALIB_REF_H,(uint8_t *)&val)!=HAL_OK)
		return HAL_ERROR;
	else
		return HAL_OK;

}
