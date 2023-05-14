#ifndef _ALTERA_HPS_SOC_SYSTEM_H_
#define _ALTERA_HPS_SOC_SYSTEM_H_

/*
 * This file was automatically generated by the swinfo2header utility.
 * 
 * Created from SOPC Builder system 'soc_system' in
 * file 'soc_system.sopcinfo'.
 */

/*
 * This file contains macros for module 'hps_0' and devices
 * connected to the following master:
 *   h2f_lw_axi_master
 * 
 * Do not include this header file and another header file created for a
 * different module or master group at the same time.
 * Doing so may result in duplicate macro names.
 * Instead, use the system header file which has macros with unique names.
 */

/*
 * Macros for device 'finished_mutex', class 'altera_avalon_mutex'
 * The macros are prefixed with 'FINISHED_MUTEX_'.
 * The prefix is the slave descriptor.
 */
#define FINISHED_MUTEX_COMPONENT_TYPE altera_avalon_mutex
#define FINISHED_MUTEX_COMPONENT_NAME finished_mutex
#define FINISHED_MUTEX_BASE 0x0
#define FINISHED_MUTEX_SPAN 8
#define FINISHED_MUTEX_END 0x7
#define FINISHED_MUTEX_OWNER_INIT 0
#define FINISHED_MUTEX_OWNER_WIDTH 16
#define FINISHED_MUTEX_VALUE_INIT 0
#define FINISHED_MUTEX_VALUE_WIDTH 16

/*
 * Macros for device 'matches_mutex', class 'altera_avalon_mutex'
 * The macros are prefixed with 'MATCHES_MUTEX_'.
 * The prefix is the slave descriptor.
 */
#define MATCHES_MUTEX_COMPONENT_TYPE altera_avalon_mutex
#define MATCHES_MUTEX_COMPONENT_NAME matches_mutex
#define MATCHES_MUTEX_BASE 0x8
#define MATCHES_MUTEX_SPAN 8
#define MATCHES_MUTEX_END 0xf
#define MATCHES_MUTEX_OWNER_INIT 0
#define MATCHES_MUTEX_OWNER_WIDTH 16
#define MATCHES_MUTEX_VALUE_INIT 0
#define MATCHES_MUTEX_VALUE_WIDTH 16

/*
 * Macros for device 'paper_mutex', class 'altera_avalon_mutex'
 * The macros are prefixed with 'PAPER_MUTEX_'.
 * The prefix is the slave descriptor.
 */
#define PAPER_MUTEX_COMPONENT_TYPE altera_avalon_mutex
#define PAPER_MUTEX_COMPONENT_NAME paper_mutex
#define PAPER_MUTEX_BASE 0x10
#define PAPER_MUTEX_SPAN 8
#define PAPER_MUTEX_END 0x17
#define PAPER_MUTEX_OWNER_INIT 0
#define PAPER_MUTEX_OWNER_WIDTH 16
#define PAPER_MUTEX_VALUE_INIT 0
#define PAPER_MUTEX_VALUE_WIDTH 16

/*
 * Macros for device 'tobacco_mutex', class 'altera_avalon_mutex'
 * The macros are prefixed with 'TOBACCO_MUTEX_'.
 * The prefix is the slave descriptor.
 */
#define TOBACCO_MUTEX_COMPONENT_TYPE altera_avalon_mutex
#define TOBACCO_MUTEX_COMPONENT_NAME tobacco_mutex
#define TOBACCO_MUTEX_BASE 0x18
#define TOBACCO_MUTEX_SPAN 8
#define TOBACCO_MUTEX_END 0x1f
#define TOBACCO_MUTEX_OWNER_INIT 0
#define TOBACCO_MUTEX_OWNER_WIDTH 16
#define TOBACCO_MUTEX_VALUE_INIT 0
#define TOBACCO_MUTEX_VALUE_WIDTH 16


#endif /* _ALTERA_HPS_SOC_SYSTEM_H_ */
