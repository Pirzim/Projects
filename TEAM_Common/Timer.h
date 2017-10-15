/**
 * \file
 * \brief Timer driver interface.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements the driver for the timers used in the system.
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "Platform.h"
#if PL_CONFIG_HAS_TIMER

/*! \todo Check timer tick frequency */
#define TMR_TICK_MS  200					// Muesste hier nicht 1000 stehen? in TI1 ist 1000ms als periodendauer eingetragen?!
  /*!< we get called every TMR_TICK_MS ms */

/*! \brief Function called from timer interrupt every TMR_TICK_MS. */
void TMR_OnInterrupt(void);

/*! \brief Timer driver initialization */
void TMR_Init(void);

/*! \brief Timer driver de-initialization */
void TMR_Deinit(void);

#endif /* PL_CONFIG_HAS_TIMER */

#endif /* TIMER_H_ */
