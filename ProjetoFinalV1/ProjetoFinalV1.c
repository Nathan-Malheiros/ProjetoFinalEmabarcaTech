#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware_config.h"
#include "ssd1306.h"
#include "matriz_led.h"
#include "pico/cyw43_arch.h"

// INCLUSoES PARA UDP LOCAL 
#include "lwip/pbuf.h"
#include "lwip/udp.h"


// CONFIGURACAO DE REDE

#define WIFI_SSID "iPhone de Nathan "
#define WIFI_PASS "123456789"

#define ID_PACIENTE "Paciente 01 (Quarto A)"


#define IP_DO_NOTEBOOK "172.20.10.12" 
#define PORTA_UDP 5000

volatile bool wifi_conectado = false;



#define MIC_PIN 28
#define MIC_ADC_CHANNEL 2

// Nota - quanto menor, mais sensivel, quanto maior, mais tolerante
#define LIMIAR_BARULHO 1800

ssd1306_t display;

#define NUM_OPCOES 4
const char* menu_textos[NUM_OPCOES] = {
    "PRECISO DE AJUDA",
    "SINTO DOR",
    "QUERO AGUA",
    "ESTOU CANSADO"
};

// Auxilio de IA para mapeamento dos icones de LED no Neopixel
// ÍCONES DO OLED E MATRIZ

const uint8_t icon_ajuda[32] = {
    0x01,0x80, 0x01,0x80, 0x01,0x80, 0x01,0x80,
    0x1F,0xF8, 0x1F,0xF8, 0x1F,0xF8, 0x1F,0xF8,
    0x01,0x80, 0x01,0x80, 0x01,0x80, 0x01,0x80,
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00
};
const uint8_t icon_dor[32] = {
    0x07,0xE0, 0x0F,0xF0, 0x1C,0x38, 0x1C,0x38,
    0x1C,0x38, 0x0E,0x70, 0x07,0xE0, 0x03,0xC0,
    0x03,0xC0, 0x01,0x80, 0x00,0x00, 0x01,0x80,
    0x03,0xC0, 0x03,0xC0, 0x00,0x00, 0x00,0x00
};
const uint8_t icon_agua[32] = {
    0x01,0x80, 0x03,0xC0, 0x03,0xC0, 0x07,0xE0,
    0x07,0xE0, 0x0F,0xF0, 0x0F,0xF0, 0x1F,0xF8,
    0x3F,0xFC, 0x3F,0xFC, 0x3F,0xFC, 0x1F,0xF8,
    0x0F,0xF0, 0x07,0xE0, 0x03,0xC0, 0x00,0x00
};
const uint8_t icon_sono[32] = {
    0x3F,0x80, 0x1F,0x80, 0x07,0x00, 0x0E,0x00,
    0x1C,0x00, 0x3F,0x80, 0x3F,0x80, 0x00,0x00,
    0x00,0xFE, 0x00,0x7E, 0x00,0x1C, 0x00,0x38,
    0x00,0x70, 0x00,0xFE, 0x00,0xFE, 0x00,0x00
};
const uint8_t* menu_icones[NUM_OPCOES] = {icon_ajuda, icon_dor, icon_agua, icon_sono};

const uint8_t icone_cruz[25] =   {0,0,1,0,0, 0,0,1,0,0, 1,1,1,1,1, 0,0,1,0,0, 0,0,1,0,0};
const uint8_t icone_ponto[25] =  {0,0,0,0,0, 0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0, 0,0,0,0,0};
const uint8_t icone_cantos[25] = {1,0,0,0,1, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 1,0,0,0,1};
const uint8_t icone_tracos[25] = {0,0,0,0,0, 0,1,1,1,0, 0,0,0,0,0, 0,1,1,1,0, 0,0,0,0,0};

const uint8_t icon_heart_small[25] = {0,0,0,0,0, 0,0,1,0,0, 0,1,1,1,0, 0,1,0,1,0, 0,0,0,0,0};
const uint8_t icon_heart_big[25] =   {0,0,1,0,0, 0,1,1,1,0, 1,1,1,1,1, 1,1,1,1,1, 0,1,0,1,0};

