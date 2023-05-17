/*
 * agent.c
 *
 *  Created on: 09.05.2023.
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
#include "agent.h"
#include "hwlib.h"
#include "socal/alt_gpio.h"
#include "socal/socal.h"
#include "../hps_soc_system.h"

#define LW_BRIDGE_OFFSET 0xff260000
#define ALL_DONE 13

/**
 *
 */
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

void close_physical_memory_device()
{
	close(fd_dev_mem);
}

void mmap_fpga_peripherals()
{
	h2f_lw_axi_master = mmap(NULL, h2f_lw_axi_master_span, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dev_mem, h2f_lw_axi_master_ofst);
	if(h2f_lw_axi_master == MAP_FAILED){
		printf("ERROR: h2f_lw_axi_master mmap() failed.\n");
		printf("    errno = %s\n", strerror(errno));
		close(fd_dev_mem);
		exit(EXIT_FAILURE);
	}


//	mailbox_0 = mmap(NULL, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dev_mem, LW_BRIDGE_OFFSET + MAILBOX_0_BASE);
//	if(mailbox_0 == MAP_FAILED){
//		printf("ERROR: mailbox0 mmap() failed.\n");
//		printf("    errno = %s\n", strerror(errno));
//		close(fd_dev_mem);
//		exit(EXIT_FAILURE);
//	}
//
//	mailbox_1 = mmap(NULL, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dev_mem, LW_BRIDGE_OFFSET + MAILBOX_1_BASE);
//		if(mailbox_1 == MAP_FAILED){
//			printf("ERROR: mailbox1 mmap() failed.\n");
//			printf("    errno = %s\n", strerror(errno));
//			close(fd_dev_mem);
//			exit(EXIT_FAILURE);
//		}
//
//	mailbox_2 = mmap(NULL, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dev_mem, LW_BRIDGE_OFFSET + MAILBOX_2_BASE);
//		if(mailbox_1 == MAP_FAILED){
//			printf("ERROR: mailbox1 mmap() failed.\n");
//			printf("    errno = %s\n", strerror(errno));
//			close(fd_dev_mem);
//			exit(EXIT_FAILURE);
//		}
//
//	mailbox_4 = mmap(NULL, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dev_mem, LW_BRIDGE_OFFSET + MAILBOX_4_BASE);
//		if(mailbox_1 == MAP_FAILED){
//			printf("ERROR: mailbox1 mmap() failed.\n");
//			printf("    errno = %s\n", strerror(errno));
//			close(fd_dev_mem);
//			exit(EXIT_FAILURE);
//		}

	mailbox_0 = h2f_lw_axi_master + MAILBOX_0_BASE;
	mailbox_1 = h2f_lw_axi_master + MAILBOX_1_BASE;
	mailbox_2 = h2f_lw_axi_master + MAILBOX_2_BASE;
	mailbox_4 = h2f_lw_axi_master + MAILBOX_4_BASE;
}

void munmap_fpga_peripherals(){
	if(munmap(h2f_lw_axi_master, h2f_lw_axi_master_span) != 0){
		printf("ERROR: h2f_lw_axi_master munmap() failed.\n");
		printf("    errno = %s\n", strerror(errno));
		close(fd_dev_mem);
		exit(EXIT_FAILURE);
	}

	h2f_lw_axi_master = NULL;
	mailbox_0  = NULL;
	mailbox_1  = NULL;
	mailbox_2  = NULL;
	mailbox_4  = NULL;
}

int main(void)
{
	uint32_t message;
	uint32_t send = 1;
	open_physical_memory_device();
    mmap_fpga_peripherals();

	while(true)
	{
		// Picking ingredients
		message = rand() % 3 + 1;

		if (send)
		{
			// Notifying nios processors
			alt_write_word(mailbox_0, message);
			alt_write_word(mailbox_1, message);
			alt_write_word(mailbox_2, message);

			printf("Agent placed two ingredients on the table, they are %d\n", message);

			send = 0;
		}

		message = alt_read_word(mailbox_4);

        // If a nios processors is done we can put new ingredients on the table
		if (message >= ALL_DONE)
		{
			send = 1;
		}

		sleep(1);
	}

	munmap_fpga_peripherals();
	close_physical_memory_device();

	return 0;
}
