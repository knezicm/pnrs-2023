/*
 * agent.c
 *
 *
 *      Author: Nikola Cetić
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
#include "altera_avalon_mutex.h"

void open_physical_memory_device() {
	fd_dev_mem = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd_dev_mem == -1) {
		printf("ERROR: could not open \"/dev/mem\"...\n");
		printf("    errno = %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void mmap_fpga_peripherals() {
	h2f_lw_axi_master = mmap(NULL, h2f_lw_axi_master_span,
			PROT_READ | PROT_WRITE, MAP_SHARED, fd_dev_mem,
			h2f_lw_axi_master_ofst);

	if (h2f_lw_axi_master == MAP_FAILED) {
		printf("ERROR: h2f_lw_axi_master mmap() failed.\n");
		printf("    errno = %s\n", strerror(errno));
		close(fd_dev_mem);
		exit(EXIT_FAILURE);
	}

	paper_mutex = h2f_lw_axi_master + PAPER_MUTEX_BASE;
	matches_mutex = h2f_lw_axi_master + MATCHES_MUTEX_BASE;
	tobacco_mutex = h2f_lw_axi_master + TOBACCO_MUTEX_BASE;
	finished_mutex = h2f_lw_axi_master + FINISHED_MUTEX_BASE;
}

void munmap_fpga_peripherals() {

	if (munmap(h2f_lw_axi_master, h2f_lw_axi_master_span) != 0) {
		printf("ERROR: h2f_lw_axi_master munmap() failed.\n");
		printf("    errno = %s\n", strerror(errno));
		close(fd_dev_mem);
		exit(EXIT_FAILURE);
	}

	h2f_lw_axi_master = NULL;
	/* Memory-mapped FPGA hardware mutexes */
	paper_mutex = NULL;
	tobacco_mutex = NULL;
	matches_mutex = NULL;
	finished_mutex = NULL;
}

void mmap_peripherals() {
	mmap_fpga_peripherals();
}

void munmap_peripherals() {
	munmap_fpga_peripherals();
}

void close_physical_memory_device() {
	close(fd_dev_mem);
}

int main() {
	printf("DE1-SoC linux demo\n");

	open_physical_memory_device();
	mmap_peripherals();

	int state = 0;
	while (true) {
		// acquire the mutex, setting the value to one
		altera_avalon_mutex_lock(finished_mutex, 1);
		printf("Agent is placing resources on table!\n");
		// unlock and then lock it as different owner in this case userID=124
		altera_avalon_mutex_unlock(finished_mutex);
		my_mutex_lock(finished_mutex, 1);
//
//			// lock resources
		altera_avalon_mutex_lock(matches_mutex, 1);
		altera_avalon_mutex_lock(paper_mutex, 1);
		altera_avalon_mutex_lock(tobacco_mutex, 1);
		state = rand()%3;
		if (state == 0) {
			state = 1;
			altera_avalon_mutex_unlock(matches_mutex);
			altera_avalon_mutex_unlock(paper_mutex);
			usleep(100000);
		} else if(state == 1) {
			state = 0;
			altera_avalon_mutex_unlock(matches_mutex);
			altera_avalon_mutex_unlock(tobacco_mutex);
			usleep(100000);
		}else if(state == 2) {
			state = 0;
			altera_avalon_mutex_unlock(paper_mutex);
			altera_avalon_mutex_unlock(tobacco_mutex);
			usleep(100000);
		}
	}

	munmap_peripherals();
	close_physical_memory_device();

	return 0;
}