int menu_selecionado = 0; 
volatile bool flag_confirmacao = false;
volatile bool flag_emergencia = false;
volatile bool flag_cancelar = false; 
volatile uint32_t ultimo_tempo_irq = 0;

uint8_t brilho_rgb = 0;
int8_t passo_rgb = 1;
#define MAX_BRILHO_RGB 15 


// FUNÇÕES DE PERIF

void init_pwm_rgb() {
    uint pins[] = {LED_R_PIN, LED_G_PIN, LED_B_PIN};
    for(int i=0; i<3; i++) {
        gpio_set_function(pins[i], GPIO_FUNC_PWM);
        uint slice = pwm_gpio_to_slice_num(pins[i]);
        pwm_config config = pwm_get_default_config();
        pwm_config_set_clkdiv(&config, 4.0f); 
        pwm_init(slice, &config, true);
        pwm_set_wrap(slice, 255); 
        pwm_set_chan_level(slice, pwm_gpio_to_channel(pins[i]), 0);
    }
}

void set_led_rgb_pwm(uint8_t r, uint8_t g, uint8_t b) {
    pwm_set_gpio_level(LED_R_PIN, r);
    pwm_set_gpio_level(LED_G_PIN, g);
    pwm_set_gpio_level(LED_B_PIN, b);
}

void atualizar_cor_matriz() {
    switch (menu_selecionado) {
        case 0: matriz_led_draw_pattern(icone_cruz, 1, 0, 0); break;   
        case 1: matriz_led_draw_pattern(icone_ponto, 1, 1, 0); break;  
        case 2: matriz_led_draw_pattern(icone_cantos, 0, 0, 1); break; 
        case 3: matriz_led_draw_pattern(icone_tracos, 0, 1, 0); break; 
    }
}

void tocar_buzzer_pino(uint pino, uint frequencia) {
    gpio_set_function(pino, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pino);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_init(slice_num, &config, true);
    uint32_t wrap = 1000000 / frequencia;
    pwm_set_wrap(slice_num, wrap);
    pwm_set_gpio_level(pino, wrap / 2); 
}

void parar_buzzer_pino(uint pino) {
    pwm_set_gpio_level(pino, 0);
    gpio_set_function(pino, GPIO_FUNC_SIO);
}

void tocar_alarme_duplo(uint duracao_ms) {
    tocar_buzzer_pino(BUZZER_A_PIN, 1500); 
    tocar_buzzer_pino(BUZZER_B_PIN, 1550); 
    set_led_rgb_pwm(50, 0, 0); 
    sleep_ms(duracao_ms);
    parar_buzzer_pino(BUZZER_A_PIN); 
    parar_buzzer_pino(BUZZER_B_PIN); 
    set_led_rgb_pwm(0, 0, 0);
}

void play_startup_animation() {
    ssd1306_clear(&display);
    ssd1306_draw_string(&display, 10, 20, "INICIANDO");
    ssd1306_draw_string(&display, 10, 40, "SISTEMA...");
    ssd1306_show(&display);

    for (int i = 0; i < 3; i++) {
        matriz_led_draw_pattern(icon_heart_small, 1, 0, 0); 
        set_led_rgb_pwm(30, 0, 0); 
        tocar_buzzer_pino(BUZZER_A_PIN, 600); 
        sleep_ms(150);
        parar_buzzer_pino(BUZZER_A_PIN);
        set_led_rgb_pwm(0, 0, 0);
        sleep_ms(100);

        matriz_led_draw_pattern(icon_heart_big, 4, 0, 0); 
        set_led_rgb_pwm(255, 0, 0); 
        tocar_buzzer_pino(BUZZER_A_PIN, 800); 
        sleep_ms(200);
        parar_buzzer_pino(BUZZER_A_PIN);
        set_led_rgb_pwm(0, 0, 0);

        sleep_ms(600);
    }
    matriz_led_clear();
}

