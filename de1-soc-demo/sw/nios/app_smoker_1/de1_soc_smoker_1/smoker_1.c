/*
 * smoker_1.c
 *
 *  Created on: May 19, 2023
 *      Author: popovict
 *
 *  This smoker has tobacco.
 */

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_mutex_regs.h"
#include "altera_avalon_mutex.h"
#include "altera_avalon_mailbox_simple.h"
#include "altera_avalon_mailbox_simple_regs.h"

alt_mutex_dev* matches_mutex;
alt_mutex_dev* paper_mutex;
alt_mutex_dev* tobacco_mutex;

altera_avalon_mailbox_dev* message_rx;
altera_avalon_mailbox_dev* message_tx;


int main()
{
	alt_u32 message = 0;
    alt_u32 smoking_is_done = 1;
	// Get hardware mutex handle for tobacco and matches
	tobacco_mutex = altera_avalon_mutex_open(MUTEX_1_NAME);
	paper_mutex = altera_avalon_mutex_open(MUTEX_2_NAME);
    matches_mutex = altera_avalon_mutex_open(MUTEX_0_NAME);

	// Init the mailbox hps to fpga
	message_rx = altera_avalon_mailbox_open(MAILBOX_1_NAME, NULL, NULL);

	// Init the mailbox fpga to hps
	message_tx = altera_avalon_mailbox_open(MAILBOX_3_NAME, NULL, NULL);

	// This smoker has tobacco
	altera_avalon_mutex_trylock(tobacco_mutex, 1);

	while(1)
	{
		if(altera_avalon_mailbox_retrieve_poll(message_rx, &message, 100))
		{
			// Acquire the mutex, setting the value to one
			altera_avalon_mutex_trylock(matches_mutex, 1);
			altera_avalon_mutex_trylock(paper_mutex, 1);

			printf("Smoker 0: Acquired ingredients, rolling the cigarette!\n");

			// Simulate rolling the cigarette by delaying between 1 and 5 seconds
			usleep((rand() % 5 + 1) * 1000000);

			// Release the mutex
			altera_avalon_mutex_unlock(tobacco_mutex);
			altera_avalon_mutex_unlock(paper_mutex);
			altera_avalon_mutex_unlock(matches_mutex);

			// Signal the agent smoking is done
			altera_avalon_mailbox_send(message_tx, &smoking_is_done, 0, POLL);

		}

	}


}
