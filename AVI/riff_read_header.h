/*
 *  Библиотека работы с аудио-форматами (кодеки)
 *
 *  Author: VadRov
 *  Copyright (C) 2019, VadRov, all right reserved.
 *
 *  Допускается свободное распространение.
 *  При любом способе распространения указание автора ОБЯЗАТЕЛЬНО.
 *  В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО.
 *  Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */
#ifndef RIFF_READ_HEADER_H_
#define RIFF_READ_HEADER_H_

#include "ff.h"
#include "main.h"

#define CHUNK_NAME(c1,c2,c3,c4)  (((uint32_t)c4<<24)+((uint32_t)c3<<16)+((uint16_t)c2<<8)+(uint8_t)c1)

#define chunk_RIFF		CHUNK_NAME('R','I','F','F')
#define chunk_fmt		CHUNK_NAME('f','m','t',' ')
#define chunk_data		CHUNK_NAME('d','a','t','a')
#define chunk_DISP		CHUNK_NAME('D','I','S','P')
#define chunk_LIST		CHUNK_NAME('L','I','S','T')
#define chunk_fact		CHUNK_NAME('f','a','c','t')
#define chunk_PEAK		CHUNK_NAME('P','E','A','K')
#define chunk_INFO		CHUNK_NAME('I','N','F','O')
#define chunk_id3		CHUNK_NAME('i','d','3',' ')

#define chunk_hdrl		CHUNK_NAME('h','d','r','l')
#define chunk_avih		CHUNK_NAME('a','v','i','h')
#define chunk_strl		CHUNK_NAME('s','t','r','l')
#define chunk_strh		CHUNK_NAME('s','t','r','h')
#define chunk_strf		CHUNK_NAME('s','t','r','f')
#define chunk_vids		CHUNK_NAME('v','i','d','s')
#define chunk_auds		CHUNK_NAME('a','u','d','s')
#define chunk_JUNK		CHUNK_NAME('J','U','N','K')
#define chunk_movi		CHUNK_NAME('m','o','v','i')
#define chunk_idx1		CHUNK_NAME('i','d','x','1')

#define TYPE_AVI  		CHUNK_NAME('A','V','I',' ')
#define TYPE_AVIX  		CHUNK_NAME('A','V','I','X')
#define TYPE_WAVE 		CHUNK_NAME('W','A','V','E')
#define TYPE_RMID 		CHUNK_NAME('R','M','I','D')
#define TYPE_RDIB 		CHUNK_NAME('R','D','I','B')
#define TYPE_RMMP 		CHUNK_NAME('R','M','M','P')
#define TYPE_PAL  		CHUNK_NAME('P','A','L',' ')

//Статусы выполнения процедуры чтения заголовка wav файла
typedef enum {
		RIFF_OK = 0,		//Заголовок прочитан без ошибок
		RIFF_FMT_ERROR,   	//Ошибка в формате
		RIFF_IO_ERROR,		//Ошибка чтения заголовка файла
		RIFF_PCM_ERROR,		//Данные закодированы кодеком (формат не PCM)
		RIFF_CNL_ERROR,		//Ошибка в числе каналов воспроизведения
		RIFF_BPS_ERROR,		//Ошибка в разрядности оцифровки данных
		RIFF_SR_ERROR		//Ошибка в частоте дискретизации
} RIFF_HDR_STATUS;
//-----------------------------------------------------------------------------------------------------------------------------

typedef enum {
	FourccType_AVI,
	FourccType_WAVE,
	FourccType_RMID,
	FourccType_RDIB,
	FourccType_RMMP,
	FourccType_PAL,
	FourccType_Unknow
} FourccType;

typedef struct {
	uint32_t chunk_id;
	uint32_t chunk_size;
	uint32_t chunk_type;
} Riff_Chunk;


