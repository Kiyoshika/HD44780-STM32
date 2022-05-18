#include "commands.h"
#include "hd44780.h"
#include <stdio.h>

/****************************
 * INTERNAL DATA STRUCTURES *
 ****************************/
const uint8_t DDRAM_LOOKUP_TABLE[2][16] = {
    /* ROW 1 */ {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F},
    /* ROW 2 */ {0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F}
};

/****************************
 * INTERNAL FUNCTIONS       *
 ****************************/

/* A pseudo delay function to give send_data_pulse() enough time to process the request */
static void pseudo_delay()
{
    for (size_t i = 0; i < 10000; ++i);
}

/*
 * Send data from data pins (D7 - D0) to LCD for processing.
 * 
 * To send data to LCD, user must toggle E pin high then low with a short delay.
 * I am not configuring any timers so I use a fake delay function to wait a short period of time to process the request.
 */
static void send_data_pulse(lcd* _lcd)
{
    _lcd->e_port->ODR |= (1 << _lcd->e_pin);
    pseudo_delay();
    _lcd->e_port->ODR &= ~(1 << _lcd->e_pin);
    pseudo_delay();
}

/* Extract specific bit from command and write it to output data register. */
/* e.g. if command = 0b01101010, then write_bit_from_command(GPIOB, 2 0b01101010, 3) will write 1 (0b0110[1]010) to 2nd pin in GPIOB */
static void write_bit_from_command(GPIO_TypeDef* data_port, uint8_t data_pin, uint8_t command, uint8_t command_bit)
{
    data_port->ODR &= ~(1 << data_pin); // reset bit before writing to it
    data_port->ODR |= (((command >> command_bit) & 1) << data_pin);
}

static void send_command_4bit(lcd* _lcd, uint8_t command)
{
   size_t command_bit = 7;

   // write highest order bits to D7 - D4 first
   for (size_t i = 0; i < 4; ++i)
   {
       write_bit_from_command(_lcd->data_ports[i], _lcd->data_pins[i], command, command_bit);
       command_bit -= 1;
   }
   send_data_pulse(_lcd);

    // write lowest order bits to D7 - D4 second
   for (size_t i = 0; i < 4; ++i)
   {
       write_bit_from_command(_lcd->data_ports[i], _lcd->data_pins[i], command, command_bit);
       command_bit -= 1;
   }
   send_data_pulse(_lcd);
}

/* Send commands in 8-bit mode.
 * User can write all bits into D7 - D0 bits at once and send_data_pulse a single time, unlike 4-bit mode which requires two pulses.
 */
static void send_command_8bit(lcd* _lcd, uint8_t command)
{
    // UNTESTED !!! (but in theory should work......)
    size_t command_bit = 7;
    for (size_t i = 0; i < 7; ++i)
    {
        write_bit_from_command(_lcd->data_ports[i], _lcd->data_pins[i], command, command_bit);
        command_bit -= 1;
    }
    send_data_pulse(_lcd);
}

/****************************
 * API IMPLEMENTATIONS      *
 ****************************/

void lcd_reset(lcd* _lcd)
{
    /*************
     NOTE: the LCD can startup in different states. To do a proper "reset", we send the 8-bit function set command three times
    // refer to wikipedia for more info: https://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller
    **************/

    // write 0 to RS pin
    _lcd->rs_port->ODR &= ~(1 << _lcd->rs_pin);

    // D7 = 0
    // D6 = 0
    // D5 = 1
    // D4 = 1
    _lcd->data_ports[0]->ODR &= ~(1 << _lcd->data_pins[0]); // set to 0
    _lcd->data_ports[1]->ODR &= ~(1 << _lcd->data_pins[1]); // set to 0
    _lcd->data_ports[2]->ODR |=  (1 << _lcd->data_pins[2]); // write 1 (if it's not already)
    _lcd->data_ports[3]->ODR |=  (1 << _lcd->data_pins[3]); // write 1 (if it's not already)

    for (size_t i = 0; i < 3; ++i)
        send_data_pulse(_lcd);
}

