/*
 * hps_linux.c
 *
 *  Created on: Apr 23, 2021
 *      Author: mknezic
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

void setup_hps_gpio()
{
	/* Initialize the HPS PIO controller
	 *     Set the direction of the HPS_LED_GPIO bit to "output"
	 *     Set the direction of the HPS_KEY_N GPIO bit to "input"
	 */
	void *hps_gpio_direction = ALT_GPIO_SWPORTA_DDR_ADDR(hps_gpio);
	alt_setbits_word(hps_gpio_direction, ALT_GPIO_PIN_OUTPUT << HPS_LED_PORT_BIT);
	alt_setbits_word(hps_gpio_direction, ALT_GPIO_PIN_INPUT << HPS_KEY_N_PORT_BIT);
}

void handle_hps_led()
{
	void *hps_gpio_data = ALT_GPIO_SWPORTA_DR_ADDR(hps_gpio);
	void *hps_gpio_port = ALT_GPIO_EXT_PORTA_ADDR(hps_gpio);

	uint32_t hps_gpio_input = alt_read_word(hps_gpio_port) & HPS_KEY_N_MASK;

	/* HPS_KEY_N is active-low */
	bool toggle_hps_led = (~hps_gpio_input & HPS_KEY_N_MASK);

	if (toggle_hps_led)
	{
		uint32_t hps_led_value = alt_read_word(hps_gpio_data);
		hps_led_value >>= HPS_LED_PORT_BIT;
		hps_led_value = !hps_led_value;
		hps_led_value <<= HPS_LED_PORT_BIT;
		alt_replbits_word(hps_gpio_data, HPS_LED_MASK, hps_led_value);
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

	fpga_buttons = h2f_lw_axi_master + BUTTONS_0_BASE;
	fpga_hex_displays[0] = h2f_lw_axi_master + HEX_0_BASE;
	fpga_hex_displays[1] = h2f_lw_axi_master + HEX_1_BASE;
	fpga_hex_displays[2] = h2f_lw_axi_master + HEX_2_BASE;
	fpga_hex_displays[3] = h2f_lw_axi_master + HEX_3_BASE;
	fpga_hex_displays[4] = h2f_lw_axi_master + HEX_4_BASE;
	fpga_hex_displays[5] = h2f_lw_axi_master + HEX_5_BASE;
}

bool is_fpga_button_pressed(uint32_t button_number)
{
	/* Buttons are active-low */
	return ((~alt_read_word(fpga_buttons)) & (1 << button_number));
}

void setup_hex_displays()
{
	int i;
	/* Turn all hex displays off */
	for(i=0; i < HEX_DISPLAY_COUNT; i++)
	{
		alt_write_word(fpga_hex_displays[i], HEX_DISPLAY_CLEAR);
	}
}

void set_hex_displays(uint32_t value)
{
	char current_char[2] = " \0";
	char hex_counter_hex_string[HEX_DISPLAY_COUNT + 1];

	/* Get hex string representation of input value on HEX_DISPLAY_COUNT 7-segment displays */
	snprintf(hex_counter_hex_string, HEX_DISPLAY_COUNT + 1, "%0*x", HEX_DISPLAY_COUNT, (unsigned int)value);

	uint32_t hex_display_index;
	for (hex_display_index = 0; hex_display_index < HEX_DISPLAY_COUNT; hex_display_index++)
	{
		current_char[0] = hex_counter_hex_string[HEX_DISPLAY_COUNT - hex_display_index - 1];

		/* Get decimal representation for this 7-segment display */
		uint32_t number = (uint32_t)strtol(current_char, NULL, 16);

		/* Use lookup table to find active-low value representing number on the 7-segment display */
		uint32_t hex_value_to_write = hex_display_table[number];

		alt_write_word(fpga_hex_displays[hex_display_index], hex_value_to_write);
	}
}

void handle_hex_displays(uint32_t *hex_counter)
{
	static bool button_0_pressed = false;
	static bool button_1_pressed = false;
	static bool hex_up_direction = true;

	if(hex_up_direction)
	{
		(*hex_counter)++;
	}
	else
	{
		(*hex_counter)--;
	}

	set_hex_displays(*hex_counter);

	if(is_fpga_button_pressed(0))
	{
		if(!button_0_pressed)
		{
			hex_up_direction = !hex_up_direction;
			button_0_pressed = true;
		}
	}
	else
	{
		button_0_pressed = false;
	}

	if(is_fpga_button_pressed(1))
	{
		if(!button_1_pressed)
		{
			*hex_counter = 0;
			button_1_pressed = true;
		}
	}
	else
	{
		button_1_pressed = false;
	}
}

void munmap_fpga_peripherals()
{
	int i;

	if(munmap(h2f_lw_axi_master, h2f_lw_axi_master_span) != 0)
	{
		printf("ERROR: h2f_lw_axi_master munmap() failed.\n");
		printf("    errno = %s\n", strerror(errno));
		close(fd_dev_mem);
		exit(EXIT_FAILURE);
	}

	h2f_lw_axi_master = NULL;
	fpga_buttons = NULL;

	for (i=0; i < HEX_DISPLAY_COUNT; i++)
	{
		fpga_hex_displays[i] = NULL;
	}
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
	printf("DE1-SoC linux demo\n");

	open_physical_memory_device();
	mmap_peripherals();

	setup_hps_gpio();
	setup_hex_displays();

	uint32_t hex_counter = 0;
	while(true)
	{
		handle_hex_displays(&hex_counter);
		handle_hps_led();
		usleep(ALT_MICROSECS_IN_A_SEC / 10);
	}

	munmap_peripherals();
	close_physical_memory_device();

	return 0;
}
