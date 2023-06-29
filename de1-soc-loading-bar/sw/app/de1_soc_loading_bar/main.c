/*
 * main.c
 *
 *  Created on: 2023-06-12
 *      Author: Tanja Popovic
 */

#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_pio_regs.h"

static void timer_isr(void* context)
{
	volatile int* cnt_ptr = (volatile int*) context;
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);

		*cnt_ptr *= 2 + 1;

		if (*cnt_ptr == 1024)
			*cnt_ptr = 0;
}


int main()
{
	volatile int cnt;
	cnt = 0x00;
	// set up timer
	// one fourth of second period, 12.5e6 counts = 0xBEBC20
	IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, 0xbc20);
	IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, 0xbe);
	// set RUN, set CONTuous, set ITO
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 7);

	// register the interrupt (and turn it on)
	alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID,TIMER_0_IRQ, timer_isr,(void*)&cnt, 0x0);

		// now just loop and start loading bar
		while(1)
		{
			// display current count
			IOWR_ALTERA_AVALON_PIO_DATA(LEDS_0_BASE, cnt);
		}

}
