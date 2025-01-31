#ifndef PWM_BUTTON_BLINKY_H
#define PWM_BUTTON_BLINKY_H

/**
 * This is the main program
 */
void blinky(void);

/**
 * This is the button press callback function
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif
