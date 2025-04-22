#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "lib/pwm.h"

#include "lib/led.h"
#include "lib/push_button.h"
#include "lib/ssd1306.h"
#include "lib/ws2812b.h"

#define BUZZER 21
#define JOYSTIC_X_PIN 27
#define JOYSTIC_Y_PIN 26
#define PWM_DIVISER 20
#define PWM_WRAP 2000 // aprox 3.5kHz freq

_ws2812b *ws; /* 5x5 matrix */
ssd1306_t ssd;
volatile alarm_id_t timer_id = 0;
static volatile uint32_t last_time = 0;
static volatile bool leds_on = true;
static volatile int x_pos = 0;
static volatile int y_pos = 0;
static volatile int led = 11;

/**
 * @brief Initialize the SSD1306 display
 *
 */
void init_display()
{
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, I2C_ADDRESS, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
}

/**
 * @brief Handler function to interruption timer event
 *
 * @param id timer id
 * @param user_data The data that the user can pass to the function
 * 
 * @return
 *      false - dont repeat
 *      true - repeat
 */
int64_t timer_callback(alarm_id_t id, void *user_data)
{
    printf("Timer executado!\n");
    ws2812b_turn_off(ws);
    timer_id = 0; // Zera o ID para saber que terminou
    return 0;         // 0 = não repete
}

/**
 * @brief Initialize the all GPIOs that will be used in project
 *      - I2C;
 *      - RGB LED;
 *      - Push buttons A and B
 *      - Buzzer;
 */
void init_gpio()
{
    /** initialize i2c communication */
    int baudrate = 400 * 1000; // 400kHz baud rate for i2c communication
    i2c_init(I2C_PORT, baudrate);

    // set GPIO pin function to I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL);

    /** initialize RGB LED */
    pwm_init_(PIN_BLUE_LED);
    pwm_setup(PIN_BLUE_LED, PWM_DIVISER, PWM_WRAP);
    pwm_start(PIN_BLUE_LED, 0);

    pwm_init_(PIN_RED_LED);
    pwm_setup(PIN_RED_LED, PWM_DIVISER, PWM_WRAP);
    pwm_start(PIN_RED_LED, 0);

    pwm_init_(PIN_GREEN_LED);
    pwm_setup(PIN_GREEN_LED, PWM_DIVISER, PWM_WRAP);
    pwm_start(PIN_GREEN_LED, 0);

    /** initialize buttons */
    init_push_button(PIN_BUTTON_A);
    init_push_button(PIN_BUTTON_B);
    init_push_button(PIN_BUTTON_J);

    /** initialize buzzer */
    pwm_init_(BUZZER);
    pwm_setup(BUZZER, PWM_DIVISER, PWM_WRAP);
    pwm_start(BUZZER, 0);
}

/**
 * @brief Update the display informations
 */
void update_display()
{
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 0, 0, 128, 64, true, false);
    ssd1306_rect(&ssd, y_pos, x_pos, 8, 8, true, true);
    ssd1306_send_data(&ssd); // update display
}

/**
 * @brief Handler function to interruption
 *
 * @param gpio GPIO that triggered the interruption
 * @param event The event which caused the interruption
 */
void gpio_irq_handler(uint gpio, uint32_t event)
{
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - last_time >= 200)
    { // debounce
        if (timer_id != 0)
        {
            cancel_alarm(timer_id);
        }
        if (gpio == PIN_BUTTON_A){
            leds_on = !leds_on;
            ws2812b_plot(ws, &ARROW_LEFT);
            timer_id = add_alarm_in_ms(1000, timer_callback, NULL, false);
            printf("Turn off LEDs.");
        }
        else if (gpio == PIN_BUTTON_B){
            pwm_set_gpio_level(led, 0);
            led = led >= 13 ? 11 : led + 1;
            ws2812b_plot(ws, &ARROW_RIGHT);
            timer_id = add_alarm_in_ms(1000, timer_callback, NULL, false);
            printf("Changing LED. LED number %d.", &led);
        }
        last_time = current_time;
    }
}

int main()
{
    PIO pio = pio0;
    bool ok;
    uint16_t x = 0, y = 0, x_led_level = 0, y_led_level = 0;

    // set clock freq to 128MHz
    ok = set_sys_clock_khz(128000, false);
    if (ok)
        printf("clock set to %ld\n", clock_get_hz(clk_sys));

    // init gpios and stdio functions
    stdio_init_all();
    init_gpio();

    // init 5x5 matrix
    ws = init_ws2812b(pio,PIN_WS2812B);
    ws2812b_turn_off(ws);

    // get ws and ssd struct
    init_display();

    // Clear display
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // init adc channels
    adc_init();
    adc_gpio_init(JOYSTIC_X_PIN);
    adc_gpio_init(JOYSTIC_Y_PIN);

    // configure interruptions handlers
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_A, GPIO_IRQ_EDGE_FALL, 1, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_B, GPIO_IRQ_EDGE_FALL, 1, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_J, GPIO_IRQ_EDGE_FALL, 1, &gpio_irq_handler);
    sleep_ms(50);

    while (1)
    {
        adc_select_input(0);
        y = adc_read();
        y_pos = (int)(-0.00836 * y) + 44.08; // linear function to convert y 10-4075 for 10-44
        adc_select_input(1);
        x = adc_read();
        x_pos = (int)(0.0226 * x) + 9.774; // linear function to convert x 10-4075 for 10-102
        if (leds_on)
        {
            if (x <= 2100 && x >= 1800)
                x_led_level = 0; // if joystick on center, turn off LED
            else if (x > 2100)
                x_led_level = x - 2100; // if joystick out of center, turn on LED according distance
            else if (x < 1800)
                x_led_level = (x - 1800) * -1; // if joystick out of center, turn on LED according distance

            if (y <= 2100 && y >= 1800)
                y_led_level = 0; // if joystick on center, turn off LED
            else if (y > 2100)
                y_led_level = y - 2100; // if joystick out of center, turn on LED according distance
            else if (y < 1800)
                y_led_level = (y - 1800) * -1; // if joystick out of center, turn on LED according distance
            
            if(x_led_level > y_led_level){
                pwm_set_gpio_level(led, x_led_level);
                pwm_set_gpio_level(BUZZER, x_led_level);
            }
            else{
                pwm_set_gpio_level(led, y_led_level);
                pwm_set_gpio_level(BUZZER, y_led_level);
            }
        }
        else{
            pwm_set_gpio_level(led, 0);
            pwm_set_gpio_level(BUZZER, 0);
        }
        printf("X: %d", &x);
        printf("Y: %d", &y);
        update_display();
        sleep_ms(100);
    }

    return 0;
}