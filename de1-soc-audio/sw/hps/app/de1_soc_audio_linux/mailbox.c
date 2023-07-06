/*
 * mailbox.c
 *
 *  Created on: 20.06.2023.
 *      Author: Aleksandar Beslic
 *
 *      Big thanks Nemanja Cenic for providing this code!
 */


#include "mailbox.h"

uint32_t mailbox_status(int sts){
	uint32_t mailbox_sts = 0;
	mailbox_sts = (alt_read_word(rec_mailbox + 8) & (0x00000003));
	if(sts == 0){
		mailbox_sts = mailbox_sts & (0x00000001);
	}
	else{
		mailbox_sts = (mailbox_sts & (0x00000002)) >> 1;
	}

    return mailbox_sts;
}

uint32_t mailbox_status_r(int sts){
	uint32_t mailbox_sts = 0;
	mailbox_sts = (alt_read_word(send_mailbox + 8) & (0x00000003));
	if(sts == 0){
		mailbox_sts = mailbox_sts & (0x00000001);
	}
	else{
		mailbox_sts = (mailbox_sts & (0x00000002)) >> 1;
	}

    return mailbox_sts;
}

int receive_message(uint32_t *message, uint32_t timeout)
{
    uint32_t status = 0;
    uint32_t data;

    if (message != NULL)
    {
        data = alt_read_word(rec_mailbox + 12);
        alt_write_word(rec_mailbox + 12, data & (~(0x00000001)));

        if (timeout == 0)
        {
            do
            {
                status = mailbox_status(0);
            } while (status == 0);
        } else
        {
            do
            {
                 status = mailbox_status(0);
                 timeout--;
            } while ((status == 0) && timeout);
        }

        if (status)
        {
            message[1] = alt_read_word(rec_mailbox + 4);
            message[0] = alt_read_word(rec_mailbox);
        }

		alt_write_word(rec_mailbox + 12, data);

        if (status)
          return 1;
      }

      message[1] = 0;
      message[0] = 0;
      return 0;
}

uint32_t mailbox_post(void *message)
{
    uint32_t *mbox_msg = (uint32_t*) message ;

    if (mbox_msg != NULL) {
		alt_write_word(send_mailbox + 4, mbox_msg[1]);
		alt_write_word(send_mailbox, mbox_msg[0]);
        return 0;
    }

    return -1;
}

int mailbox_send( void *message, int timeout)
{
    int status = 0;
    uint32_t mbox_status;

	if (timeout ==0)
	{
		do
		{
			mbox_status = mailbox_status_r(1);
		} while (mbox_status);
	} else
	{
		do
		{
			mbox_status = mailbox_status_r(1);
			timeout--;
		} while (mbox_status && (timeout != 0));
		if (timeout == 0)
		{
			return -1;
		}
	}
	status = mailbox_post(message);

	return status;
}
