#include "includes.h"
#include "stdlib.h"
#include    <math.h>

#include "Protocol.h"
#include "Queue.h"
#include "motorconfig.h"
#include "an41908a.h"


#include "bsp_spi_flash.h"

void timer0_int(void);
void timer0_initial(void);
void timer3_init(void);
u8 zf_key_handle(void);

u8 af_action_cnt = 0;
u8 af_action_onepush_cnt = 0;

#define CAMERA_POWER_PIN    GPIO_Pin_2
#define CAMERA_SET_KEY_PIN    GPIO_Pin_3

#define CAMERA_POWER_PORT    GPIOD
#define CAMERA_SET_KEY_PORT   GPIOB

void camera_power_key_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = CAMERA_SET_KEY_PIN;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CAMERA_SET_KEY_PORT, &GPIOD_InitStructure);

	GPIOD_InitStructure.GPIO_Pin = CAMERA_POWER_PIN;
    GPIO_Init(CAMERA_POWER_PORT, &GPIOD_InitStructure);


	GPIO_ResetBits(CAMERA_POWER_PORT,CAMERA_POWER_PIN);	
	//GPIO_SetBits(GPIOC,CAMERA_POWER_PIN);	



	
	GPIO_SetBits(CAMERA_SET_KEY_PORT,CAMERA_SET_KEY_PIN);	



	PWR_BackupAccessCmd(ENABLE);//�����޸�RTC �ͺ󱸼Ĵ���
	RCC_LSEConfig(RCC_LSE_OFF);//�ر��ⲿ�����ⲿʱ���źŹ��� ��PC13 PC14 PC15 �ſ��Ե���ͨIO�á�
	BKP_TamperPinCmd(DISABLE);//�ر����ּ�⹦�ܣ�Ҳ���� PC13��Ҳ���Ե���ͨIO ʹ��
	PWR_BackupAccessCmd(DISABLE);//��ֹ�޸ĺ󱸼Ĵ���

	//RCC_LSICmd(DISABLE);
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	
	GPIO_SetBits(GPIOC,GPIO_Pin_14);	
	GPIO_SetBits(GPIOC,GPIO_Pin_15);
	GPIO_SetBits(GPIOC,GPIO_Pin_13);


	
	//GPIO_SetBits(CAMERA_POWER_PORT,CAMERA_POWER_PIN);	
}

void camera_power_off_on(u8 mode)
{
    if(mode)//on
        GPIO_SetBits(CAMERA_POWER_PORT,CAMERA_POWER_PIN);	
    else
        GPIO_ResetBits(CAMERA_POWER_PORT,CAMERA_POWER_PIN);	
}

void camera_sensor_format_set_with_long_key(void)
{
    camera_power_off_on(0);
    delay_X1ms(3000);
    GPIO_ResetBits(CAMERA_SET_KEY_PORT,CAMERA_SET_KEY_PIN);	
    camera_power_off_on(1);
    delay_X1ms(9000);
    GPIO_SetBits(CAMERA_SET_KEY_PORT,CAMERA_SET_KEY_PIN);

}

void camera_ntsc_pal_switch(u8 mode)
{
    camera_power_off_on(0);
    delay_X1ms(3000);
    GPIO_ResetBits(GPIOC,GPIO_Pin_14);	
    camera_power_off_on(1);
    delay_X1ms(9000);
    GPIO_SetBits(GPIOC,GPIO_Pin_14);
}

void camera_hd_switch(u8 mode)
{
    camera_power_off_on(0);
    delay_X1ms(3000);
    GPIO_ResetBits(GPIOC,GPIO_Pin_13);	
    camera_power_off_on(1);
    delay_X1ms(9000);
    GPIO_SetBits(GPIOC,GPIO_Pin_13);
}

void camera_cvbs_switch(u8 mode)
{
    camera_power_off_on(0);
    delay_X1ms(3000);
    GPIO_ResetBits(GPIOC,GPIO_Pin_15);	
    camera_power_off_on(1);
    delay_X1ms(9000);
    GPIO_SetBits(GPIOC,GPIO_Pin_15);
}



void zoom_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = ZOOM_PIN_A1|ZOOM_PIN_A2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ZOOM_PORT, &GPIOD_InitStructure);	


    GPIO_ResetBits(ZOOM_PORT,ZOOM_PIN_A1);	
    GPIO_ResetBits(ZOOM_PORT,ZOOM_PIN_A2);	

}

u8 iii = 2;

void focus_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = FOCUS_PIN_A1|FOCUS_PIN_A2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FOCUS_PORT, &GPIOD_InitStructure);

	GPIO_ResetBits(FOCUS_PORT,FOCUS_PIN_A1);	
	GPIO_ResetBits(FOCUS_PORT,FOCUS_PIN_A2);	


}


void iris_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = IRIS_PIN_PLUS|IRIS_PIN_SUB;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IRIS_PORT, &GPIOD_InitStructure);

	GPIO_ResetBits(IRIS_PORT,IRIS_PIN_PLUS);	
	GPIO_ResetBits(IRIS_PORT,IRIS_PIN_SUB);	


}


void iris_auto_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = IRIS_AUTO_PIN_A1|IRIS_AUTO_PIN_A2|IRIS_AUTO_PIN_B1|IRIS_AUTO_PIN_B2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IRIS_AUTO_PORT, &GPIOD_InitStructure);

	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_A1);	
	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_A2);	
	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_B1);	
	GPIO_ResetBits(IRIS_AUTO_PORT,IRIS_AUTO_PIN_B2);	

//        md127_keepmode();
}



//���þ�ͷ�����������������ѹ
void motor_voltage_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = VOLTAGE_VL3|VOLTAGE_VL2|VOLTAGE_VL1;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	

	GPIO_SetBits(GPIOB,VOLTAGE_VL3);	
	GPIO_ResetBits(GPIOB,VOLTAGE_VL2);	
	GPIO_ResetBits(GPIOB,VOLTAGE_VL1);	
}

void camera_power_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	

	GPIO_SetBits(GPIOC,GPIO_Pin_2);	

	

}

void camera_power_on_off(u8 mode)
{
    if(mode)
        GPIO_SetBits(GPIOC,GPIO_Pin_2);	
    else
        GPIO_ResetBits(GPIOC,GPIO_Pin_2);	   
}

