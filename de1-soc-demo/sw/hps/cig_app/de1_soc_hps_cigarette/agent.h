/*
 * agent.h
 *
 *  Created on: 09.05.2023.
 *      Author: Aleksandar Beslic
 */

#ifndef AGENT_H_
#define AGENT_H_

#include "socal/hps.h"
#include "../hps_soc_system.h"

/* Physical memory file descriptor */
int fd_dev_mem = 0;

/* Memory-mapped FPGA peripherals */
void *h2f_lw_axi_master = NULL;
size_t h2f_lw_axi_master_span = ALT_LWFPGASLVS_UB_ADDR - ALT_LWFPGASLVS_LB_ADDR + 1;
size_t h2f_lw_axi_master_ofst = ALT_LWFPGASLVS_OFST;

void *mailbox_0  = NULL;
void *mailbox_1  = NULL;
void *mailbox_2  = NULL;
void *mailbox_4  = NULL;

void open_physical_memory_device();
void close_physical_memory_device();
void mmap_fpga_peripherals();
void munmap_fpga_peripherals();

#endif /* AGENT_H_ */
