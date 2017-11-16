/**
 * \file
 * \brief Semaphore usage
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module using semaphores.
 */

/**
 * \file
 * \brief Semaphore usage
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module using semaphores.
 */

#include "Platform.h" /* interface to the platform */
#if PL_CONFIG_HAS_SEMAPHORE
#include "FRTOS1.h"
#include "Sem.h"
#include "LED.h"

//static xSemaphoreHandle sem = NULL;

static void vSlaveTask(void *pvParameters) {
	xSemaphoreHandle sem = (xSemaphoreHandle)pvParameters;
	for(;;){
		if(xSemaphoreTake(sem, portMAX_DELAY)==pdTRUE){
			LED1_Neg();
		}
	}
}

static void vMasterTask(void *pvParameters) {
  xSemaphoreHandle sem = xSemaphoreCreateBinary();
	xTaskHandle taskHnd2;
	if(!FRTOS1_xTaskCreate(vSlaveTask, "Slave", configMINIMAL_STACK_SIZE+50, sem, tskIDLE_PRIORITY, &taskHnd2)){
		for(;;);//error
	}
  if(sem==NULL){
	  for(;;);	// something went wrong
  }
  for(;;){
	  xSemaphoreGive(sem);
	  vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void SEM_Deinit(void) {
}

/*! \brief Initializes module */
void SEM_Init(void) {
	xTaskHandle taskHndl;
	if(!FRTOS1_xTaskCreate(vMasterTask, "Master", configMINIMAL_STACK_SIZE+100, (void*)NULL, tskIDLE_PRIORITY, &taskHndl)){
		for(;;);//error
	}
}
#endif /* PL_CONFIG_HAS_SEMAPHORE */
