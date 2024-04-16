Copyright (C)2023, VadRov, all right reserved / www.youtube.com/@VadRov / www.dzen.ru/vadrov
# LED-контроллер для отображения графической информации на экране, сформированном путем последовательного соединения лент с адресными светодиодами. 
Контроллер отображает на экране информацию из AVI-файла (файл с расширением avi, контейнер riff), кадры которого представляют собой изображения в формате несжатого bmp (данные пикселей в файле хранятся как двумерный массив).\
Допускается свободное распространение. При любом способе распространения указание автора ОБЯЗАТЕЛЬНО. В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО. Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск. Автор не предоставляет никаких гарантий.
## Компоненты:
- Отладочная плата (МК) STM32G031F6P6/STM32G031F8P6;
- SPI дисплей на контроллере ST7789 разрешением 240x240;
- адаптер/переходник с SD карты на микро-SD;
- инкрементарный механический энкодер;
- 2 резистора с номиналоми 390-680 Ом и 4.7-8.2 кОм.
 
https://github.com/vadrov/stm32g031f6p6-led-lent-screen-controller/assets/111627147/38e19da3-de05-4f13-ab73-b497f85633d7

https://github.com/vadrov/stm32g031f6p6-led-lent-screen-controller/assets/111627147/aa63d80d-37d3-4e82-a94f-37d700e6b066

![схема соединений LED контроллера](https://github.com/vadrov/stm32g031f6p6-led-lent-screen-controller/assets/111627147/7e9db14c-1e46-4c8c-ba71-1c4ffcef843f)

![controller_side1](https://github.com/vadrov/stm32g031f6p6-led-lent-screen-controller/assets/111627147/383d8c5a-8d73-4b3e-b896-358a0a315c2e)

![controller_side2](https://github.com/vadrov/stm32g031f6p6-led-lent-screen-controller/assets/111627147/6ea11f9b-ff75-495c-a7b1-adb1f4f86806)

![screen](https://github.com/vadrov/stm32g031f6p6-led-lent-screen-controller/assets/111627147/9ab5b044-a5a9-4ee6-aa04-8c1c01cf9d0f)



