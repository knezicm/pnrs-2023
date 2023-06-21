/*
 * de1_soc_audio_processing.c
 *
 *  Created on: Jun 16, 2023
 *      Author: Nemanja
 */

#include "stdint.h"
#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_mailbox_simple.h"
#include "altera_up_avalon_audio.h"

#define BUFF_MAX_VALUE 65536 // 2^16 --> audio data buff width 16 bit
#define LEDS_NUM 10 // number of LEDs
#define THD (BUFF_MAX_VALUE-1)/(LEDS_NUM+1) // LEDS_NUM+1 intervals

altera_avalon_mailbox_dev* mailbox_0; // NIOSII_0 mailbox
altera_avalon_mailbox_dev* mailbox_1; // HPS mailbox
alt_up_audio_dev* audio_dev;

alt_u16 blink = 0;

/* Sheared memory*/
uint32_t __attribute__((section (".l_buffer"))) l_buffer[262143];
uint32_t __attribute__((section (".r_buffer"))) r_buffer[262143];

static void timer_isr(void* isr_context, alt_u32 id)
{
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_1_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_DATA(LEDS_0_BASE, blink);
	blink = (blink+1)%2;
}

void vometer(unsigned int value)
{
	alt_u16 val = 0, mask = 0b1;
	for(int i = 1; i <= 10; i++){
		if(value > i*THD){
			val += mask;
			mask <<= 1;
		}
		else
			break;
	}
	IOWR_ALTERA_AVALON_PIO_DATA(LEDS_0_BASE, val);
}

void start_stop_timer(int start_stop) // 1 -> start, 0 -> stop
{
	IOWR_ALTERA_AVALON_PIO_DATA(LEDS_0_BASE, 0);
	switch(start_stop){
		case 0:
			IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, 8);
			break;
		case 1:
			IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, 7);
			break;
	}
}
int main(void)
{
	unsigned int message[2];
	unsigned int l_buf;
	unsigned int r_buf;
	int state = 1; // 1 -> loopback, 2 -> record, 3 -> play

	// one second period, 50e6 counts = 0x2FAF080
	IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_1_BASE, 0xf080);
	IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_1_BASE, 0x2fa);
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, 8);

	// register the interrupt (and turn it on)
	alt_irq_register(TIMER_1_IRQ, NULL, timer_isr);

	mailbox_0 = altera_avalon_mailbox_open(MAILBOX_0_NAME, NULL, NULL);
	mailbox_1 = altera_avalon_mailbox_open(MAILBOX_1_NAME, NULL, NULL);
	audio_dev = alt_up_audio_open_dev(AUDIO_0_NAME);

	while(1)
	{
		altera_avalon_mailbox_retrieve_poll(mailbox_0, message, 100);
		if(message[1] != 0)
			state = message[1];

		switch(state){
			case 1:
				start_stop_timer(0); // stop timer
				if (alt_up_audio_read_fifo_avail(audio_dev, ALT_UP_AUDIO_RIGHT) > 0) // check if data is available
				{
					// read audio buffer
					alt_up_audio_read_fifo(audio_dev, &(r_buf), 1, ALT_UP_AUDIO_RIGHT);
					alt_up_audio_read_fifo(audio_dev, &(l_buf), 1, ALT_UP_AUDIO_LEFT);

					// turn on LED-s
					vometer(r_buf);

					// write audio buffer
					alt_up_audio_write_fifo(audio_dev, &(r_buf), 1, ALT_UP_AUDIO_RIGHT);
					alt_up_audio_write_fifo(audio_dev, &(l_buf), 1, ALT_UP_AUDIO_LEFT);
				}
				break;
			case 2:
				start_stop_timer(1); // start timer
				if (alt_up_audio_read_fifo_avail(audio_dev, ALT_UP_AUDIO_RIGHT) > 0) // check if data is available
				{
					// read audio buffer
					alt_up_audio_read_fifo(audio_dev, &(r_buf), 1, ALT_UP_AUDIO_RIGHT);
					alt_up_audio_read_fifo(audio_dev, &(l_buf), 1, ALT_UP_AUDIO_LEFT);

					// write to share memory

					// signalize HPS that buffer is ready (1 -> left buff, 2 -> right buff)
				}
				break;
			case 3:
				start_stop_timer(0); // stop timer
				break;
		}
	}
}
