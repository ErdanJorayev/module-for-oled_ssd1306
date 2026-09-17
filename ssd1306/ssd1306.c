#include "ssd1306.h"
#include "stm32f4xx.h"
#include <string.h> // Для memcpy(), чтобы очистить буфер кадра

// Буфер кадра: 128x64 пикселей / 8 бит = 1024 байта
static uint8_t frame_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Команды инициализации матрицы
static const uint8_t INIT_CMDS[] = 
{
    0xAE,       // Display OFF
    0x20, 0x00, // Horizontal Addressing Mode
    0x8D, 0x14, // Charge Pump Enable (7.5V)
    0xAF        // Display ON
};

// Команды сброса указателя адреса (на верхний левый угол 0,0)
static const uint8_t RESET_ADDR_CMDS[] = 
{
    0x21, 0, 127, // Set Column Range (0..127)
    0x22, 0, 7    // Set Page Range (0..7)
};

// Отправка команд
static void SSD1306_WriteCmds(const uint8_t *cmds, uint16_t count) 
{
    // 1. Старт и адрес
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));

    I2C1->DR = 0x78;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;

    // 2. Управляющий байт 0x00: "Все следующие байты — это команды!"
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = 0x00; 

    // 3. Отправляем весь массив в цикле
    for (uint16_t i = 0; i < count; i++) 
    {
        while (!(I2C1->SR1 & I2C_SR1_TXE));
        I2C1->DR = cmds[i];
    }

    // 4. Завершение транзакции
    while (!(I2C1->SR1 & I2C_SR1_BTF));
    I2C1->CR1 |= I2C_CR1_STOP;
}


// Отправка данных
static void SSD1306_WriteData() 
{ 
    SSD1306_WriteCmds(RESET_ADDR_CMDS, sizeof(RESET_ADDR_CMDS));

    // Генерируем сигнал START и ждем готовности шины (флаг SB)
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));

    // Передаем адрес SSD1306 (0x78 = 0x3C << 1 | Write) и ждем ACK от экрана (ADDR)
    I2C1->DR = 0x78;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2; // Сбрасываем флаг ADDR обязательным чтением SR2

    // Инструктируем SSD1306: "ВСЕ следующие байты в этой сессии — это ДАННЫЕ пикселей!"
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = 0x40; 

    // Заталкиваем весь буфер подряд
    for (int i = 0; i < 1024; i++) 
    {
        while (!(I2C1->SR1 & I2C_SR1_TXE)); // Ждем освобождения регистра DR
        I2C1->DR = frame_buffer[i];         // Кидаем очередной байт графики
    }

    // Ждем полной физической отгрузки последнего бита по проводам (флаг BTF)
    while (!(I2C1->SR1 & I2C_SR1_BTF));
    
    // Формируем сигнал STOP — завершаем сессию и освобождаем шину I2C
    I2C1->CR1 |= I2C_CR1_STOP;
}

// Настраиваем модуль I2C1
void SSD1306_Init() 
{
    I2C1->CR2   |= 16;         // Частота APB1 = 16 МГц
    I2C1->CCR    = 80;         // Делитель для получения 100 кГц
    I2C1->TRISE  = 17;         // Максимальное время подъема сигнала
    I2C1->CR1   |= I2C_CR1_PE; // Включаем модуль I2C1 (Peripheral Enable)

    // Програмная задержка чтобы все стабилизировалось
    for (volatile int i = 0; i < 100000; i++);

    SSD1306_WriteCmds(INIT_CMDS, sizeof(INIT_CMDS));

    SSD1306_Clear();
    SSD1306_Update();
}

void SSD1306_Clear(void) 
{
    // Заполняет весь frame_buffer значением BLACK (0x00)
    memset(frame_buffer, BLACK, sizeof(frame_buffer));
}

// Рисуем пиксель
void SSD1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_Color_t color) 
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) 
        return;

    if (color == WHITE) 
        frame_buffer[x + (y / 8) * SSD1306_WIDTH] |= (1 << (y % 8));
    else 
        frame_buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
}

// Обновление
void SSD1306_Update() 
{
    SSD1306_WriteData();
}

// Отрисовка символа
void SSD1306_WriteChar(uint8_t x, uint8_t y, char ch, SSD1306_Color_t color) 
{
    // Проверка на печатаемые символы ASCII
    if (ch < 32 || ch > 126) 
        ch = '?';

    // Проходим по 5 столбцам шрифта
    for (uint8_t i = 0; i < 5; i++) 
    {
        // Берем байт столбца из таблицы шрифта
        uint8_t line = Font5x7[ch - 32][i];

        // Проверяем каждый бит из 8 по вертикали
        for (uint8_t j = 0; j < 8; j++) 
        {
            if (line & (1 << j)) 
                SSD1306_DrawPixel(x + i, y + j, color);
            else
                SSD1306_DrawPixel(x + i, y + j, (color == WHITE) ? BLACK : WHITE);
        }
    }
}

// Отрисовка строки
void SSD1306_WriteString(uint8_t x, uint8_t y, const char *str, SSD1306_Color_t color) 
{
    while (*str) 
    {
        // Проверка: помещается ли следующий символ по ширине экрана (128px)
        if (x + 6 >= 128) 
        {
            x = 0;      // Перенос на следующую строку по X
            y += 8;     // Сдвиг вниз по Y на высоту символа
            if (y >= 64) break; // Экран закончился
        }
        SSD1306_WriteChar(x, y, *str, color);
        x += 6; // Сдвиг на 5 пикселей символа + 1 пиксель межсимвольный интервал
        str++;
    }
}