/*
 * audio.h
 *
 *  Created on: 18.06.2023.
 *      Author: Aleksandar Beslic
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "socal/socal.h"

#include "mailbox.h"

#define STOP_RECORDING 2
#define READ_BUFF_1 3
#define READ_BUFF_2 4

#define STOP_PLAYING_RECORDING 5
#define WRITE_BUFF_1 6
#define WRITE_BUFF_2 7
#define BUFF_1_FILLED 9
#define BUFF_2_FILLED 10

#define BUFF_SIZE 1024
#define FILE_NAME "saved_samples.txt"

extern void* rec_mailbox;
extern void* send_mailbox;
extern void* l_buffer;
extern void* r_buffer;


void hps_save_sampels();

void hps_read_sampels();

#endif /* AUDIO_H_ */
