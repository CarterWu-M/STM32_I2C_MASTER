#ifndef INC_API_CMDPROC_H_
#define INC_API_CMDPROC_H_

//command ID
#define CMD_R_MCU_TEMP  (0x0A)
#define CMD_R_FAN_SPEED (0x01)
#define CMD_W_FAN_PWM   (0x02)

int Cmd_procS(uint8_t cmdId, uint8_t arrReq[], uint8_t *pReqLen, uint8_t arrRes[], uint8_t *pResLen);
int Cmd_procM(uint8_t addr, uint8_t cmdId, uint8_t arrRes[], uint8_t resLen);

#endif /* INC_API_CMDPROC_H_ */
