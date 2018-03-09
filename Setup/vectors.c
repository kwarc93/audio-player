/** \file vectors.c
 * \brief STM32L4x vector table and __Default_Handler()
 * \details STM32L4x vector table and __Default_Handler()
 * \author Freddie Chopin, http://www.freddiechopin.info/
 * \date 2017-03-15
 */

/******************************************************************************
* chip: STM32L4x
* compiler: arm-none-eabi-gcc (GNU Tools for ARM Embedded Processors) 7.2.1
******************************************************************************/

/*------------------------------------------------------------------------*//**
* \brief Default interrupt handler.
* \details Default interrupt handler, used for interrupts that don't have their
* own handler defined.
*//*-------------------------------------------------------------------------*/

static void __Default_Handler(void) __attribute__ ((interrupt));
static void __Default_Handler(void)
{
	while (1);
}

/*
+=============================================================================+
| assign all unhandled interrupts to the default handler
+=============================================================================+
*/

// Non-maskable interrupt (RCC clock security system)
void NMI_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// All class of fault
void HardFault_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Memory management
void MemManage_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Pre-fetch fault, memory access fault
void BusFault_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Undefined instruction or illegal state
void UsageFault_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x1C
void __Reserved_0x1C_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x20
void __Reserved_0x20_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x24
void __Reserved_0x24_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x28
void __Reserved_0x28_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// System service call via SWI instruction
void SVC_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Debug monitor
void DebugMon_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Reserved 0x34
void __Reserved_0x34_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Pendable request for system service
void PendSV_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// System tick timer
void SysTick_Handler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Window WatchDog
void WWDG_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// PVD through EXTI Line detection
void PVD_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// Tamper and TimeStamps through the EXTI line
void TAMP_STAMP_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// RTC Wakeup through the EXTI line
void RTC_WKUP_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// FLASH
void FLASH_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// RCC
void RCC_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// EXTI Line0
void EXTI0_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// EXTI Line1
void EXTI1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// EXTI Line2
void EXTI2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// EXTI Line3
void EXTI3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// EXTI Line4
void EXTI4_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA1 Channel 1
void DMA1_Channel1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA1 Channel 2
void DMA1_Channel2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA1 Channel 3
void DMA1_Channel3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA1 Channel 4
void DMA1_Channel4_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA1 Channel 5
void DMA1_Channel5_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA1 Channel 6
void DMA1_Channel6_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA1 Channel 7
void DMA1_Channel7_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// ADC1, ADC2
void ADC1_2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// CAN1 TX
void CAN1_TX_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// CAN1 RX0
void CAN1_RX0_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// CAN1 RX1
void CAN1_RX1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// CAN1 SCE
void CAN1_SCE_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// External Line[9:5]s
void EXTI9_5_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM1 Break and TIM15
void TIM1_BRK_TIM15_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM1 Update and TIM116
void TIM1_UP_TIM16_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM1 Trigger and Commutation and TIM17
void TIM1_TRG_COM_TIM17_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM1 Capture Compare
void TIM1_CC_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM2
void TIM2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM3
void TIM3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM4
void TIM4_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// I2C1 Event
void I2C1_EV_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// I2C1 Error
void I2C1_ER_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// I2C2 Event
void I2C2_EV_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// I2C2 Error
void I2C2_ER_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// SPI1
void SPI1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// SPI2
void SPI2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// USART1
void USART1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// USART2
void USART2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// USART3
void USART3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// External Line[15:10]s
void EXTI15_10_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// RTC Alarm (A and B) through EXTI Line
void RTC_Alarm_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DFSDM1_FLT3
void DFSDM1_FLT3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM8 Break
void TIM8_BRK_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM8 Update
void TIM8_UP_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM8 Trigger and Commutation
void TIM8_TRG_COM_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM8 Capture Compare
void TIM8_CC_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// ADC3
void ADC3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// FMC
void FMC_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// SDMMC1
void SDMMC1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM5
void TIM5_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// SPI3
void SPI3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// UART4
void UART4_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// UART5
void UART5_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM6 and DAC1&2 underrun errors
void TIM6_DAC_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TIM7
void TIM7_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA2 Channel 1
void DMA2_Channel1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA2 Channel 2
void DMA2_Channel2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA2 Channel 3
void DMA2_Channel3_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA2 Channel 4
void DMA2_Channel4_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA2 Channel 5
void DMA2_Channel5_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DFSDM1 FLT0
void DFSDM1_FLT0_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DFSDM1 FLT1
void DFSDM1_FLT1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DFSDM1 FLT2
void DFSDM1_FLT2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// COMP
void COMP_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// LPTIM1
void LPTIM1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// LPTIM2
void LPTIM2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// USB OTG FS
void OTG_FS_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA2 Channel 6
void DMA2_Channel6_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DMA2 Channel 7
void DMA2_Channel7_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// LPUART1
void LPUART1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// QUADSPI
void QUADSPI_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// I2C3 event
void I2C3_EV_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// I2C3 error
void I2C3_ER_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// SAI1
void SAI1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// SAI2
void SAI2_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// SWPMI1
void SWPMI1_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// TSC
void TSC_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// DCMI
void LCD_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// AES
void AES_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// RNG
void RNG_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