void iris_ex_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB, ENABLE);
    
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIOD_InitStructure);	

	GPIO_SetBits(GPIOC,GPIO_Pin_5);	

	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);	
}


void iris_ex_pin_set(u8 mode)
{
	if(mode > 1)
		return;

	    system_para.system_para.para_iris_ex_mode = mode;

	switch(mode)
	{
	case 1:
		GPIO_SetBits(GPIOC,GPIO_Pin_5);	
		GPIO_SetBits(GPIOB,GPIO_Pin_0);	
		break;
	case 0:
		GPIO_ResetBits(GPIOC,GPIO_Pin_5);	
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);	
		break;

	}

}

void extern_function_pin_Init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	

	GPIO_ResetBits(GPIOB,GPIO_Pin_12);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);	

    system_para.system_para.para_ex_io_1_mode = 1;   

    GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	
    
	extern_io2_output(0); 
	extern_io3_output(0); 

}

void key_long_osd_emu(void)
{
    GPIO_SetBits(GPIOB,GPIO_Pin_8);	
    GPIO_SetBits(GPIOB,GPIO_Pin_9);	
    delay_X1ms(4000);   
    GPIO_ResetBits(GPIOB,GPIO_Pin_8);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);	
}

void key_init(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = KEY_LEFT_PIN|KEY_RIGHT_PIN|KEY_UP_PIN|KEY_DOWN_PIN;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_PORT, &GPIOD_InitStructure);	
}


// recieve datas from keyboard
void uart0_init(void)
{
// UCSR0B = 0x00; //disable while setting baud rate
// UCSR0A = 0x00;
// UCSR0C = 0x06;
//-----------�ȹرմ��ڹ���
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART1, DISABLE);


	BSP_USART1_Init();

	switch(Baud_rate)
	{
    case DOME_BAUDRATE_1200: 
				APP_USART1_Baudrate(1200);
			   break;    
	case DOME_BAUDRATE_2400: 
				APP_USART1_Baudrate(2400);
			   break;
	case DOME_BAUDRATE_4800: 
				APP_USART1_Baudrate(4800);
			   break;
	case DOME_BAUDRATE_9600: 
				APP_USART1_Baudrate(9600);
			   break;
	case DOME_BAUDRATE_14400:
		APP_USART1_Baudrate(14400);
	       break;
	case DOME_BAUDRATE_19200:
		APP_USART1_Baudrate(19200);
	       break;
	case DOME_BAUDRATE_115200:
		APP_USART1_Baudrate(115200);
	       break;	   
	default:   
		APP_USART1_Baudrate(2400);
	    break;
	}

	/* Enable USART1 receive intterrupt */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
  	/* Enable USART1 */
  	USART_Cmd(USART1, ENABLE);
	
	/* set the control pin at receive status */
	//GPIO_ResetBits(GPIOA, RS485Control);

	RS485_RX_ENABLE;
	//RS485_TX_ENABLE;

}

void serial_int1_send(void)	   //send data to USAR1		   
{
    if (cam_counter_send<cam_cmd_lenth)
    {
    	USART_SendData(USART2, txd1_buffer[cam_counter_send-1]);
    	USART_ClearITPendingBit(USART2, USART_IT_TXE);
    	cam_counter_send++; 
    }
    else
    {
    	USART_SendData(USART2, txd1_buffer[cam_counter_send-1]);
    	USART_ClearITPendingBit(USART2, USART_IT_TXE);
    	cam_counter_send=1;
    	txd1_buff_cFlag=1;
    	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    }
	if (cam_counter_send>=200)
		cam_counter_send=1; 
}

u8 cam_cmd_recieve_len = 0;
u8 cam_recieve_cnt_public=0;
void serial_int1_receive(uchar udr1)//receive data from USAR1
{
     
    rxd1_buffer[counter_receive] = udr1;

    if(counter_receive == 3)
    {
        cam_recieve_cnt_public=0;
        if(rxd1_buffer[2]==0x20)
        {
            cam_cmd_recieve_len = rxd1_buffer[counter_receive]+6;
        }
        else if(rxd1_buffer[2]==0x28)
        {
            cam_cmd_recieve_len = 7;
        }
        else
            cam_cmd_recieve_len = rxd1_buffer[counter_receive]+6;

		if(rxd1_buffer[2] == 0x10)
			cam_cmd_recieve_len--;
    }
    
   	counter_receive++;

   
    if (counter_receive >= cam_cmd_recieve_len && counter_receive>3)
   //if (counter_receive >= 6 && counter_receive>3)
   {
        cam_recieve_cnt_public = cam_cmd_recieve_len;
    	rxd1_buff_cFlag=1;
    	counter_receive=0;
        cam_cmd_recieve_len=0;
    }

    if (counter_receive >= 200)
    	counter_receive = 0;
}

// control the camera
void uart_to_camera_init(void)
{
    BSP_USART2_Init();
    APP_USART2_Baudrate(19200);
    
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);// Enable USART2 receive intterrupt 
    USART_Cmd(USART2, ENABLE); // Enable USART2

}

//������Ƭ����
void cam_filt_cutter_int(void)
{
	GPIO_InitTypeDef GPIOD_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
    GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIOD_InitStructure);	


    GPIO_ResetBits(GPIOB,GPIO_Pin_12);	
    GPIO_ResetBits(GPIOB,GPIO_Pin_13);	

}

void cam_filt_cutter_set(u8 mode)
{
    if(mode)
    {// fog
        GPIO_SetBits(GPIOB,GPIO_Pin_13);	

    }
    else
    {// ir

        GPIO_ResetBits(GPIOB,GPIO_Pin_13);	

    }

}

void ports_initial(void)
{
	BSP_Init();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	// �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_Disable SWJ ��ȫ���ã�JTAG+SW-DP��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	// �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_JTAGDisable ��JTAG-DP ���� + SW-DP ʹ��

	camera_power_key_pin_Init();

    //DC Motor driver ic: BA6208
	zoom_pin_Init();
	focus_pin_Init();
    iris_pin_Init();
    
	extern_function_pin_Init();
    //camera_power_pin_Init();
    motor_voltage_pin_Init();
    //iris_auto_pin_Init();
    
    
	timer0_initial();

	iris_ex_pin_Init();
	
//����UART4 PC10,PC11����ʱ���ΰ������� 
	//key_init();


	
    uart_to_camera_init();

	timer3_init();
	timer1_init();


	SPI_FLASH_Init();
//spi_flash_init();
    an41908a_spi_pin_init();
	
}

