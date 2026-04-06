#include "hardware_config.h"
#include <stdio.h>
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/uart.h"

void setup_hardware() {
    stdio_init_all(); 

  
    // 1. CONFIGURAÇÃO DOS BOTÕES
  
    // Botão A (Pino 5)
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);

    // Botão B (Pino 6)
    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_pull_up(BTN_B_PIN);

    // Botão do Joystick (Pino 22)
    gpio_init(JOY_BTN_PIN);
    gpio_set_dir(JOY_BTN_PIN, GPIO_IN);
    gpio_pull_up(JOY_BTN_PIN);

   
    // 2. CONFIGURAÇÃO DO JOYSTICK (ADC)
    
    adc_init();
    adc_gpio_init(JOY_X_PIN);
    adc_gpio_init(JOY_Y_PIN); 

  
    // 3. CONFIGURAÇÃO DO ECRÃ (I2C)
  
    i2c_init(I2C_PORT, 400 * 1000); // Frequência I2C de 400 kHz
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    
    // 4. CONFIGURAÇÃO DOS BUZZERS
  
    gpio_init(BUZZER_A_PIN);
    gpio_set_dir(BUZZER_A_PIN, GPIO_OUT);
    gpio_put(BUZZER_A_PIN, 0); 

    gpio_init(BUZZER_B_PIN);
    gpio_set_dir(BUZZER_B_PIN, GPIO_OUT);
    gpio_put(BUZZER_B_PIN, 0); 

    
    // 5. CONFIGURAÇÃO DO LED RGB
   
    gpio_init(LED_R_PIN); 
    gpio_set_dir(LED_R_PIN, GPIO_OUT); 
    gpio_put(LED_R_PIN, 0); 
    
    gpio_init(LED_G_PIN); 
    gpio_set_dir(LED_G_PIN, GPIO_OUT); 
    gpio_put(LED_G_PIN, 0); 
    
    gpio_init(LED_B_PIN); 
    gpio_set_dir(LED_B_PIN, GPIO_OUT); 
    gpio_put(LED_B_PIN, 0); 

    printf("Hardware inicializado com sucesso. Modulos prontos!\n");
}