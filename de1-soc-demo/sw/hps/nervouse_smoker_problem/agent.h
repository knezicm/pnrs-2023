/*
 * agent.h
 *
 *
 *      Author: Nikola Cetić
 */

#ifndef HPS_LINUX_H_
#define HPS_LINUX_H_

#include "socal/hps.h"
#include "../hps_soc_system.h"


/* Physical memory file descriptor */
int fd_dev_mem = 0;


/* Memory-mapped FPGA peripherals */
void *h2f_lw_axi_master = NULL;
size_t h2f_lw_axi_master_span = ALT_LWFPGASLVS_UB_ADDR - ALT_LWFPGASLVS_LB_ADDR + 1;
size_t h2f_lw_axi_master_ofst = ALT_LWFPGASLVS_OFST;

/* Memory-mapped FPGA hardware mutexes */
void *paper_mutex = NULL, *tobacco_mutex = NULL, *matches_mutex = NULL, *finished_mutex = NULL;



void open_physical_memory_device();
void close_physical_memory_device();
void mmap_fpga_peripherals();
void munmap_fpga_peripherals();
void mmap_peripherals();
void munmap_peripherals();

#endif /* HPS_LINUX_H_ */
