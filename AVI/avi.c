/*
 *      Author: VadRov
 */

#include "bmp.h"
#include "riff_read_header.h"
#include "keyboard.h"
#include "avi.h"
#include <stdlib.h>
#include <string.h>
#include "display.h"

#define f_abs(x)	(x < 0 ? -x : x)

extern uint32_t millis;

//загружает bmp картинку из файла на LED экран
uint8_t LED_Load_BMP (LED_Handler *led, uint16_t x, uint16_t y, uint16_t w, uint16_t h, FIL *file, uint32_t *load_bytes)
{
	bmpinfo bmphdr;
	FRESULT res;
	UINT b_read = 0;
	res = f_read(file, (void*)&bmphdr, sizeof(bmpinfo), &b_read);
	if (res != FR_OK || b_read != sizeof(bmpinfo)) return 1;
	*load_bytes = sizeof(bmpinfo);
	if (bmphdr.bfType != 0x4D42) return 2;
	if (bmphdr.biSize != 40) return 3;
	if (bmphdr.biBitCount != 24) return 4;
	if (bmphdr.biCompression != BI_RGB) return 5; //BI_RGB - данные пикселей хранятся, как двумерный массив
	int sign = 1;
	if (h > f_abs(bmphdr.biHeight)) h = f_abs(bmphdr.biHeight);
	if (w > bmphdr.biWidth) w = bmphdr.biWidth;
	uint32_t padding = 0;
	if ((3 * bmphdr.biWidth) % 4)	{			//Число байт в ряде кратно 4 байтам ?
		padding = 4 - (3 * bmphdr.biWidth) % 4; //Строки/ряды в BMP файле дополнены нолями до кратного 4 байтам размера.
												//Исходя из этого, определяем параметр выравнивания для перевода позиции чтения
												//в файле на начало данных новой строки/ряда
	}
	uint8_t colors[3 * bmphdr.biWidth + padding];
	//На начало пиксельных данных
	if (f_lseek(file, f_tell(file) + (bmphdr.bfOffBits - sizeof(bmpinfo))) != FR_OK) return 1;
	*load_bytes += bmphdr.bfOffBits - sizeof(bmpinfo);
	if (bmphdr.biHeight > 0) {
		sign = -1;
		y += h - 1;
	}
	while (h--) {
		res = f_read(file, colors, 3 * bmphdr.biWidth + padding, &b_read);
		if (res != FR_OK || b_read != 3 * bmphdr.biWidth + padding) return 1;
		*load_bytes += 3 * bmphdr.biWidth + padding;
		uint8_t *color_ptr = colors;
		for (int i = 0; i < w; i++) {
			int id = LED_GetIdByXY(led, x + i, y);
			LED_SetPixelByIdRGB(led, id, color_ptr[2], color_ptr[1], color_ptr[0]);
			color_ptr += 3;
		}
		y += sign;
	}
	return 0; //успешное завершение
}

