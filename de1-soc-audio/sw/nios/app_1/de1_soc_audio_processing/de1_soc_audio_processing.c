/*
 * de1_soc_audio_processing.c
 *
 *  Created on: Jun 16, 2023
 *      Author: Nemanja
 */

#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_mailbox_simple.h"
#include "altera_up_avalon_audio.h"

#define BUFF_MAX_VALUE 65536 // 2^16 --> audio data buff width 16 bit
#define LEDS_NUM 10 // number of LEDs
#define THD (BUFF_MAX_VALUE-1)/(LEDS_NUM+1) // LEDS_NUM+1 intervals

altera_avalon_mailbox_dev* mailbox_0;
altera_avalon_mailbox_dev* mailbox_1;
alt_up_audio_dev* audio_dev;

void vometer(unsigned int value){
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

int main(void)
{
	unsigned int l_buf;
	unsigned int r_buf;

	audio_dev = alt_up_audio_open_dev(AUDIO_0_NAME);

	while(1)
	{
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
	}
}
