/* USER CODE BEGIN Header */
/*
 *  Copyright (C) 2019 - 2023, VadRov, all right reserved.
 *
 *  LED-контроллер для отображения графической информации на экране, состоящем из
 *  соединенных последовательно лент с адресными светодиодами на чипе ws2812b.
 *
 *  Допускается свободное распространение. При любом способе распространения указание
 *  автора ОБЯЗАТЕЛЬНО. В случае внесения изменений и распространения модификаций
 *  указание первоначального автора ОБЯЗАТЕЛЬНО. Распространяется по типу "как есть",
 *  то есть использование осуществляется на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *  Версия: 1.1G0 (для STM32G0)
 *
 *  Общая информация
 *  LED-контроллер отображает на экране информацию из AVI-файла (файл с расширением avi,
 *  контейнер riff), кадры которого представляют собой изображения в формате несжатого
 *  bmp (данные пикселей в файле хранятся как двумерный массив).
 *  LED-контроллер собран на монтажной плате с использованием отладочного мудуля на базе
 *  микроконтроллера STM32G031F6P6/STM32G031F8P6. К микроконтроллеру подключен дисплей
 *  ST7789 (через spi), модуль (адаптер/переходник на micro-SD) SD-карты (через spi),
 *  инкрементарный механический энкодер с кнопкой. В контроллере реализован файловый менеджер
 *  с графическим интерфейсом для просмотра содержимого sd-карты и выбора файлов для
 *  воспроизведения на LED-экране. Контроллер имеет 1 выход (порт) для управления адресными
 *  светодиодными лентами. Протокол управления адресными светодиодными лентами реализован
 *  посредством PWM c DMA с таймингами управляющего сигнала, соответствующими спецификации
 *  модулей ws2812b. Для корректной передачи сигналов на первый адресный чип, выходной порт
 *  контроллера реализован по схеме открытый сток и подтянут к питанию +5В (резистор R2 на схеме),
 *  что обеспечивает уровни сигналов на входе DIN, соответствующими спецификации ws2812b.
 *  Резистор R1 ограничивает ток, проходящий через выходной вывод микроконтроллера и предохраняет
 *  его от повреждения в том случае, например, если пользователь захочет обновить ПО
 *  микроконтроллера, забыв при этом отключить его от экрана с отключенным питанием (в этом случае
 *  лента будет "тянуть питание" через этот вывод, и он может "выгореть" при превышении
 *  допустимого значения тока).
 *  Перемещение по файлам и каталогам в файловом менеджере осуществляется путем вращения
 *  ручки энкодера. Выбор avi-файла для воспроизведения или переход в подкаталог(возврат в каталог)
 *  осуществляется коротким нажатием (кликом) кнопки энкодера. Для остановки воспроизведения
 *  текущего файла следует длительно (2 и более секунд) удерживать нажатой, а затем отпустить
 *  кнопку энкодера.
 *
 *  Подготовка файлов для воспроизведения
 *  Как было отмечено выше, LED-контроллер воспроизводит AVI-файлы (файл с расширением avi,
 *  контейнер riff). Формат файла должен соответствовать спецификации microsoft. При этом кадры
 *  видеопотока AVI-файла должны быть в формате несжатого BMP (несжатые данные RGB, хранящиеся
 *  последовательно, как двумерный массив). Для генерации последовательности BMP-изображений
 *  (цветовые эффекты, бегущие строки и т.д.), формирующих непрервывный видеоряд, можно, например,
 *  воспользоваться программой Jinx!. Настройка Jinx! заключается в определении опций матрицы
 *  (ширина и высота матрицы в пикселях - в нашем случае выражается в количестве светодиодных
 *  модулей на экране по горизонтали и вертикали), а также свойств выводного устройства
 *  (куда и как сохранять изображения). Размеры матрицы задаются через Setup->Matrix Options.
 *  Параметры устройства вывода определяются через Setup->Output Devices, где следует выбрать Add.
 *  Затем в списке Devices Type выбирается пункт Bitmap Export. Требуемый каталог для хранения
 *  последовательности изображений задают через кнопку Select. Выбрав и настроив требуемый эффект,
 *  можно стартовать запись кадров в указанный каталог. Для этого необходимо выбрать в меню
 *  Setup->Start Output. По просшествии требуемого периода времени, запись можно остановить,
 *  снова выбрав в меню Setup->Start Output.
 *  Полученные кадры изображения можно "склеить" в один AVI-файл. Для этого можно использовать
 *  библиотеку ffmpeg, введя, например, такую командную строку, находясь в каталоге с картинками:
 *  	ffmpeg -i %8d.bmp -r 25 -vcodec bmp video.avi
 *  где:
 *	-i %8d.bmp - входные данные (файлы-картинки). %8d означает, что имя файла состоит из 8-значного числа;
 *	-r 25 - частота кадров (25 кадров/секунду) в получаемом видеофайле (этот параметр указан в окне Jinx!);
 *	-vcodec bmp - используется кодек bmp (указание на то, что используются несжатые данные RGB);
 *	video.avi - название выходного (целевого) файла.
 *	Другой способ получения требуемого формата изображения для LED-контроллера и крана заключается
 *	в конвертации любого видеофайла той же библиотекой ffmpeg. Например:
 *		ffmpeg -i file.mp4 -r 25 -vcodec bmp -s 60x20 video.avi
 *	где:
 *	-i file.mp4 - входной видеофайл;
 *	-r 25 - частота кадров (25 кадров/секунду) - Вы можете, например, ограничить частоту кадров;
 *	-vcodec bmp - используется кодек bmp (указание на то, что используются несжатые данные RGB);
 *	-s 60x20 - размер кадра 60 на 20 пикселей (один пиксель соответствует одному светодиодному модулю);
 *	video.avi - название выходного (целевого) файла.
 *	Дополнительной опцией -an можно исключать звуковую дорожку из файла.
 *
 *	Компоненты:
 *	- Отладочная плата (МК) STM32G031F6P6/STM32G031F8P6;
 *	- SPI дисплей на контроллере ST7789 разрешением 240x240;
 *	- Адаптер/переходник с SD карты на микро-SD;
 *	- Инкрементарный механический энкодер;
 *	- 2 резистора с номиналами: 390-680 Ом и 4.7-8.2 кОм;
 *	- Адресные светодиодные ленты для формирования экрана (в данном проекте используется 20 лент
 *	по 60 адресных светодиодов на чипе ws2812b. Итого 1200 адресных светодиода для экрана с
 *	разрешением 60х20);
 *	- Блок питания на 5В для запитывания контроллера и экрана из расчета 0.3 Вт на один модуль
 *	ws2812b (в данном проетке используется блок питания на 350 Вт. При свечении всех модулей
 *	белым цветом просадок по напряжению нет).
 *
 *	Вариант LED-экрана из лент с адресными светодиодами
 *	В данном проекте основой для крепления светодиодных лент служит обычный б/у пластиковый подоконник
 *	окрашенный в черный матовый цвет (с предварительным нанесением праймера для пластика). Пиксели
 *	экрана сформированы 20 отрезками метровых лент по 60 светодиодных модулей в каждой (всего 1200 модулей).
 *	Светодиодные ленты закреплены к основе (окрашенному подоконнику) с помощью двустороннего скотча по
 *	предварительно нанесенной разметке. Питание к лентам подведено с двух сторон, что исключает просадки
 *	напряжения в различных участках ленты.
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "display.h"
#include "st7789.h"
#include <stdlib.h>
#include "filemanager.h"
#include "keyboard.h"
#include "encoder.h"
#include "ws2812b.h"
#include "avi.h"
#include "ff.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//счетчик "системного времени", мс
volatile uint32_t millis = 0;

//Переменные для библиотеки FatFs
char Path[4];
FATFS FatFS;

//Обработчик энкодера
ENCODER_Handler encoder1;

#define FONT_DEFAULT	Font_8x13

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM3_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */
	//Включение буфера предварительной выборки
	FLASH->ACR |= FLASH_ACR_PRFTEN;
	//Включение кеша инструкций
	FLASH->ACR |= FLASH_ACR_ICEN;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, 3);

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  //настраиваем системный таймер (прерывания 1000 раз в секунду)
  SysTick_Config(SystemCoreClock/1000);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_SPI2_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  LCD_DMA_TypeDef dma_tx = { .dma =     DMA1,
		  	  	  	  	  	 .channel = 1 };

  LCD_BackLight_data bkl_data = { .tim_bk =         TIM3,
		  	  	  	  	  	  	  .channel_tim_bk = 1,
								  .blk_port =       0,
								  .blk_pin =        0,
								  .bk_percent =     80 };

  LCD_SPI_Connected_data spi_con = { .spi =        SPI1,
		  	  	  	  	  	  	  	 .dma_tx =     dma_tx,
									 .reset_port = LCD_RES_GPIO_Port,
									 .reset_pin =  LCD_RES_Pin,
									 .dc_port =    LCD_DC_GPIO_Port,
									 .dc_pin =     LCD_DC_Pin,
									 .cs_port =    LCD_CS_GPIO_Port,
								 	 .cs_pin =     LCD_CS_Pin };