int PlayAVI(char *dir, char *fname, LED_Handler *led, uint16_t x, uint16_t y, uint16_t win_wdt, uint16_t win_hgt)
{
	FIL file; //Файловый объект
	//"Склеим" путь до файла с именем файла
	char *tmp = malloc(strlen(dir) + strlen(fname) + 2);
	strcpy(tmp, dir);
	strcat(tmp, "/");
	strcat(tmp, fname);
	//Откроем файл для чтения (FA_READ) с проверкой на ошибку при открытии res
	FRESULT res = f_open(&file, tmp, FA_READ);
	free(tmp);
	if (res != FR_OK) return 1; //Ошибка открытия файла
	RiffHDR *RHdr = calloc(1, sizeof(RiffHDR)); //Объявим обработчик riff
	//Теперь проверим, а дествительно ли это AVI файл. Если это он, то считаем все необходимые для
	//воспроизведения параметры
	if (Read_RIFF_Header(RHdr, &file) != RIFF_OK) {
		free(RHdr);
		f_close(&file);
		return 2;
	}
	if (RHdr->mediaType != FourccType_AVI ||
		RHdr->vidFmt.biCompression != 0x00000000) { //AVI с кадрами без сжатия (BI_RGB)
		if (RHdr->AVIStreams) free(RHdr->AVIStreams);
		free(RHdr);
		f_close(&file);
		return 3;
	}
	//Указатель чтения файла перемещаем на первый кадр данных
	f_lseek(&file, RHdr->startPosData);
	//Объявим структуру, которая определяет загрузчик (чанк, chunk, "кусок") кадра в потоках avi
	struct {
		uint8_t streamCode[4];	//Тип кадра, по которому мы будем определять, с каким кадром
								//имеем дело (с видеокадром или аудиокадром)
		uint32_t len;			//Длина данных кадра. Она должна быть кратна 2, если нет, то мы должны
								//увеличить на 1 это значение.
	} chunk;
	uint32_t MksPerFrame = RHdr->AVIHeader.dwMicroSecPerFrame / 1000; //Время отображения одного видеокадра, мс
	if (RHdr->AVIStreams) free(RHdr->AVIStreams);
	free(RHdr);
	UINT read_b = 0; //Переменная для фактического числа считанных байт из файлового потока
	uint8_t buff[16], fl_video_frame;
	uint32_t t_frame; //Время, фактически затраченное на отображение кадра, мс
	while (f_tell(&file) < f_size(&file)) { //В цикле читаем кадры из файла до тех пор, пока не будет достигнут его конец
		t_frame = millis;
		//Читаем загрузчик кадра - чанк (в нем информация о типе и длине кадра).
		res = f_read(&file, &chunk, 8, &read_b);
		if (res || read_b != 8) break; //Если возникла ошибка при чтении - прерываем цикл
		//-------------------------- проверка на следующий "кусок" AVI файла --------------------------
		//Если avi файл больше 1 Гб, то видео может быть "порезано" на "куски", начинающиеся с чанка RIFF
		//формат заголовка "куска" "RIFF[nnnn]AVIXLIST[mmmm]movi, где:
		//nnnn и mmmm длины чанков. После чанка movi продолжают следовать кадры.
		if (*((uint32_t*)chunk.streamCode) == chunk_RIFF) { //Обнаружили новый "кусок" AVI?
			res = f_read(&file, buff, 16, &read_b);
			if (res || read_b != 16) break; //Если возникла ошибка при чтении - прерываем цикл.
			if ( *((uint32_t*)&buff[0]) == TYPE_AVIX  &&   //Проверяем формат заголовка "куска"
				 *((uint32_t*)&buff[4]) == chunk_LIST &&
				 *((uint32_t*)&buff[12]) == chunk_movi   ) continue; //Успех -> возобновление чтения кадров.
				else break; //Формат заголовка продолжения avi неверный -> прерываем цикл, завершая работу плеера.
		}
		if (chunk.len & 1) chunk.len++; //Размер данных кадра должен быть четным.
		uint32_t load_bytes = 0;
		fl_video_frame = 0; //Флаг видеокадра. Говорит о том, является ли текущий кадр в потоке видеокадром или нет.
		if ((chunk.streamCode[2]=='d' && chunk.streamCode[3]=='c') ||
			(chunk.streamCode[2]=='D' && chunk.streamCode[3]=='C')	 ) { //Проверка на то, что текущий кадр является видеокадром.
			fl_video_frame = 1; //Текущий кадр в потоке является видеокадром
			if (!chunk.len) continue; //Пустой кадр пропускаем.
			LED_DeviceWaitUpdate(led);
			uint8_t res1 = LED_Load_BMP(led, x, y, win_wdt, win_hgt, &file, &load_bytes);
			LED_DeviceUpdate(led);
			if (res1) break; //В случае ошибки вывода картинки выходим
		}
		if (load_bytes != chunk.len) { //Если обработали не все данные видеокадра, то оставшиеся данные пропускаем.
			if (f_lseek(&file, f_tell(&file) + chunk.len - load_bytes) != FR_OK) break;
		}
		//Проверка на наличие события от энкодера.
		if (KEYB_kbhit()) {
			uint16_t keys = KEYB_Inkeys();	//Считываем ключи "кнопок"-событий энкодера.
			if (keys & (1 << KEYB_LEFT)) {	//Удержание кнопки -> принудительное завершение воспроизведения.
				f_close(&file); //Закрываем файл
				//"Гасим" изображение на устройстве
				LED_DeviceWaitUpdate(led); 	//Ожидаем завершения обновления данных на устройстве
				LED_FillColor(led, 0);		//Задаем 0 цвет всем компонентам
				LED_DeviceUpdate(led);		//Разрешаем обновление данных на устройстве
				return 4;
			}
			if (keys & (1 << KEYB_DOWN)) {   //Увеличение яркости устройства.
				if (led->bright < 255) {
					led->bright++;
				}

			}
			if (keys & (1 << KEYB_UP)) {	//Уменьшение яркости устройства.
				if (led->bright > 0) {
					led->bright--;
				}
			}
		}
		//Синхронизация (пауза перед отображением следующего кадра) с допуском в пределах длительности показа одного видеокадра.
		//Применяется только для кадров потока, являющихся видеокадрами (флаг fl_video_frame установлен).
		t_frame = millis - t_frame;
		if (MksPerFrame > t_frame && fl_video_frame) {
			t_frame = MksPerFrame - t_frame;
			LL_mDelay(t_frame);
		}
	}
	f_close(&file); //Закрываем файл
	//"Гасим" изображение на устройстве
	LED_DeviceWaitUpdate(led); 	//Ожидаем завершения обновления данных на устройстве
	LED_FillColor(led, 0);		//Задаем 0 цвет всем компонентам
	LED_DeviceUpdate(led);		//Разрешаем обновление данных на устройстве
	return 0;
}
