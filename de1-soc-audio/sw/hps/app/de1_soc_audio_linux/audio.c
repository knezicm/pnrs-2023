/*
 * audio.c
 *
 *  Created on: 18.06.2023.
 *      Author: Aleksandar Beslic
 */


#include "audio.h"

/**
 * @brief When recording was started this function is started,
 * it saves samples to a file from two buffer. Depending on what buffer is full
 * it will read those samples and save them.
 *
 * To save the first buffer send READ_BUFF_1 through the mailbox, for the second
 * buffer send READ_BUFF_2 and to stop recording send STOP_RECORDING.
 * 
 */
void hps_save_sampels()
{
	uint32_t *sampels_buff1 = (uint32_t*) l_buffer;
	uint32_t *sampels_buff2 = (uint32_t*) r_buffer;
	uint32_t left_chn, right_chn;
	uint32_t message[2];
	FILE *file = fopen(FILE_NAME, "w");

	/* Can't create a recording*/
	if (file == NULL)
	{
		// TODO: Inform NIOS(2) that we can't save the recording
		return;
	}

	while(1)
	{
		/* Read mailbox message*/
		receive_message(message, 0);

		if (message[1] == STOP_RECORDING)
		{
			/* Closing the file and returning to normal state */
			fclose(file);
			return;
		}
		else if (message[1] == READ_BUFF_1)
		{
			for (int i = 0; i < BUFF_SIZE; i=+2)
			{
				/* Getting left and right channels and writing them to a file */
				left_chn = sampels_buff1[i];
				right_chn = sampels_buff1[i + 1];

				fprintf(file, "%d %d\n", left_chn, right_chn);
			}
		}
		else if (message[1] == READ_BUFF_2)
		{
			for (int i = 0; i < BUFF_SIZE; i=+2)
			{
				/* Getting left and right channels and writing them to a file */
				left_chn = sampels_buff2[i];
				right_chn = sampels_buff2[i + 1];

				fprintf(file, "%d %d\n", left_chn, right_chn);
			}
		}
	}

}

void hps_read_sampels()
{
	uint32_t *sampels_buff1 = (uint32_t*) l_buffer;
	uint32_t *sampels_buff2 = (uint32_t*) r_buffer;
	uint32_t left_chn, right_chn;
	uint32_t message[2];
	FILE *file = fopen(FILE_NAME, "r");

	/* No recording was found*/
	if (file == NULL)
	{
		// TODO: Inform NIOS(2) that there is no recording
		return;
	}

	while(1)
	{
		/* Read mailbox message*/
		receive_message(message, 0);

		if (message[1] == STOP_PLAYING_RECORDING)
		{
			/* Closing the file and returning to normal state */
			fclose(file);
			return;
		}
		else if (message[1] == READ_BUFF_1)
		{
			for (int i = 0; i < BUFF_SIZE; i=+2)
			{
				/* Reading left and right channels from the file and putting them into buff 1*/
				fscanf(file, "%d %d\n", &left_chn, &right_chn);

				sampels_buff1[i] = left_chn;
				sampels_buff1[i + 1] = right_chn;

			}

			// Informing NIOS(2) that the buffer is filled
			message[0] = BUFF_2_FILLED;
			message[1] = BUFF_2_FILLED;

			mailbox_send(message, 0);
		}
		else if (message[1] == READ_BUFF_2)
		{
			for (int i = 0; i < BUFF_SIZE; i=+2)
			{
				/* Reading left and right channels from the file and putting them into buff 2*/
				fscanf(file, "%d %d\n", &left_chn, &right_chn);

				sampels_buff2[i] = left_chn;
				sampels_buff2[i + 1] = right_chn;
			}

			// Informing NIOS(2) that the buffer is filled
			message[0] = BUFF_2_FILLED;
			message[1] = BUFF_2_FILLED;

			mailbox_send(message, 0);
		}
	}

}

