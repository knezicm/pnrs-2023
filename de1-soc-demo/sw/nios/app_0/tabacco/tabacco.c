/*
 * matches_smoker.c
 *
 *  Created on: May 9, 2023
 *      Author: Medion
 */

#include <stdio.h>
#include <stdlib.h>

#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_mutex_regs.h"
#include "altera_avalon_mutex.h"
#include "altera_avalon_mailbox_simple.h"

alt_mutex_dev* paper,* matches;
altera_avalon_mailbox_dev* mbox_s,* mbox_r;

alt_u32 message[2] = {0x00001111, 0};


int main(void)
{
	srand(NULL);
	// get hardware mutex handle
	paper = altera_avalon_mutex_open(MUTEX_0_NAME);
	matches = altera_avalon_mutex_open(MUTEX_2_NAME);

	// init the maibox
	mbox_s = altera_avalon_mailbox_open(MAILBOX_0_NAME, NULL, NULL);
	mbox_r = altera_avalon_mailbox_open(MAILBOX_1_NAME, NULL, NULL);

	message[1] = 0;
	altera_avalon_mailbox_send(mbox_s, message, 0, POLL);

	// now just loop and blink some lights
	while(1)
	{
		if(altera_avalon_mutex_trylock(matches, 1) == 0){
			if(altera_avalon_mutex_trylock(paper, 1) == 0){
				altera_avalon_mutex_unlock(matches);
				altera_avalon_mutex_unlock(paper);
				message[1] = 2;
				altera_avalon_mailbox_send(mbox_s, message, 0, POLL);
				printf("Tabacco: pusi...\n");
				usleep((rand() % 5 + 1)*1000000);
				message[1] = 1;
				altera_avalon_mailbox_send(mbox_s, message, 0, POLL);
				printf("Zavrsio\n");
			}
			else
				altera_avalon_mutex_unlock(matches);
		}
		usleep(1000);
	}
}
