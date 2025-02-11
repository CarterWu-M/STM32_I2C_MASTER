#include "main.h"
#include "API/CmdProc.h"

static int getMcuTemp(uint8_t addr, uint8_t arrData[], uint8_t dataLen);
static int getFanSpeed(uint8_t addr, uint8_t arrData[], uint8_t dataLen);
static int checkRet(uint8_t addr, uint8_t arrData[], uint8_t dataLen);

static struct {
	uint8_t cmdId;
	int (*pProcFun)(uint8_t addr, uint8_t arrData[], uint8_t dataLen);
} arrT[] = {
	{CMD_R_MCU_TEMP,  getMcuTemp},
	{CMD_R_FAN_SPEED, getFanSpeed},
	{CMD_W_FAN_PWM,   checkRet},
};
#define TBL_NUM (sizeof(arrT) / sizeof(arrT[0]))

int Cmd_procM(uint8_t addr, uint8_t cmdId, uint8_t arrRes[], uint8_t resLen)
{
	if (!arrRes || !resLen) {
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

	if (arrT[idx].pProcFun(addr, arrRes, resLen)) {
		return -1;
	}

	return 0;
}

static int getMcuTemp(uint8_t addr, uint8_t arrData[], uint8_t dataLen)
{
	if (!arrData || !dataLen) {
		return -1;
	}

	//example
	if (0x10 == addr) {
		//do something
	}
	else if (0x11 == addr) {
		//do something
	}

	return 0;
}

static int getFanSpeed(uint8_t addr, uint8_t arrData[], uint8_t dataLen)
{
	if (!arrData || !dataLen) {
		return -1;
	}

	//example
	if (0x10 == addr) {
		//do something
	}
	else if (0x11 == addr) {
		//do something
	}

	return 0;
}

static int checkRet(uint8_t addr, uint8_t arrData[], uint8_t dataLen)
{
	if (!arrData || !dataLen) {
		return -1;
	}

	//return: 0 => success
	if (0 != arrData[0]) {
		return -1;
	}

	return 0;
}
