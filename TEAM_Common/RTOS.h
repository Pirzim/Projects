/**
 * \file
 * \brief RTOS interface.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module the main tasks of the RTOS.
 */

#ifndef RTOS_H_
#define RTOS_H_

#include "Platform.h"
#include "Application.h"
#include "LED.h"
#if PL_CONFIG_HAS_MOTOR
	#include "Motor.h"
#endif
#if PL_CONFIG_HAS_REFLECTANCE
#include "reflectance.h"
#endif
#if PL_CONFIG_HAS_PID
#include "Pid.h"
#endif
#if PL_CONFIG_HAS_TRIGGER
#include "trigger.h"
#endif
#if PL_CONFIG_HAS_BUZZER
#include "buzzer.h"
#endif
#if PL_CONFIG_HAS_EVENTS
#include "event.h"
#endif
#if PL_CONFIG_HAS_DRIVE
#include "drive.h"
#endif
#if PL_CONFIG_HAS_LINE_FOLLOW
#include "LineFollow.h"
#endif
#if PL_CONFIG_HAS_TURN
#include "Turn.h"
#endif
#if PL_HAS_DISTANCE_SENSOR
#include "Distance.h"
#endif
#if PL_CONFIG_HAS_QUADRATURE
#include "Q4CLeft.h"
#include "Q4CRight.h"
#endif
#if PL_CONFIG_HAS_RTOS
/*! \brief Initializes the RTOS module */
void RTOS_Init(void);

/*! \brief De-Initializes the RTOS module */
void RTOS_Deinit(void);

void doDriving(void);
static void delayOver(void* param);

typedef enum {
	stop,
	initDrive,
	forward,
	backward,
	go,
	left,
	right,
	ready,
	Linefolowing,
	SUMO,
	backwardRight,
	backwardLeft,
	waitForTurn
} driveState;

typedef enum{
	turnLeft,
	turnRight,
	turnBack
}turn_t;

#endif /* PL_CONFIG_HAS_RTOS */

#endif /* RTOS_H_ */
