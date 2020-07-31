# Marlin 3D Printer Firmware for Flying Bear 4S

Это конфигурация [официального Marlin](https://github.com/MarlinFirmware/Marlin) для принтера Flying Bear 4S.

 Изменения кода коснулись только в части добавления работы с экраном. Код работы с экраном взят из репозитория [inib/Marlin](https://github.com/inib/Marlin)

 На данной версии **не работает WIFI модуль**. Вариант работы с WIFI находится в ветке [FB4S_WIFI](https://github.com/Sergey1560/Marlin_FB4S/tree/FB4S_WIFI)

## Что нужно настроить

Нужно настроить направления движения по осям под свои драйвера

## Как скомпилировать прошивку

Видео Дмитрия Соркина [youtube](https://www.youtube.com/watch?v=HirIZk0rWOQ)

После компиляции, готовая прошивка лежит .pio/build/mks_robin_nano/Robin_nano35.bin

## Графический интерфейс от МКС

В версию 2.0.6 был принят код от МКС с графическим интерфейсом. Для работы интерфейса нужны изображения и шрифты, взять их можно либо напрямую у [МКС](https://github.com/makerbase-mks/Mks-Robin-Nano-Marlin2.0-Firmware/tree/master/Firmware) или в папке firmware.
Для сборки прошивки с графическим интерфейсом нужно сделать следующие настройки:

* В файле Marlin/Configuration.h, в строке 2197 закомментировать #define FSMC_GRAPHICAL_TFT
* В файле Marlin/Configuration.h, в строке 2209 включить #define TFT_LVGL_UI_FSMC
* В файле Marlin/Configuration.h, в строке 2262 закомментировать #define TOUCH_BUTTONS
* В файле Marlin/Configuration_adv.h, в строке 2038 закомментировать #define ADVANCED_PAUSE_FEATURE
* В файле Marlin/Configuration.h, в строке 1383 закомментировать #define LCD_BED_LEVELING

## Готовые сборки

Для записи прошивки просто скопируйте содержимое нужной папке на SD карту и включите принтер.

* Классический интерфейс, драйвера A4988 [Link](./firmware/classic/a4988)
* Классический интерфейс, драйвера 2208/2209 [Link](./firmware/classic/2208)
* MKS TFT интерфейс, драйвера A4988 [Link](./firmware/mks_tft/2208)
* MKS TFT интерфейс, драйвера 2208/2209 [Link](./firmware/mks_tft/2208)
