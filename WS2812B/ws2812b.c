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

#include "ws2812b.h"
#include <stdlib.h>

LED_Handler *LED_devices = 0; //список устройств: экранов, лент...

/*
 * Создает порт управления (требуется канал таймера, настроенный как PWM выход с подключенным DMA каналом), через
 * который осуществляется управление заданным количеством светодиодов) и возвращает указатель на него.
 * Каждый порт может содержать указатели на предыдующий и следующий порты, если они есть (так формируются списки
 * портов для создаваемого устройства).
 * Параметры:
 * ports - указатель на список портов (или NULL), к которому будет присоединен (если список не NULL) вновь создаваемый порт;
 * diodes_all - количество диодов, которые управляются через создаваемый порт;
 * tim - указатель на таймер, на основе которого реализован порт;
 * tim_channel - канал таймера - выход создаваемого порта (настроен на генерацию ШИМ/PWM);
 * dma - контроллер DMA, обслуживающий канал таймера;
 * dma_channel - канал контроллера DMA, обслуживающий PWM канал таймера.
 */
LED_Port* LED_AddNewPort(LED_Port *ports, uint16_t diodes_all, TIM_TypeDef *tim, uint32_t tim_channel, DMA_TypeDef *dma, uint32_t dma_channel)
{
	if (!diodes_all || !tim || !dma) return 0;
	LED_Port *port = (LED_Port *)calloc(1, sizeof(LED_Port));
	port->all_diodes = diodes_all;
	port->timer = tim;
	port->channel = tim_channel;
	port->dma = dma;
	port->dma_channel = dma_channel;
	port->next = 0;
	port->prev = 0;
	port->diodes_reset = LED_DIODES_RESET;	//По спецификации на ws2812b длительность логического нуля для RESET
											//должна быть не менее 50 мкс.
	if (!ports) return port;
	LED_Port *prev = ports;
	while (prev->next) {
		prev = (LED_Port*)prev->next;
	}
	port->prev = (void*)prev;
	prev->next = (void*)port;
	return port;
}

//удаляет порт
void LED_DeletePort(LED_Port *port)
{
	if (port) free(port);
}

//Разрешение обновления данных на устройстве
void LED_DeviceUpdate(LED_Handler *led)
{
	LED_Port *port = led->ports;
	while (port) {
		switch (port->channel) {
			case LL_TIM_CHANNEL_CH1:
				port->timer->DIER |= TIM_DIER_CC1DE;
				break;
			case LL_TIM_CHANNEL_CH2:
				port->timer->DIER |= TIM_DIER_CC2DE;
				break;
			case LL_TIM_CHANNEL_CH3:
				port->timer->DIER |= TIM_DIER_CC3DE;
				break;
			case LL_TIM_CHANNEL_CH4:
				port->timer->DIER |= TIM_DIER_CC4DE;
				break;
			default:
				break;
		}
		port = port->next;
	}
}

//Ожидание обновления данных на устройстве
void LED_DeviceWaitUpdate(LED_Handler *led)
{
	uint8_t f_update = 1;
	while (f_update) {
		LED_Port *port = led->ports;
		f_update = 0;
		while (port) {
			switch (port->channel) {
				case LL_TIM_CHANNEL_CH1:
					if (port->timer->DIER & TIM_DIER_CC1DE) f_update++;
					break;
				case LL_TIM_CHANNEL_CH2:
					if (port->timer->DIER & TIM_DIER_CC2DE) f_update++;
					break;
				case LL_TIM_CHANNEL_CH3:
					if (port->timer->DIER & TIM_DIER_CC3DE) f_update++;
					break;
				case LL_TIM_CHANNEL_CH4:
					if (port->timer->DIER & TIM_DIER_CC4DE) f_update++;
					break;
				default:
					break;
			}
			port = port->next;
		}
	}
}

