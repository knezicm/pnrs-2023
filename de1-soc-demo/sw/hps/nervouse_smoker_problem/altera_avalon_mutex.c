/*
 * altera_avalon_mutex.c
 *
 *  Created on: May 14, 2023
 *      Author: nikola
 */

/*
 * alt_mutex_trylock - Try to lock the hardware mutex
 *
 * returns 0 on success -1 otherwise
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "hwlib.h"
#include "socal/alt_gpio.h"
#include "socal/socal.h"
#include "../hps_soc_system.h"
#include "altera_avalon_mutex.h"

static int alt_mutex_trylock(void *dev, uint32_t value) {
	uint32_t id = 3;
	uint32_t data, check;
	int ret_code = -1;

	/* the data we want the mutex to hold */
	data = (id << ALTERA_AVALON_MUTEX_MUTEX_OWNER_OFST) | value;

	/* attempt to write to the mutex */
	alt_write_word(dev, data);

	check = alt_read_word(dev);

	if (check == data) {
		ret_code = 0;
	}

	return ret_code;
}
/*
 * altera_avalon_mutex_lock - Lock the hardware mutex
 *
 */
void altera_avalon_mutex_lock(void *dev, uint32_t value) {

	while (alt_mutex_trylock(dev, value) != 0)
		;
}


void altera_avalon_mutex_reset(void* dev){
	alt_write_word(dev+4,1);
}

/*
 * altera_avalon_mutex_unlock - Unlock the hardware mutex
 *
 * This function does not check that you own the Mutex if you try to release
 * a Mutex you do not own the behaviour is undefined
 *
 */
void altera_avalon_mutex_unlock( void* dev )
{
  uint32_t id = 3;
  //NIOS2_READ_CPUID(id);

  /*
  * This Mutex has been claimed and released since Reset so clear the Reset bit
  * This MUST happen before we release the MUTEX
  */
  altera_avalon_mutex_reset(dev);
  alt_write_word(dev,id << ALTERA_AVALON_MUTEX_MUTEX_OWNER_OFST);

}




static int my_mutex_trylock( void* dev, uint32_t value )
{
	uint32_t id = 124;
	uint32_t data, check;
	int ret_code = -1;

	/* the data we want the mutex to hold */
	data = (id << ALTERA_AVALON_MUTEX_MUTEX_OWNER_OFST) | value;

	/* attempt to write to the mutex */
	alt_write_word(dev, data);

	check = alt_read_word(dev);

	if (check == data) {
		ret_code = 0;
	}

	return ret_code;
}

void my_mutex_lock( void* dev, uint32_t value )
{
  /*
   * When running in a multi threaded environment, obtain the "lock"
   * semaphore. This ensures that reading from the device is thread-safe.
   */

  //ALT_SEM_PEND (dev->lock, 0);

  while ( my_mutex_trylock( dev, value ) != 0);
}
