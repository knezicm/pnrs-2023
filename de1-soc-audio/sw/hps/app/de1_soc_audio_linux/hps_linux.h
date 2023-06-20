/*
 * hps_linux.h
 *
 *  Created on: 12.06.2023.
 *      Author: Aleksandar Beslic
 */

#ifndef HPS_LINUX_H_
#define HPS_LINUX_H_

#include "socal/hps.h"
#include "../hps_soc_system.h"

#define PLAY_AUDIO 10
#define SAVE_AUDIO 11

/* Physical memory file descriptor */
int fd_dev_mem = 0;

/* Memory-mapped HPS peripherals */
void *hps_gpio = NULL;
size_t hps_gpio_span = ALT_GPIO1_UB_ADDR - ALT_GPIO1_LB_ADDR + 1;
size_t hps_gpio_ofst = ALT_GPIO1_OFST;

/* Memory-mapped FPGA peripherals */
void *h2f_lw_axi_master = NULL;
size_t h2f_lw_axi_master_span = ALT_LWFPGASLVS_UB_ADDR - ALT_LWFPGASLVS_LB_ADDR + 1;
size_t h2f_lw_axi_master_ofst = ALT_LWFPGASLVS_OFST;

void* rec_mailbox = NULL;
void* send_mailbox = NULL;

#endif /* HPS_LINUX_H_ */
