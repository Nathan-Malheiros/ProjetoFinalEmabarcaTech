#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#include "pico/stdlib.h"


// MAPEAMENTO DE HARDWARE - BITDOGLAB

// Botões e Joystick
#define BTN_A_PIN 5      // Confirmação
#define BTN_B_PIN 6      // Emergência
#define JOY_BTN_PIN 22   // Clique do Joystick
#define JOY_X_PIN 26     // Eixo X 
#define JOY_Y_PIN 27     // Eixo Y 

//OLED SSD1306
#define I2C_PORT i2c1
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15


#define BUZZER_A_PIN 21  // Buzzer principal
#define BUZZER_B_PIN 10  // Buzzer secundário 

// LED RGB
#define LED_G_PIN 11     // Verde (Sucesso/Confirmação)
#define LED_B_PIN 12     // Azul (Navegação/Standby)
#define LED_R_PIN 13     // Vermelho (Erro/Emergência)

// UART 
#define UART_ID uart0
#define UART_TX_PIN 0
#define UART_RX_PIN 1


void setup_hardware();

#endif 