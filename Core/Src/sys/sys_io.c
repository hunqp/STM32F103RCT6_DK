#include "sys_io.h"

void blinkLedLife() {
    static bool state = false;
    
    HAL_GPIO_WritePin(LED_LIFE_GPIO_Port, LED_LIFE_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    state = !state;
}

void W5x00_Reset() {
	W5x00_SetCSHigh();
	
	/* Generate a reset pulse */
	HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_RESET);
	delayInMicroseconds(100);
	HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET);
	delayInMicroseconds(100);
}

void W5x00_SetCSHigh() {
	HAL_GPIO_WritePin(SPI_W5500_NSS_GPIO_Port, SPI_W5500_NSS_Pin, GPIO_PIN_SET);
}

void W5x00_SetCSLow() {
	HAL_GPIO_WritePin(SPI_W5500_NSS_GPIO_Port, SPI_W5500_NSS_Pin, GPIO_PIN_RESET);
}

void W5x00_ReadData(uint8_t *data, uint16_t size) {
    HAL_SPI_Receive(&hspi1, data, size, 0xFF);
}

void W5x00_SendData(uint8_t *data, uint16_t size) {
	HAL_SPI_Transmit(&hspi1, data, size, 0xFF);
}

uint8_t W5x00_ReadByte() {
	uint8_t byte;
	W5x00_ReadData(&byte, sizeof(byte));

	return byte;
}

void W5x00_SendByte(uint8_t byte) {
	W5x00_SendData(&byte, sizeof(byte));
}

void softReset() {
    NVIC_SystemReset();
}

void rstHardWatchdog() {
	HAL_IWDG_Refresh(&hiwdg);
}

void delayInMicroseconds(uint32_t us) {
	volatile uint32_t count = (us * (SystemCoreClock / 1000000)) / 4;

	for (uint32_t i = 0 ; i < count; ++i) {
		count--;
	}
}

void delayInMilliseconds(uint32_t ms) {
	volatile uint32_t count = ((ms * 1000) * (SystemCoreClock / 1000000)) / 4;

	for (uint32_t i = 0 ; i < count; ++i) {
		count--;
	}
}


char* getLastReasonReset(bool clear) {
    char *ret = NULL;

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) {
        ret = "LPWR";
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) {
        ret = "WWDG";
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {
        ret = "SOFT";
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {
    	ret = "IWDG";
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) {
        ret = "POR";
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) {
        ret = "PIN";
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY)) {
        ret = "LSI";
    }
    else {
        ret = "UNK";
    }

    if (clear) {
        __HAL_RCC_CLEAR_RESET_FLAGS();
    }

    return ret;
}
