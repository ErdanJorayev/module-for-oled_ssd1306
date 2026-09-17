#include "stm32f4xx.h"
#include "ssd1306.h"


int main(void)
{
    // Включаем светодиод на PC13 (для проверки, что МК вообще ожил)
    // Плюс в целом удобно когда переводишь в режим прошивки (светодиод выключится)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER |= GPIO_MODER_MODER13_0; // Output
    GPIOC->BSRR = GPIO_BSRR_BR13;         // Зажечь светодиод
    
    // Настраиваем тактривание 
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // Тактируем порт B
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;   // Тактируем модуль I2C1

    
    // Настройка GPIO_____________________________________________________________________________
    // Чистим регистры пинов, 10 - старший бит, альтернативная функция
    GPIOB->MODER   &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7);   
    GPIOB->MODER   |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1); 
    
    // По сути запрящаем подачу напряжение на пины из самого чипа
    GPIOB->OTYPER  |= (GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);       

    // Подаем напряжение через резистор, сначала очищаем
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR6 | GPIO_PUPDR_PUPDR7);         // Очистка (00)
    GPIOB->PUPDR |=  (GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0);     // Запись (01 - Pull-Up)

    // Устанавливаем скорость работы транзисторов (по сути скорость их открытие и закрытие)
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7); // Очистка (00)
    GPIOB->OSPEEDR |=  (GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7); // Запись (11 - Very High)      
           
    // Назначаем пинам альтернативную функцию (I2C1), в начале очистка
    GPIOB->AFR[0] &= ~((15 << (6 * 4))  | (15 << (7 * 4)));
    GPIOB->AFR[0] |=  ((4 << (6 * 4))   | (4 << (7 * 4)));
    // Закончили настройку GPIO__________________________________________________________________

    
    SSD1306_Init();  // Инициализация модуля I2C1
    SSD1306_Clear(); // Очистка кадра буфера

    // Печатаем строку
    SSD1306_WriteString(0, 0, "GULJAHAN ", WHITE); 

    // Обновление кадра, отправляем данные
    SSD1306_Update();

    while (1) 
    {
        // Картинка отрисована и зафиксирована
    }
}
