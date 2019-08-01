#include "User.h"
TaskHandle_t LED_Task_Handle;	 //LED��˸���
TaskHandle_t PID_Calculate_Task;  //����PID���
TaskHandle_t Remote_Control_Task; //ң�����ݴ�����
TaskHandle_t MPU_Task_Handler;
void User_Code(void)
{
	Power1_CTRL = 1;					   //��1�ŵ�Դ����
	Power2_CTRL = 1;					   //��2�ŵ�Դ����
	Power3_CTRL = 1;					   //��3�ŵ�Դ����
	Power4_CTRL = 1;					   //��4�ŵ�Դ����
	xTaskCreate((TaskFunction_t)LED_Flash, //LED��˸����
				(const char *)"LED_TASK",
				(uint16_t)128,
				(void *)NULL,
				(UBaseType_t)2,
				(TaskHandle_t *)LED_Task_Handle);
	xTaskCreate((TaskFunction_t)PID_Calculate, //PID�������
				(const char *)"PID_Calculate",
				(uint16_t)1024,
				(void *)NULL,
				(UBaseType_t)2,
				(TaskHandle_t *)PID_Calculate_Task);
	xTaskCreate((TaskFunction_t)Remote_Control, //ң�����������
				(const char *)"Remote_Control",
				(uint16_t)128,
				(void *)NULL,
				(UBaseType_t)4,
				(TaskHandle_t *)Remote_Control_Task);
	xTaskCreate((TaskFunction_t)MPU_task,
				(const char *)"MPU_Result",
				(uint16_t)128,
				(void *)NULL,
				(UBaseType_t)4,
				(TaskHandle_t *)MPU_Task_Handler);
}