/*
 * Создает обработчик LED устройства (экрана, ленты...) и возвращает указатель на него.
 * Интервал времени в 30 мкс, необходимый для передачи данных о цвете на 1 светодиод,
 * ограничивает частоту обновления данных изображения. Так, 1024 диода с учетом сигнала
 * обновления/RESET (60 мкс в данном случае, и не менее 50 мкс по даташиту) мы можем обновлять
 * с частотой 32,5 раз в секунду. Для сохранения частоты обновления кадров на приемлемом уровне
 * используются порты, каждому из которых при создании назначается заданное количество диодов,
 * объединенных в одну цепочку, соответствующую одному из возможных вариантов LED_DISPLAY_LENT_TYPES.
 * Т.е. с использованием 3 портов по 1024 диода на каждый порт мы сможем обновлять картинку
 * с теми же 32,5 Гц, но разрешение картинки при этом в 3 раза выше - 3072 диода. Таким образом,
 * одно устройство (экран) может управляться через несколько портов, объединенных, в т.н.,
 * список портов. При этом схема соединений диодов LED_DISPLAY_LENT_TYPES для каждого из портов
 * одного устройства должна быть одинаковой.
 * Параметры:
 * orient - один из 8 вариантов LED_DISPLAY_LENT_TYPES формирования устройства из диодов;
 * diodes_in_line -  количество диодов на одну линию (вертикальную либо горизонтальную, в зависимости
 * от вида схемы формирования экрана). Если в выбранной схеме orient вначале идет указание на
 * горизонтальное направление (LED_LEFTtoRIGHT_... или LED_RIGHTtoLEFT_...), то diodes_in_line
 * определяет разрешение устройства по горизонтали. В ином случае (LED_UPtoDOWN_... или LED_DOWNtoUP_...)
 * diodes_in_line определяет разрешение устройства по вертикали. Второй параметр разрешения устройства -
 * разрешение по вертикали/горизонтали определяется, как отношение общего количество диодов устройства
 * (сумма диодов всех портов, "приписанных" к устройству) к diodes_in_line;
 * bright - яркость (0 - 255) для всех диодов устройства. Этот параметр ограничивает заданным пределом
 * составляющие цвета (r, g, b) всех диодов устройства. Все цветовые составляющие масштабируются к этому
 * значению. Масштабирование приводит к искажению цвета, но позволяет ограничивать токопотребление устройства;
 * ports - указатель на порт либо список портов, управляющих создаваемым устройством.
 */
LED_Handler* LED_CreateDevice (LED_DISPLAY_LENT_TYPES orient, uint16_t diodes_in_line, uint8_t bright, LED_Port *ports)
{
	if (!ports) return 0;									//портов нет - уходим
	LED_Handler *led = (LED_Handler *)calloc(1, sizeof(LED_Handler)); 	//выделение памяти под обработчик
	led->lent_orient = orient;
	led->bright = bright;
	led->ports = ports;
	led->all_diodes = 0;
	int first_diode_id = 0;
	LED_Port *port = ports;
	while (port) {
		led->all_diodes += port->all_diodes;
		port->first_diode_num = first_diode_id;
		port->current_diode = port->count_res_diodes = 0;
		port->fl_new_frame = 1;
		first_diode_id += port->all_diodes;
		port = (LED_Port*)port->next;
	}
	uint16_t width;
	uint16_t height;
	uint16_t two_size = !diodes_in_line ? 1 : led->all_diodes/diodes_in_line; //размер второй стороны экрана
	switch (orient) {		//определение параметров экрана в зависимости от схемы его формирования
		case LED_LEFTtoRIGHT_UPtoDOWN:
		case LED_LEFTtoRIGHT_DOWNtoUP:
		case LED_RIGHTtoLEFT_UPtoDOWN:
		case LED_RIGHTtoLEFT_DOWNtoUP:
			width = diodes_in_line;
			height = two_size;
			break;
		case LED_UPtoDOWN_LEFTtoRIGHT:
		case LED_UPtoDOWN_RIGHTtoLEFT:
		case LED_DOWNtoUP_LEFTtoRIGHT:
		case LED_DOWNtoUP_RIGHTtoLEFT:
			width = two_size;
			height = diodes_in_line;
			break;
		default:
			free(led);
			return NULL;
	}
	led->height = height;
	led->width = width;
	led->pixel_buffer = (uint8_t*)calloc(3*led->all_diodes, sizeof(uint8_t)); //выделение памяти под экранную область
	if (!led->pixel_buffer)	{
		free(led);
		return NULL;
	}

	if (!LED_devices) {
		LED_devices = led;
		return led;
	}
	LED_Handler *prev = LED_devices;
	while (prev->next) {
		prev = (LED_Handler*)prev->next;
	}
	led->prev = (void*)prev;
	prev->next = (void*)led;
	return led;
}

