/*
Metronome built on Raspberry Pi Pico w
LED is blinking at selected tempo(BPM)
Click left or right button to decrease or increase the tempo
Tempo is shown on 7 segment display
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"

#define DIODE_PIN 16
#define LEFT_BUTTON_PIN 14
#define RIGHT_BUTTON_PIN 15
#define TRANSISTOR_GPIO 28
#define TRANSISTOR_GPIO2 27
#define FIRST_GPIO 2


int tempo = 60;
int beat;
struct repeating_timer timer;
struct repeating_timer timer2;

int bits[10] = { //diode state for each number
    0b00111111, //0
    0b00000110, //1
    0b01011011, //2
    0b01001111, //3
    0b01100110, //4
    0b01101101, //5
    0b01111101, //6
    0b00000111, //7
    0b01111111, //8
    0b01101111, //9
};

int first_digit_led = 1;
int second_digit_led = 0;
int32_t mask;

bool repeating_timer_callback2(struct repeating_timer *t2){
    int first_digit;
    int second_digit;

    first_digit = tempo / 10;
    second_digit = tempo % 10;

    gpio_clr_mask(mask);
    first_digit_led = 1-first_digit_led;
    second_digit_led = 1-second_digit_led;
    if (first_digit_led == 1){
    mask = bits[second_digit] << FIRST_GPIO;
    gpio_put(TRANSISTOR_GPIO, 1);
    gpio_put(TRANSISTOR_GPIO2, 0);
    gpio_set_mask(mask);
    } else {  
    mask = bits[first_digit] << FIRST_GPIO;
    gpio_put(TRANSISTOR_GPIO, 0);
    gpio_put(TRANSISTOR_GPIO2, 1);
    gpio_set_mask(mask);
    }

    return true;
}


int led_state = 0;

bool repeating_timer_callback(struct repeating_timer *t)
{
    led_state = 1 - led_state;
    gpio_put(DIODE_PIN, led_state);
    return true;
}


int TEMPO_MIN = 30;
int TEMPO_MAX = 99;
int BEAT_BASE = 30000;
int DISPLAY_REFRESH_RATE = 10;

void update_timers(){
    beat = BEAT_BASE/tempo;
    cancel_repeating_timer(&timer);
    cancel_repeating_timer(&timer2);
    add_repeating_timer_ms(beat, repeating_timer_callback, NULL, &timer);
    add_repeating_timer_ms(DISPLAY_REFRESH_RATE, repeating_timer_callback2, NULL, &timer2);
}

void gpio_irq_handler(void){
    if(gpio_get_irq_event_mask(LEFT_BUTTON_PIN) & GPIO_IRQ_EDGE_FALL )
    {
        gpio_acknowledge_irq(LEFT_BUTTON_PIN, GPIO_IRQ_EDGE_FALL);
        tempo--;
        if (tempo < TEMPO_MIN) {
            tempo = TEMPO_MIN;
        }
        update_timers();
    }
    if(gpio_get_irq_event_mask(RIGHT_BUTTON_PIN) & GPIO_IRQ_EDGE_FALL )
    {
        gpio_acknowledge_irq(RIGHT_BUTTON_PIN, GPIO_IRQ_EDGE_FALL);
        tempo++;
        if (tempo > TEMPO_MAX){
            tempo = TEMPO_MAX;
        }
        update_timers();
    }    
}

int main()
{
    stdio_init_all();

    gpio_init(LEFT_BUTTON_PIN);
    gpio_set_dir(LEFT_BUTTON_PIN,GPIO_IN);
    gpio_pull_up(LEFT_BUTTON_PIN);
    gpio_init(RIGHT_BUTTON_PIN);
    gpio_set_dir(RIGHT_BUTTON_PIN,GPIO_IN);
    gpio_pull_up(RIGHT_BUTTON_PIN);

    irq_set_exclusive_handler(IO_IRQ_BANK0, gpio_irq_handler);
    irq_set_enabled(IO_IRQ_BANK0, true);
    gpio_set_irq_enabled(LEFT_BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(RIGHT_BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true);

    gpio_init(DIODE_PIN);
    gpio_set_dir(DIODE_PIN,GPIO_OUT);
    beat = BEAT_BASE/ tempo;

    gpio_init(TRANSISTOR_GPIO);
    gpio_set_dir(TRANSISTOR_GPIO, GPIO_OUT);
    gpio_init(TRANSISTOR_GPIO2);
    gpio_set_dir(TRANSISTOR_GPIO2, GPIO_OUT);

    for(int gpio = FIRST_GPIO; gpio < FIRST_GPIO + 7; gpio++){ //7 segment pins initialization
        gpio_init(gpio);
        gpio_set_dir(gpio, GPIO_OUT);
    }

    add_repeating_timer_ms(beat, repeating_timer_callback, NULL, &timer); //timer for blinking diode
    add_repeating_timer_ms(DISPLAY_REFRESH_RATE, repeating_timer_callback2, NULL, &timer2); //timer to multiplex 7segment display

    while (1) {
        tight_loop_contents();
        
    }
}
