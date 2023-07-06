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
#include <math.h>

#define BUFF_MAX_VALUE 16777216/2 // 2^24 --> audio data buff width 32 bit 4294967296
#define LEDS_NUM 10 // number of LEDs
#define THD (BUFF_MAX_VALUE-1)/(LEDS_NUM+1) // LEDS_NUM+1 intervals

#define LOOPBACK_STATE 1
#define RECORD_STATE 2
#define REC_STOP_STATE 3
#define PLAY_STATE 4
#define PAUSE_STATE 5
#define IDLE 6

#define SAMPLE_RATE 48000
#define DURATION 1.0
#define FREQUENCY 1000.0

altera_avalon_mailbox_dev* mailbox_0; // NIOSII_0 mailbox
altera_avalon_mailbox_dev* mailbox_1; // HPS mailbox
altera_avalon_mailbox_dev* mailbox_2;
alt_up_audio_dev* audio_dev;

alt_u8 blink = 0;
alt_u8 signal1 = 0;

/* Sheared memory*/
uint32_t __attribute__((section (".l_buffer"))) l_buffer[262016];
uint32_t __attribute__((section (".r_buffer"))) r_buffer[262016];
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
	IOWR_ALTERA_AVALON_PIO_DATA(LEDS_0_BASE, ~val);
}

void start_stop_timer(int start_stop) // 1 -> start, 0 -> stop
{
	switch(start_stop){
		case 0:
			IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, 8);
			break;
		case 1:
			IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, 7);
			break;
	}
}

int buf_avg(unsigned int* buf, int len){
	int sum = 0;
	for(int i = 0; i < len; i++){
		sum += buf[i];
	}
	return sum/len;
}

void reset_variables(){
	buff_inx = 0;
	signal1 = 0;
}

