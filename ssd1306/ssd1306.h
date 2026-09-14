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
    BLACK = 0x00, // Пиксель выключен
    WHITE = 0x01  // Пиксель светится
} SSD1306_Color_t;


// Инициализация и системные функции
void SSD1306_Init();
void SSD1306_Update();
void SSD1306_Clear();

// Рисование пикселя
void SSD1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_Color_t color);

// Вывод буквы
void SSD1306_WriteChar(uint8_t x, uint8_t y, char ch, SSD1306_Color_t color);
// Вывод текста
void SSD1306_WriteString(uint8_t x, uint8_t y, const char *str, SSD1306_Color_t color);

#endif 