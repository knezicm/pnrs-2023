/*
 * mailbox.h
 *
 *  Created on: 20.06.2023.
 *      Author: Aleksandar Beslic
 */

#ifndef MAILBOX_H_
#define MAILBOX_H_

#include "stdlib.h"
#include <stdint.h>
#include "socal/socal.h"

extern void* rec_mailbox;
extern void* send_mailbox;


int receive_message(uint32_t *message, uint32_t timeout);
int mailbox_send( void *message, int timeout);

#endif /* MAILBOX_H_ */