//удаляет обработчик LED экрана
void LED_DeleteDevice(LED_Handler *led)
{
	if (led) {
		if (led->pixel_buffer) free(led->pixel_buffer);
		free(led);
	}

}

//Старт отображения LED панели (запуск PWM канала таймера с DMA)
void LED_StartDevice(LED_Handler *led)
{
	LED_Port *port = led->ports;
	while (port) {
		//Определяем базовый адрес канала DMA
		DMA_Channel_TypeDef *dma = ((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)port->dma + CHANNEL_OFFSET_TAB[port->dma_channel])));
		//Сбрасываем все флаги прерываний заданного DMA канала
		port->dma->IFCR |= DMA_IFCR_CGIF1 << (port->dma_channel * 4);
		//Разрешение запроса к DMA от соответствующего канала захвата/сравнения
		//Запись адреса регистра соответствующего канала захвата/сравнения в адресный регистр периферии DMA
		switch (port->channel) {
			case LL_TIM_CHANNEL_CH1:
				port->timer->DIER |= TIM_DIER_CC1DE;
				dma->CPAR = (uint32_t)&port->timer->CCR1;
				break;
			case LL_TIM_CHANNEL_CH2:
				port->timer->DIER |= TIM_DIER_CC2DE;
				dma->CPAR = (uint32_t)&port->timer->CCR2;
				break;
			case LL_TIM_CHANNEL_CH3:
				port->timer->DIER |= TIM_DIER_CC3DE;
				dma->CPAR = (uint32_t)&port->timer->CCR3;
				break;
			case LL_TIM_CHANNEL_CH4:
				port->timer->DIER |= TIM_DIER_CC4DE;
				dma->CPAR = (uint32_t)&port->timer->CCR4;
				break;
			default:
				return;
		}

		//Разрешаем прерывания по окончанию передачи, передаче половины данных и ошибке
		dma->CCR |= (DMA_CCR_TCIE | DMA_CCR_HTIE | DMA_CCR_TEIE);

		port->timer->CCER |= port->channel;	//Включение выхода канала захвата/сравнения
		port->timer->BDTR |= TIM_BDTR_MOE;	//Включение всех выходов всех каналов таймера
		port->timer->CR1  |= TIM_CR1_CEN;	//Включение счетчика таймера

		dma->CMAR = (uint32_t)port->bit_buffer; //Запись адреса блока данных в адресный регистр памяти DMA
		dma->CNDTR = 48; //Запись размера блока данных в регистр количества данных для DMA передачи
		dma->CCR |= DMA_CCR_EN; //Включение канала DMA
		port = (LED_Port*)port->next;
	}
}

