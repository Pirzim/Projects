/* ###################################################################
**     Filename    : main.c
**     Project     : INTRO_Robot_Master
**     Processor   : MK22FX512VLQ12
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-09-20, 21:03, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "LEDPin1.h"
#include "BitIoLdd1.h"
#include "LEDPin2.h"
#include "BitIoLdd2.h"
#include "SW1.h"
#include "ExtIntLdd1.h"
#include "AS1.h"
#include "ASerialLdd3.h"
#include "FRTOS1.h"
#include "RTOSCNTRLDD1.h"
#include "TI1.h"
#include "TimerIntLdd1.h"
#include "TU1.h"
#include "CLS1.h"
#include "RTT1.h"
#include "SYS1.h"
#include "LED_IR.h"
#include "LEDpin3.h"
#include "BitIoLdd5.h"
#include "RefCnt.h"
#include "IR1.h"
#include "BitIoLdd6.h"
#include "IR2.h"
#include "BitIoLdd7.h"
#include "IR3.h"
#include "BitIoLdd8.h"
#include "IR4.h"
#include "BitIoLdd9.h"
#include "IR5.h"
#include "BitIoLdd10.h"
#include "IR6.h"
#include "BitIoLdd11.h"
#include "Q4CLeft.h"
#include "C12.h"
#include "BitIoLdd16.h"
#include "C23.h"
#include "BitIoLdd17.h"
#include "Q4CRight.h"
#include "C13.h"
#include "BitIoLdd18.h"
#include "C25.h"
#include "BitIoLdd19.h"
#include "MOTTU.h"
#include "DIRL.h"
#include "BitIoLdd12.h"
#include "PWMR.h"
#include "PwmLdd2.h"
#include "DIRR.h"
#include "BitIoLdd13.h"
#include "PWML.h"
#include "PwmLdd3.h"
#include "QuadInt.h"
#include "TimerIntLdd2.h"
#include "TU_QuadInt.h"
#include "TMOUT1.h"
#include "USB1.h"
#include "CDC1.h"
#include "Tx1.h"
#include "Rx1.h"
#include "USB0.h"
#include "MCUC1.h"
#include "ADC_Bat.h"
#include "TmDt1.h"
#include "IFsh1.h"
#include "IntFlashLdd1.h"
#include "BUZ1.h"
#include "BitIoLdd4.h"
#include "WAIT1.h"
#include "CS1.h"
#include "HF1.h"
#include "XF1.h"
#include "UTIL1.h"
#include "KIN1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
#include "Application.h"
#include "LED.h"
#if PL_CONFIG_HAS_MOTOR
	#include "Motor.h"
#endif
#if PL_CONFIG_HAS_REFLECTANCE
#include "reflectance.h"
#endif

void (*f)(void) = NULL;
int i;

typedef enum {
	initDrive,
	forward,
	backward,
	stop,
	go,
	left,
	right
} driveState;
#if PL_CONFIG_HAS_RTOS



extern xSemaphoreHandle buttonHandle;
void doDriving(int8_t param){
	int32_t lastPosL = (int32_t)Q4CLeft_GetPos();
	int32_t lastPosR = (int32_t)Q4CRight_GetPos();
	static bool driving_ON = 0;
	int8_t speed = param;
	int8_t newspeed;
	static bool toggle;
	static uint16_t counter;
	REF_LineKind lineKind;
	if(xSemaphoreTake(buttonHandle, 0)==pdTRUE){
		driving_ON = !driving_ON;
	}
	if(REF_IsReady()&&driving_ON){
		lineKind = REF_GetLineKind();
			switch(lineKind){
			case REF_LINE_NONE:
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT),-100);
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT),-100);
				while((int32_t)Q4CRight_GetPos()-lastPosR>-800) vTaskDelay(pdMS_TO_TICKS(5));
				lastPosL = (int32_t)Q4CLeft_GetPos();
				lastPosR = (int32_t)Q4CRight_GetPos();
				if(toggle){
					MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT),-100);
					MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT),100);
					while((int32_t)Q4CLeft_GetPos()-lastPosL>-800) vTaskDelay(pdMS_TO_TICKS(5));
					toggle = !toggle;
				}else{
					MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT),100);
					MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT),-100);
					while((int32_t)Q4CRight_GetPos()-lastPosR>-800) vTaskDelay(pdMS_TO_TICKS(5));
					toggle = !toggle;
				}

			break;
			case REF_LINE_FULL:
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT),speed);
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT),speed);
			break;
			case REF_LINE_LEFT:
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT),-100);
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT),-100);
				while((int32_t)Q4CRight_GetPos()-lastPosR>-300) vTaskDelay(pdMS_TO_TICKS(5));
				lastPosL = (int32_t)Q4CLeft_GetPos();
				lastPosR = (int32_t)Q4CRight_GetPos();
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT),-100);
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT),100);
				while((int32_t)Q4CLeft_GetPos()-lastPosL>-1000) vTaskDelay(pdMS_TO_TICKS(5));
			break;
			case REF_LINE_RIGHT:
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT),-100);
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT),-100);
				while((int32_t)Q4CRight_GetPos()-lastPosR>-300) vTaskDelay(pdMS_TO_TICKS(5));
				lastPosL = (int32_t)Q4CLeft_GetPos();
				lastPosR = (int32_t)Q4CRight_GetPos();
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT),100);
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT),-100);
				while((int32_t)Q4CRight_GetPos()-lastPosR>-1000) vTaskDelay(pdMS_TO_TICKS(5));
			break;
			case REF_LINE_STRAIGHT:
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT),speed);
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT),speed);
			break;
			default:
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT),0);
				MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT),0);
			break;
		}
	}else{
		MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT),0);
		MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT),0);
	}
}
static void DriveTask(void * pvParameters){
	(void*)pvParameters;
	int8_t speed = 90;
	MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT),10);
	MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT),10);
	for(;;){
		doDriving(speed);
/*		if((int32_t)Q4CLeft_GetPos() >= 1000){
			MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT),0);
		}
		if((int32_t)Q4CRight_GetPos() >= 1000){
			MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT),0);
		}*/
		vTaskDelay(pdMS_TO_TICKS(3));
	}
}
#endif
/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */

#if 0
  f();
  *((int*)0) = 5;
  for (i=0; i<50; i++) {
    __asm("nop");
  }
#endif

#if PL_CONFIG_HAS_RTOS

	xTaskHandle taskHndl;
	if(!FRTOS1_xTaskCreate(DriveTask, "Drivi", configMINIMAL_STACK_SIZE+100, (void*)NULL, tskIDLE_PRIORITY, &taskHndl)){
		for(;;);//error
	}

	xTaskHandle taskHndl1;
	if(!FRTOS1_xTaskCreate(APP_Start, "Application", configMINIMAL_STACK_SIZE+800, (void*)NULL, tskIDLE_PRIORITY+2, &taskHndl1)){
		for(;;);//error
	}

	vTaskStartScheduler();
#else
	APP_Start();
#endif



  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
