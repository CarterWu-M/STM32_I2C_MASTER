
#ifndef INC_SERVICES_I2C_MASTER_H_
#define INC_SERVICES_I2C_MASTER_H_

#include "main.h"
#include "API/Queue.h"

int I2CMaster_init(void);
int I2CMaster_task(void);

int I2CMaster_enqueueCmd(uint8_t arrData[MAX_DATA_LEN], uint8_t dataLen);

#endif /* INC_SERVICES_I2C_MASTER_H_ */
