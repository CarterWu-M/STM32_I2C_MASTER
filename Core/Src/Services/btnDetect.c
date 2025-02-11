#include <string.h>
#include "main.h"
#include "API/CmdPackage.h"
#include "API/CmdProc.h"
#include "API/Queue.h"
#include "Services/I2C_master.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_PIN_13 == GPIO_Pin) {
        if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOC, GPIO_Pin))
        {
    		uint8_t arrD[MAX_DATA_LEN];
    		uint8_t txLen;

    		if (Cmd_pack(0x10, CMD_R_FAN_SPEED, NULL, 0, arrD, &txLen)) {
    			return;
    		}
    		if (I2CMaster_enqueueCmd(arrD, txLen)) {
    			return;
    		}
        }
    }
}

