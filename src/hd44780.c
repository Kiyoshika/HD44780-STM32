#include "hd44780.h"
#include "commands.h"

/****************************
 * INTERNAL FUNCTIONS       *
 ****************************/
 
 /* enable clock registers for GPIO pins */
static void gpio_enable_clock(GPIO_TypeDef* data_port)
{
    if (data_port == GPIOA)
        RCC->AHB1ENR |= (1 << 0);
    else if (data_port == GPIOB)
        RCC->AHB1ENR |= (1 << 1);
    else if (data_port == GPIOC)
        RCC->AHB1ENR |= (1 << 2);
    else if (data_port == GPIOH)
        RCC->AHB1ENR |= (1 << 7);
}

/* set GPIO pins to output mode (0b01) using MODE register */
static void gpio_set_as_output(GPIO_TypeDef* data_port, uint8_t data_pin)
{
    // GPIOx_MODER has 2-bit settings, so we multiply the pin value by 2 to access the right-most bit within the register
    data_port->MODER &= ~(0b11 << (2 * data_pin)); // clear bits before writing to them
    data_port->MODER |= (0b01 << (2 * data_pin)); // set as output mode (0b01)

}

/* Setup all GPIO pins to output mode */
static void init_gpio(
    lcd* _lcd,
    size_t bit_length
)
{
    // configure data GPIO pins (D7 - D0 in descending order)
    for (size_t i = 0; i < bit_length; i++)
    {
        gpio_enable_clock(_lcd->data_ports[i]);
        gpio_set_as_output(_lcd->data_ports[i], _lcd->data_pins[i]);
    }

    // configure RS GPIO pin
    gpio_enable_clock(_lcd->rs_port);
    gpio_set_as_output(_lcd->rs_port, _lcd->rs_pin);

    // configure RW GPIO pin
    gpio_enable_clock(_lcd->rw_port);
    gpio_set_as_output(_lcd->rw_port, _lcd->rw_pin);

    // configure E GPIO pin
    gpio_enable_clock(_lcd->e_port);
    gpio_set_as_output(_lcd->e_port, _lcd->e_pin);
}

/****************************
 * API IMPLEMENTATIONS      *
 ****************************/

void lcd_init(
    lcd* _lcd,
    GPIO_TypeDef** data_ports,
    uint8_t* data_pins,
    GPIO_TypeDef* rs_port,
    uint8_t rs_pin,
    GPIO_TypeDef* rw_port,
    uint8_t rw_pin,
    GPIO_TypeDef* e_port,
    uint8_t e_pin,
    lcd_bit_mode bit_mode
)
{
    _lcd->row = 0;
    _lcd->column = 0;

    _lcd->bit_mode = bit_mode;

    // setup data registers and pins (can be either 4 or 8 depending on bit mode)
    size_t bit_length = bit_mode == FOUR ? 4 : 8;
    for (size_t i = 0; i < bit_length; i++)
    {
        _lcd->data_ports[i] = data_ports[i];
        _lcd->data_pins[i] = data_pins[i];
    }

    _lcd->rs_port = rs_port;
    _lcd->rs_pin = rs_pin;

    _lcd->rw_port = rw_port;
    _lcd->rw_pin = rw_pin;
    
    _lcd->e_port = e_port;
    _lcd->e_pin = e_pin;

    // initialize GPIO pins and set all to output mode
    init_gpio(_lcd, bit_length);

    // force LCD into 8-bit mode to clear any pending waiting signals
    // WARNING: if this is not done, LCD can startup in an awkward state and write garbage to the screen
    lcd_reset(_lcd);

    // enable 4-bit mode (if using) before running any commands (CRITICAL)
    if (_lcd->bit_mode == FOUR)
        lcd_enable_4bit_mode(_lcd);

    // clear screen on startup
    lcd_clear_screen(_lcd);
}