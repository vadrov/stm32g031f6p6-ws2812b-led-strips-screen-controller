/*
 *  Copyright (C) 2019 - 2023, VadRov, all right reserved.
 *
 *  Драйвер для управления светодиодными лентами и экранами на чипе ws2812b
 *
 *  Допускается свободное распространение. При любом способе распространения указание
 *  автора ОБЯЗАТЕЛЬНО. В случае внесения изменений и распространения модификаций
 *  указание первоначального автора ОБЯЗАТЕЛЬНО. Распространяется по типу "как есть",
 *  то есть использование осуществляется на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *  Версия: 1.1G0 (для STM32G0)
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#include "main.h"

#define LED_HIGH_LEVEL		54			//Определяет скважность для логической единицы - 0.85 мкс высокого и 0.4 мкс низкого уровня.
#define LED_LOW_LEVEL		25			//Определяет скважность для логического нуля - 0.4 мкс высокого и 0.85 мкс низкого уровня.
#define LED_RESET			0			//Определяет скважность для RESET (обновление данных) - нулевая скважность длительностью более 50 мкс.
#define LED_DIODES_RESET	3			//Определяет необходимое минимальное количество виртуальных светодиодов для формирования сигнала RESET.
										//Длительность передачи данных для 1 диода = 24 * 1,25 = 30 мкс, а надо не менее 50 мкс,
										//т.е. при 1 мы получим сигнал со скважностью LED_RESET длительностью 30 мкс, при 2 - 60 мкс, при 3- 90 мкс и т.д.)
										//В силу специфики работы драйвера зададим параметр LED_DIODES_RESET равным 3.

//#define WS2812_GAMMA_CORRECTION		//Режим корректироваки компонентов цвета по таблице (корректировка яркости).
										//В этом режиме не поддерживается ручная регулировка яркости (задаваемый параметр яркости
										//игнорируется).

//Виды направлений ленты при формировании экрана
typedef enum {
	LED_LEFTtoRIGHT_UPtoDOWN,
	LED_LEFTtoRIGHT_DOWNtoUP,
	LED_RIGHTtoLEFT_UPtoDOWN,
	LED_RIGHTtoLEFT_DOWNtoUP,
	LED_UPtoDOWN_LEFTtoRIGHT,
	LED_UPtoDOWN_RIGHTtoLEFT,
	LED_DOWNtoUP_LEFTtoRIGHT,
	LED_DOWNtoUP_RIGHTtoLEFT
} LED_DISPLAY_LENT_TYPES;
/*
 * 						Примеры видов направления ленты при формировании дисплея:
 *              LED_LEFTtoRIGHT_UPtoDOWN							    LED_LEFTtoRIGHT_DOWNtoUP
 *
 *    DI >-------->>>>>>>-----------------------+			   +----------------->>>>>>>>>--------------> DO
 *       										|			   |
 *       +-----<<<<<<<<-------------------------+			   +--------------<<<<<<<<<<<---------------+
 *       |																								|
 *       +-------------->>>>>>>>>>--------------+			   +------------>>>>>>>>>>>>----------------+
 *       										|			   |
 *       +-----------<<<<<<<<<<<<<--------------+			   +------------<<<<<<<<<<<<<---------------+
 *       |																								|
 *       +-------------->>>>>>>>>>>------------> DO         DI >--------------->>>>>>>>>>>>>------------+
 *
 *
 *               LED_RIGHTtoLEFT_DOWNtoUP								LED_RIGHTtoLEFT_UPtoDOWN
 *
 *    DO <-------<<<<<<<<<<<<-------------------+			   +------------<<<<<<<<<<<<<---------------< DI
 *       										|			   |
 *       +----->>>>>>>>>>>>>--------------------+			   +------------>>>>>>>>>>>-----------------+
 *       |													   											|
 *       +-----------------------<<<<<<<<<<<----+			   +------------<<<<<<<<<<<<----------------+
 *       										|			   |
 *       +----------->>>>>>>>>>>>>>-------------+			   +------------->>>>>>>>>>-----------------+
 *       |																								|
 *       +--------------<<<<<<<<<<<<-----------< DI			DO <-------------<<<<<<<<<<-----------------+
 *
 */