//Остановка обновления LED панели
void LED_StopDevice(LED_Handler *led)
{
	LED_FillColor(led, 0); //"Гасим" все светодиоды панели.
	LL_mDelay(200);		   //Гарантированная задержка для обновления данных на всех
						   //диодах панели.
	LED_Port *port = led->ports;
	while (port) {
		//Запрет запроса к DMA от соответствующего канала захвата/сравнения
		switch (port->channel) {
			case LL_TIM_CHANNEL_CH1:
				port->timer->DIER &= ~TIM_DIER_CC1DE;
				break;
			case LL_TIM_CHANNEL_CH2:
				port->timer->DIER &= ~TIM_DIER_CC2DE;
				break;
			case LL_TIM_CHANNEL_CH3:
				port->timer->DIER &= ~TIM_DIER_CC3DE;
				break;
			case LL_TIM_CHANNEL_CH4:
				port->timer->DIER &= ~TIM_DIER_CC4DE;
				break;
			default:
				return;
		}
		//Определяем базовый адрес канала DMA
		DMA_Channel_TypeDef *dma = ((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)port->dma + CHANNEL_OFFSET_TAB[port->dma_channel])));
		dma->CCR &= ~DMA_CCR_EN;				//Выключение канала DMA
		port->timer->CCER &= ~port->channel;	//Выключение выхода канала захвата/сравнения
		port->timer->BDTR &= ~TIM_BDTR_MOE;		//Выключение всех выходов всех каналов таймера
		port->timer->CR1 &= ~TIM_CR1_CEN;		//Выключение счетчика таймера
		port = (LED_Port*)port->next;
	}
}

void LED_SetBright(LED_Handler *led, uint8_t bright)
{
	led->bright = bright;
}

uint8_t LED_GetBright(LED_Handler *led)
{
	return led->bright;
}

void LED_SetPixelByIdRGB(LED_Handler *led, int id, uint8_t r, uint8_t g, uint8_t b)
{
	if (id < 0 || id >= led->all_diodes) return;
	uint8_t *ptr = led->pixel_buffer + 3*id;
	*ptr++ = r;
	*ptr++ = g;
	*ptr   = b;
}

//HSV цветовая модель (тон, насыщенность, значение/яркость)
//тон 0-359,  насыщенность 0-255, значение/яркость 0-255
void LED_SetPixelByIdHSV(LED_Handler *led, int id, uint16_t Hue, uint8_t Saturation, uint8_t Brightness)
{
	uint16_t Sector, Fracts, p, q, t;
	uint8_t r = 0, g = 0, b = 0;
	if(Saturation == 0)	{
		r = g = b = Brightness;
	}
	else {
		if(Hue >= 360) Hue = 359;
		Sector = Hue / 60;
		Fracts = Hue % 60;
		p = (Brightness * (255 - Saturation)) >> 8;
		q = (Brightness * (255 - (Saturation * Fracts)/60)) >> 8;
		t = (Brightness * (255 - (Saturation * (59 - Fracts))/60)) >> 8;
		switch(Sector) {
			case 0:
				r = Brightness;
				g = (uint8_t)t;
				b = (uint8_t)p;
				break;
			case 1:
				r = (uint8_t)q;
				g = Brightness;
				b = (uint8_t)p;
				break;
			case 2:
				r = (uint8_t)p;
				g = Brightness;
				b = (uint8_t)t;
				break;
			case 3:
				r = (uint8_t)p;
				g = (uint8_t)q;
				b = Brightness;
				break;
			case 4:
				r = (uint8_t)t;
				g = (uint8_t)p;
				b = Brightness;
				break;
			default:
				r = Brightness;
				g = (uint8_t)p;
				b = (uint8_t)q;
				break;
		}
	}
	LED_SetPixelByIdRGB(led, id, r, g, b);
}

void LED_FillColor(LED_Handler *led, uint32_t color)
{
	uint8_t *tmp =(uint8_t*)&color;
	uint8_t r = tmp[3], g = tmp[2], b = tmp[1];
	for (int i = 0; i < led->all_diodes; i++)
		LED_SetPixelByIdRGB(led, i, r, g, b);
}

uint32_t LED_GetPixelById(LED_Handler *led, int id)
{
	if (id < 0 || id >= led->all_diodes) return 0;
	uint8_t r, g, b;
	uint8_t *ptr = led->pixel_buffer + 3*id;
	r = *ptr++;
	g = *ptr++;
	b = *ptr;
	return (r<<16) | (g<<8) | b;
}

uint32_t LED_GetPixelByXY(LED_Handler *led, uint16_t x, uint16_t y)
{
	int id = LED_GetIdByXY(led, x, y);
	if (id < 0) return 0;
	return LED_GetPixelById(led, id);
}

