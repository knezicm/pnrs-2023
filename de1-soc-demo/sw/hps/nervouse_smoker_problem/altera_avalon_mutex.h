/*
 * altera_avalon_mutex.h
 *
 *  Created on: May 14, 2023
 *      Author: nikola
 */

#ifndef ALTERA_AVALON_MUTEX_H_
#define ALTERA_AVALON_MUTEX_H_


#define ALTERA_AVALON_MUTEX_MUTEX_OWNER_OFST 16

extern void altera_avalon_mutex_lock( void* dev, uint32_t value );
extern int altera_avalon_mutex_trylock( void* dev, uint32_t value  );
extern void altera_avalon_mutex_unlock( void* dev );
void my_mutex_lock( void* dev, uint32_t value );

#endif /* ALTERA_AVALON_MUTEX_H_ */