/*******************************************************************************
* Function Name  : InterruptConfig
* Description    : Configures the used IRQ Channels and sets their priority.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InterruptConfig(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;


	#ifdef  VECT_TAB_RAM  
  		/* Set the Vector Table base location at 0x20000000 */
  		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH */
  		/* Set the Vector Table base location at 0x08000000 */
  		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif

  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  	
		
  /* Enable the EXTI9_5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the EXTI15_10 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  



  /* Enable the TIM1 UP Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the EXTI Line17 Interrupt */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

//////////////////////////////////////////////////////////////////
#if 1//CAMERA_HITACHI_SC110

//return: 1,success;0,overtime
u8 RS485_SendBytes(u16 bytes_size,u8 *send_buff)
{
	u32 overtime=0;
	u16 i;
	RS485_TX_ENABLE;
	for(i=0;i<bytes_size;i++)
	{
		overtime=0;
		USART1->DR = send_buff[i];
		while((USART1->SR&0x40)==0)
		{
			if(overtime>UART_SEND_OVERTIME)
				return 0;
			overtime++;
		}
	}
    RS485_RX_ENABLE;
	return 1;
}

#endif


void SerialPutString(unsigned char *Sent_value,int lenghtbuf)
{
    int k ;
//	RS485TX_Enable();
	 for (k=0;k<lenghtbuf;k++)   // ������λ��
	     {
	       USART_SendData(USART1, *(Sent_value+k));
		   while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		  // delay_half_ms(12);
		 }
//      delay_half_ms(20);
//	 RS485TX_DisEnable() ;
   
}

void timer0_initial(void)
{
// TCCR0 = 0x00; //stop timer0
// TCNT0 = 0x83; //set count
// TIMSK |= 0x01; //timer interrupt sources
// TCCR0 = 0x06; //start timer0
	BSP_Timer2_Init();
}


//////////////////////////////////////////////////////////////
//TIMER3 initialisation - prescale:64
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 128uSec
// actual value: 128.000uSec (0.0%)
void timer3_init(void)
{	 
 /*TCCR3B = 0x00; //stop
 TCNT3H = 0xFF; //setup
 TCNT3L = 0xa0;
 TCCR3A = 0x00;          //���ﲻ������ʱ��
// TCCR3B = 0x02;*/
	BSP_Timer5_Init();
}

u16 key_check_period_cnt = 0;
u32 timer_cnt_x2ms	= 0;

void timer0_int(void)////2 ms timer
{
	if(key_check_period_cnt>0)
		key_check_period_cnt--;

    if(af_action_cnt < 0xff)
        af_action_cnt++;

    if(af_action_onepush_cnt < 0xff)
        af_action_onepush_cnt++;

	if(timer_cnt_x2ms < 0xffffffff)
		timer_cnt_x2ms++;
}

void read_protect(void)
{
	if(FLASH_GetReadOutProtectionStatus() != SET)
	{
	       FLASH_Unlock();//������FALSH���ܻᵼ���޷��������ö�����
	       FLASH_ReadOutProtection(ENABLE);
	}
}



u8 key_get(u16 *data)
{
	u16 temp = 0;
	u8 ret = 0;
	temp = (GPIO_ReadInputData(KEY_PORT)>>4&0x000f);
	if(temp)
	{
		delayms(3);
		if(temp == (GPIO_ReadInputData(KEY_PORT)>>4)&0x000f)
		{
			*data = temp;
			ret = 1;
		}
	}
	return ret;
}

u16 KeyPre = 0;//  �ϴΰ�����״̬
u16 KeyCurrent = 0;//  ���ΰ�����״̬
u16 KeyStbLast = 0;// ���˲�����ϴΰ�����״̬
u16 KeyStb = 0;//  ���˲���ı��ΰ�����״̬
u16 KeyAvlUp = 0;// ����̧����Ч����Ĵ���
u16 KeyAvlDown = 0;//  ����������Ч����Ĵ���

#define	KEY_DOWN_STATE		0

u16 key_val_table[] = {KEY_LEFT_PIN,KEY_RIGHT_PIN,KEY_UP_PIN,KEY_DOWN_PIN,KEY_SET_PIN};


// ������÷���ÿ��10MS ���� 20MS ����һ��������򼴿�
#if 1
u8 CheckKey(void)
{
	u16 KTmp1 = 0,KTmp2 = 0 ;
	u16 KeyStatu = 0;
	u16 val = 0;
	u8 i =0;
		
	//if(!key_check_period_cnt)	
	if(1)
	{
		val = GPIO_ReadInputData(KEY_PORT);

		//val = val>>8;
		val &= 0x1f00;

		KeyStatu = val ;	

		KeyPre = KeyCurrent ;  //�����ϴΰ���״̬
		KeyCurrent = KeyStatu ;   //���浱ǰ����״̬

		delayms(10);

		val = GPIO_ReadInputData(KEY_PORT);

		//val = val>>8;
		val &= 0x1f00;	

		KeyCurrent = val ;
		

		key_check_period_cnt = 8;
		return 1;
	}

	return 0;
}


#else
void CheckKey(void)
{
	u16 KTmp1 = 0,KTmp2 = 0 ;
	u16 KeyStatu = 0;
	u16 val = 0;

	if(!key_check_period_cnt)	
	{
		val = GPIO_ReadInputData(KEY_PORT);

		//val = val>>8;
		val &= 0x1f00;

		KeyStatu = val ;	

		KeyPre = KeyCurrent ;  //�����ϴΰ���״̬
		KeyCurrent = KeyStatu ;   //���浱ǰ����״̬
			
		KTmp1 = (KeyCurrent | KeyPre) ; //00 -> 0 ȡ�˲�ֵ0
		KTmp2 = (KeyCurrent & KeyPre) ; //11 -> 1 ȡ�˲�ֵ1
		
		KeyStbLast = KeyStb ;	//�����ϴξ��˲����ֵ	
		KeyStb &= KTmp1 ; //��ֵ��ǰ���˲����ֵ
		KeyStb |= KTmp2 ;

		//ȥ����ĵ�ǰ����״̬ 1��Ч
		KeyAvlUp = (KeyStb ^ KeyStbLast) & KeyStbLast; //�����ذ��±�־ ��ȡ1��0��״̬	
		KeyAvlDown = (KeyStb ^ KeyStbLast) & KeyStb ;	//�½��ذ��±�־ ��ȡ0��1��״̬		

		key_check_period_cnt = 8;
	}
	
}

