#ifndef COMMANDS_H
#define COMMANDS_H
//#include "hd44780.h"
#include <stdint.h> // uint...
#include <stddef.h> // size_t
#include <string.h> // strlen

/****************************
 * FORWARD DECLARATIONS     *
 ****************************/
typedef struct lcd lcd;

/****************************
 * API DECLARATIONS         *
 ****************************/

/**
 * @brief Force the LCD into 8-bit mode.
 * 
 * The LCD can startup in different states. To do a proper "reset", we send the 8-bit function set command three times.
 * Refer to wikipedia for more info: https://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller
 * 
 * @param _lcd 
 */
void lcd_reset(lcd *_lcd);

/**
 * @brief Send a generic command to the LCD
 * 
 * @param _lcd Pointer to LCD struct
 * @param rs_pin_value Some commands require the RS pin to be either 0 (for config stuff) or 1 (to write characters).
 * @param command A binary, hex, decimal or character value to send to the LCD.
 */
void lcd_send_command(lcd* _lcd, uint8_t rs_pin_value, uint8_t rw_pin_value, uint8_t command);

/**
 * @brief Enable 4-bit mode. MUST DO THIS BEFORE SENDING ANY OTHER COMMANDS (if you are actually using 4-bit mode).
 * 
 * 4 bit mode requires only 1 pulse instead of the general 2 pulses we send in general commands (e.g. lcd_send_command) which is why this function is separate.
 * 
 * @param _lcd Pointer to LCD struct
 */
void lcd_enable_4bit_mode(lcd* _lcd);

/**
 * @brief Enable a blinking cursor on LCD.
 * 
 * @param _lcd Pointer to LCD struct
 */
void lcd_enable_cursor(lcd* _lcd);

/**
 * @brief Clear the LCD screen.
 * 
 * @param _lcd Pointer to LCD struct
 */
void lcd_clear_screen(lcd* _lcd);

/**
 * @brief "Clear" a character at (row, column) position on the LCD
 * 
 * There is actually no "clear" option for a specific index. Instead, we write a space character.
 * 
 * @param _lcd Pointer to LCD struct.
 */
void lcd_clear_char(lcd* _lcd, uint8_t row, uint8_t column);

/**
 * @brief "Clear" character at current cursor position and move back one.
 * 
 * There is actuall no "clear" option for characters. Instead, we write a space character.
 * 
 * @param _lcd 
 */
void lcd_backspace(lcd* _lcd);

/**
 * @brief "Clear" a row on the LCD
 * 
 * There is no "clear" option for a row. Instead, we replace the row with space characters.
 * @param _lcd Pointer to LCD struct
 * @param row Row to clear (0-1)
 */
void lcd_clear_row(lcd* _lcd, uint8_t row);

/**
 * @brief "Clear" a section within a row on the LCD. The cursor will be set at begin_column_index after clearing.
 * 
 * There is no "clear" option for characters. Instead, we replace them with space characters.
 * 
 * @param _lcd Pointer to LCD struct
 * @param row The row (0-1) in which to clear data from
 * @param begin_column_index Starting index (inclusive) of which characters to clear
 * @param end_column_index Ending index (inclusive) of which characters to clear
 */
void lcd_clear_section(lcd* _lcd, uint8_t row, uint8_t begin_column_index, uint8_t end_column_index);

/**
 * @brief Write a single character to LCD string.
 * 
 * @param _lcd Pointer to LCD struct
 * @param row Either 0 or 1 indicating which row of the LCD to write to
 * @param c Character to write
 */
void lcd_write_char(lcd* _lcd, const char c);

/**
 * @brief Write a string to LCD
 * 
 * @param _lcd Pointer to LCD struct
 * @param row Either 0 or 1 indicating which row of the LCD to write to
 * @param str String to write
 */
void lcd_write_string(lcd* _lcd, const char* str);

/**
 * @brief Shift the cursor to the right by one character.
 * 
 * @param _lcd Pointer to LCD struct
 */
void lcd_shift_cursor_right(lcd* _lcd);

/**
 * @brief Shift the cursort to the left by one character.
 * 
 * @param _lcd Pointer to LCD struct
 */
void lcd_shift_cursor_left(lcd* _lcd);

/**
 * @brief Set the cursor position to (row, column) on the LCD.
 * 
 * @param _lcd Pointer to LCD struct
 * @param row Row position (0-1)
 * @param column Column position (0-15)
 */
void lcd_set_cursor(lcd* _lcd, uint8_t row, uint8_t column);

#endif