#ifndef LCD_DYNAMIC_MEM
  LCD_Handler lcd1;
#endif

/*  Для дисплея на контроллере ST7789   */
  LCD = LCD_DisplayAdd( LCD,
#ifndef LCD_DYNAMIC_MEM
		  	  	  	  	&lcd1,
#endif
		  	  	  	  	240,
		   				240,
						ST7789_CONTROLLER_WIDTH,
						ST7789_CONTROLLER_HEIGHT,
						//Задаем смещение по ширине и высоте для нестандартных или бракованных дисплеев:
						0,		//смещение по ширине дисплейной матрицы
						0,		//смещение по высоте дисплейной матрицы
						PAGE_ORIENTATION_PORTRAIT,
						ST7789_Init,
						ST7789_SetWindow,
						ST7789_SleepIn,
						ST7789_SleepOut,
						&spi_con,
						LCD_DATA_16BIT_BUS,
						bkl_data );
  /*  Для дисплея на контроллере ILI9341   */
/*
  LCD = LCD_DisplayAdd( LCD,
#ifndef LCD_DYNAMIC_MEM
		  	  	  	  	&lcd1,
#endif
		  	  	  	  	320,
		   				240,
						ILI9341_CONTROLLER_WIDTH,
						ILI9341_CONTROLLER_HEIGHT,
						//Задаем смещение по ширине и высоте для нестандартных или бракованных дисплеев:
						0,		//смещение по ширине дисплейной матрицы
						0,		//смещение по высоте дисплейной матрицы
						//PAGE_ORIENTATION_PORTRAIT_MIRROR,
						PAGE_ORIENTATION_LANDSCAPE,
						ILI9341_Init,
						ILI9341_SetWindow,
						ILI9341_SleepIn,
						ILI9341_SleepOut,
						&spi_con,
						LCD_DATA_16BIT_BUS,
						bkl_data );
*/
    LCD_Handler *lcd = LCD; //указатель на первый дисплей в списке
  	LCD_Init(lcd);
	LCD_Fill(lcd, COLOR_BLACK);
	//------------------------------------------------------------------------------------------

	//-------------------------------- Информация об устройстве --------------------------------
	LCD_WriteString(lcd, 0, 0, "LED Panel Driver V1.1G0", &FONT_DEFAULT, COLOR_YELLOW, COLOR_BLACK, LCD_SYMBOL_PRINT_FAST);
	lcd->AtPos.y += FONT_DEFAULT.height;
	LCD_WriteString(lcd, 0, lcd->AtPos.y, "Copyright (C) 2023, VadRov", &FONT_DEFAULT, COLOR_CYAN, COLOR_BLACK, LCD_SYMBOL_PRINT_FAST);
	lcd->AtPos.y += FONT_DEFAULT.height;
	LL_mDelay(2000);

	//----------------------------------- Монтирование диска -----------------------------------
	if (f_mount(&FatFS, Path, 1) != FR_OK) {
		LCD_WriteString(lcd, 0, lcd->AtPos.y, "Disk error!", &FONT_DEFAULT, COLOR_RED, COLOR_BLACK, LCD_SYMBOL_PRINT_FAST);
		lcd->AtPos.y += FONT_DEFAULT.height;
		LCD_WriteString(lcd, 0, lcd->AtPos.y,"Insert new disk and press reset...", &FONT_DEFAULT, COLOR_YELLOW,	0x319bb1, LCD_SYMBOL_PRINT_FAST);
		while (1) ;
	}
	//------------------------------------------------------------------------------------------

	//----------------------------- Настройка файлового менеджера ------------------------------
	/* Определение цветовой схемы */
	File_Manager_Color_Scheme color_scheme_pl = { COLOR_YELLOW,		/* цвет текста наименования каталога */
			  	  	  	  	  	  	  	  	  	  COLOR_WHITE,		/* цвет текста наименования файла */
												  COLOR_BLACK,		/* цвет текста выбранного файла/каталога */
												  0x319bb1,			/* цвет фона окна менеджера (для пустых строк) */
												  0x319bb1,			/* цвет фона четных строк */
												  0x319bb1,			/* цвет фона нечетных строк */
												  COLOR_LIGHTGREY,	/* цвет курсора текущего файла/каталога */
												  COLOR_LIGHTGREY,	/* цвет ползунка вертикальной прокрутки */
												  0x319bb1,			/* цвет полосы вертикальной прокрутки */
												  1					/* флаг закрашивания курсора:
																  	  	  0 - текст выбранного файла/каталога обводится
																  	  	  	  прямоугольником цвета курсора (cursor_color).
														 	 	  	  	  >0 - фон текста выбранного файла/каталога приравнивается
														 	 	  	  	  	  цвету курсора (cursor_color) */
												 	 };

	File_Manager_Handler *fm = FileManagerNew();	/* Создание обработчика файлового менеджера */
	fm->SetDisplay(fm, lcd);						/* Дисплей, на который выводится файловый менеджер (указатель) */
	fm->SetWin(fm, 0, 0, lcd->Width, lcd->Height);  /* Параметры окна файлового менеджера:
	  	  	  	  	  	  	  	  	  	  	  	  	   - позиция левого верхнего угла окна по горизонтали;
	  	  	  	  	  	  	  	  	  	  	  	  	   - позиция левого верхнего угла окна по вертикали;
	  	  	  	  	  	  	  	  	  	  	  	  	   - ширина окна;
	  	  	  	  	  	  	  	  	  	  	  	  	   - высота окна. */
	fm->SetColor(fm, &color_scheme_pl);				 /* Цветовая схема (указатель) */
	fm->SetFont(fm, &FONT_DEFAULT);					 /* Шрифт (указатель) */
	fm->SetKeys(fm, KEYB_UP, KEYB_DOWN, KEYB_RIGHT); /* Кнопки управления:
	   	   	   	   	   	   	   	   	   	   	   	   	    - номер бита кнопки вверх;
	   	   	   	   	   	   	   	   	   	   	   	   	    - номер бита кнопки вниз;
	   	   	   	   	   	   	   	   	   	   	   	   	    - номер бита кнопки ввод/выбор. */
	//-----------------------------------------------------------------------------------------

	//----------------------------------- Настройка энкодера ----------------------------------
	//Будем работать с модулем Keyboard и эмулировать для него нажатие кнопок через
	//специальную функцию, связывающую обработчик энкодера с модулем для работы с кнопками.
	//Такую роль выполняет функция EncoderEventToKeyboard, находящаяся в файле stm32f4xx_it.c
	KEYB_all_button = 3; //Используем 3 кнопки для модуля Keyboard
	//Инициализируем обработчик энкодера
	EncoderInit(&encoder1,
				ENCODER_A_GPIO_Port,
				ENCODER_A_Pin,
				ENCODER_B_GPIO_Port,
				ENCODER_B_Pin,
				ENCODER_C_GPIO_Port,
				ENCODER_C_Pin);
	EncoderSetInversion(&encoder1, ENCODER_INV_ON); //Если энкодер работает в противоположную сторону
													 //от ожидаемого направления, то здесь можно задать
													 //инверсию для энкодера, передав функции параметр ENCODER_INV_ON.
													 //Это позволит не переподключать выводы энкодера A и B.
	//------------------------------------------------------------------------------------------

	//------------------------------- Создание LED устройства ----------------------------------
	//Создание LED портов с учетом возможностей периферии (наличие свободных таймеров с каналами PWM DMA)
	LED_Port *ports = LED_AddNewPort(0, 1200, TIM1, LL_TIM_CHANNEL_CH4, DMA1, LL_DMA_CHANNEL_4);
	//LED_Handler *led = LED_CreateDevice(LED_LEFTtoRIGHT_UPtoDOWN, 60, 255, ports);
	LED_Handler *led = LED_CreateDevice(LED_DOWNtoUP_LEFTtoRIGHT, 60, 63, ports);
	if (!led) {
		LCD_WriteString(lcd, 0, lcd->AtPos.y, "Error! Failed to create device!", &FONT_DEFAULT, COLOR_RED, COLOR_BLACK, LCD_SYMBOL_PRINT_FAST);
		while (1) ;
	}
	LED_StartDevice(led);
	//--------------------------------------------------------------------------------------------

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		/* Заливаем дисплей синим цветом (цветом фона файлового менеджера) */
		LCD_Fill(lcd, 0x319bb1);
		/* Запуск менеджера файлов */
		fm->Show(fm);
		/* Проверка статуса завершения работы менеджера файлов */
		if (fm->GetStatus(fm) == File_Manager_OK) { /* Завершение без ошибок с выбором файла? */
			LCD_SleepIn(lcd);
			while (!PlayAVI(fm->GetFilePath(fm), fm->GetFileName(fm), led, 0, 0, led->width, led->height)) { ; }
			LCD_SleepOut(lcd);
		}
		else {
			/* Выводим сообщение, что работа файлового менеджера завершена с ошибкой */
			LCD_WriteString(lcd, 0, 0, "Error FileManager!",
							fm->font, fm->color_scheme->text_color_file,
					        fm->color_scheme->bg_color, LCD_SYMBOL_PRINT_FAST);
			/* Ожидаем нажатия любой кнопки */
			while(!KEYB_kbhit()) ;
			(void)KEYB_Inkeys();
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
  }

  /* HSI configuration and activation */
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 8, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }

  /* Set AHB prescaler*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  /* Sysclk activation on the main PLL */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* Set APB1 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

  LL_Init1msTick(64000000);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(64000000);
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PA1   ------> SPI1_SCK
  PA2   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = LCD_SCL_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(LCD_SCL_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_SDA_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(LCD_SDA_GPIO_Port, &GPIO_InitStruct);

  /* SPI1 DMA Init */

  /* SPI1_TX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMAMUX_REQ_SPI1_TX);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_16BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_EnableNSSPulseMgt(SPI1);
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**SPI2 GPIO Configuration
  PB7   ------> SPI2_MOSI
  PA0   ------> SPI2_SCK
  PB2   ------> SPI2_MISO
  */
  GPIO_InitStruct.Pin = SD_MOSI_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(SD_MOSI_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SD_SCK_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(SD_SCK_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SD_MISO_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(SD_MISO_GPIO_Port, &GPIO_InitStruct);

  /* SPI2 DMA Init */

  /* SPI2_RX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_2, LL_DMAMUX_REQ_SPI2_RX);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_2, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PRIORITY_MEDIUM);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MDATAALIGN_BYTE);

  /* SPI2_TX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_3, LL_DMAMUX_REQ_SPI2_TX);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PRIORITY_MEDIUM);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MDATAALIGN_BYTE);

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI2, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_EnableNSSPulseMgt(SPI2);
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
  LL_TIM_BDTR_InitTypeDef TIM_BDTRInitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetTIMClockSource(LL_RCC_TIM1_CLKSOURCE_PCLK1);

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

  /* TIM1 DMA Init */

  /* TIM1_CH4 Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_4, LL_DMAMUX_REQ_TIM1_CH4);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PRIORITY_VERYHIGH);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MODE_CIRCULAR);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PDATAALIGN_HALFWORD);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MDATAALIGN_HALFWORD);

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  TIM_InitStruct.Prescaler = 0;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 79;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  TIM_InitStruct.RepetitionCounter = 0;
  LL_TIM_Init(TIM1, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM1);
  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH4);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 39;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
  TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH4, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH4);
  LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_RESET);
  LL_TIM_SetTriggerOutput2(TIM1, LL_TIM_TRGO2_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM1);
  TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
  TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
  TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
  TIM_BDTRInitStruct.DeadTime = 0;
  TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
  TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
  TIM_BDTRInitStruct.BreakFilter = LL_TIM_BREAK_FILTER_FDIV1;
  TIM_BDTRInitStruct.BreakAFMode = LL_TIM_BREAK_AFMODE_INPUT;
  TIM_BDTRInitStruct.Break2State = LL_TIM_BREAK2_DISABLE;
  TIM_BDTRInitStruct.Break2Polarity = LL_TIM_BREAK2_POLARITY_HIGH;
  TIM_BDTRInitStruct.Break2Filter = LL_TIM_BREAK2_FILTER_FDIV1;
  TIM_BDTRInitStruct.Break2AFMode = LL_TIM_BREAK_AFMODE_INPUT;
  TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
  LL_TIM_BDTR_Init(TIM1, &TIM_BDTRInitStruct);
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**TIM1 GPIO Configuration
  PA11 [PA9]   ------> TIM1_CH4
  */
  GPIO_InitStruct.Pin = LED_LENT_0_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
  LL_GPIO_Init(LED_LENT_0_GPIO_Port, &GPIO_InitStruct);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  TIM_InitStruct.Prescaler = 999;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 159;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM3, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM3);
  LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH1);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 79;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM3, LL_TIM_CHANNEL_CH1);
  LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM3);
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**TIM3 GPIO Configuration
  PA6   ------> TIM3_CH1
  */
  GPIO_InitStruct.Pin = LCD_BKLT_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(LCD_BKLT_GPIO_Port, &GPIO_InitStruct);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* Init with LL driver */
  /* DMA controller clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Channel1_IRQn, 2);
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1);
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
  /* DMA1_Ch4_5_DMAMUX1_OVR_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Ch4_5_DMAMUX1_OVR_IRQn, 0);
  NVIC_EnableIRQ(DMA1_Ch4_5_DMAMUX1_OVR_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

  /**/
  LL_GPIO_SetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin);

  /**/
  LL_GPIO_SetOutputPin(LED_INDICTR_GPIO_Port, LED_INDICTR_Pin);

  /**/
  LL_GPIO_SetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin);

  /**/
  LL_GPIO_SetOutputPin(LCD_RES_GPIO_Port, LCD_RES_Pin);

  /**/
  LL_GPIO_SetOutputPin(SD_CS_GPIO_Port, SD_CS_Pin);

  /**/
  LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTB, LL_EXTI_CONFIG_LINE9);

  /**/
  LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTC, LL_EXTI_CONFIG_LINE15);

  /**/
  LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTA, LL_EXTI_CONFIG_LINE7);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_9;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_15;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_7;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  LL_GPIO_SetPinPull(ENCODER_B_GPIO_Port, ENCODER_B_Pin, LL_GPIO_PULL_UP);

  /**/
  LL_GPIO_SetPinPull(ENCODER_C_GPIO_Port, ENCODER_C_Pin, LL_GPIO_PULL_UP);

  /**/
  LL_GPIO_SetPinPull(ENCODER_A_GPIO_Port, ENCODER_A_Pin, LL_GPIO_PULL_UP);

  /**/
  LL_GPIO_SetPinMode(ENCODER_B_GPIO_Port, ENCODER_B_Pin, LL_GPIO_MODE_INPUT);

  /**/
  LL_GPIO_SetPinMode(ENCODER_C_GPIO_Port, ENCODER_C_Pin, LL_GPIO_MODE_INPUT);

  /**/
  LL_GPIO_SetPinMode(ENCODER_A_GPIO_Port, ENCODER_A_Pin, LL_GPIO_MODE_INPUT);

  /**/
  GPIO_InitStruct.Pin = LCD_DC_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LCD_DC_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED_INDICTR_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(LED_INDICTR_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LCD_CS_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LCD_RES_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(LCD_RES_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = SD_CS_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(SD_CS_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI4_15_IRQn, 0);
  NVIC_EnableIRQ(EXTI4_15_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
