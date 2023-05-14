/*
 * nios_cpu_0.c
 *
 *  Created on: Apr 28, 2021
 *      Author: mknezic
 */

#include <stdio.h>
#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_mutex_regs.h"
#include "altera_avalon_mutex.h"
#include <unistd.h>

alt_mutex_dev* agent_mutex;
alt_mutex_dev* paper_mutex;
alt_mutex_dev* matches_mutex;



static int my_mutex_trylock( alt_mutex_dev* dev, alt_u32 value )
{
  alt_u32 id=124, data, check;
  int ret_code = -1;

  /* the data we want the mutex to hold */
  data = (id << ALTERA_AVALON_MUTEX_MUTEX_OWNER_OFST) | value;

  /* attempt to write to the mutex */
  IOWR_ALTERA_AVALON_MUTEX_MUTEX(dev->mutex_base, data);

  check = IORD_ALTERA_AVALON_MUTEX_MUTEX(dev->mutex_base);

  if ( check == data)
  {
    ret_code = 0;
  }

  return ret_code;
}

void my_mutex_lock( alt_mutex_dev* dev, alt_u32 value )
{
  /*
   * When running in a multi threaded environment, obtain the "lock"
   * semaphore. This ensures that reading from the device is thread-safe.
   */

  //ALT_SEM_PEND (dev->lock, 0);

  while ( my_mutex_trylock( dev, value ) != 0);
}

int main(void)
{

	// get hardware mutex handle
	agent_mutex = altera_avalon_mutex_open(TOBACCO_MUTEX_NAME);
	paper_mutex = altera_avalon_mutex_open(PAPER_MUTEX_NAME);
	matches_mutex = altera_avalon_mutex_open(MATCHES_MUTEX_NAME);
	// now just loop and blink some lights
	int state=0;
	while(1)
	{
			// acquire the mutex, setting the value to one
			altera_avalon_mutex_lock(agent_mutex, 1);
			printf("Agent is placing resources on table!\n");
			// unlock and then lock it as different owner in this case userID=124
			altera_avalon_mutex_unlock(agent_mutex);
			my_mutex_lock(agent_mutex,1);
//
//			// lock resources
			altera_avalon_mutex_lock(paper_mutex,1);
			altera_avalon_mutex_lock(matches_mutex,1);
			if(state==0){
				state=1;
				altera_avalon_mutex_unlock(matches_mutex);
				usleep(100000);
			}
			else{
				state=0;
				altera_avalon_mutex_unlock(paper_mutex);
				usleep(100000);
			}
	}
}
