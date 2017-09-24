/*
 * cpu_utils.h
 *
 *  Created on: 20.05.2017
 *      Author: Kwarc
 */

#ifndef CPU_UTILS_H_
#define CPU_UTILS_H_


#ifdef __cplusplus
 extern "C" {
#endif

#define CALCULATION_PERIOD    1000

unsigned short int Get_CPU_Usage(void);

#ifdef __cplusplus
}
#endif

#endif /* CPU_UTILS_H_ */
