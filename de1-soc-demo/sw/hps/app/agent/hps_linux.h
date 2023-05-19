/*
 * hps_linux.h
 *
 *  Created on: Apr 23, 2021
 *      Author: Nemanja
 */

#ifndef HPS_LINUX_H_
#define HPS_LINUX_H_

#include "socal/hps.h"
#include "../hps_soc_system.h"

/* Physical memory file descriptor */
int fd_dev_mem = 0;

char objects;

/* Memory-mapped HPS peripherals */
void *hps_gpio = NULL;
size_t hps_gpio_span = ALT_GPIO1_UB_ADDR - ALT_GPIO1_LB_ADDR + 1;
size_t hps_gpio_ofst = ALT_GPIO1_OFST;

/* Memory-mapped FPGA peripherals */
void *h2f_lw_axi_master = NULL;
size_t h2f_lw_axi_master_span = ALT_LWFPGASLVS_UB_ADDR - ALT_LWFPGASLVS_LB_ADDR + 1;
size_t h2f_lw_axi_master_ofst = ALT_LWFPGASLVS_OFST;

void *mailbox_0 = NULL;
void *mailbox_1 = NULL;
void *tabacco_mutex = NULL;
void *paper_mutex = NULL;
void *matches_mutex = NULL;

void open_physical_memory_device();
void close_physical_memory_device();
void mmap_hps_peripherals();
void munmap_hps_peripherals();
void mmap_fpga_peripherals();
void munmap_fpga_peripherals();
void mmap_peripherals();
void munmap_peripherals();

#endif /* HPS_LINUX_H_ */
