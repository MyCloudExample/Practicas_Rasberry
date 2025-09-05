#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>

#define BUTTON_PIN 18
#define ADC_PIN 26
#define DEBOUNCE_TIME_MS 50
#define MULTI_PRESS_TIMEOUT 300

// Variables ADC
float var1, var2, var3;
uint8_t selected_var = 0; // 0: ninguna, 1: var1, 2: var2, 3: var3

// Variables para detección de pulsaciones
uint32_t last_press_time = 0;
uint8_t press_count = 0;

void setup() {
    stdio_init_all();
    
    // Inicializar ADC
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(0);
    
    // Configurar botón con pull-up
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    
    printf("Sistema iniciado. Espere a la selección de variable...\n");
}

void handle_button_press() {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    // Detectar flanco de bajada (botón presionado)
    if(!gpio_get(BUTTON_PIN)) {
        if(current_time - last_press_time > DEBOUNCE_TIME_MS) {
            press_count++;
            last_press_time = current_time;
            printf("Pulsación detectada. Contador: %d\n", press_count);
        }
    }
    
    // Detectar timeout para múltiples pulsaciones
    if(press_count > 0 && (current_time - last_press_time > MULTI_PRESS_TIMEOUT)) {
        if(press_count == 1) {
            selected_var = 1;
            printf("Variable 1 seleccionada\n");
        } 
        else if(press_count == 2) {
            selected_var = 2;
            printf("Variable 2 seleccionada\n");
        } 
        else if(press_count >= 3) {
            selected_var = 3;
            printf("Variable 3 seleccionada\n");
        }
        press_count = 0; // Resetear contador
    }
}

void loop() {
    // Leer valor del ADC
    adc_select_input(0);
    uint16_t adc_value = adc_read();
    float voltage = adc_value * 3.3f / (1 << 12);
    
    // Manejar pulsaciones del botón
    handle_button_press();
    
    // Asignar valor solo a la variable seleccionada
    switch(selected_var) {
        case 1:
            var1 = voltage;
            break;
        case 2:
            var2 = voltage;
            break;
        case 3:
            var3 = voltage;
            break;
        default:
            // Ninguna variable seleccionada
            break;
    }
    
    // Mostrar valores cada segundo (opcional)
    static uint32_t last_display = 0;
    if(to_ms_since_boot(get_absolute_time()) - last_display > 1000) {
        printf("Valores: Var1=%.2fV, Var2=%.2fV, Var3=%.2fV\n", var1, var2, var3);
        last_display = to_ms_since_boot(get_absolute_time());
    }
    
    sleep_ms(10);
}

int main() {
    setup();
    while(1) {
        loop();
    }
    return 0;
}