#endif


//����������� �����һ������ ������밴��������Ч
u8 CmdK1DOWN(u16 key)
{
	if((KeyAvlDown & key) == 0) 
		return 0;
	KeyAvlDown &= ~key ;
	return 1;
	//����������
}

//����������� �����һ������ ������밴��̧������Ч
u8 CmdK1UP(u16 key)
{
	if((KeyAvlUp & key) == 0) 
		return 0;
	KeyAvlUp &= ~key ;

	return 1;
	//����������
}

volatile enum ZF_TYPE zoom_state_cur = ALL_STOP;
volatile enum ZF_TYPE focus_state_cur = ALL_STOP;
volatile enum ZF_TYPE zf_state_cur = ALL_STOP;

u8 key_zf_convert(void)
{
	if(CmdK1DOWN(FOCUS_PLUS_PIN))
		focus_state_cur = FOCUS_FAR;

	if(CmdK1DOWN(FOCUS_SUB_PIN))
		focus_state_cur = FOCUS_NEAR;

	if(CmdK1DOWN(ZOOM_PLUS_PIN))
		zoom_state_cur = ZOOM_TELE;

	if(CmdK1DOWN(ZOOM_SUB_PIN))
		zoom_state_cur = ZOOM_WIDE;

//-----------------------------------------
	if(CmdK1UP(FOCUS_PLUS_PIN))
		focus_state_cur = FOCUS_STOP;

	if(CmdK1UP(FOCUS_SUB_PIN))
		focus_state_cur = FOCUS_STOP;

	if(CmdK1UP(ZOOM_PLUS_PIN))
		zoom_state_cur = ZOOM_STOP;

	if(CmdK1UP(ZOOM_SUB_PIN))
		zoom_state_cur = ZOOM_STOP;

	return 1;
}

u8 key_monitor(void)
{
	u16 key_tmp;
	
	if(CheckKey())
	{

		key_tmp = KeyCurrent;
		
		return 1;

	}

	else
		return 0;
}


#define		KEY_DEFAULT		0X1F00
void keyscan(void)// ����ɨ�躯��
{ 
uchar xx;//�ֲ�����
u16 val;

val = GPIO_ReadInputData(KEY_PORT);
val &= 0x1f00;

	if(val!=KEY_DEFAULT) 
	{ 
		delayms(10); 
		if(val!=KEY_DEFAULT) 
		{ 
			
			val = GPIO_ReadInputData(KEY_PORT);
			val &= KEY_DEFAULT;
			
			switch(val) //���ݰ���״̬�����ò�ͬ���Ӻ���
			{ 
			case 0xfe:

			break; 

			case 0xfd: 
			break; 
			default:break; 
			 
			} 
		} 


	} 

}






u8 key_check(u16 key)
{
	u16 key_tmp;
	u8 i=0;
	
	if(CheckKey())
	{
		for(i=0;i<5;)
		{

			if(key_val_table[i] == key)
				break;

			i++;
		}
		
		if(KeyCurrent & key)
		{

			if(i < 2)
			{	
				zf_state_cur = FOCUS_STOP;
				focus_state_cur = FOCUS_STOP;
			}
			else
			{
				zf_state_cur = ZOOM_STOP;
				zoom_state_cur = ZOOM_STOP;
				}

		}
		else
		{
			
			switch(i)
				{
			case 0:

				zf_state_cur = FOCUS_FAR;
				focus_state_cur = zf_state_cur;
				break;
			case 1:
				zf_state_cur = FOCUS_NEAR;
				focus_state_cur = zf_state_cur;
				break;
			case 2:
				zf_state_cur = ZOOM_TELE;
				zoom_state_cur = zf_state_cur;
				break;
			case 3:
				zf_state_cur = ZOOM_WIDE;
				zoom_state_cur = zf_state_cur;
				break;
			default:
				zf_state_cur = ALL_STOP;
				zoom_state_cur = zf_state_cur;
				focus_state_cur = zf_state_cur;
				break;
			}

			

		}
		
		return 1;

	}

	else
		return 1;


}

void zoom_focus_handle(u8 mode)
{
	u8 i;

	
	focus_action(zf_state_cur);
	zoom_action(zf_state_cur);
	
}


u8 zf_key_handle(void)
{
	u8 i;

	static enum ZF_TYPE zf_state_cur_pre = ALL_STOP;

	
	zf_state_cur_pre = zf_state_cur;
	
	for(i=0;i<5;i++)
	{
		if( !key_check(key_val_table[i]))
			continue;
		
		if(i<2 )
		{
			
			if(zf_state_cur == FOCUS_NEAR || zf_state_cur == FOCUS_FAR)
				break;
		}
		else
		{
			if(focus_state_cur == FOCUS_STOP && zoom_state_cur == ZOOM_STOP)
				zf_state_cur = ALL_STOP;
				
			if(zf_state_cur == ZOOM_WIDE || zf_state_cur == ZOOM_TELE)
			{

				zoom_run_state = 1;

				break;

			}

			
		}
	}

	i = 0x00;
	
	if(zf_state_cur_pre != zf_state_cur)
	{	
		i = 0x01;

		if(zf_state_cur == FOCUS_STOP || zf_state_cur == ZOOM_STOP || zf_state_cur == ALL_STOP)
			{
			command_byte = 5;
			}
		else if(zf_state_cur == ZOOM_TELE)
			{
				command_byte = 1;

		}
		else if(zf_state_cur == ZOOM_WIDE)
		{
			command_byte = 2;

		}
		else if(zf_state_cur == FOCUS_FAR)
		{
			command_byte = 3;

		}
		else if(zf_state_cur == FOCUS_NEAR)
		{
			command_byte = 4;

		}
	}

	return i;

}


