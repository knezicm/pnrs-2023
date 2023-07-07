/*
 * user_input.c
 *
 *  Created on: 2023-06-12
 *      Author: Aleksandar Beslic
 */

#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_mailbox_simple.h"
#include <io.h>
#include <unistd.h>

altera_avalon_mailbox_dev* mailbox_0;
volatile int mode = 0;

void clear_display()
{
	/* Clear Hex 0 to 4 */
	IOWR(HEX_0_BASE, 0, 0x001000FF);
	IOWR(HEX_1_BASE, 0, 0x010000FF);
	IOWR(HEX_2_BASE, 0, 0x010000FF);
	IOWR(HEX_3_BASE, 0, 0x010000FF);
	IOWR(HEX_4_BASE, 0, 0x001000FF);
}

void display_start()
{
	/* Display Hex_0 letter S */
	IOWR(HEX_0_BASE, 0, 0x00000012);

	/* Display Hex_1 letter t */
	IOWR(HEX_1_BASE, 0, 0x00000038);

	/* Display Hex_2 letter A */
	IOWR(HEX_2_BASE, 0, 0x00000001);

	/* Display Hex_3 letter r */
	IOWR(HEX_3_BASE, 0, 0x0000003D);

	/* Display Hex_4 letter t */
	IOWR(HEX_4_BASE, 0, 0x00000038);
}

void display_play()
{
	/* Display Hex_0 letter P */
	IOWR(HEX_0_BASE, 0, 0x00000021);

	/* Display Hex_1 letter L */
	IOWR(HEX_1_BASE, 0, 0x00000078);

	/* Display Hex_2 letter A */
	IOWR(HEX_2_BASE, 0, 0x00000001);

	/* Display Hex_3 letter Y */
	IOWR(HEX_3_BASE, 0, 0x0000000A);
}

void display_rec()
{
	/* Display Hex_0 letter r */
	IOWR(HEX_0_BASE, 0, 0x0000003D);

	/* Display Hex_1 letter E */
	IOWR(HEX_1_BASE, 0, 0x00000030);

	/* Display Hex_2 letter C */
	IOWR(HEX_2_BASE, 0, 0x00000070);
}

void display_loop()
{
	/* Display Hex_0 letter L */
	IOWR(HEX_0_BASE, 0, 0x00000078);

	/* Display Hex_1 letter O */
	IOWR(HEX_1_BASE, 0, 0x00000040);

	/* Display Hex_2 letter O */
	IOWR(HEX_2_BASE, 0, 0x00000040);

	/* Display Hex_3 letter P */
	IOWR(HEX_3_BASE, 0, 0x00000021);
}

void display_stop()
{
	/* Display Hex_0 letter S */
	IOWR(HEX_0_BASE, 0, 0x00000012);

	/* Display Hex_1 letter t */
	IOWR(HEX_1_BASE, 0, 0x00000038);

	/* Display Hex_2 letter O */
	IOWR(HEX_2_BASE, 0, 0x00000040);

	/* Display Hex_3 letter P */
	IOWR(HEX_3_BASE, 0, 0x00000021);
}

void display_pause()
{
	/* Display Hex_0 letter P */
	IOWR(HEX_0_BASE, 0, 0x00000021);

	/* Display Hex_1 letter A */
	IOWR(HEX_1_BASE, 0, 0x00000001);

	/* Display Hex_2 letter U */
	IOWR(HEX_2_BASE, 0, 0x00000048);

	/* Display Hex_3 letter S */
	IOWR(HEX_3_BASE, 0, 0x00000012);

	/* Display Hex_4 letter E */
	IOWR(HEX_4_BASE, 0, 0x00000030);
}

void display_mode(int mode)
{
	/* Mode: 1 - play, 2 - record, 3 - loopback. */
	if(mode == 1)
	{
		display_loop();
	}
	else if(mode == 2)
	{
		display_rec();
	}
	else if(mode == 3)
	{
		display_stop();
	}
	else if(mode == 4)
	{
		display_play();
	}
	else if(mode == 5)
	{
		display_pause();
	}
	else
	{
		clear_display();
	}
}

static void button_isr(void* isr_context, alt_u32 id)
{
	IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_0_BASE);
	unsigned short buttons;
	short key_1 = 0;
	short key_2 = 0;
	buttons = IORD_ALTERA_AVALON_PIO_DATA(BUTTON_0_BASE);

	/*
	 * 0xE - pritisnut KEY_0
	 * 0xD - pritisnut KEY_1
	 * 0xB - pritisnut KEY_2
	 */

	/* Citanje vrijednosti buttona. */
	if(buttons == 0xE)
	{
		mode = 1;
	}
	else if(buttons == 0xD)
	{
		usleep(150000);
		key_1++;
		if(mode != 2 && key_1 > 0)
		{
			mode = 2;
			key_1 = 0;
		}
		else if(mode == 2 && key_1 > 0)
		{
			mode = 3;
			key_1 = 0;
		}
	}
	else if(buttons == 0xB)
	{
		usleep(150000);
		key_2++;
		if(mode != 4 && key_2 > 0)
		{
			mode = 4;
		}
		else if(mode == 4 && key_2 > 0)
		{
			mode = 5;
		}
	}

	//Write to edge capture register to reset it
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_0_BASE,0);
}


static void init_interrupt_pio()
{
    // Enable a single interrupt input by writing a one to the corresponding interruptmask bit locations
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BUTTON_0_BASE,0x7);

    // Reset the edge capture register
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_0_BASE,0);

    alt_ic_isr_register(BUTTON_0_IRQ_INTERRUPT_CONTROLLER_ID, BUTTON_0_IRQ, button_isr, NULL, 0x0);

}

int main()
{
	unsigned int signal[2] = {0x00001111, 0};

	/* Inicijalizacija mailboxa. */
	mailbox_0 = altera_avalon_mailbox_open(MAILBOX_0_NAME, NULL, NULL);

	/* Registrovanje interapta za dugmice */
	init_interrupt_pio();

	clear_display();
	display_start();
	usleep(1000000);
	mode = 1;
	while(1)
	{
		clear_display();
		display_mode(mode);

		/* Slanje moda rada na HPS. */
		signal[1] = mode;
		altera_avalon_mailbox_send(mailbox_0, signal, 0, POLL);
	}
}
