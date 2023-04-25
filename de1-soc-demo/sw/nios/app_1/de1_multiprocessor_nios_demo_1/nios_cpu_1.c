/*
 * nios_cpu_1.c
 *
 *  Created on: 2023-04-25
 *      Author: Aleksandar Beslic
 */

#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_mutex_regs.h"
#include "altera_avalon_mutex.h"
#include "altera_avalon_mailbox_simple.h"

alt_mutex_dev* mutex ;
altera_avalon_mailbox_dev* mbox;

static void timer_isr(void* context)
{
	volatile int* cnt_ptr = (volatile int*) context;
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_1_BASE, 0);

	if (altera_avalon_mutex_is_mine(mutex))
	{
		*cnt_ptr = *cnt_ptr + 1;

		if (*cnt_ptr > 31)
			*cnt_ptr = 0;
	}
}

int main(void)
{
	volatile int cnt;
	int sw;
	alt_u32 message[2];
	cnt = 0x00;
	// set up timer
	// one second period, 50e6 counts = 0x2FAF080
	IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_1_BASE, 0xf080);
	IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_1_BASE, 0x2fa);
	// set RUN, set CONTuous, set ITO
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, 7);

	// register the interrupt (and turn it on)
	alt_ic_isr_register(TIMER_1_IRQ_INTERRUPT_CONTROLLER_ID,
					  TIMER_1_IRQ, timer_isr,
					  (void*)&cnt, 0x0);

	// get hardware mutex handle
	mutex = altera_avalon_mutex_open(MUTEX_0_NAME);

	// init the maibox
	mbox = altera_avalon_mailbox_open(MAILBOX_0_NAME, NULL, NULL);

	// now just loop and blink some lights
	while(1)
	{
		// read the switches
		sw = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_0_BASE);
		if (sw & (1 << 0)) // if sw0 is up, then get mutex
		{
			// acquire the mutex, setting the value to one
			altera_avalon_mutex_lock(mutex, 1);
		}
		else // switch is not up, release the lock
		{
			altera_avalon_mutex_unlock(mutex);
		}

		// display two different results depending on mutex
		if (altera_avalon_mutex_is_mine(mutex))
		{
			// display local value
			IOWR_ALTERA_AVALON_PIO_DATA(LEDS_1_BASE, cnt);
		}
		else
		{
			// display message from processor 1
			altera_avalon_mailbox_retrieve_poll(mbox, message, 100);
			IOWR_ALTERA_AVALON_PIO_DATA(LEDS_1_BASE, message[1]);
		}
	}
}

