#include "main.h"
#include "API/CmdProc.h"

static int getMcuTemp(uint8_t arrRes[], uint8_t *pResLen);
static int getFanSpeed(uint8_t arrRes[], uint8_t *pResLen);
static int setFanPWM(uint8_t arrReq[], uint8_t *pReqLen);

enum CmdType {
	CMD_WRITE,
	CMD_READ,
	CMD_TYPE_NUM
};

static struct {
	uint8_t cmdId;
	enum CmdType cmdType;
	int (*pProcFun)(uint8_t *parrData, uint8_t *pDataLen);
} arrT[] = {
	{CMD_R_MCU_TEMP,  CMD_READ,  getMcuTemp},
	{CMD_R_FAN_SPEED, CMD_READ,  getFanSpeed},
	{CMD_W_FAN_PWM,   CMD_WRITE, setFanPWM},
};
#define TBL_NUM (sizeof(arrT) / sizeof(arrT[0]))

int Cmd_procS(uint8_t cmdId, uint8_t arrReq[], uint8_t *pReqLen, uint8_t arrRes[], uint8_t *pResLen)
{
	if (!arrReq || !arrRes || !pResLen) {
		return -1;
	}

	uint8_t idx;
	for (idx = 0; TBL_NUM > idx; idx++) {
		if (arrT[idx].cmdId == cmdId) {
			break;
		}
	}
	if (TBL_NUM == idx) {
		return -1; //the cmdId is not found
	}

	if (CMD_WRITE == arrT[idx].cmdType) {
		arrT[idx].pProcFun(arrReq, pReqLen);
		*pResLen = 0;
	}
	else {//CMD_READ
		arrT[idx].pProcFun(arrRes, pResLen);
	}

	return 0;
}

static int getMcuTemp(uint8_t arrRes[], uint8_t *pResLen)
{
	if (!arrRes || !pResLen) {
		return -1;
	}
	//example
	arrRes[0] = 30;
	*pResLen = 1;

	return 0;
}

static int getFanSpeed(uint8_t arrRes[], uint8_t *pResLen)
{
	if (!arrRes || !pResLen) {
		return -1;
	}
	//example
	arrRes[0] = 145;
	*pResLen = 1;

	return 0;
}

static int setFanPWM(uint8_t arrReq[], uint8_t *pReqLen)
{
	if (!arrReq || !pReqLen) {
		return -1;
	}
	//example
	uint8_t fanPwm = arrReq[0];

	return 0;
}

