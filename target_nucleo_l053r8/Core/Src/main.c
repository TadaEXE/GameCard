/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app/app.h"
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_gpio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void pin_set(GPIO_TypeDef* port, uint16_t pin) { HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET); }
void pin_reset(GPIO_TypeDef* port, uint16_t pin) { HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET); }
void set_cs(void) { pin_set(SPI1_CS_GPIO_Port, SPI1_CS_Pin); }
void reset_cs(void) { pin_reset(SPI1_CS_GPIO_Port, SPI1_CS_Pin); }
void set_dc(void) { pin_set(SPI1_DC_GPIO_Port, SPI1_DC_Pin); }
void reset_dc(void) { pin_reset(SPI1_DC_GPIO_Port, SPI1_DC_Pin); }
void set_rst(void) { (pin_set(SPI1_RST_GPIO_Port, SPI1_RST_Pin)); }
void reset_rst(void) { (pin_reset(SPI1_RST_GPIO_Port, SPI1_RST_Pin)); }
void wait(uint32_t delay) { HAL_Delay(delay); }
void spi_write(uint8_t data) { HAL_SPI_Transmit(&hspi1, &data, 1, 100); }
void spi_write_mul(uint8_t* data, size_t len) { HAL_SPI_Transmit(&hspi1, data, len, 100); }

void sh1107_write_cmd(uint8_t cmd) {
  reset_cs();
  reset_dc();
  spi_write(cmd);
  set_cs();
  set_dc();
}

void sh1107_write_data(uint8_t* data, size_t len) {
  reset_cs();
  spi_write_mul(data, len);
  set_cs();
}

void oled_init(void) {
  reset_rst();
  set_rst();
  wait(10);
  reset_rst();
  wait(10);
  set_rst();
  wait(10);
  sh1107_write_cmd(0xAF);
}

void oled_all_on(void) {
  oled_init();
  sh1107_write_cmd(0xAE);
  sh1107_write_cmd(0xA5);
  sh1107_write_cmd(0xAF);
  wait(1000);
}

uint8_t page_count = 16;
uint8_t col_count = 128;
uint8_t empty[128] = {0};
void oled_clear(void) {
  sh1107_write_cmd(0x20);
  for (uint8_t page = 0; page < page_count; ++page) {
    sh1107_write_cmd(0xB0 | page);
    sh1107_write_cmd(0x00);
    sh1107_write_cmd(0x10);
    sh1107_write_data(empty, sizeof(empty));
  }
}

uint8_t logo_bmp[] = {0x00, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x03, 0xe0, 0xf3, 0xe0, 0xfe, 0xf8, 0x7e, 0xff, 0x33, 0x9f,
                      0x1f, 0xfc, 0x0d, 0x70, 0x1b, 0xa0, 0x3f, 0xe0, 0x3f, 0xf0, 0x7c, 0xf0, 0x70, 0x70, 0x00, 0x30};
// bitmap_16x16_sh1107_page: 32 bytes
unsigned char bitmap_16x16_sh1107_page[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x38, 0x0C, 0x04, 0x06, 0x0E, 0x3A, 0x82, 0x46, 0x4C,
    0x78, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x39, 0xEF, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x03, 0xFE, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x1C, 0x04, 0x06, 0x02, 0x02, 0x02, 0x06, 0x0F, 0x7C,
    0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x03, 0x01, 0x03, 0x06, 0x1E, 0xF0, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1F, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00,
};
void draw_bitmap_vertical(uint8_t* bmp, size_t width, size_t height, uint8_t x, uint8_t y) {
  size_t written = 0;
  for (size_t col = 0; col < width; ++col) {
    sh1107_write_cmd(0xB0 | y);
    if (col + x >= col_count) break;
    sh1107_write_cmd(0x00 | ((col + x) & 0x0F));
    sh1107_write_cmd(0x10 | ((col + x) >> 4));
    sh1107_write_data(bmp + written, height / 8);
    written += height / 8;
  }
}

void draw_bitmap_page(uint8_t* bmp, size_t width, size_t height, uint8_t x, uint8_t y) {
  size_t written = 0;
  for (size_t page = 0; page < height / 8; ++page) {
    sh1107_write_cmd(0xB0 | (page + y));
    sh1107_write_cmd(0x00 | (x & 0x0F));
    sh1107_write_cmd(0x10 | (x >> 4));
    sh1107_write_data(bmp + written, width);
    written += width;
  }
}

static const uint8_t square[8] = {0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F};
void oled_move_square(void) {}

// uint8_t data[] = {0x7f, 0x7F, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f};
uint8_t data[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
uint8_t page = 0;
uint8_t column = 0x00;
uint8_t low_mask = 0x0F;
uint8_t high_mask = 0xF0;
void oled_write_data(void) {
  oled_init();
  oled_clear();
  sh1107_write_cmd(0x20);
  sh1107_write_cmd(0xB0 | page);
  uint8_t x = 0;
  uint8_t y = 0;
  uint8_t empty[32] = {0};
  draw_bitmap_page(bitmap_16x16_sh1107_page, 32, 32, 60, 60);
  // draw_bitmap_vertical(logo_bmp, 16, 16, x, y);
  // while (column < col_count) {
  // sh1107_write_cmd(0x00 | (column & low_mask));
  // sh1107_write_cmd(0x10 | (column >> 4));
  // sh1107_write_data(logo_bmp, sizeof(logo_bmp));
  // column += 4;
  // }
  // while (1) {
  //   wait(100);
  //   sh1107_write_cmd(0xA6);
  //   wait(100);
  //   sh1107_write_cmd(0xA7);
  // }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  set_rst();
  set_dc();
  set_cs();
  oled_write_data();
  start_app();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void) {
  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin | SPI1_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SPI1_DC_Pin | SPI1_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
  GPIO_InitStruct.Pin = USART_TX_Pin | USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI1_DC_Pin SPI1_RST_Pin */
  GPIO_InitStruct.Pin = SPI1_DC_Pin | SPI1_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