int main(void)
{
	unsigned int message[2];
	unsigned int message_hps[2] = {111, 111};
	unsigned int signal_hps[2] = {0x00001111, 0};
	unsigned int l_buf1[128];
	unsigned int r_buf1[128], r[128];
	unsigned int l_buf;
	unsigned int r_buf;
	int state = 1; // 1 -> loopback, 2 -> record, 3 -> stop recording, 4 -> play, 5 -> pause
	int play_flag = 2;

	// one second period, 25e6 counts = 0x17D7840
	IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_1_BASE, 0x7840);
	IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_1_BASE, 0x17D);
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, 8);

	// register the interrupt (and turn it on)
	alt_irq_register(TIMER_1_IRQ, NULL, timer_isr);

	mailbox_0 = altera_avalon_mailbox_open(MAILBOX_0_NAME, NULL, NULL);
	mailbox_1 = altera_avalon_mailbox_open(MAILBOX_1_NAME, NULL, NULL);
	mailbox_2 = altera_avalon_mailbox_open(MAILBOX_2_NAME, NULL, NULL);
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
				if (alt_up_audio_read_fifo_avail(audio_dev, ALT_UP_AUDIO_RIGHT) >= 127) // check if data is available
				{

					// read audio buffer
					alt_up_audio_read_fifo(audio_dev, r_buf1, 128, ALT_UP_AUDIO_RIGHT);
					alt_up_audio_read_fifo(audio_dev, l_buf1, 128, ALT_UP_AUDIO_LEFT);


					for(int i = 0; i < 128; i++){
						r_buf1[i] &= 0x00ffffff;
						l_buf1[i] &= 0x00ffffff;
					}

					// turn on LED-s
					vometer(buf_avg(r_buf1, 128));

					// write audio buffer
					alt_up_audio_write_fifo(audio_dev, r_buf1, 128, ALT_UP_AUDIO_RIGHT);
					alt_up_audio_write_fifo(audio_dev, l_buf1, 128, ALT_UP_AUDIO_LEFT);
				}
				play_flag = 2;
				break;
			case RECORD_STATE:
				start_stop_timer(1); // start timer
				if(signal1 == 0){
					signal1 = 1;
					signal_hps[1] = 11; // SAVE_AUDIO
					altera_avalon_mailbox_send(mailbox_1, signal_hps, 0, POLL);
					memset(l_buffer, 0, sizeof(l_buffer));
					memset(r_buffer, 0, sizeof(r_buffer));
				}
				if (alt_up_audio_read_fifo_avail(audio_dev, ALT_UP_AUDIO_RIGHT) >= 127) // check if data is available
				{
					// read audio buffer
					alt_up_audio_read_fifo(audio_dev, r_buf1, 128, ALT_UP_AUDIO_RIGHT);
					alt_up_audio_read_fifo(audio_dev, l_buf1, 128, ALT_UP_AUDIO_LEFT);


					for(int i = 0; i < 128; i++){
						r_buf1[i] &= 0x00ffffff;
						l_buf1[i] &= 0x00ffffff;
					}

					for(int i = 0; i < 128; i++, buff_inx++){
						// write to share memory
						if(left_right)
							r_buffer[buff_inx] = r_buf1[i];
						else
							l_buffer[buff_inx] = r_buf1[i];
					}

					if(buff_inx >= 262016){
						buff_inx = 0;
						if(left_right){
							signal_hps[1] = 4;
							memset(l_buffer, 0, sizeof(l_buffer));
						}
						else{
							signal_hps[1] = 3;
							memset(r_buffer, 0, sizeof(r_buffer));
						}

						left_right = (left_right+1)%2;

						// signalize HPS that buffer is ready (3 -> left buff, 4 -> right buff)
						altera_avalon_mailbox_send(mailbox_1, signal_hps, 0, POLL);
					}
				}
				play_flag = 2;
				break;
			case REC_STOP_STATE:
				if(left_right)
					signal_hps[1] = 4;
				else
					signal_hps[1] = 3;

				left_right = (left_right+1)%2;
				// signalize HPS that buffer is ready (3 -> left buff, 4 -> right buff)
				altera_avalon_mailbox_send(mailbox_1, signal_hps, 0, POLL);

				IOWR_ALTERA_AVALON_PIO_DATA(LEDS_0_BASE, 0);
				start_stop_timer(0); // stop timer
				reset_variables();
				signal_hps[1] = 2;
				altera_avalon_mailbox_send(mailbox_1, signal_hps, 0, POLL);
				state = IDLE;
				play_flag = 2;
				break;
			case PLAY_STATE:
				IOWR_ALTERA_AVALON_PIO_DATA(LEDS_0_BASE, 0);
				start_stop_timer(0); // stop timer
				if(signal1 == 0){
					signal1 = 1;
					signal_hps[1] = 10; // PLAY_AUDIO
					altera_avalon_mailbox_send(mailbox_1, signal_hps, 0, POLL);
				}

				/* Ako je u pitanju prvo citanje semplova. */
				if(play_flag == 2)
				{
					/* Signal da HPS puni lijevi bafer. */
					signal_hps[1] = 6;
					altera_avalon_mailbox_send(mailbox_1, signal_hps, 0, POLL);
				}

				/* Provjeri da li je bafer spreman za citanje. */
				altera_avalon_mailbox_retrieve_poll(mailbox_2, message_hps, 100);

				if(message_hps[1] == 9) // Spreman lijevi bafer.
				{
					/* Blokiranje ponovnog citanja lijevog bafera ukoliko desni jos nije spreman */
					if(play_flag != 1)
					{
						/* Signal da HPS puni desni bafer. */
						signal_hps[1] = 7;
						altera_avalon_mailbox_send(mailbox_1, signal_hps, 0, POLL);

						buff_inx = 0;
						for(buff_inx = 0; buff_inx < sizeof(l_buffer)/sizeof(l_buffer[0]); buff_inx+=2)
						{
							r_buf = l_buffer[buff_inx];
							l_buf = l_buffer[buff_inx];

							altera_avalon_mailbox_retrieve_poll(mailbox_0, message, 100);
							if(message[1] != PLAY_STATE)
								break;

//							vometer(r_buf);
//							vometer(buf_avg(r_buf1, 3));

							alt_up_audio_write_fifo(audio_dev, &r_buf, 1, ALT_UP_AUDIO_RIGHT);
							alt_up_audio_write_fifo(audio_dev, &l_buf, 1, ALT_UP_AUDIO_LEFT);
						}

						play_flag = 1;
					}
				}
				else if(message_hps[1] == 10) // Spreman desni bafer.
				{
					/* Blokiranje ponovnog citanja desnog bafera ukoliko lijevi jos nije spreman. */
					if(play_flag != 0)
					{
						/* Signal da HPS puni lijevi bafer. */
						signal_hps[1] = 6;
						altera_avalon_mailbox_send(mailbox_1, signal_hps, 0, POLL);

						buff_inx = 0;
						for(buff_inx = 0; buff_inx < sizeof(l_buffer)/sizeof(l_buffer[0]); buff_inx+=2)
						{
							r_buf = l_buffer[buff_inx];
							l_buf = l_buffer[buff_inx];

							altera_avalon_mailbox_retrieve_poll(mailbox_0, message, 100);
							if(message[1] != PLAY_STATE)
								break;

//							vometer(buf_avg(r_buf1, 3));
//							vometer(r_buf);

							alt_up_audio_write_fifo(audio_dev, &r_buf, 1, ALT_UP_AUDIO_RIGHT);
							alt_up_audio_write_fifo(audio_dev, &l_buf, 1, ALT_UP_AUDIO_LEFT);
						}

						play_flag = 0;
					}
				}
				break;
			case PAUSE_STATE:
				IOWR_ALTERA_AVALON_PIO_DATA(LEDS_0_BASE, 0);
				start_stop_timer(0); // stop timer
				reset_variables();
				signal_hps[1] = 5;
				altera_avalon_mailbox_send(mailbox_1, signal_hps, 0, POLL);
				state = IDLE;
				play_flag = 2;
				break;
			default:
				break;
		}
	}
}