u8 zf_key_osd_handle(void)
{
	u8 i;

	static enum ZF_TYPE zf_state_cur_pre = ALL_STOP;

	
	zf_state_cur_pre = zf_state_cur;
	
	for(i=0;i<5;i++)
	{
		if( !key_check(key_val_table[i]))
			continue;
		
		if(i<2 )
		{
			
			if(zf_state_cur == FOCUS_NEAR || zf_state_cur == FOCUS_FAR)
				break;
		}
		else
		{
			if(focus_state_cur == FOCUS_STOP && zoom_state_cur == ZOOM_STOP)
				zf_state_cur = ALL_STOP;
				
			if(zf_state_cur == ZOOM_WIDE || zf_state_cur == ZOOM_TELE)
			{

				zoom_run_state = 1;

				break;

			}

			
		}
	}

	i = 0x00;
	
	if(zf_state_cur_pre != zf_state_cur)
	{	
		i = 0x01;

		if(zf_state_cur == FOCUS_STOP || zf_state_cur == ZOOM_STOP || zf_state_cur == ALL_STOP)
			{
			command_byte = 5;
			}
		else if(zf_state_cur == ZOOM_TELE)
			{
				command_byte = 6;

		}
		else if(zf_state_cur == ZOOM_WIDE)
		{
			command_byte = 7;

		}
		else if(zf_state_cur == FOCUS_FAR)
		{
			command_byte = 8;

		}
		else if(zf_state_cur == FOCUS_NEAR)
		{
			command_byte = 9;

		}
	}

	return i;

}



void pt_action(uchar action,uchar pan_speed,uchar tilt_speed) //action:0~7 ->left,right....; action=8 means stop command
{ 
    com_buffer[1] = pt_code2motor[action<<1];
    com_buffer[3] = pt_code2motor[(action<<1)+1];
	com_buffer[0] = 0x55; 								//��ѧϰ����
	com_buffer[2] = pan_speed;
	com_buffer[4] = tilt_speed;
    com_buffer[5] = 0;

    if(action<=1)
    	MotorControll(); 
    else if(action <= 3)
        focus_MotorControll();
        
}

void pt_action_tilt(uchar action,uchar tilt_speed) //action:0~7 ->left,right....; action=8 means stop command
{ 
    com_buffer_tilt[1] = pt_code2motor[action<<1];
    com_buffer_tilt[3] = pt_code2motor[(action<<1)+1];
	com_buffer_tilt[0] = 0x55; 								//��ѧϰ����
	com_buffer_tilt[4] = tilt_speed;
    com_buffer_tilt[5] = 0;

    focus_MotorControll();
        
}


void ptz_execute(void)
{
    uchar temp;

    switch(command_byte)
	{
    case 0x01://zoom
    case 0x02:	
        temp = command_byte - 1; 
        zoom_action(ZOOM_TELE+temp);
        zoom_run_state = 1;
    break;
    
    case 0x03://focus
    case 0x04:	
        temp = command_byte - 3; 
        //pt_action_tilt(temp,1);
        focus_action(FOCUS_FAR+temp);
        
        focus_run_state = 1;
    break;
    case 0x05:  
        MotorStop();
		focus_action(ALL_STOP);
		zoom_action(ALL_STOP);
		jiguang_zoom_state = 0;
    break;

    default:break;
    }
}

uchar prePoint_num_Function(uchar point)//let preset point be in 1~128 continually, or it returns a value 0
{
    #if 0
	if(point==0)
		return 0;

	if ((point >= 79) && (point <= 101) ) 
    	return 0;				

	if ((point >= 40) && (point <= 70) ) 
    	return 0;				

	if ((point >= 10) && (point <= 20) ) 
    	return 0;	
	

    return 1;
    #else
    return 0;
    #endif
}

#define FOCUS_LEVEL_1       580
#define FOCUS_LEVEL_2       770
#define FOCUS_LEVEL_3       900
#define FOCUS_LEVEL_4       960
#define FOCUS_LEVEL_5       1000
#define FOCUS_LEVEL_6       1030
#define FOCUS_LEVEL_7       1000


#define		AUTO_ZF_ZERO_POS_STEPS		(double)((double)16.24/ZOOM_LEN_PER_STEP)

void auto_focus_onepush(void)
{
    s32 temp = 0;
    double focus_temp = 0;
    
    	
	temp = panpos;

	if(temp >= 2044)
		temp = 2043;
	
	if(temp < 0)
		temp = 0;

	focus_temp = focus_pos_table[temp];
    focus_temp = focus_temp/FOCUS_LEN_PER_MOTOR_STEP;
    
#if 0

	if(focus_temp<=0)
		temp = fabs(focus_temp) + FOCUS_SENSOR_POS_SENSOR_TO_FAR_STEPS;
	else
		temp = FOCUS_SENSOR_POS_SENSOR_TO_FAR_STEPS - focus_temp;

    
    
#else
	if(focus_temp<=0)
		temp = fabs(focus_temp) + FOCUS_ZERO_TO_FAR_END_STEPS;
	else
		temp = FOCUS_ZERO_TO_FAR_END_STEPS - focus_temp;
#endif	
    focus_goto_pos_absolutely(temp,0,0);    
}

#define     FOCUS_TABLE_SIZE     sizeof(focus_pos_table)/sizeof(double)
#if 1
void auto_focus_track_zoom(void)
{
    s32 temp = 0;
    double focus_temp = 0;
    
   
    
    if(af_action_cnt > 20)
    {        
        
        if(!focus_run_state && zoom_run_state)
        {
    			
    			temp = panpos;

    			if(temp >= FOCUS_TABLE_SIZE)
    				temp = FOCUS_TABLE_SIZE-1;
    			
    			if(temp < 0)
    				temp = 0;

    			focus_temp = focus_pos_table[temp];
                
                focus_temp = focus_temp/FOCUS_LEN_PER_MOTOR_STEP; 
    			//focus_temp *= 8;
                
    			if(focus_temp<=0)
    				temp = fabs(focus_temp) + FOCUS_ZERO_TO_FAR_END_STEPS;
            	else
    				temp = FOCUS_ZERO_TO_FAR_END_STEPS - focus_temp;

               
            focus_goto_pos_absolutely(temp,0,0);    

            
        }

        af_action_cnt = 0;
    }

     if(af_action_onepush_cnt > 50)
    {
        if((zoom_run_state_pre == 1 && !zoom_run_state) || zoom_pos_limit_state)
        {
            zoom_pos_limit_state = 0;
            
            temp = panpos;
			if(temp >= FOCUS_TABLE_SIZE)
				temp = FOCUS_TABLE_SIZE-1;			
			if(temp < 0)
				temp = 0;
			focus_temp = focus_pos_table[temp];
            //focus_temp *= 8;

            focus_temp = focus_temp/FOCUS_LEN_PER_MOTOR_STEP;
            //focus_temp = fabs(focus_temp);

			if(focus_temp<=0)
				temp = fabs(focus_temp) + FOCUS_ZERO_TO_FAR_END_STEPS;
        	else
				temp = FOCUS_ZERO_TO_FAR_END_STEPS - focus_temp;   	

            
            
            focus_goto_pos_absolutely(temp,0,0);   

            zoom_run_state_pre = zoom_run_state;

            return;
        }
        af_action_onepush_cnt = 0;
    }    
    
}


