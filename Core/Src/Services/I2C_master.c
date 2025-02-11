#include <string.h>
#include "Services/I2C_master.h"
#include "API/CmdPackage.h"
#include "API/CmdProc.h"
#include "API/Queue.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c2;

static Queue cmdQ;
static uint8_t arrMaRx[MAX_DATA_LEN];
static uint8_t arrMaTx[MAX_DATA_LEN];
static uint8_t rxCnt = 0;
static bool isRxDone = false;
static bool isTxDone = false;
static uint8_t i2cAddr = 0;

static int xferf(uint8_t arrMaTx[], uint8_t txLen, uint8_t arrMaRx[], uint8_t *pRxLen)
{
	if (!arrMaTx || !txLen || !arrMaRx || !pRxLen) {
		return -1;
	}

	enum {
		STATE_TX,
		STATE_TX_DONE,
		STATE_RX,
		STATE_RX_DONE,
		STATE_NUM
	};

	static uint8_t state = STATE_TX;
	static uint32_t timeBaseTX = 0;
	static uint32_t timeBaseRX = 0;

	switch (state) {
		case STATE_TX:
			i2cAddr = arrMaTx[0];
			if (HAL_OK != HAL_I2C_Master_Seq_Transmit_DMA(&hi2c2, i2cAddr << 1, arrMaTx, txLen, I2C_FIRST_FRAME)) {
				HAL_I2C_DeInit(&hi2c2);
				HAL_I2C_Init(&hi2c2);
				return -1;
			}
			timeBaseTX = HAL_GetTick();
			state = STATE_TX_DONE;
		case STATE_TX_DONE:
			if (isTxDone) {
				isTxDone = false;
				timeBaseTX = HAL_GetTick();
				state = STATE_RX;
			}
			else if (5 < HAL_GetTick() - timeBaseTX) {
				state = STATE_TX;
				return 2;//tx timeout
			}
			break;
		case STATE_RX:
			if (1 < HAL_GetTick() - timeBaseTX) {
				rxCnt = 0;
				if (HAL_OK != HAL_I2C_Master_Seq_Receive_DMA(&hi2c2, i2cAddr << 1, arrMaRx, sizeof(CmdHdr), I2C_NEXT_FRAME)) {
					HAL_I2C_DeInit(&hi2c2);
					HAL_I2C_Init(&hi2c2);
					return -1;
				}
				timeBaseRX = HAL_GetTick();
				state = STATE_RX_DONE;
			}
			break;
		case STATE_RX_DONE:
			if (isRxDone) {
				isRxDone = false;
				*pRxLen = rxCnt;
				state = STATE_TX;
				return 0;//done
			}
			else if (10 <= HAL_GetTick() - timeBaseRX) {
				state = STATE_TX;
				return 2;//rx timeout
			}

			break;
		default:
			//reserve
			break;
	}

	return 1;//busy
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if (I2C2 == hi2c->Instance) {
		isTxDone = true;
	}
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	static uint8_t dataLen = 0;
	if (I2C2 == hi2c->Instance) {
		if (0 == rxCnt) {
			rxCnt += sizeof(CmdHdr);
			CmdHdr *pHdr = (CmdHdr*)arrMaRx;
			dataLen = pHdr->dataLen;

			if (HAL_OK != HAL_I2C_Master_Seq_Receive_DMA(hi2c, i2cAddr << 1, arrMaRx + sizeof(CmdHdr), dataLen + CRC_LEN, I2C_LAST_FRAME)) {
				HAL_I2C_DeInit(&hi2c2);
				HAL_I2C_Init(&hi2c2);
				return;
			}
		}
		else {
			rxCnt += (dataLen + CRC_LEN);
			isRxDone = true;
		}
	}
}

int I2CMaster_enqueueCmd(uint8_t arrData[MAX_DATA_LEN], uint8_t dataLen)
{
	if (!arrData || !dataLen) {
		return -1;
	}

	if (Queue_enqueue(&cmdQ, arrData, dataLen)) {
		return -1;
	}

	return 0;
}

int I2CMaster_init(void)
{
	Queue_init(&cmdQ);
	return 0;
}

int I2CMaster_task(void)
{
	enum {
		STATE_WAITING,
		STATE_XFERING,
		STATE_PROCESSING,
		STATE_NUM
	};

	static uint8_t txLen = 0;
	static uint8_t rxLen = 0;
	static uint8_t state = STATE_WAITING;
	int ret = -1;

	switch (state) {
		case STATE_WAITING:
			if (!Queue_isEmpty(&cmdQ)) {
				if (Queue_dequeue(&cmdQ, arrMaTx, &txLen)) {
					return -1;
				}
				memset(arrMaRx, 0, sizeof(arrMaRx));
				state = STATE_XFERING;
			}
			else {
				break;
			}

		case STATE_XFERING:
			if (0 > (ret = xferf(arrMaTx, txLen, arrMaRx, &rxLen))) {
				return -1;
			}
			if (1 == ret) {//still in transfer
				break;
			}
			if (2 == ret) {//timeout
				state = STATE_WAITING;
				break;
			}
			if (0 == ret) {
				state = STATE_PROCESSING;
			}

		case STATE_PROCESSING:
			uint8_t addr = 0;
			uint8_t cmdId = 0;
			uint8_t arrRes[MAX_DATA_LEN] = {0};
			uint8_t resLen = 0;

			if (Cmd_unpack(&addr, &cmdId, arrMaRx, rxLen, arrRes, &resLen)) {
				return -1;
			}
			if (Cmd_procM(addr, cmdId, arrRes, resLen)) {
				return -1;
			}
			state = STATE_WAITING;
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);//Blinking LED for observation
			break;

		default:
			//reserve
			break;

	}

	//polling task
	static uint32_t timeBase;
	if (1000 <= HAL_GetTick() - timeBase) {
		timeBase = HAL_GetTick();

		uint8_t arrD[MAX_DATA_LEN];
		uint8_t arrCmd[] = {CMD_R_MCU_TEMP};
		uint8_t arrAddr[] = {0x10, 0x11};
#define CMD_NUM sizeof(arrCmd) / sizeof(arrCmd[0])
#define ADDR_NUM sizeof(arrAddr) / sizeof(arrAddr[0])

		for (uint8_t i = 0; CMD_NUM > i; i++) {
			for(uint8_t j = 0; ADDR_NUM > j; j++) {
				if (Cmd_pack(arrAddr[j], arrCmd[i], NULL, 0, arrD, &txLen)) {
					return -1;
				}
				if (Queue_enqueue(&cmdQ, arrD, txLen)) {
					return -1;
				}
			}
		}
	}

	return 0;
}


