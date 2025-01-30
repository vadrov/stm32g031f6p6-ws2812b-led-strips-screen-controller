Copyright (C)2023, VadRov, all right reserved / www.youtube.com/@VadRov / www.dzen.ru/vadrov
# LED-контроллер для отображения графической информации на экране, состоящем из соединенных последовательно лент с адресными светодиодами на чипе ws2812b. LED controller for displaying graphic information on a screen consisting of strips connected in series with addressable LEDs on a ws2812b chip.
## Общая информация о проекте и технических решениях
LED-контроллер отображает на экране информацию из AVI-файла (файл с расширением avi, контейнер riff), кадры которого представляют собой изображения в формате несжатого bmp (данные пикселей в файле хранятся как двумерный массив).\
LED-контроллер собран на монтажной плате с использованием отладочного мудуля на базе микроконтроллера STM32G031F6P6/STM32G031F8P6. К микроконтроллеру подключен дисплей ST7789 (через spi), модуль (адаптер/переходник на micro-SD) SD-карты (через spi), инкрементарный механический энкодер с кнопкой. В контроллере реализован файловый менеджер с графическим интерфейсом для просмотра содержимого sd-карты и выбора файлов для воспроизведения на LED-экране. Контроллер имеет 1 выход (порт) для управления адресными светодиодными лентами. Протокол управления адресными светодиодными лентами реализован посредством PWM c DMA с таймингами управляющего сигнала, соответствующими спецификации модулей ws2812b. Для корректной передачи сигналов на первый адресный чип, выходной порт контроллера реализован по схеме открытый сток и подтянут к питанию +5В (резистор R2 на схеме), что обеспечивает уровни сигналов на входе DI, соответствующими спецификации ws2812b. Резистор R1 ограничивает ток, проходящий через выходной вывод микроконтроллера и предохраняет его от повреждения в том случае, например, если пользователь захочет обновить ПО микроконтроллера, забыв при этом отключить его от экрана с отключенным питанием (в этом случае лента будет "тянуть питание" через этот вывод, и он может повредиться при превышения допустимого значения тока).
Перемещение по файлам и каталогам в файловом менеджере осуществляется путем вращения ручки энкодера. Выбор avi-файла для воспроизведения или переход в подкаталог(возврат в каталог) осуществляется коротким нажатием (кликом) кнопки энкодера. Для остановки воспроизведения текущего файла следует длительно (2 и более секунд) удерживать нажатой, а затем отпустить кнопку энкодера.
## General information about the project and technical solutions
The LED controller displays information on the screen from an AVI file (file with avi extension, riff container), the frames of which are images in uncompressed bmp format (pixel data in the file is stored as a two-dimensional array).\
The LED controller is assembled on a circuit board using a debug module based on the STM32G031F6P6/STM32G031F8P6 microcontroller. The ST7789 display (via spi), a module (adapter/adapter to micro-SD) SD card (via spi), and an incremental mechanical encoder with a button are connected to the microcontroller. The controller implements a file manager with a graphical interface for viewing the contents of the SD card and selecting files for playback on the LED screen. The controller has 1 output (port) for controlling addressable LED strips. The control protocol for addressable LED strips is implemented using PWM with DMA with control signal timings corresponding to the specification of the ws2812b modules. To correctly transmit signals to the first address chip, the output port of the controller is implemented according to an open drain circuit and is connected to a +5V supply (resistor R2 in the diagram), which ensures signal levels at the DI input that comply with the ws2812b specification. Resistor R1 limits the current passing through the output pin of the microcontroller and protects it from damage in the event that, for example, the user wants to update the microcontroller software, but forgets to disconnect it from the screen with the power turned off (in this case, the tape will “pull power” through this output, and it may be damaged if the permissible current value is exceeded).
Moving through files and directories in the file manager is done by rotating the encoder knob. Selecting an avi file for playback or moving to a subdirectory (returning to the directory) is carried out by briefly pressing (clicking) the encoder button. To stop playback of the current file, hold down the encoder button for a long time (2 or more seconds) and then release it.
## Подготовка файлов для воспроизведения
Как было отмечено выше, LED-контроллер воспроизводит AVI-файлы (файл с расширением avi, контейнер riff). Формат файла должен соответствовать спецификации microsoft. При этом кадры видеопотока AVI-файла должны быть в формате несжатого BMP (несжатые данные RGB, хранящиеся последовательно, как двумерный массив). Для генерации последовательности BMP-изображений (цветовые эффекты, бегущие строки и т.д.), формирующих непрервывный видеоряд, можно, например, воспользоваться программой Jinx!. Настройка Jinx! заключается в определении опций матрицы (ширина и высота матрицы в пикселях - в нашем случае выражается в количестве светодиодных модулей на экране по горизонтали и вертикали), а также свойств выводного устройства (куда и как сохранять изображения). Размеры матрицы задаются через Setup->Matrix Options. 
## Preparing files for playback
As noted above, the LED controller plays AVI files (file with avi extension, riff container). The file format must comply with the microsoft specification. In this case, the frames of the AVI file video stream must be in uncompressed BMP format (uncompressed RGB data stored sequentially as a two-dimensional array). To generate a sequence of BMP images (color effects, tickers, etc.) that form a continuous video sequence, you can, for example, use the Jinx! program. Setting up Jinx! consists in determining the matrix options (the width and height of the matrix in pixels - in our case expressed in the number of LED modules on the screen horizontally and vertically), as well as the properties of the output device (where and how to save images). The matrix dimensions are set via Setup->Matrix Options.

