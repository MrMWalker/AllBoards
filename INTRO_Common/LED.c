/**
 * \file
 * \brief LED driver
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This is a generic module to drive one or multiple LEDs.
 */

#include "Platform.h"
#if PL_CONFIG_HAS_LEDS
#include "LED.h"
#include "Shell.h"
#include "CLS1.h"
#include "FRTOS1.h"

void LED_Deinit(void) {
  LED1_Off();
  LED2_Off();
  LED3_Off();
#if PL_CONFIG_NOF_LEDS>=4
  #error "only 3 LEDs supported"
#endif
}

#if PL_CONFIG_HAS_SHELL
static uint8_t LED_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"LED", (unsigned char*)"Group of LED commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Shows LED help or status\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  tog", (unsigned char*)"Switch LED 1 & 2 on/off\r\n", io->stdOut);

#if PL_CONFIG_HAS_RTOS
  CLS1_SendHelpStr((unsigned char*)"  blink <ms>", (unsigned char*)"Blink LED 1 & 2 with freq <ms>\r\n", io->stdOut);
#endif

  return ERR_OK;
}

static uint8_t LED_PrintStatus(const CLS1_StdIOType *io) {
	uint8_t buf[16];

	CLS1_SendStatusStr("LED", "\r\n", io->stdOut);
	UTIL1_Num32sToStr(buf, sizeof(buf), LED1_Get());
	UTIL1_strcat(buf, sizeof(buf), "\r\n");
	CLS1_SendStatusStr("  LED1", buf, io->stdOut);

	UTIL1_Num32sToStr(buf, sizeof(buf), LED2_Get());
	UTIL1_strcat(buf, sizeof(buf), "\r\n");
	CLS1_SendStatusStr("  LED2", buf, io->stdOut);


	return ERR_OK;
}

static uint8_t LED_Tog(const CLS1_StdIOType *io){
	LED1_Neg();
	LED2_Neg();
}

static void LEDBlink(void* param) {
  const int freq = param;

  (void)param; /* avoid compiler warning */
  for(;;) {
    	LED1_Neg();
    	FRTOS1_vTaskDelay(pdMS_TO_TICKS(freq));
    	LED1_Neg();
    	LED2_Neg();
    	FRTOS1_vTaskDelay(pdMS_TO_TICKS(freq));
    	LED2_Neg();
    }
    /* \todo handle your application code here */
  }

uint8_t LED_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
  const unsigned char *p;
  uint32_t freq, duration;

  if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, (char*)"LED help")==0) {
    *handled = TRUE;
    return LED_PrintHelp(io);
  } else if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, (char*)"LED status")==0) {
    *handled = TRUE;
    return LED_PrintStatus(io);
  } else if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, (char*)"LED tog")==0) {
	*handled = TRUE;
	return LED_Tog(io);
  } else if (UTIL1_strncmp(cmd, "LED blink ", sizeof("LED blink ")-1)==0) {
	    p = cmd+sizeof("LED blink ")-1;
	    if (UTIL1_xatoi(&p, &freq)==ERR_OK) {
	      *handled = TRUE;
	      if (FRTOS1_xTaskCreate(LEDBlink, (signed portCHAR *)"LEDBlink", configMINIMAL_STACK_SIZE, freq, tskIDLE_PRIORITY + 2, NULL) != pdPASS) {
	    	  for(;;){}/* error case only, stay here! */
	      }
	    }
  }
  return ERR_OK;

}
#endif /* PL_CONFIG_HAS_SHELL */



void LED_Init(void) {
  LED1_Off();
  LED2_Off();
  LED3_Off();
}

#endif /* PL_CONFIG_HAS_LEDS */