void LED_SetPixelByXY(LED_Handler *led, uint16_t x, uint16_t y, uint32_t color)
{
	int id = LED_GetIdByXY(led, x, y);
	uint8_t *tmp =(uint8_t*)&color;
	LED_SetPixelByIdRGB(led, id, tmp[3], tmp[2], tmp[1]);
}

void LED_SetPixelByXY_HSV(LED_Handler *led, uint16_t x, uint16_t y, uint16_t Hue, uint8_t Saturation, uint8_t Brightness)
{
	int id = LED_GetIdByXY(led, x, y);
	if (id < 0) return;
	LED_SetPixelByIdHSV(led, id, Hue, Saturation, Brightness);
}

int LED_GetIdByXY(LED_Handler *led, uint16_t x, uint16_t y)
{
	int id = -1;
	if (x >= led->width || y >= led->height) return id;
	switch (led->lent_orient) {
		case LED_LEFTtoRIGHT_UPtoDOWN:
			id = led->width * y;
			id += y & 1 ? led->width - x - 1 : x;
			break;
		case LED_LEFTtoRIGHT_DOWNtoUP:
			id = led->width * (led->height - y - 1);
			if (led->height & 1)
				id += y & 1 ? led->width - x - 1 : x;
			else
				id += y & 1 ? x : led->width - x - 1;
			break;
		case LED_RIGHTtoLEFT_UPtoDOWN:
			id = led->width * y;
			id += y & 1 ? x : led->width - x - 1;
			break;
		case LED_RIGHTtoLEFT_DOWNtoUP:
			id = led->width * (led->height - y - 1);
			if (led->height & 1)
				id += y & 1 ? x : led->width - x - 1;
			else
				id += y & 1 ? led->width - x - 1 : x;
			break;
		case LED_UPtoDOWN_LEFTtoRIGHT:
			id = led->height*x;
			id += x & 1 ? led->height - y - 1 : y;
			break;
		case LED_UPtoDOWN_RIGHTtoLEFT:
			id = led->height*(led->width - x - 1);
			if (led->width & 1)
				id += x & 1 ? led->height - y - 1 : y;
			else
				id += x & 1 ? y : led->height - y - 1;
			break;
		case LED_DOWNtoUP_LEFTtoRIGHT:
			id = led->height*x;
			id += x & 1 ? y : led->height - y - 1;
			break;
		case LED_DOWNtoUP_RIGHTtoLEFT:
			id = led->height*(led->width - x - 1);
			if (led->width & 1)
				id += x & 1 ? y : led->height - y - 1;
			else
				id += x & 1 ? led->height - y - 1 : y;
			break;
		default:
			break;
	}
	return id;
}

#ifdef WS2812_GAMMA_CORRECTION
const uint8_t gamma_corr[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
	    						  2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
								  10, 11, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21,
								  22, 23, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 35, 35, 36, 37, 38,
								  38, 39, 40, 41, 42, 42, 43, 44, 45, 46, 47, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 56, 57, 58,
								  59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 84,
								  85, 86, 87, 88, 89, 91, 92, 93, 94, 95, 97, 98, 99, 100, 102, 103, 104, 105, 107, 108, 109, 111,
								  112, 113, 115, 116, 117, 119, 120, 121, 123, 124, 126, 127, 128, 130, 131, 133, 134, 136, 137,
								  139, 140, 142, 143, 145, 146, 148, 149, 151, 152, 154, 155, 157, 158, 160, 162, 163, 165, 166,
								  168, 170, 171, 173, 175, 176, 178, 180, 181, 183, 185, 186, 188, 190, 192, 193, 195, 197, 199,
								  200, 202, 204, 206, 207, 209, 211, 213, 215, 217, 218, 220, 222, 224, 226, 228, 230, 232, 233,
								  235, 237, 239, 241, 243, 245, 247, 249, 251, 253, 255 };