void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    if (tempo_atual - ultimo_tempo_irq > 250) { 
        if (gpio == BTN_A_PIN) flag_confirmacao = true;
        else if (gpio == BTN_B_PIN) flag_emergencia = true;
        else if (gpio == JOY_BTN_PIN) flag_cancelar = true; 
        ultimo_tempo_irq = tempo_atual;
    }
}

void draw_interface() {
    ssd1306_clear(&display);
    if (wifi_conectado) ssd1306_draw_string(&display, 0, 0, "WIFI "); 
    else ssd1306_draw_string(&display, 0, 0, "OFF ");
    
    if (menu_selecionado > 0) ssd1306_draw_string(&display, 60, 0, "^");
    if (menu_selecionado < NUM_OPCOES - 1) ssd1306_draw_string(&display, 60, 50, "v");
    
    ssd1306_draw_icon_scaled(&display, 48, 12, menu_icones[menu_selecionado]);

    const char* texto = menu_textos[menu_selecionado];
    int len_texto = strlen(texto);
    int x_texto = (128 - (len_texto * 6)) / 2; 
    ssd1306_draw_string(&display, x_texto, 55, texto);

    for(int i=0; i<NUM_OPCOES; i++) {
        if (i == menu_selecionado) ssd1306_draw_string(&display, 115, 10 + (i*10), "-");
        else ssd1306_draw_string(&display, 120, 10 + (i*10), ".");
    }

    ssd1306_show(&display);
    atualizar_cor_matriz();
}

void draw_action_screen(const char* titulo, const char* acao) {
    ssd1306_clear(&display);
    ssd1306_draw_string(&display, 10, 20, titulo);
    ssd1306_draw_string(&display, 10, 40, acao);
    ssd1306_show(&display);
}

//  (WIFI E UDP)

void conectar_wifi() {
    ssd1306_clear(&display);
    ssd1306_draw_string(&display, 10, 20, "CONECTANDO");
    ssd1306_draw_string(&display, 10, 40, "WI-FI...");
    ssd1306_show(&display);

    printf("Inicializando modulo Wi-Fi...\n");
    if (cyw43_arch_init()) {
        printf("Erro critico: Falha ao inicializar o Wi-Fi!\n");
        return;
    }
    cyw43_arch_enable_sta_mode();
    
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Falha ao conectar no Wi-Fi.\n");
        wifi_conectado = false;
    } else {
        int tentativas = 0;
        while (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP && tentativas < 15) {
            sleep_ms(1000);
            tentativas++;
        }
        if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP) {
            printf("Wi-Fi Conectado com sucesso!\n");
            wifi_conectado = true;
        } else {
            wifi_conectado = false;
        }
    }
}

// ENVIA DADOS DIRETO PARA O PYTHON 
void enviar_mensagem_udp(const char* mensagem) {
    if (!wifi_conectado) {
        printf("Erro: Wi-Fi offline. Nao foi possivel enviar.\n");
        return;
    }

    ip_addr_t ip_destino;
    if (!ip4addr_aton(IP_DO_NOTEBOOK, &ip_destino)) {
        printf("Erro: IP do Notebook invalido.\n");
        return;
    }

    struct udp_pcb *pcb = udp_new();
    if (!pcb) return;

   
    char pacote[150];
    snprintf(pacote, sizeof(pacote), "%s|%s", ID_PACIENTE, mensagem);

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, strlen(pacote), PBUF_RAM);
    if (!p) {
        udp_remove(pcb);
        return;
    }

    memcpy(p->payload, pacote, strlen(pacote));

    cyw43_arch_lwip_begin();
    udp_sendto(pcb, p, &ip_destino, PORTA_UDP);
    cyw43_arch_lwip_end();

    pbuf_free(p);
    udp_remove(pcb);
    printf("Enviado para o Notebook: %s\n", pacote);
}


// Main.c

