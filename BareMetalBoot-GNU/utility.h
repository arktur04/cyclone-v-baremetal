#ifndef UTILITY_H
#define UTILITY_H

int uart_put_char(char value);
void delay_us(int us);
void watchdog_kick(void);

#endif