#pragma pack(push, 1) //Выравнивание 1 байт
typedef struct
{
	uint32_t ChunID; 					//Должен быть 'avih'
	uint32_t ChunkSize; 				//Размер этой структуры данных, исключая первые 8 байтов (поля ID и Size)
	uint32_t dwMicroSecPerFrame; 		//Время, необходимое для отображения каждого кадра, мкс
	uint32_t dwMaxBytesPerSec; 			//Максимальная скорость передачи данных
	uint32_t dwPaddingGranularity; 		//Длина блока записи должна быть кратна этому значению, обычно 2048
	uint32_t dwFlages; 					//Специальные атрибуты файла AVI, например, содержит ли он индексные блоки и хранятся ли аудио и видео данные поперек
	uint32_t dwTotalFrame; 				//Количество кадров в файле
	uint32_t dwInitialFrames; 			//Cколько нужно кадров перед началом воспроизведения
	uint32_t dwStreams; 				//Количество потоков данных, содержащихся в файле
	uint32_t dwSuggestedBufferSize; 	//Рекомендуемый размер буфера
	uint32_t dwWidth; 					//Ширина изображения
	uint32_t dwHeight; 					//Высота изображения
	uint32_t dwReserved[4]; 			//Зарезервированные значения
} MainAVIHeader;

typedef struct
{
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
} RECT;

typedef struct
{
	 uint32_t fccType; 				 //4 байта, определяют тип потока данных, vids - поток видео данных, а auds - поток аудио данных
	 uint32_t fccHandler; 			 //4 байта, определяют код драйвера для декодирования потока данных
	 uint32_t dwFlags; 				 //Атрибуты потока данных
	 uint16_t wPriority; 			 //Приоритет этого потока данных
	 uint16_t wLanguage; 			 //Код языка аудио
	 uint32_t dwInitalFrames;		 //Сколько нужно кадров, прежде чем начать воспроизведение
	 uint32_t dwScale; 				 //Объем данных, размер каждого кадра видео или размер выборки аудио
	 uint32_t dwRate; 				 //dwScale / dwRate = количество выборок в секунду
	 uint32_t dwStart;				 //Позиция, с которой начинает воспроизводиться поток данных, в dwScale
	 uint32_t dwLength; 			 //Объем данных потока данных, в dwScale
	 uint32_t dwSuggestedBufferSize; //Рекомендуемый размер буфера
	 uint32_t dwQuality; 			 //Параметр качества. Чем больше это значение, тем выше качество
	 uint32_t dwSampleSize; 		 //Размер выборки аудио
	 RECT rcFrame; 					 //Прямоугольник, занимаемый кадром видеоизображения
} AVIStreamHeader;

typedef struct
{
	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
} BITMAPINFOHEADER;

typedef struct
{
	uint16_t wFormatTag;
	uint16_t nChannels; 			//Количество каналов
	uint32_t nSamplesPerSec; 		//Частота выборки
	uint32_t nAvgBytesPerSec; 		//Количество данных в секунду в звуке WAVE
	uint16_t nBlockAlign; 			//Флаг выравнивания блока данных
	uint16_t biSize; 				//Размер этой структуры
} WAVEFORMAT;
#pragma pack(pop)

//структура содержит информацию о некоторых параметрах wav файла
typedef struct {
	FourccType mediaType;			//Тип медиа данных потока, например FourccType_WAVE - звуковой файл в формате WAV
	MainAVIHeader AVIHeader;  		//Хидер avi
	AVIStreamHeader *AVIStreams; 	//Хидеры потоков avi (видео, аудио, текст)
	uint8_t AllStreams;				//Количество потоков avi
	BITMAPINFOHEADER vidFmt;		//Параметры видеопотока
	WAVEFORMAT audFmt;				//Параметры аудио потока
	FSIZE_t startPosIdx1;   		//Стартовая позиция индексных данных в файле
	uint32_t lenghtIdx1;    		//Длина индексных данных
	FSIZE_t startPosData;   		//Стартовая позиция блока данных в файле
	uint32_t lenghtData;    		//Длина блока данных
} RiffHDR;

RIFF_HDR_STATUS Read_RIFF_Header (RiffHDR *RHdr, FIL *file);

#endif /* RIFF_READ_HEADER_H_ */
