#ifndef HD44780_H
#define HD44780_H

// NOTE: This code was written specifically for STM32F411xx.
// This MAY OR MAY NOT WORK on your controller depending on how the registers are set up.
// Although, it should not be difficult to refactor the code to get it working for your specific MCU.
#include "stm32f411xe.h"
#include "commands.h"

#include <stdint.h> // uint...
#include <stddef.h> // size_t

/* REFERENCE DATASHEET: https://www.sparkfun.com/datasheets/LCD/HD44780.pdf */

/** TODO:
 * 1. Add support for read operations (need to enable RW pin)
 * 2. Wait for busy flag before sending commands (requires #1)
 */



/**
 * @brief Specify 4-bit or 8-bit mode.
 * 4-bit mode uses D7 - D4 pins and 8-bit mode uses all D7 - D0 pins.
 */
typedef enum lcd_bit_mode
{
    FOUR, // 4-bit mode (D7 - D4 pins)
    EIGHT // 8-bit mode (D7 - D0 pins)
} lcd_bit_mode;


/**
 * @brief Struct to contain all the information about the pins and data registers used to interact with the LCD.
 * @param row row index of the cursor (0-1)
 * @param column column index of the cursor (0-15)
 * @param bit_mode FOUR or EIGHT to specify 4-bit or 8-bit mode
 * @param data_ports[8]Array of GPIOx pointers for the registers you want to assign to the D7 - D0 pins (left to right). If using 4-bit mode, only the first 4 elements need to be written to (D7 - D4).
 * @param data_pins[8] Array of integers for the corresponding pins to data_ODR[8] you want to assign to the D7 - D0 pins (left to right). If using 4-bit mode, only the first 4 elements need to be written to (D7 - D4).
 * @param rs_port GPIOx pointer you wish to assign the RS pin to.
 * @param rs_pin Corresponding pin to rs_port you wish to assign RS pin to.
 * @param rw_port GPIOx pointer you wish to assign the RW pin to.
 * @param rw_pin Corresponding pin to rw_port you wish to assign RW pin to.
 * @param e_port GPIOx pointer you wish to assign the E pin to.
 * @param e_pin Corresponding pin to e_port you wish to assign E pin to.
 */
typedef struct lcd
{
    uint8_t row; // row index of the cursor (0-1)
    uint8_t column; // column index of the cursor (0-15)
    lcd_bit_mode bit_mode; // FOUR or EIGHT to specify 4-bit or 8-bit mode
    GPIO_TypeDef* data_ports[8]; // Array of GPIOx pointers for the registers you want to assign to the D7 - D0 pins (left to right). If using 4-bit mode, only the first 4 elements need to be written to (D7 - D4).
    uint8_t data_pins[8]; // Array of integers for the corresponding pins to data_ODR[8] you want to assign to the D7 - D0 pins (left to right). If using 4-bit mode, only the first 4 elements need to be written to (D7 - D4).
    GPIO_TypeDef* rs_port; // GPIOx pointer you wish to assign the RS pin to.
    uint8_t rs_pin; // Corresponding pin to rs_port you wish to assign RS pin to.
    GPIO_TypeDef* rw_port; // GPIOx pointer you wish to assign the RW pin to.
    uint8_t rw_pin; // Corresponding pin to rw_port you wish to assign RW pin to.
    GPIO_TypeDef* e_port; // GPIOx pointer you wish to assign the E pin to.
    uint8_t e_pin; // Corresponding pin to e_port you wish to assign E pin to.
} lcd;

/**
 * @brief Initialize LCD display by setting up data registers and pins that will be used.
 * 
 * @param _lcd Pointer to lcd struct
 * @param data_ports[8]Array of GPIOx pointers for the registers you want to assign to the D7 - D0 pins (left to right). If using 4-bit mode, only the first 4 elements need to be written to (D7 - D4).
 * @param data_pins[8] Array of integers for the corresponding pins to data_ODR[8] you want to assign to the D7 - D0 pins (left to right). If using 4-bit mode, only the first 4 elements need to be written to (D7 - D4).
 * @param rs_port GPIOx pointer you wish to assign the RS pin to.
 * @param rs_pin Corresponding pin to rs_port you wish to assign RS pin to.
 * @param rw_port GPIOx pointer you wish to assign the RW pin to.
 * @param rw_pin Corresponding pin to rw_port you wish to assign RW pin to.
 * @param e_port GPIOx pointer you wish to assign the E pin to.
 * @param e_pin Corresponding pin to e_port you wish to assign E pin to.
 * @param bit_mode FOUR or EIGHT to specify 4-bit or 8-bit mode
 */
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
);

#endif