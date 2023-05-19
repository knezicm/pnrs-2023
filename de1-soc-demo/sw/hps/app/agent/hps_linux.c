/*
 * hps_linux.c
 *
 *  Created on: Apr 23, 2021
 *      Author: Nemanja
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

#define CPU_ID 1

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

	mailbox_0 = h2f_lw_axi_master + MAILBOX_0_BASE;
	mailbox_1 = h2f_lw_axi_master + MAILBOX_1_BASE;
	tabacco_mutex = h2f_lw_axi_master + MUTEX_0_BASE;
	paper_mutex = h2f_lw_axi_master + MUTEX_1_BASE;
	matches_mutex = h2f_lw_axi_master + MUTEX_2_BASE;
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
	mailbox_0 = NULL;
	mailbox_1 = NULL;
	tabacco_mutex = NULL;
	paper_mutex = NULL;
	matches_mutex = NULL;
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

uint32_t mailbox_status(int sts){
	uint32_t mailbox_sts = 0;
	mailbox_sts = (alt_read_word(mailbox_0 + 8) & (0x00000003));
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
        data = alt_read_word(mailbox_0 + 12);
        alt_write_word(mailbox_0 + 12, data & (~(0x00000001)));

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
            message[1] = alt_read_word(mailbox_0 + 4);
            message[0] = alt_read_word(mailbox_0);
        }

		alt_write_word(mailbox_0 + 12, data);

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
		alt_write_word(mailbox_1 + 4, mbox_msg[1]);
		alt_write_word(mailbox_1, mbox_msg[0]);
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
			mbox_status = mailbox_status(1);
		} while (mbox_status);
	} else
	{
		do
		{
			mbox_status = mailbox_status(1);
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

int mutex_trylock(void* mutex, uint32_t value)
{
  uint32_t data, check;
  int ret_code = 0;

  data = (CPU_ID << 16) | value;

  alt_write_word(mutex, data);

  check = alt_read_word(mutex);

  printf("Data: %x, Check: %x\n", data, check);

  if ( check == data)
  {
    ret_code = 1;
  }

  return ret_code;
}

void mutex_unlock(void* mutex)
{
	alt_write_word(mutex + 4, 0x1);
	alt_write_word(mutex, CPU_ID << 16);
}

uint16_t get_owner(uint32_t nesto){
	uint16_t b = nesto >> 16;
	return b;
}


void lock_all_mutexes(){
	mutex_trylock(tabacco_mutex, 1);
	mutex_trylock(paper_mutex, 1);
	mutex_trylock(matches_mutex, 1);
}

void random_objects(){
	objects = rand() % 3;
	if(objects == 0){
		mutex_unlock(tabacco_mutex);
		mutex_unlock(paper_mutex);
		printf("duvan i papir ");
	}
	else if(objects == 1){
		mutex_unlock(tabacco_mutex);
		mutex_unlock(matches_mutex);
		printf("duvan i sibice ");
	}
	else{
		mutex_unlock(matches_mutex);
		mutex_unlock(paper_mutex);
		printf("sibice i papir ");
	}
	printf("na stol!\n");
}

void remove_objects(){
	if(objects == 0){
		mutex_trylock(tabacco_mutex, 1);
		mutex_trylock(paper_mutex, 1);
	}
	else if(objects == 1){
		mutex_trylock(tabacco_mutex, 1);
		mutex_trylock(matches_mutex, 1);
	}
	else{
		mutex_trylock(matches_mutex, 1);
		mutex_trylock(paper_mutex, 1);
	}
}

void unlock_all_mutexes(){
	mutex_unlock(tabacco_mutex);
	mutex_unlock(paper_mutex);
	mutex_unlock(matches_mutex);
}

void mmap_peripherals() {
    mmap_hps_peripherals();
    mmap_fpga_peripherals();
}

void munmap_peripherals() {
    munmap_hps_peripherals();
    munmap_fpga_peripherals();
}

void close_physical_memory_device()
{
	close(fd_dev_mem);
}

int main()
{
	uint32_t message[2];
	srand(NULL);
	printf("Agent\n");

	open_physical_memory_device();
	mmap_peripherals();

	unlock_all_mutexes();
	lock_all_mutexes();
	while(true)
	{
		printf("Agent je bacio ");
		random_objects();
		do{
			receive_message(message, 100);
			if(message[1] == 2)
				remove_objects();
		}while(message[1] != 1);
		sleep(rand() % 9 + 1);
	}

	unlock_all_mutexes();
	munmap_peripherals();
	close_physical_memory_device();

	return 0;
}
