/*
 * Cigarette smokers problem
 *
 * I have tobacco.c but NO paper or matches :(
 *
 *  Created on: 2023-05-08
 *      Author: Aleksandar Beslic
 *
 * P.S. Smoking kills and NO you DON'T look cool doing it
 */


#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_mutex_regs.h"
#include "altera_avalon_mutex.h"
#include "altera_avalon_mailbox_simple.h"
#include "altera_avalon_mailbox_simple_regs.h"

#define MY_NEEDS 2
#define ALL_DONE 14

alt_mutex_dev* paper_mutex;
alt_mutex_dev* matches_mutex;
altera_avalon_mailbox_dev* mbox_receive;
altera_avalon_mailbox_dev* mbox_send;

int main(void)
{
	alt_u32 message[2];
	alt_u32 consume_time;
	alt_u32 send_msg = ALL_DONE;

	// Get hardware mutex handle for paper and matches
	paper_mutex = altera_avalon_mutex_open(MUTEX_0_NAME);
	matches_mutex = altera_avalon_mutex_open(MUTEX_2_NAME);

	// Init the mailbox hps to fpga
	mbox_receive = altera_avalon_mailbox_open(MAILBOX_1_NAME, NULL, NULL);

	// Init the mailbox fpga to hps
	mbox_send = altera_avalon_mailbox_open(MAILBOX_4_NAME, NULL, NULL);

	while(1)
	{
		altera_avalon_mailbox_retrieve_poll(mbox_receive, message, 100);

		// Can I smoke my cig
		if (message[0] == MY_NEEDS)
		{
			// Acquire the mutex, setting the value to one
			altera_avalon_mutex_trylock(paper_mutex, 2);
			altera_avalon_mutex_trylock(matches_mutex, 2);

				// Smoking my cigarette
				consume_time = rand() % 10 + 1;
				printf("Smoker with tobacco, time: %ld\n", consume_time);

			// Release the mutex
			altera_avalon_mutex_unlock(paper_mutex);
			altera_avalon_mutex_unlock(matches_mutex);

			// Telling the agent that I'm done smoking
			altera_avalon_mailbox_send(mbox_send, &send_msg, 0, POLL);
		}

		usleep(1000010);
	}

	return 0;
}