![jinx_matrix_options](https://github.com/vadrov/stm32g031f6p6-led-lent-screen-controller/assets/111627147/c513b7a0-72bb-46d3-817d-3182883e8733)

Параметры устройства вывода определяются через Setup->Output Devices, где следует выбрать Add. Затем в списке Devices Type выбирается пункт Bitmap Export. Требуемый каталог для хранения последовательности изображений задают через кнопку Select. Выбрав и настроив требуемый эффект, можно стартовать запись кадров в указанный каталог. Для этого необходимо выбрать в меню Setup->Start Output. По просшествии требуемого периода времени, запись можно остановить, снова выбрав в меню Setup->Start Output.\
Output device parameters are determined via Setup->Output Devices, where Add should be selected. Then in the Devices Type list, select Bitmap Export. The required directory for storing the sequence of images is specified using the Select button. Having selected and configured the desired effect, you can start recording frames into the specified directory. To do this, select Setup->Start Output from the menu. After the required period of time has passed, recording can be stopped by selecting Setup->Start Output from the menu again.

![jinx_output_devices](https://github.com/vadrov/stm32g031f6p6-led-lent-screen-controller/assets/111627147/a01735f5-6dc6-463a-a37c-f295db8f879b)

Полученные кадры изображения можно "склеить" в один AVI-файл. Для этого можно использовать библиотеку ffmpeg, введя, например, такую командную строку, находясь в каталоге с картинками:
```
ffmpeg -i %8d.bmp -r 25 -vcodec bmp video.avi
где:
-i %8d.bmp - входные данные (файлы-картинки). %8d означает, что имя файла состоит из 8-значного числа;
-r 25 - частота кадров (25 кадров/секунду) в получаемом видеофайле (этот параметр указан в окне Jinx!);
-vcodec bmp - используется кодек bmp (указание на то, что используются несжатые данные RGB);
video.avi - название выходного (целевого) файла.
```
Другой способ получения требуемого формата изображения для LED-контроллера и экрана заключается в конвертации любого видеофайла той же библиотекой ffmpeg. Например:
```
ffmpeg -i file.mp4 -r 25 -vcodec bmp -s 60x20 video.avi
где:
-i file.mp4 - входной видеофайл;
-r 25 - частота кадров (25 кадров/секунду) - Вы можете, например, ограничить частоту кадров;
-vcodec bmp - используется кодек bmp (указание на то, что используются несжатые данные RGB);
-s 60x20 - размер кадра 60 на 20 пикселей (один пиксель соответствует одному светодиодному модулю);
video.avi - название выходного (целевого) файла.
```
Дополнительной опцией **-an** можно исключать звуковую дорожку из файла.\
The resulting image frames can be “glued” into one AVI file. To do this, you can use the ffmpeg library by entering, for example, the following command line while in the directory with pictures:
```
ffmpeg -i %8d.bmp -r 25 -vcodec bmp video.avi
Where:
-i %8d.bmp - input data (image files). %8d means the file name consists of an 8-digit number;
-r 25 - frame rate (25 frames/second) in the resulting video file (this parameter is specified in the Jinx window!);
-vcodec bmp - bmp codec is used (indicating that uncompressed RGB data is used);
video.avi - name of the output (target) file.
```
Another way to get the required image format for the LED controller and screen is to convert any video file with the same ffmpeg library. For example:
```
ffmpeg -i file.mp4 -r 25 -vcodec bmp -s 60x20 video.avi
Where:
-i file.mp4 - input video file;
-r 25 - frame rate (25 frames/second) - You can, for example, limit the frame rate;
-vcodec bmp - bmp codec is used (indicating that uncompressed RGB data is used);
-s 60x20 - frame size 60 by 20 pixels (one pixel corresponds to one LED module);
video.avi - name of the output (target) file.
```
With the additional option **-an** you can exclude the audio track from the file.
## Компоненты
- Отладочная плата (МК) STM32G031F6P6/STM32G031F8P6;
- SPI дисплей на контроллере ST7789 разрешением 240x240;
- адаптер/переходник с SD карты на микро-SD;
- инкрементарный механический энкодер;
- 2 резистора с номиналами: 390-680 Ом и 4.7-8.2 кОм;
- адресные светодиодные ленты для формирования экрана (в данном проекте используется 20 лент по 60 адресных светодиодов на чипе ws2812b. Итого 1200 адресных светодиода для экрана с разрешением 60х20);
- блок питания на 5В для запитывания контроллера и экрана из расчета 0.3 Вт на один модуль ws2812b (в данном проетке используется блок питания на 350 Вт. При свечении всех модулей белым цветом просадок по напряжению нет).
## Components
- Development board (MK) STM32G031F6P6/STM32G031F8P6;
- SPI display on the ST7789 controller with a resolution of 240x240;
- adapter/adapter from SD card to micro-SD;
- incremental mechanical encoder;
- 2 resistors with ratings: 390-680 Ohm and 4.7-8.2 kOhm;
- addressable LED strips for forming a screen (this project uses 20 strips of 60 addressable LEDs on a ws2812b chip. A total of 1200 addressable LEDs for a screen with a resolution of 60x20);
- a 5V power supply for powering the controller and screen at the rate of 0.3 W per ws2812b module (this project uses a 350 W power supply. When all modules are lit white, there is no voltage drop).
## Схема соединений модулей и компонентов Connection diagram of modules and components
![схема соединений LED контроллера](https://github.com/vadrov/stm32g031f6p6-led-lent-screen-controller/assets/111627147/7e9db14c-1e46-4c8c-ba71-1c4ffcef843f)
## Демонстрация работы интерфейса (файловый менеджер)   Demonstration of the interface (file manager)
https://github.com/vadrov/stm32g031f6p6-led-lent-screen-controller/assets/111627147/38e19da3-de05-4f13-ab73-b497f85633d7
## Демонстрация воспроизведения LED-контроллером мультика на экране из адресных светодиодных лент   Demonstration of how an LED controller plays a cartoon on a screen made from addressable LED strips
https://github.com/vadrov/stm32g031f6p6-led-lent-screen-controller/assets/111627147/aa63d80d-37d3-4e82-a94f-37d700e6b066
## Лицевая сторона монтажной платы   Front side of circuit board
![controller_side1](https://github.com/vadrov/stm32g031f6p6-led-lent-screen-controller/assets/111627147/383d8c5a-8d73-4b3e-b896-358a0a315c2e)
## Оборотная сторона монтажной платы   Back side of the circuit board
![controller_side2](https://github.com/vadrov/stm32g031f6p6-led-lent-screen-controller/assets/111627147/6ea11f9b-ff75-495c-a7b1-adb1f4f86806)
## Вариант LED-экрана из лент с адресными светодиодами
В данном проекте основой для крепления светодиодных лент служит обычный б/у пластиковый подоконник окрашенный в черный матовый цвет (с предварительным нанесением праймера для пластика). Пиксели экрана сформированы 20 отрезками метровых лент по 60 светодиодных модулей в каждой (всего 1200 модулей). Светодиодные ленты закреплены к основе (окрашенному подоконнику) с помощью двустороннего скотча по предварительно нанесенной разметке. Питание к лентам подведено с двух сторон, что исключает просадки напряжения в различных участках ленты.
## Option for an LED screen made from strips with addressable LEDs
In this project, the basis for attaching LED strips is an ordinary used plastic window sill painted matte black (with preliminary application of a primer for plastic). The screen pixels are formed by 20 sections of meter strips of 60 LED modules each (1200 modules in total). LED strips are attached to the base (painted window sill) using double-sided tape according to pre-applied markings. Power is supplied to the tapes from both sides, which eliminates voltage drops in different sections of the tape.
![screen](https://github.com/vadrov/stm32g031f6p6-led-lent-screen-controller/assets/111627147/9ab5b044-a5a9-4ee6-aa04-8c1c01cf9d0f)

Допускается свободное распространение. При любом способе распространения указание автора ОБЯЗАТЕЛЬНО. В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО. Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск. Автор не предоставляет никаких гарантий.\
Free distribution allowed. For any method of distribution, attribution to the author is MANDATORY. In case of changes and distribution of modifications, attribution to the original author is MANDATORY. Distributed "as is", that is, use is at your own peril and risk. The author does not provide any guarantees.

Автор проекта: **VadRov**\
Контакты: [Youtube](https://www.youtube.com/@VadRov) [Дзен](https://dzen.ru/vadrov) [VK](https://vk.com/vadrov) [Telegram](https://t.me/vadrov_channel)\
Поддержать автора: [donate.yoomoney](https://yoomoney.ru/to/4100117522443917)
