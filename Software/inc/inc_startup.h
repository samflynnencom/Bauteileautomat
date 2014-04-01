/*
 * inc_startup.h
 *
 *  Created on: 21.10.2013
 *      Author: einball
 */

#ifndef INC_STARTUP_H_
#define INC_STARTUP_H_

#define TARGET_IS_BLIZZARD_RA2
#define TM4C1233H6PM

#include <stdint.h>
#include <stdbool.h>

#include <driverlib/interrupt.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <driverlib/ssi.h>
#include <driverlib/gpio.h>
#include <driverlib/watchdog.h>
#include <driverlib/timer.h>
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>

#include <hardware/hw_ints.h>
#include <hardware/hw_sysctl.h>
#include <hardware/hw_uart.h>
#include <hardware/hw_ssi.h>
#include <hardware/hw_gpio.h>
#include <hardware/hw_watchdog.h>
#include <hardware/hw_timer.h>
#include <hardware/hw_memmap.h>

#include <inc_msg.h>

void setup_GPIO();
void setup_UART();
void setup_SPI();
void setup_INT();
void setup_HOME_POS();
void setup_JOB_QUEUE(sJobQueue *Queue);

#endif /* INC_STARTUP_H_ */
