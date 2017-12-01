/**
 * \file
 * \brief Real Time Operating System (RTOS) main program.
 * \author Erich Styger, erich.styger@hslu.ch
 */

#include "Platform.h"
#if PL_CONFIG_HAS_RTOS
#include "RTOS.h"
#include "FRTOS1.h"


extern xSemaphoreHandle buttonHandle, time_5s_handle, OFF_Handle;

void doDriving(int32_t* speedR, int32_t* speedL){
	static bool driving_ON = 0;
	static driveState state;
	REF_LineKind lineKind;
	bool firstTimeON = FALSE;

	bool driving_ON_past = driving_ON;
	if(xSemaphoreTake(buttonHandle, 0)==pdTRUE){
		driving_ON = !driving_ON;
	}
	if(!driving_ON){
		state = stop;
	}else if(driving_ON_past){
		state = initDrive;
		firstTimeON = TRUE;
	}else{
		lineKind = REF_GetLineKind();
	}

	if(xSemaphoreTake(OFF_Handle,0)==pdTRUE){
		state = stop;
	}

	switch(state){
	case initDrive:
			if(firstTimeON){
				TRG_SetTrigger(TRG_5S, 5000/TRG_TICKS_MS, (TRG_Callback)EVNT_SetEvent, (TRG_CallBackDataPtr)EVNT_5s_done);
			}
			BUZ_Beep(300,10);
			if(xSemaphoreTake(time_5s_handle, 0)==pdTRUE){
				state = ready;
			}
		break;
	case stop:
		*speedR = 0;
		*speedL = 0;
		break;
	case normalDrive:
		break;

	}

#if 0
	int32_t lastPosL = (int32_t)Q4CLeft_GetPos();
	int32_t lastPosR = (int32_t)Q4CRight_GetPos();
	static bool driving_ON = 0;
	int8_t speed = param;
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
#endif
}


static void SUMO_Drive(void * pvParameters){
	(void*)pvParameters;
	int32_t setSpeedR, setSpeedL;

	setSpeedL = 1000;
	setSpeedR = 1000;
	DRV_SetSpeed(setSpeedL, setSpeedR);
	for(;;){
		//doDriving(&setSpeedR, &setSpeedL);

		vTaskDelay(pdMS_TO_TICKS(10));
	}
}


void RTOS_Init(void) {

	xTaskHandle taskHndl;
	if(!FRTOS1_xTaskCreate(SUMO_Drive, "Sumo_Drive", configMINIMAL_STACK_SIZE+100, (void*)NULL, tskIDLE_PRIORITY, &taskHndl)){
		for(;;);//error
	}

}

void RTOS_Deinit(void) {
  /* nothing needed for now */
}

#endif /* PL_CONFIG_HAS_RTOS */
