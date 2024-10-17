#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#define LED_GPIO_PORT  (GPIOB)
#define LED_GPIO_PIN   (GPIO_PIN_5)

#define HARDWARE_SPI

#define SPI_SCK_PIN                  GPIO_PIN_5
#define SPI_MOSI_PIN                 GPIO_PIN_6
#define SPI_GPIO_PORT                GPIOC

#define SPI_NCS_PIN                  GPIO_PIN_3
#define SPI_NCS_PORT                 GPIOA

#define BUTTON0_PIN                  GPIO_PIN_4
#define BUTTON1_PIN                  GPIO_PIN_5
#define BUITTON_PORT                 GPIOD

#define TIM4_PERIOD       124


#endif