// FPU
void FPU_IRQHandler(void) __attribute__ ((interrupt, weak, alias("__Default_Handler")));

/*
+=============================================================================+
| Vector table
+=============================================================================+
*/

extern const char __main_stack_end;			// imported main stack end (from linker script)

void Reset_Handler(void);					// import the address of Reset_Handler()

void (* const vectors[])(void) __attribute__ ((section(".vectors"))) = {
		(void (*)(void))&__main_stack_end,	// Main stack end address
		Reset_Handler,						// Reset
		NMI_Handler,						// Non-maskable interrupt (RCC clock security system)
		HardFault_Handler,					// All class of fault
		MemManage_Handler,					// Memory management
		BusFault_Handler,					// Pre-fetch fault, memory access fault
		UsageFault_Handler,					// Undefined instruction or illegal state
		__Reserved_0x1C_Handler,			// Reserved 0x1C
		__Reserved_0x20_Handler,			// Reserved 0x20
		__Reserved_0x24_Handler,			// Reserved 0x24
		__Reserved_0x28_Handler,			// Reserved 0x28
		SVC_Handler,						// System service call via SWI instruction
		DebugMon_Handler,					// Debug monitor
		__Reserved_0x34_Handler,			// Reserved 0x34
		PendSV_Handler,						// Pendable request for system service
		SysTick_Handler,					// System tick timer
		WWDG_IRQHandler,					// Window WatchDog
		PVD_IRQHandler,						// PVD through EXTI Line detection
		TAMP_STAMP_IRQHandler,				// Tamper and TimeStamps through the EXTI line
		RTC_WKUP_IRQHandler,				// RTC Wakeup through the EXTI line
		FLASH_IRQHandler,					// FLASH
		RCC_IRQHandler,						// RCC
		EXTI0_IRQHandler,					// EXTI Line0
		EXTI1_IRQHandler,					// EXTI Line1
		EXTI2_IRQHandler,					// EXTI Line2
		EXTI3_IRQHandler,					// EXTI Line3
		EXTI4_IRQHandler,					// EXTI Line4
		DMA1_Channel1_IRQHandler,			// DMA1 Channel 1
		DMA1_Channel2_IRQHandler,			// DMA1 Channel 2
		DMA1_Channel3_IRQHandler,			// DMA1 Channel 3
		DMA1_Channel4_IRQHandler,			// DMA1 Channel 4
		DMA1_Channel5_IRQHandler,			// DMA1 Channel 5
		DMA1_Channel6_IRQHandler,			// DMA1 Channel 6
		DMA1_Channel7_IRQHandler,			// DMA1 Channel 7
		ADC1_2_IRQHandler,					// ADC1, ADC2
		CAN1_TX_IRQHandler,					// CAN1 TX
		CAN1_RX0_IRQHandler,				// CAN1 RX0
		CAN1_RX1_IRQHandler,				// CAN1 RX1
		CAN1_SCE_IRQHandler,				// CAN1 SCE
		EXTI9_5_IRQHandler,					// External Line[9:5]s
		TIM1_BRK_TIM15_IRQHandler,			// TIM1 Break and TIM15
		TIM1_UP_TIM16_IRQHandler,			// TIM1 Update and TIM16
		TIM1_TRG_COM_TIM17_IRQHandler,		// TIM1 Trigger and Commutation and TIM17
		TIM1_CC_IRQHandler,					// TIM1 Capture Compare
		TIM2_IRQHandler,					// TIM2
		TIM3_IRQHandler,					// TIM3
		TIM4_IRQHandler,					// TIM4
		I2C1_EV_IRQHandler,					// I2C1 Event
		I2C1_ER_IRQHandler,					// I2C1 Error
		I2C2_EV_IRQHandler,					// I2C2 Event
		I2C2_ER_IRQHandler,					// I2C2 Error
		SPI1_IRQHandler,					// SPI1
		SPI2_IRQHandler,					// SPI2
		USART1_IRQHandler,					// USART1
		USART2_IRQHandler,					// USART2
		USART3_IRQHandler,					// USART3
		EXTI15_10_IRQHandler,				// External Line[15:10]s
		RTC_Alarm_IRQHandler,				// RTC Alarm (A and B) through EXTI Line
		DFSDM1_FLT3_IRQHandler,				// DFSDM1 FLT3
		TIM8_BRK_IRQHandler,				// TIM8 Break
		TIM8_UP_IRQHandler,					// TIM8 Update
		TIM8_TRG_COM_IRQHandler,			// TIM8 Trigger and Commutation
		TIM8_CC_IRQHandler,					// TIM8 Capture Compare
		ADC3_IRQHandler,					// ADC3
		FMC_IRQHandler,						// FMC
		SDMMC1_IRQHandler,					// SDMMC1
		TIM5_IRQHandler,					// TIM5
		SPI3_IRQHandler,					// SPI3
		UART4_IRQHandler,					// UART4
		UART5_IRQHandler,					// UART5
		TIM6_DAC_IRQHandler,				// TIM6 and DAC1&2 underrun errors
		TIM7_IRQHandler,					// TIM7
		DMA2_Channel1_IRQHandler,			// DMA2 Channel 1
		DMA2_Channel2_IRQHandler,			// DMA2 Channel 2
		DMA2_Channel3_IRQHandler,			// DMA2 Channel 3
		DMA2_Channel4_IRQHandler,			// DMA2 Channel 4
		DMA2_Channel5_IRQHandler,			// DMA2 Channel 5
		DFSDM1_FLT0_IRQHandler,				// DFSDM1 FLT0
		DFSDM1_FLT1_IRQHandler,				// DFSDM1 FLT1
		DFSDM1_FLT2_IRQHandler,				// DFSDM1 FLT2
		COMP_IRQHandler,					// COMP
		LPTIM1_IRQHandler,					// LPTIM1
		LPTIM2_IRQHandler,					// LPTIM2
		OTG_FS_IRQHandler,					// OTG FS
		DMA2_Channel6_IRQHandler,			// DMA2 Channel 6
		DMA2_Channel7_IRQHandler,			// DMA2 Channel 7
		LPUART1_IRQHandler,					// LPUART1
		QUADSPI_IRQHandler,					// QUADSPI
		I2C3_EV_IRQHandler,					// I2C3 event
		I2C3_ER_IRQHandler,					// I2C3 error
		SAI1_IRQHandler,					// SAI1
		SAI2_IRQHandler,					// SAI2
		SWPMI1_IRQHandler,					// SWPMI1
		TSC_IRQHandler,						// TSC
		LCD_IRQHandler,						// LCD
		AES_IRQHandler,						// AES
		RNG_IRQHandler,						// RNG
		FPU_IRQHandler,						// FPU
};

/******************************************************************************
* END OF FILE
******************************************************************************/