//структура описывает порт
//каждый порт управляет заданным числом диодов
//канал таймера должен быть в режиме PWM с подключенным ДМА
typedef struct {
	TIM_TypeDef *timer;  		//обработчик таймера
	uint32_t channel;			//канал таймера
	DMA_TypeDef *dma;			//контроллер DMA
	uint32_t dma_channel;		//канал DMA
	uint16_t bit_buffer[48];	//битовый буфер (последовательность скважностей)
	uint16_t all_diodes;		//количество диодов, управляемое портом
	int	first_diode_num;		//id первого диода порта
	uint16_t current_diode;		//счетчик диодов, записанных в битовый буфер порта
	int diodes_reset;			//количество диодов для формирования сигнала RESET (не меньше 2)
	volatile uint8_t fl_new_frame;	//флаг обновления - указывает, что будут передаваться новые данные (для генерации сигнала RESET)
	int count_res_diodes;		//счетчик количества диодов для формирования сигнала RESET
	void *prev;					//указатель на предыдующий порт
	void *next;					//указатель на следующий порт
} LED_Port;

typedef struct {						//обработчик LED экрана
	int	all_diodes;						//количество управляемых диодов (id номер диода в буфере pixel_buffer: от 0 до all_diodes-1)
	LED_Port *ports;					//указатель на первый порт списка портов
	uint8_t *pixel_buffer;				//буфер LED экрана с пиксельными данными: 3 байта на диод/пиксель (по байту на цветовую составляющие r, g, b)
	LED_DISPLAY_LENT_TYPES lent_orient;	//тип направления ленты при формировании дисплея
	uint16_t width;						//размер экрана по горизонтали, пиксели
	uint16_t height;					//размер экрана по вертикали, пиксели
	uint8_t bright;						//максимальная яркость (все световые составляющие масштабируются к этому значению)
	void *prev;							//указатель на предыдующий обработчик
	void *next;							//указатель на следующий обработчик
} LED_Handler;

extern LED_Handler *LED_devices;

//создает порт управления (канал таймера с PWM, через который осуществляется управление заданным количеством светодиодов)
LED_Port* LED_AddNewPort(LED_Port *ports, uint16_t diodes_all, TIM_TypeDef *tim, uint32_t tim_channel, DMA_TypeDef *dma, uint32_t dma_channel);

//удаляет порт
void LED_DeletePort(LED_Port *port);

//создает обработчик LED экрана
LED_Handler* LED_CreateDevice(LED_DISPLAY_LENT_TYPES orient, uint16_t diodes_in_line, uint8_t bright, LED_Port *ports);

//удаляет обработчик LED экрана
void LED_DeleteDevice(LED_Handler*);

//запускает обработчик LED экрана
void LED_StartDevice(LED_Handler*);

//останавливает обработчик LED экрана
void LED_StopDevice(LED_Handler*);

//задает цвет модели RGB для диода с уникальным номером
void LED_SetPixelByIdRGB(LED_Handler *led, int id, uint8_t r, uint8_t g, uint8_t b);

//задает цвет модели HSV для диода с уникальным номером
void LED_SetPixelByIdHSV(LED_Handler *led, int id, uint16_t Hue, uint8_t Saturation, uint8_t Brightness);

//устанавливает заданный цвет для всех диодов
void LED_FillColor(LED_Handler *led, uint32_t color);

//возвращает цвет диода с уникальным номером
uint32_t LED_GetPixelById(LED_Handler *led, int id);

//устанавливает цвет пикселя в позиции (x, y) экранной области
void LED_SetPixelByXY(LED_Handler *led, uint16_t x, uint16_t y, uint32_t color);

//возвращает уникальный номер диода, соответствующий позиции пикселя (x, y) в экранной области
int LED_GetIdByXY(LED_Handler *led, uint16_t x, uint16_t y);

//устанавливает максимальную яркость, которой ограничиваются цветовые составляющие (не могут быть выше этого значения, масштабируются по отношении к этому значению)
void LED_SetBright(LED_Handler *led, uint8_t bright);

//возвращает значение максимальной яркости
uint8_t LED_GetBright(LED_Handler *led);

//Разрешение обновления данных на устройстве
void LED_DeviceUpdate(LED_Handler *led);

//Ожидание обновления данных на устройстве
void LED_DeviceWaitUpdate(LED_Handler *led);

void LED_Callback(DMA_TypeDef *dma_x, uint32_t channel);

#endif /* INC_WS2812B_H_ */