void lcd_enable_4bit_mode(lcd* _lcd)
{
    // write 0 to RS pin
    _lcd->rs_port->ODR &= ~(1 << _lcd->rs_pin);

    // D7 = 0
    // D6 = 0
    // D5 = 1
    // D4 = 0
    _lcd->data_ports[0]->ODR &= ~(1 << _lcd->data_pins[0]); // set to 0
    _lcd->data_ports[1]->ODR &= ~(1 << _lcd->data_pins[1]); // set to 0
    _lcd->data_ports[2]->ODR |=  (1 << _lcd->data_pins[2]); // write 1 (if it's not already)
    _lcd->data_ports[3]->ODR &= ~(1 << _lcd->data_pins[3]); // set to 0

    send_data_pulse(_lcd);
}

void lcd_send_command(lcd* _lcd, uint8_t rs_pin_value, uint8_t rw_pin_value, uint8_t command)
{
    // set RS & RW pin value before processing command
    _lcd->rs_port->ODR &= ~(1 << _lcd->rs_pin); // clear bit
    _lcd->rs_port->ODR |= (rs_pin_value << _lcd->rs_pin);

    _lcd->rw_port->ODR &= ~(1 << _lcd->rw_pin); // clear bit
    _lcd->rw_port->ODR |= (rw_pin_value << _lcd->rw_pin);

    switch(_lcd->bit_mode)
    {
        case FOUR:
            send_command_4bit(_lcd, command);
            break;
        case EIGHT:
            send_command_8bit(_lcd, command);
            break;
    }
}

void lcd_enable_cursor(lcd* _lcd)
{
    lcd_send_command(_lcd, 0, 0, 0b00001111);
}

void lcd_disable_cursor(lcd* _lcd)
{
    lcd_send_command(_lcd, 0, 0, 0b00001100);
}

void lcd_clear_screen(lcd* _lcd)
{
    lcd_send_command(_lcd, 0, 0, 0b00000001);
}

void lcd_write_char(lcd* _lcd, const char c)
{
    lcd_send_command(_lcd, 1, 0, c);
}

void lcd_write_string(lcd* _lcd, const char* str)
{
    for (size_t i = 0; i < strlen(str); ++i)
        lcd_write_char(_lcd, str[i]);
}

void lcd_shift_cursor_right(lcd* _lcd)
{
    lcd_send_command(_lcd, 0, 0, 0b00010100);
}

void lcd_shift_cursor_left(lcd* _lcd)
{
    lcd_send_command(_lcd, 0, 0, 0b00010000);
}

void lcd_set_cursor(lcd* _lcd, uint8_t row, uint8_t column)
{
    uint8_t position = 0b10000000 | DDRAM_LOOKUP_TABLE[row][column];
    _lcd->row = row;
    _lcd->column = column;
    lcd_send_command(_lcd, 0, 0, position);
}

void lcd_backspace(lcd* _lcd)
{
    if (_lcd->column == 0) return;
    _lcd->column -= 1;
    lcd_write_char(_lcd, ' ');
    lcd_set_cursor(_lcd, _lcd->row, _lcd->column);
}

void lcd_clear_char(lcd* _lcd, uint8_t row, uint8_t column)
{
    _lcd->row = row;
    _lcd->column = column;
    lcd_set_cursor(_lcd, row, column);
    lcd_write_char(_lcd, ' ');
    lcd_set_cursor(_lcd, row, column);

}

void lcd_clear_row(lcd* _lcd, uint8_t row)
{
    _lcd->column = 0;
    lcd_set_cursor(_lcd, row, 0);
    for (uint8_t i = 0; i < 15; ++i)
        lcd_write_char(_lcd, ' ');
    lcd_set_cursor(_lcd, row, 0);
}

void lcd_clear_section(lcd* _lcd, uint8_t row, uint8_t begin_column_index, uint8_t end_column_index)
{
    _lcd->column = begin_column_index;
    lcd_set_cursor(_lcd, row, begin_column_index);
    for (uint8_t i = begin_column_index; i <= end_column_index; ++i)
        lcd_write_char(_lcd, ' ');
    lcd_set_cursor(_lcd, row, begin_column_index);
}