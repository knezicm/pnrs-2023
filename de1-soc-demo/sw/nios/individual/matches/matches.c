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

alt_mutex_dev* mutex;
alt_mutex_dev* agent_mutex;

static int resource_mutex_trylock( alt_mutex_dev* dev, alt_u32 value ,alt_u32 id)
{
  alt_u32 data, check;
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

void resource_mutex_lock( alt_mutex_dev* dev, alt_u32 value )
{
  /*
   * When running in a multi threaded environment, obtain the "lock"
   * semaphore. This ensures that reading from the device is thread-safe.
   */

  //ALT_SEM_PEND (dev->lock, 0);
  alt_u32 agent_id = 2;
  while ( resource_mutex_trylock( dev, value, agent_id ) != 0);
}



void release_resources(){
	// unlock taken resource
	altera_avalon_mutex_unlock( mutex );
	// lock resource from agent perspective
	resource_mutex_lock(mutex,1);
}


void notify_mutex_unlock( alt_mutex_dev* dev, alt_u32 value )
{
	  alt_u32 id = 124;

	  /*
	  * This Mutex has been claimed and released since Reset so clear the Reset bit
	  * This MUST happen before we release the MUTEX
	  */
	  IOWR_ALTERA_AVALON_MUTEX_RESET(dev->mutex_base,
	                                  ALTERA_AVALON_MUTEX_RESET_RESET_MSK);
	  IOWR_ALTERA_AVALON_MUTEX_MUTEX(dev->mutex_base,
	                                  id << ALTERA_AVALON_MUTEX_MUTEX_OWNER_OFST);

	  /*
	  * Now that access to the hardware Mutex is complete, release the thread lock
	  */
	  //ALT_SEM_POST (dev->lock);
}

void notify_agent(alt_mutex_dev* dev ){

	notify_mutex_unlock(agent_mutex,0);
}

int main(void)
{


	// get hardware mutex handle
	mutex = altera_avalon_mutex_open(MATCHES_MUTEX_NAME);
	agent_mutex = altera_avalon_mutex_open(TOBACCO_MUTEX_NAME);

	while(1)
	{
			// acquire the mutex, setting the value to one
			altera_avalon_mutex_lock(mutex, 1);
			printf("Smoker with MATCHES!\n");
			usleep(500000);
			release_resources();
			notify_agent(agent_mutex);
			usleep(500000);
	}
}
