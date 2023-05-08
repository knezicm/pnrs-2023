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
#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_mutex_regs.h"
#include "altera_avalon_mutex.h"
#include "altera_avalon_mailbox_simple.h"
#include "altera_avalon_mailbox_simple_regs.h"

#define MY_NEEDS 2

alt_mutex_dev* paper_mutex;
alt_mutex_dev* matches_mutex;
altera_avalon_mailbox_dev* mbox_receive;
altera_avalon_mailbox_dev* mbox_send;

int main(void)
{
	alt_u32 message[2];

	// Get hardware mutex handle for paper and matches
	paper_mutex = altera_avalon_mutex_open(MUTEX_0_NAME);
	matches_mutex = altera_avalon_mutex_open(MUTEX_2_NAME);

	// Init the mailbox hps to fpga
	mbox_receive = altera_avalon_mailbox_open(MAILBOX_0_NAME, NULL, NULL);

	// Init the mailbox fpga to hps
	mbox_send = altera_avalon_mailbox_open(MAILBOX_1_NAME, NULL, NULL);

	while(1)
	{
		altera_avalon_mailbox_retrieve_poll(mbox_receive, message, 100);

		if (message[1] == MY_NEEDS)
		{
			// Acquire the mutex, setting the value to one
			altera_avalon_mutex_lock(paper_mutex, 1);
			altera_avalon_mutex_lock(matches_mutex, 1);
		}
		else // My need are not met
		{
			// Release the mutex
			altera_avalon_mutex_unlock(paper_mutex);
			altera_avalon_mutex_unlock(matches_mutex);
		}

		// Can I smoke my cig
		if(altera_avalon_mutex_is_mine(paper_mutex) && altera_avalon_mutex_is_mine(matches_mutex))
		{
			// Do stuff
			printf("Imam duvan ima sve\n");
		}
	}

	return 0;
}
