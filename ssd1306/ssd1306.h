#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include "fonts.h" // Для типов шрифтов

// Геометрия дисплея
#define SSD1306_WIDTH   128
#define SSD1306_HEIGHT  64


// Цвета
typedef enum 
{
    OFF = 0x00, // Пиксель выключен
    ON  = 0x01  // Пиксель светится
} PixelState;


// Инициализация и системные функции
void SSD1306_Init();
void SSD1306_Update();
void SSD1306_Clear();

// Рисование пикселя
void SSD1306_DrawPixel(uint8_t x, uint8_t y, PixelState state);

// Вывод буквы
void SSD1306_WriteChar(uint8_t x, uint8_t y, char ch, PixelState state);
// Вывод текста
void SSD1306_WriteString(uint8_t x, uint8_t y, const char *str, PixelState state);

#endif 