#else
void auto_focus_track_zoom(void)
{
    s32 temp = 0;
    
    if(!focus_run_state)
    {
        if(panpos<FOCUS_LEVEL_1)
            temp = panpos;
        else if(panpos < FOCUS_LEVEL_2)
        {
            temp = FOCUS_LEVEL_1 + (panpos - FOCUS_LEVEL_1)/2;
        }
        else if(panpos < FOCUS_LEVEL_3)
        {
            temp = FOCUS_LEVEL_1 + (FOCUS_LEVEL_2 - FOCUS_LEVEL_1)/2 + (panpos - FOCUS_LEVEL_2)/3;
        }
        else if(panpos < FOCUS_LEVEL_4)
        {
            temp = FOCUS_LEVEL_1 + (FOCUS_LEVEL_2 - FOCUS_LEVEL_1)/2 + (FOCUS_LEVEL_3 - FOCUS_LEVEL_2)/3 + (panpos- FOCUS_LEVEL_3)/4;
        }  
        else if(panpos < FOCUS_LEVEL_5)
        {
            temp = FOCUS_LEVEL_1 + (FOCUS_LEVEL_2 - FOCUS_LEVEL_1)/2 + (FOCUS_LEVEL_3 - FOCUS_LEVEL_2)/3 + (FOCUS_LEVEL_3 - FOCUS_LEVEL_2)/4 + (panpos- FOCUS_LEVEL_4)/8;
        } 
        else if(panpos < FOCUS_LEVEL_6)
        {
            temp = FOCUS_LEVEL_1 + (FOCUS_LEVEL_2 - FOCUS_LEVEL_1)/2 + (FOCUS_LEVEL_3 - FOCUS_LEVEL_2)/3 + (FOCUS_LEVEL_3 - FOCUS_LEVEL_2)/4 - (panpos- FOCUS_LEVEL_4)/8;
        }  
        
        focus_goto_pos_absolutely(temp,0,0);    
    }

}

#endif


#define	IRIS_OPEN_DIR		0
#define	IRIS_CLOSE_DIR		1

#define	IRIS_STEPS_MAX		IRIS_LENS_STEPS_MAX

s16 iris_step_cnt = 0;//����CLOSE��Ϊ0����OPEN���ʱΪ���ֵ35







#define	 AN41908_STEPS_ONE_TIME (system_expand_para.system_para.para_stepsmotor_pulse_per_step)


void iris_open_set_relatively(u8 mstep)
{
	u8 data;

	if(iris_step_cnt >= IRIS_LENS_STEPS_MAX)
		return;
	
	if((iris_step_cnt + mstep)>IRIS_LENS_STEPS_MAX)
		data = IRIS_LENS_STEPS_MAX-iris_step_cnt;
	else
		data = mstep;

		u8 i;
	for(i=0;i<data;i++)
	{	
		LenDrvZoomMove(IRIS_OPEN_DIR,AN41908_STEPS_ONE_TIME);
	    delay_X1ms(20);
	}

	
	iris_step_cnt += data;
	if(iris_step_cnt>IRIS_STEPS_MAX)
		iris_step_cnt = IRIS_STEPS_MAX;
}

void iris_close_set_relatively(u8 mstep)
{
	u8 data;

	if(iris_step_cnt<=0)
		return;
	
	if(iris_step_cnt - mstep < 0)
		data = IRIS_LENS_STEPS_MAX-iris_step_cnt;
	else
		data = mstep;

	u8 i;
	for(i=0;i<data;i++)
	{
		LenDrvZoomMove(IRIS_CLOSE_DIR,AN41908_STEPS_ONE_TIME);
    	delay_X1ms(20);
	}
	
	iris_step_cnt -= data;
	if(iris_step_cnt < 0)
		iris_step_cnt = 0;
}

void iris_pos_set_absolutely(s8 mstep)
{
	u8 data;

	if(mstep<0)
		return;
	if(mstep > IRIS_STEPS_MAX)
		return;
	
	if(iris_step_cnt > mstep)
	{
		data = iris_step_cnt - mstep;
		LenDrvZoomMove(IRIS_CLOSE_DIR,data);
	    delay_X1ms(80*data);
		iris_step_cnt = mstep;
	}
	else if(iris_step_cnt < mstep)
	{
		data = mstep - iris_step_cnt;
		LenDrvZoomMove(IRIS_OPEN_DIR,data);
	    delay_X1ms(80*data);

		iris_step_cnt = mstep;
	}

}


void iris_pos_set_open_steps(u8 msteps)
{

}

u8 iris_auto_manual_state = 0;//0,iris auto;1,iris manual

//1,oopen,0,close
void iris_manual_by_shutter_handle(u8 mode)
{

	u16 val = 0;
	static u16 val_pre = 0xFFFF;
	static u8 cnt_m = 0;
	static u8 mstate = 0;
	static u8 cnt2 = 0;
	u16 tmp;

	u16 agc_v1;
	static u16 agc_v1_pre = 0;

	u8 speedtmp = 0;

	
	if(iris_auto_manual_state)
	{
		
		//if(cam_shutter_value_get(&val))
		{

			if(val > val_pre)
				tmp = val - val_pre;
			else
				tmp = val_pre -val;
			

			speedtmp = Rocket_fir_data;

			if(speedtmp > 8)
				speedtmp = 8;

			if(speedtmp == 0)
				speedtmp = 1;
			
			switch(speedtmp)
				{
			case 5:
				speedtmp = speedtmp*2;
				break;
			case 6:
				speedtmp = speedtmp*3;
				break;
			case 7:
				speedtmp = speedtmp*4;
				break;
			case 8:
				speedtmp = speedtmp*5;
				break;
			default:
				break;
			}

			
				//if((val_pre==0xffff) || (val != val_pre  && tmp > 100))
				{
					if(mode)
					{
						iris_open_set_relatively(speedtmp);
					}
					else
					{

						iris_close_set_relatively(speedtmp);
					}

				}			

			val_pre = val;
			
		}
	}
}


