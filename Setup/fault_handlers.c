/*
 * fault_handlers.c
 *
 *  Created on: 9 mar 2018
 *      Author: Kwarc
 */

void HardFault_Handler(void)
{
	asm volatile ("BKPT 0");
}

void MemManage_Handler(void)
{
	asm volatile ("BKPT 0");
}

void BusFault_Handler(void)
{
	asm volatile ("BKPT 0");
}

void UsageFault_Handler(void)
{
	asm volatile ("BKPT 0");
}