#endif
void LED_Callback(DMA_TypeDef *dma_x, uint32_t channel)
{
	int offset = -1;
	if (dma_x->ISR & (DMA_ISR_HTIF1 << (channel * 4))) {
		offset = 0;
	}
	else if (dma_x->ISR & (DMA_ISR_TCIF1 << (channel * 4))) {
		offset = 1;
	}
	dma_x->IFCR |= DMA_IFCR_CGIF1 << (channel * 4);
	if (offset < 0) return;
	LED_Handler *led = LED_devices;
	while (led) {
		LED_Port *port = led->ports;
		while (port) {
			if (port->dma == dma_x && port->dma_channel == channel) {
				uint32_t tmp;
				uint8_t r, g, b;
				uint8_t *color = led->pixel_buffer + 3 * (port->first_diode_num + port->current_diode);
				uint16_t *bitbuf = port->bit_buffer + 24 * offset;
#ifndef WS2812_GAMMA_CORRECTION
				uint16_t bright = led->bright + 1;
#endif
				if (!port->fl_new_frame) { //Формирование данных для обновления информации чипов о цвете
#ifdef WS2812_GAMMA_CORRECTION
					r = gamma_corr[color[0]]; //корректирование яркости
					g = gamma_corr[color[1]];
					b = gamma_corr[color[2]];

#else
					r = (color[0] * bright)>>8; //Ограничиваем яркость
					g = (color[1] * bright)>>8; //bright определяет максимальную яркость,
					b = (color[2] * bright)>>8; //т.е. составляющие цвета "масштабируются" с учетом макс. яркости
#endif
					tmp = (g << 16) | (r << 8) | b; //порядок цветовых составляющих чипа ws2812b - G, R, B
					for (int i = 0; i < 24; i++) { 	//цикл по количеству битов, составляющих цвет (24 бита)
						bitbuf[i] = tmp & 0x800000 ? LED_HIGH_LEVEL : LED_LOW_LEVEL; //скважность "1" либо "0" в зависимости от состояния бита D23
						tmp <<= 1; //сдвигаем биты влево; для чипа ws2812b последовательность приема данных от старшего бита к младшему
					}
					port->current_diode++;
					if (port->current_diode >= port->all_diodes) {
						port->current_diode = 0;
						port->fl_new_frame = 1;
					}
				}
				else {	//Формирование сигнала RESET для смены данных.
					for (int i = 0; i < 24; i++) {
						bitbuf[i] = LED_RESET;
					}
					port->count_res_diodes++;
					if (port->count_res_diodes >= port->diodes_reset) {
						port->fl_new_frame = port->count_res_diodes = 0;
						switch (port->channel) {
							case LL_TIM_CHANNEL_CH1:
								port->timer->DIER &= ~TIM_DIER_CC1DE;
								break;
							case LL_TIM_CHANNEL_CH2:
								port->timer->DIER &= ~TIM_DIER_CC2DE;
								break;
							case LL_TIM_CHANNEL_CH3:
								port->timer->DIER &= ~TIM_DIER_CC3DE;
								break;
							case LL_TIM_CHANNEL_CH4:
								port->timer->DIER &= ~TIM_DIER_CC4DE;
								break;
							default:
								break;
						}
					}
				}
				return;
			}
			port = (LED_Port*)port->next;
		}
		led = (LED_Handler*)led->next;
	}
}

void LED_DrawImage(LED_Handler *led, uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint32_t *buf)
{
	for (uint16_t y = y0; y < y0 + height; y++) {
		for (uint16_t x = x0; x < x0 + width; x++) {
			LED_SetPixelByXY(led, x, y, *buf);
			buf++;
		}
	}
}

void LED_DrawFilledRectangle(LED_Handler *led, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color)
{
	uint16_t tmp;
	if (x0 > x1) {
		tmp = x0;
		x0 = x1;
		x1 = tmp;
	}
	if (y0 > y1) {
		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}
	for (uint16_t y = y0; y <= y1; y++) {
		for (uint16_t x = x0; x <= x1; x++)	{
			LED_SetPixelByXY(led, x, y, color);
		}
	}
}
