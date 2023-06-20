/*
 * hps_linux.c
 *
 *  Created on: 12.06.2023.
 *      Author: Aleksandar Beslic
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "alt_generalpurpose_io.h"
#include "hps_linux.h"
#include "hwlib.h"
#include "socal/alt_gpio.h"
#include "socal/socal.h"
#include "../hps_soc_system.h"

#include "audio.h"

void open_physical_memory_device()
{
	fd_dev_mem = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd_dev_mem == -1)
	{
		printf("ERROR: could not open \"/dev/mem\"...\n");
		printf("    errno = %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void mmap_hps_peripherals()
{
	hps_gpio = mmap(NULL, hps_gpio_span, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dev_mem, hps_gpio_ofst);
	if (hps_gpio == MAP_FAILED)
	{
		printf("ERROR: hps_gpio mmap() failed.\n");
		printf("    errno = %s\n", strerror(errno));
		close(fd_dev_mem);
		exit(EXIT_FAILURE);
	}
}

void mmap_fpga_peripherals()
{
	h2f_lw_axi_master = mmap(NULL, h2f_lw_axi_master_span, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dev_mem, h2f_lw_axi_master_ofst);

	if(h2f_lw_axi_master == MAP_FAILED)
	{
		printf("ERROR: h2f_lw_axi_master mmap() failed.\n");
		printf("    errno = %s\n", strerror(errno));
		close(fd_dev_mem);
		exit(EXIT_FAILURE);
	}

	// TODO: map peripherals
}

void munmap_fpga_peripherals()
{
	if(munmap(h2f_lw_axi_master, h2f_lw_axi_master_span) != 0){
		printf("ERROR: h2f_lw_axi_master munmap() failed.\n");
		printf("    errno = %s\n", strerror(errno));
		close(fd_dev_mem);
		exit(EXIT_FAILURE);
	}

	rec_mailbox = NULL;
}

void munmap_hps_peripherals()
{
	if(munmap(hps_gpio, hps_gpio_span) != 0)
	{
		printf("ERROR: hps_gpio munmap() failed.\n");
		printf("    errno = %s\n", strerror(errno));
		close(fd_dev_mem);
		exit(EXIT_FAILURE);
	}

	hps_gpio = NULL;
}

void mmap_peripherals()
{
    mmap_hps_peripherals();
    mmap_fpga_peripherals();
}

void munmap_peripherals()
{
    munmap_hps_peripherals();
    munmap_fpga_peripherals();
}

void close_physical_memory_device()
{
	close(fd_dev_mem);
}

int main()
{
	uint32_t message;
	open_physical_memory_device();
	mmap_peripherals();

	while(1)
	{
		message = alt_read_word(rec_mailbox);

		if(message == SAVE_AUDIO)
		{
			hps_save_sampels();
		}
		else if (message == PLAY_AUDIO)
		{
			hps_read_sampels();
		}
	}

	munmap_peripherals();
	close_physical_memory_device();

	return 0;
}

