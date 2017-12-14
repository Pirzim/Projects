/**
 * \file
 * \brief Real Time Operating System (RTOS) main program.
 * \author Erich Styger, erich.styger@hslu.ch
 */

#include "Platform.h"
#if PL_CONFIG_HAS_RTOS
#include "RTOS.h"
#include "FRTOS1.h"

#define LINEFOLOWING 1				// change for Sumo: 0, Linefollowing: 1

extern xSemaphoreHandle buttonHandle, time_5s_handle, OFF_Handle;
static driveState state;
static turn_t turn;

static void delayOver(void* param){
	(void)param;
	if(state == initDrive){
		state = ready;
	}
}

void doDriving(void){
	static bool driving_ON;
	static uint16_t timeout;
	int testvar;
	REF_LineKind lineKind;
	bool firstTimeON = FALSE;
	bool driving_ON_past = driving_ON;
	static bool turned;
	bool kurve;

	if(xSemaphoreTake(buttonHandle, 0)==pdTRUE){	// ein/ ausschalten befehl erhalten
		driving_ON = !driving_ON;

	}
	if(!driving_ON){	// ausschalten
		state = stop;
#if LINEFOLOWING
		LF_StopFollowing();
#endif

	}else if(!driving_ON_past){		// wenn erstes mal eingeschaltet
		state = initDrive;
		firstTimeON = TRUE;
	}else{							//wen eingeschaltet
		lineKind = REF_GetLineKind();
	}

	if(xSemaphoreTake(OFF_Handle,0)==pdTRUE){
		state = stop;

	}

	switch(state){
	case initDrive:
			if(firstTimeON){
				TRG_SetTrigger(TRG_5S, 50/TRG_TICKS_MS, (TRG_Callback)delayOver, NULL);
			}
			BUZ_Beep(300,10);
		break;
	case stop:
		driving_ON = FALSE;
		DRV_SetMode(DRV_MODE_STOP);
		break;
	case ready:
#if LINEFOLOWING
		state = Linefolowing;
#else
		state = SUMO;
#endif
		break;
	case Linefolowing:
		LF_StartFollowing();
		if((turned == FALSE) && (lineKind == REF_LINE_FULL)){
			LF_StopFollowing();
			Q4CLeft_SetPos(0);
			Q4CRight_SetPos(0);
			TURN_MoveToPos(250,250, TRUE,NULL,100);
			TURN_Turn(TURN_LEFT180, 0);
			LF_StartFollowing();
			turned = TRUE;
		}
		else if((kurve == FALSE) &&  turned && (lineKind==REF_LINE_LEFT)){
			Q4CLeft_SetPos(0);
			Q4CRight_SetPos(0);
			TURN_MoveToPos(0,350, TRUE,NULL,100);
			kurve = TRUE;
		}
		else if(turned && (lineKind==REF_LINE_NONE)){
			LF_StopFollowing();
			BUZ_PlayTune(BUZ_TUNE_AENTLISONG);
			state = stop;
			turned = FALSE;
		}
		break;
	case SUMO:
		if(lineKind==REF_LINE_NONE){
			state = backward;
			turn = turnBack;
			timeout++;
		}else if(lineKind==REF_LINE_RIGHT){
			state = backward;
			turn = turnRight;
			timeout = 0;
		}else if(lineKind==REF_LINE_LEFT){
			state = backward;
			turn = turnLeft;
			timeout = 0;
		}else{
			if(DIST_NearFrontObstacle(400)){		// front obstracle
				DRV_SetMode(DRV_MODE_SPEED);
				DRV_SetSpeed(8000,8000);
			}else if(DIST_NearRearObstacle(400)){	// rear obstracle
				TURN_Turn(TURN_LEFT180, 0);
				DRV_SetMode(DRV_MODE_SPEED);
				DRV_SetSpeed(8000,8000);
			}else if(DIST_NearRightObstacle(400)){	// right obstracle
				DRV_SetMode(DRV_MODE_SPEED);
				TURN_Turn(TURN_RIGHT90, 0);
			}else if(DIST_NearLeftObstacle(400)){	//left obstracle
				DRV_SetMode(DRV_MODE_SPEED);
				TURN_Turn(TURN_LEFT90, 0);
			}else{
				DRV_SetMode(DRV_MODE_SPEED);
				DRV_SetSpeed(4000,4000);					// nothing detected
			}
			timeout = 0;
		}
		if(timeout >= 1000){
			state = stop;
		}

		break;
	case backward:
		Q4CLeft_SetPos(0);
		Q4CRight_SetPos(0);
		state = waitForTurn;
		TURN_MoveToPos(-1000,-1000, TRUE,NULL,100);
		switch(turn){
		case turnLeft:
			TURN_TurnAngle(-80, 0);		// TURN left
			break;
		case turnRight:
			TURN_TurnAngle(80, 0);		// TURN right
			break;
		case turnBack:
			TURN_TurnAngle(120,0);
			break;
		}
		state = SUMO;
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
	for(;;){
		doDriving();
		vTaskDelay(pdMS_TO_TICKS(4));
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