u8 shutter_val_auto_iris_level = 1;

void iris_auto_by_shutter_handle(void)
{
	u16 val = 0;
	static u16 val_pre = 0;
	static u8 cnt_m = 0;
	static u8 mstate = 0;
	static u8 cnt2 = 0,cnt3 = 0;
	u16 tmp;

	u16 agc_v1;
	static u16 agc_v1_pre = 0;

	static u8 shutter_val_auto_iris_level_pre=0xff;
	
	u16 shutter_threshold_val_min = 200,shutter_threshold_val_max = 300;
	

	if(iris_auto_manual_state != 0)
		return;
	
	if(timer_cnt_x2ms > 500)
	{
		
		if(cam_shutter_value_get(&val))
		{
			if(shutter_val_auto_iris_level_pre != shutter_val_auto_iris_level)
			{
				val_pre = 0xffff;
				
			}
			
			shutter_val_auto_iris_level_pre = shutter_val_auto_iris_level;
			
			switch(shutter_val_auto_iris_level)
			{
			case 0:
				//shutter_threshold_val_min = 95;
				//shutter_threshold_val_max = 244;
				break;
				
			case 1:
				shutter_threshold_val_min = 176;
				shutter_threshold_val_max = 280;//320;
				break;

			case 2:
				shutter_threshold_val_min = 230;
				shutter_threshold_val_max = 396;
				break;

			case 3:
				shutter_threshold_val_min = 400;
				shutter_threshold_val_max = 500;
				break;

			default:
				
				break;
			}

			if(val > val_pre)
				tmp = val - val_pre;
			else
				tmp = val_pre -val;

			//if(cam_agc_level_get(&agc_v1))
			//{

			//	if(agc_v1 == agc_v1_pre || abs(agc_v1-agc_v1_pre)<100)
			//	{
			//		mstate = 1;
			//	}
			//	else
			//	{
			//		mstate = 0;
			//	}
			//	agc_v1_pre = agc_v1;
			//}


			if(!val)
			{

				iris_open_set_relatively(2);;
				
			}
			else if(val < shutter_threshold_val_min)
			{
				iris_open_set_relatively(2);
			
			}
			else if(val > shutter_threshold_val_max)
			{
					
				iris_close_set_relatively(2);

			}

			val_pre = val;
			
		}
		timer_cnt_x2ms = 0;
	}

}

#if 1
void iris_pos_init(void)
{
	u16 val,val_pre;

	u16 cnt_m=0;

			
	if(cam_shutter_value_get(&val))
	{

		while(1)
		{
			LenDrvZoomMove(IRIS_OPEN_DIR,1);
		    delay_X1ms(500);

			val_pre = val;
			if(cam_shutter_value_get(&val))
			{
				if(val > val_pre)
				{
					continue;
				}
				if(val < val_pre)
				{
					continue;
				}
				
				if(val == val_pre && val!=0)
				{
					LenDrvZoomMove(IRIS_OPEN_DIR,3);
				    delay_X1ms(1000);
					val_pre = val;
					if(cam_shutter_value_get(&val))
					{
						if(val == val_pre && val!=0)
						{
							goto LABEL_IRIS_EXIT;
						}
					}
				}

				if(val==0)
				{
					cnt_m++;
					if(cnt_m > 20)
						goto LABEL_IRIS_EXIT;
				}

				
			}
		}
	}

LABEL_IRIS_EXIT:


	an41908_motor_on_off(0);
	 delay_X1ms(100);

	//an41908_motor_on_off(1);
	// delay_X1ms(100);

	for(val=0;val<17;val++)
	{
		LenDrvZoomMove(IRIS_CLOSE_DIR,1);
	    delay_X1ms(200);

	}
	iris_step_cnt = 18;

	while(1);
}
#else
void iris_pos_init(void)
{
	u16 val,val_pre;

	u16 cnt_m=0;

			
	if(cam_shutter_value_get(&val))
	{
		
		LenDrvZoomMove(IRIS_OPEN_DIR,2);
	    delay_X1ms(200);
		val_pre = val;
		if(cam_shutter_value_get(&val))
		{
			if(val > val_pre)
			{
				goto LABEL_OPEN_IRIS;
			}
		}
			
		if(!val)
		{
LABEL_OPEN_IRIS:			
			val_pre = val;
			LenDrvZoomMove(IRIS_OPEN_DIR,1);
		    delay_X1ms(200);
			while(1)
			{
				delay_X1ms(500);

				val_pre = val;
				if(cam_shutter_value_get(&val))
				{
					cnt_m++;
					if(val > val_pre)
					{
						//LenDrvZoomMove(IRIS_OPEN_DIR,1);
					    delay_X1ms(80);

					}
					else if(val == val_pre && val!=0 && cnt_m > 1)
					{
						//LenDrvZoomMove(IRIS_OPEN_DIR,3);
					    delay_X1ms(200);
						iris_step_cnt = IRIS_STEPS_MAX;
						break;
					}
					else if(val==0 && cnt_m>30)
					{
						//LenDrvZoomMove(IRIS_OPEN_DIR,1);
					    delay_X1ms(80);
						iris_step_cnt = IRIS_STEPS_MAX;
						break;
					}
				}
				//delay_X1ms(500);
				LenDrvZoomMove(IRIS_OPEN_DIR,1);
			    delay_X1ms(80);
			}

		}
		else 
		{
			val_pre = val;
			LenDrvZoomMove(IRIS_CLOSE_DIR,1);
		    delay_X1ms(80);
			while(1)
			{
				val_pre = val;

				delay_X1ms(500);

				if(cam_shutter_value_get(&val))
				{
					cnt_m++;
					if(val < val_pre)
					{
						//LenDrvZoomMove(IRIS_CLOSE_DIR,1);
					    delay_X1ms(80);

					}
					else if(val == val_pre && cnt_m > 1)
					{
						//LenDrvZoomMove(IRIS_CLOSE_DIR,3);
					    //delay_X1ms(200);
						iris_step_cnt = 0;
						break;
					}
					else if(cnt_m>40)
					{
						iris_step_cnt = 0;
						break;
					}
					
				}
				
				LenDrvZoomMove(IRIS_CLOSE_DIR,1);
			    delay_X1ms(80);
				
			}

		
		}
	}


	iris_pos_set_absolutely(15);
}
#endif


