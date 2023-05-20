/*
 * agent.c
 *
 *  Created on: May 20, 2023
 *      Author: popovict
 *
 *  Ingredients: 1-paper, 2-tobacco, 3-matches
 *  mailbox_0 will find a smoker with matches
 *  mailbox_1 will find a smoker with tobacco
 *  mailbox_2 will find a smoker with paper
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

void mmap_fpga_peripherals()
{
	h2f_lw_axi_master = mmap(NULL, h2f_lw_axi_master_span, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dev_mem, h2f_lw_axi_master_ofst);
	if(h2f_lw_axi_master == MAP_FAILED){
		printf("ERROR: h2f_lw_axi_master mmap() failed.\n");
		printf("    errno = %s\n", strerror(errno));
		close(fd_dev_mem);
		exit(EXIT_FAILURE);
	}

	mailbox_0 = h2f_lw_axi_master + MAILBOX_0_BASE;
	mailbox_1 = h2f_lw_axi_master + MAILBOX_1_BASE;
	mailbox_2 = h2f_lw_axi_master + MAILBOX_2_BASE;
	mailbox_3 = h2f_lw_axi_master + MAILBOX_3_BASE;
}

void munmap_fpga_peripherals()
{
	if(munmap(h2f_lw_axi_master, h2f_lw_axi_master_span) != 0)
	{
		printf("ERROR: h2f_lw_axi_master munmap() failed.\n");
		printf("    errno = %s\n", strerror(errno));
		close(fd_dev_mem);
		exit(EXIT_FAILURE);
	}

	h2f_lw_axi_master = NULL;
	mailbox_0  = NULL;
	mailbox_1  = NULL;
	mailbox_2  = NULL;
	mailbox_3  = NULL;
}

void close_physical_memory_device()
{
	close(fd_dev_mem);
}

int main()
{
	open_physical_memory_device();
	mmap_fpga_peripherals();

	uint32_t message = true;
    uint32_t smoking_is_done = 0;
    uint32_t put_ingredients = 1;

	while(1)
	{
		// Generate two random ingredients: paper, matches or tobacco
		int ingredient_1 = rand() % 3 + 1;
		int ingredient_2 = rand() % 3 + 1;

      if(put_ingredients == 1)
      {

		 // Signal the appropriate smoker based on the generated ingredients
		 if (ingredient_1 != ingredient_2)
		 {
		   if (ingredient_1 == 1 && ingredient_2 == 2)
		   {
		       printf("Agent: Providing ingredients %d and %d to Smoker 1\n", ingredient_1, ingredient_2);
		                alt_write_word(mailbox_0, message);
		                put_ingredients = 0;
		   }
		   else if (ingredient_1 == 1 && ingredient_2 == 3)
		   {
		       printf("Agent: Providing ingredients %d and %d to Smoker 2\n", ingredient_1, ingredient_2);
		                alt_write_word(mailbox_1, message);
		                put_ingredients = 0;
		   }
		   else if (ingredient_1 == 2 && ingredient_2 == 3)
		   {
		       printf("Agent: Providing ingredients %d and %d to Smoker 3\n", ingredient_1, ingredient_2);
		                alt_write_word(mailbox_2, message);
		                put_ingredients = 0;
		   }
		 }
		else
		     {
		            printf("Agent: Generated duplicate ingredients, retrying...\n");
		     }
	}

        // Checking if smoking is done
        smoking_is_done = alt_read_word(mailbox_3);

		if(smoking_is_done)
		{
			put_ingredients = 1;
		}
		        // Delay before generating the next ingredients
		        usleep(2000000);
	}



	munmap_fpga_peripherals();
	close_physical_memory_device();

	return 0;
}

