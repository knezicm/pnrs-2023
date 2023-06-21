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

#define BUFF_MAX_VALUE 4294967296 // 2^32 --> audio data buff width 32 bit
#define LEDS_NUM 10 // number of LEDs
#define THD (BUFF_MAX_VALUE-1)/(LEDS_NUM+1) // LEDS_NUM+1 intervals

#define LOOPBACK_STATE 1
#define RECORD_STATE 2
#define REC_STOP_STATE 3
#define PLAY_STATE 4
#define PAUSE_STATE 5
#define IDLE 6

/*#define PLAY_AUDIO 10
#define SAVE_AUDIO 11
*/

altera_avalon_mailbox_dev* mailbox_0; // NIOSII_0 mailbox
altera_avalon_mailbox_dev* mailbox_1; // HPS mailbox
alt_up_audio_dev* audio_dev;

alt_u8 blink = 0;
alt_u8 signal = 0;

/* Sheared memory*/
uint32_t __attribute__((section (".l_buffer"))) l_buffer[262143];
uint32_t __attribute__((section (".r_buffer"))) r_buffer[262143];
int buff_inx = 0, left_right = 1;

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
	//IOWR_ALTERA_AVALON_PIO_DATA(LEDS_0_BASE, 0);
	switch(start_stop){
		case 0:
			IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, 8);
			break;
		case 1:
			IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, 7);
			break;
	}
}

void reset_variables(){
	buff_inx = 0;
	signal = 0;
}

int main(void)
{
	unsigned int message[2];
	unsigned int signal_hps[2] = {0x00001111, 0};
	uint32_t l_buf;
	uint32_t r_buf;
	int state = 1; // 1 -> loopback, 2 -> record, 3 -> stop recording, 4 -> play, 5 -> pause

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
			case LOOPBACK_STATE:
				start_stop_timer(0); // stop timer
				reset_variables();
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
			case RECORD_STATE:
				start_stop_timer(1); // start timer
				if(signal == 0){
					signal = 1;
					signal_hps[1] = 11; // SAVE_AUDIO
					altera_avalon_mailbox_send(mailbox_1, signal_hps, 0, POLL);
				}
				if (alt_up_audio_read_fifo_avail(audio_dev, ALT_UP_AUDIO_RIGHT) > 0) // check if data is available
				{
					// read audio buffer
					alt_up_audio_read_fifo(audio_dev, &(r_buf), 1, ALT_UP_AUDIO_RIGHT);
					alt_up_audio_read_fifo(audio_dev, &(l_buf), 1, ALT_UP_AUDIO_LEFT);

					// write to share memory
					if(left_right)
						r_buffer[buff_inx] = r_buf;
					else
						l_buffer[buff_inx] = r_buf;

					buff_inx++;
					if(buff_inx == sizeof(r_buffer)){
						buff_inx = 0;
						if(left_right)
							signal_hps[1] = 4;
						else
							signal_hps[1] = 3;

						left_right = (left_right+1)%2;
						// signalize HPS that buffer is ready (3 -> left buff, 4 -> right buff)
						altera_avalon_mailbox_send(mailbox_1, signal_hps, 0, POLL);
					}
				}
				break;
			case REC_STOP_STATE:
				start_stop_timer(0); // stop timer
				reset_variables();
				signal_hps[1] = 2;
				altera_avalon_mailbox_send(mailbox_1, signal_hps, 0, POLL);
				state = IDLE;
				break;
			case PLAY_STATE:
				start_stop_timer(0); // stop timer
				if(signal == 0){
					signal = 1;
					signal_hps[1] = 10; // PLAY_AUDIO
					altera_avalon_mailbox_send(mailbox_1, signal_hps, 0, POLL);
				}

				// code for playing recorded audio

				break;
			case PAUSE_STATE:
				start_stop_timer(0); // stop timer
				reset_variables();
				signal_hps[1] = 5;
				altera_avalon_mailbox_send(mailbox_1, signal_hps, 0, POLL);
				state = IDLE;
				break;
			default:
				break;
		}
	}
}
