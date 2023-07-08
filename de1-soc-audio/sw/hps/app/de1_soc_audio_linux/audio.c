/*
 * audio.c
 *
 *  Created on: 18.06.2023.
 *      Author: Aleksandar Beslic
 */


#include "audio.h"

//#define BLOCK_RW


uint32_t block_saved = 0;
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
	uint32_t left_chn;
//	uint32_t right_chn;
	uint32_t message[2];
	FILE *file;

	remove(FILE_NAME);

#ifdef BLOCK_RW
	file = fopen("saved_samples.dat", "w");
#else
	file = fopen(FILE_NAME, "w");
#endif

	block_saved = 0;

	/* Can't create a recording*/
	if (file == NULL)
	{
		/* Inform NIOS(2) that we can't save the recording */
		message[0] = CANT_SAVE_RECORDING;
		message[1] = CANT_SAVE_RECORDING;

		mailbox_send(message, 0);

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
#ifdef BLOCK_RW

			fwrite(sampels_buff1, sizeof(sampels_buff1[0]), BUFF_SIZE, file);

#else
			for (int i = 0; i < BUFF_SIZE; i++)
			{
				/* Getting left and right channels and writing them to a file */
				left_chn = sampels_buff1[i];
//				right_chn = sampels_buff1[i + 1];

				fprintf(file, "%u\n", left_chn);
			}
#endif
			block_saved++;
		}
		else if (message[1] == READ_BUFF_2)
		{
#ifdef BLOCK_RW

			fwrite(sampels_buff2, sizeof(sampels_buff2[0]), BUFF_SIZE, file);

#else

			for (int i = 0; i < BUFF_SIZE; i++)
			{
				/* Getting left and right channels and writing them to a file */
				left_chn = sampels_buff2[i];
//				right_chn = sampels_buff2[i + 1];

				fprintf(file, "%u\n", left_chn);
			}

#endif
			block_saved++;
		}
	}

}

void hps_read_sampels()
{
	uint32_t *sampels_buff1 = (uint32_t*) l_buffer;
	uint32_t *sampels_buff2 = (uint32_t*) r_buffer;
	uint32_t left_chn;
//	uint32_t right_chn;
	uint32_t message[2];
	uint32_t blocks_read = 0;
	FILE *file;

#ifdef BLOCK_RW
	file = fopen("saved_samples.dat", "w");
#else
	file = fopen(FILE_NAME, "w");
#endif


	/* No recording was found*/
	if (file == NULL)
	{
		/* Inform NIOS(2) that there is no recording */
		message[0] = NO_RECORDING;
		message[1] = NO_RECORDING;

		mailbox_send(message, 0);
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
		else if (message[1] == WRITE_BUFF_1)
		{
#ifdef BLOCK_RW

			fread(sampels_buff1, sizeof(sampels_buff1[0]), BUFF_SIZE, file);

#else

			for (int i = 0; i < BUFF_SIZE; i++)
			{
				/* Reading left and right channels from the file and putting them into buff 1*/
				fscanf(file, "%u\n", &left_chn);

				sampels_buff1[i] = left_chn;
//				sampels_buff1[2 * i + 1] = right_chn;

				/* If we reach EOF, we start from the beginning*/
				if(feof(file) || left_chn == 0)
				{
					rewind(file);
				}

			}

#endif

			blocks_read++;

			// Informing NIOS(2) that the buffer is filled
			message[0] = 9;
			message[1] = 9;

			mailbox_send(message, 0);
		}
		else if (message[1] == WRITE_BUFF_2)
		{

#ifdef BLOCK_RW

			fread(sampels_buff2, sizeof(sampels_buff2[0]), BUFF_SIZE, file);

#else

			for (int i = 0; i < BUFF_SIZE ; i++)
			{
				/* Reading left and right channels from the file and putting them into buff 2*/
				fscanf(file, "%u\n", &left_chn);

				sampels_buff2[i] = left_chn;
//				sampels_buff2[2 * i + 1] = right_chn;

				/* If we reach EOF, we start from the beginning*/
				if(feof(file) || left_chn == 0)
				{
					rewind(file);
				}

			}

#endif

			blocks_read++;

			// Informing NIOS(2) that the buffer is filled
			message[0] = BUFF_2_FILLED;
			message[1] = BUFF_2_FILLED;

			mailbox_send(message, 0);
		}

		/* If read more the we saved we start from the beginning*/
		if (feof(file))
		{
			blocks_read = 0;
			rewind(file);
		}
	}

}