int main() {
    setup_hardware();
    ssd1306_init(&display, I2C_PORT);
    matriz_led_init(); 
    init_pwm_rgb(); 
    adc_gpio_init(MIC_PIN);

    play_startup_animation();
    conectar_wifi();

    gpio_set_irq_enabled_with_callback(BTN_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(BTN_B_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(JOY_BTN_PIN, GPIO_IRQ_EDGE_FALL, true);

    draw_interface(); 
    
    uint32_t ultimo_tempo_joystick = 0;
    uint32_t ultimo_tempo_animacao = 0;
    uint32_t ultimo_som_alto = 0;
    
    while (true) {
        uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

        if (tempo_atual - ultimo_tempo_animacao > 40) { 
            brilho_rgb += passo_rgb;
            if (brilho_rgb >= MAX_BRILHO_RGB) passo_rgb = -1; 
            if (brilho_rgb <= 0) passo_rgb = 1;               
            
            switch (menu_selecionado) {
                case 0: set_led_rgb_pwm(brilho_rgb, 0, 0); break;   
                case 1: set_led_rgb_pwm(brilho_rgb, brilho_rgb, 0); break; 
                case 2: set_led_rgb_pwm(0, 0, brilho_rgb); break;   
                case 3: set_led_rgb_pwm(0, brilho_rgb, 0); break;   
            }
            ultimo_tempo_animacao = tempo_atual;
        }

        adc_select_input(MIC_ADC_CHANNEL);
        uint16_t mic_val = adc_read();
        
        if (abs(mic_val - 2048) > LIMIAR_BARULHO) {
            if (tempo_atual - ultimo_som_alto > 5000) {
                printf("SOM ALTO DETECTADO!\n");
                flag_emergencia = true;
                ultimo_som_alto = tempo_atual;
            }
        }

        adc_select_input(0);
        uint16_t joy_y = adc_read();

        if (tempo_atual - ultimo_tempo_joystick > 250) {
            if (joy_y < 1000) {  
                menu_selecionado = (menu_selecionado + 1 >= NUM_OPCOES) ? 0 : menu_selecionado + 1;
                draw_interface();
                ultimo_tempo_joystick = tempo_atual;
            } 
            else if (joy_y > 3000) { 
                menu_selecionado = (menu_selecionado - 1 < 0) ? NUM_OPCOES - 1 : menu_selecionado - 1;
                draw_interface();
                ultimo_tempo_joystick = tempo_atual;
            }
        }

        // EMERGENCIA CHAMADA
        if (flag_emergencia) {
            flag_emergencia = false; 
            draw_action_screen("EMERGENCIA!", "AJUDA SOLICITADA");
            matriz_led_set_color(5, 0, 0); 
            
            for(int i=0; i<4; i++){
                tocar_alarme_duplo(200);
                sleep_ms(100);
            }
            sleep_ms(2000); 
            draw_interface();

            enviar_mensagem_udp("ALERTA DE EMERGENCIA: AJUDA SOLICITADA AGORA!");
        }

        // PEDIDO conf
        if (flag_confirmacao) {
            flag_confirmacao = false; 
            set_led_rgb_pwm(0, 20, 0); 
            tocar_buzzer_pino(BUZZER_A_PIN, 3000); 
            draw_action_screen("ENVIANDO:", menu_textos[menu_selecionado]);
            
            sleep_ms(150);
            parar_buzzer_pino(BUZZER_A_PIN);
            sleep_ms(1850); 
            draw_interface();
            
            enviar_mensagem_udp(menu_textos[menu_selecionado]);
        }

        // ABORTAR
        if (flag_cancelar) {
            flag_cancelar = false;
            draw_action_screen("CANCELADO", "VOLTANDO...");
            set_led_rgb_pwm(20, 20, 0); 
            matriz_led_clear(); 
            tocar_buzzer_pino(BUZZER_A_PIN, 500); 
            
            sleep_ms(300);
            parar_buzzer_pino(BUZZER_A_PIN);
            sleep_ms(1000);
            draw_interface();
        }

        sleep_ms(5); 
    }
    
    return 0;
}