void iris_lens_pos_init(void)
{
	u16
	k = IRIS_LENS_STEPS_MAX/2;
    while(k)
    {
		LenDrvZoomMove(IRIS_CLOSE_DIR,AN41908_STEPS_ONE_TIME);
		
		delay_X1ms(40);
		k--;
    }
		
	delay_X1ms(300);

	k = IRIS_LENS_STEPS_MAX;
    while(k)
    {
		LenDrvZoomMove(IRIS_OPEN_DIR,AN41908_STEPS_ONE_TIME);
		
		delay_X1ms(40);
		k--;
    }

	delay_X1ms(300);

	k = IRIS_LENS_STEPS_MAX/2;
    while(k)
    {
		LenDrvZoomMove(IRIS_CLOSE_DIR,AN41908_STEPS_ONE_TIME);
		
		delay_X1ms(40);
		k--;
    }
	iris_step_cnt = IRIS_LENS_STEPS_MAX/2;
	
	delay_X1ms(300);
	
}

//u8 ttest[2000];
//double myd = 0.00078125;
u32 ttlen,ttlens,FlashID;
//#define VECT_TAB_RAM
extern void delay1Ms(int ms);


int main(void)
{	
    u16 k;


	ttlen = sizeof(system_para.system_para);
	
	InterruptConfig();
	ports_initial();
    delay_X1ms(600);


	camera_power_off_on(1);
	
    delay_X1ms(1200);

	sys_expand_para_initial();
	
	system_para.system_para.para_system_para_flag1 = 0x12; 
	system_para.system_para.para_system_para_flag2 = 0x34;

//spi_flash_test();


	/* Get SPI Flash ID */
	//FlashID = SPI_FLASH_ReadID();

	
#if 0
    while(1)
    {
        LenDrvZoomMove(1,160);
//LensDrvFocusMove(1,100);
        delay_X1ms(300);
        LenDrvZoomMove(0,160);
        //LensDrvFocusMove(0,100);
        delay_X1ms(300);
        //delay_X1ms(2000);
    }

#endif

 	//eeprom_byte_write(EEPROM_SHUTTER_THREASHOLD_ADDRESS,1);

	iris_lens_pos_init();

#if 0
 while(1)
 {
     for(k=0;k<2000;k++)
         eeprom_byte_write(k, 0x58);
     //
     for(k=0;k<2000;k++)
         ttest[k] = eeprom_byte_read(k);
	 delay_X1ms(300);

 }
#endif

#if 1
    uart_to_camera_init(); 
	delay_X1ms(600);
    //cam_get_ID(0);


    load_system_para();
    //cam_get_ID(0);
    //cam_get_ID(0);
	jiguang_comm_init();


	Protocol_No = 0;

	uart0_init();
	//cam_init();
	
	System_data_init();
    auto_focus_onepush();

	delay_X1ms(12000);
#endif
	//iris_pos_init();


		
#if 0

    while(1)
    {
		return_data_to_rs485(system_para.system_para.para_ex_io_1_mode,(kb_motor_step<<4)|(iris_auto_manual_state&0x0f),(u8)iris_step_cnt); 			

		
			delay_X1ms(800);
	
    }
		
		

#endif

	while(1)
	{
		//if (command_analysis() || zf_key_handle()) 
		//if (zf_key_handle()) 

        //return_cmd_to_camlink(0xcc,8);
        //delay_half_second(1);

		iris_auto_by_shutter_handle();
		
		if (command_analysis()) 
		{
		


			if(command_byte != 0x0e && command_byte!=0x0f)
			{
				jiguang_zoom_state = 0;


			}
			ptz_execute();
            switch(command_byte)
		    {
			 	case 0x0E://open iris
					switch(iris_ctl_mode)
					{
					case IRIS_JIGUANG_ZOOM:
						
						jiguang_zoom_state = 1;
						
						break;
					
					case IRIS_JIGUANG_CURRENT:
						jiguang_current_set(1);
						break;

					case IRIS_CAM_IRIS:
						if(iris_auto_manual_state)
						{
							iris_manual_by_shutter_handle(1);
						}
						else
						{
							iris_motor_plus();
						}
						break;

					default:
						break;
					}								 

					break;
			 	case 0x0F://close iris
			 		switch(iris_ctl_mode)
		 			{
					case IRIS_JIGUANG_ZOOM:
						jiguang_zoom_state = 2;
						break;
					
					case IRIS_JIGUANG_CURRENT:
						jiguang_current_set(0);
						break;

					case IRIS_CAM_IRIS:
						if(iris_auto_manual_state)
						{
							iris_manual_by_shutter_handle(0);
						}
						else
						{

							iris_motor_sub();
						}
						break;

					default:
						break;
					}
			 	    break;
			 
			 	case 0x10://set preset point
					k = prePoint_num_Function(Rocket_fir_data); //return 0 or 1~128 preset point
					if (k)
					{

					}
					else           //function preset point
						dome_func_control(command_byte,Rocket_fir_data);
					break;
			 
			 	case 0x11://call preset point
					k = prePoint_num_Function(Rocket_fir_data); //return 0 or 1~128 preset point
					if (k)
                    {
                    
                    }               
					else           //function preset point
						dome_func_control(command_byte,Rocket_fir_data);
					break;
			 	case 0x90:
                    iris_ctl_mode = IRIS_CAM_OSD_ENTER;
                	osdSwitchFlag = 0;
                	//cam_osd_control(1);
        			//camera_osd_switch(0);
                    key_long_osd_emu();
                    //camera_osd_key_emulate(OSD_ENTER);
                    delay_half_ms(20);
                    break;
             	default:
				break;
	   	    }

			cam_osd_control(1);
			
		}

		//zoom_focus_handle();

		jiguang_zoom_handle(jiguang_zoom_state);

		//auto_focus_track_zoom();
			
        //key_monitor();
	}
}


