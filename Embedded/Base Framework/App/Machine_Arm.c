#include "User.h"
void Blanket_Open(void)		//�����̴�
{
	PWM_SetDuty(&htim5,TIM_CHANNEL_3,1.5/20.0);
	vTaskDelay(1000);
}
