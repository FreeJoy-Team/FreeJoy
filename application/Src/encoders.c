/**
  ******************************************************************************
  * @file           : encoders.c
  * @brief          : Encoders driver implementation
		
		FreeJoy software for game device controllers
    Copyright (C) 2020  Yury Vostrenkov (yuvostrenkov@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
		
  ******************************************************************************
  */

#include "encoders.h"

/* ---------------------------------------------------------------------------
 * Tabelas de quadratura originais — mantidas para modo 4x (ALPS RKJXT1F42001)
 * --------------------------------------------------------------------------- */
const int8_t enc_array_1 [16] =
{
	0,  0,  0,  0,
	-1,  0,  0,  0,
	1,  0,  0,  0,
	0,  0,  0,  0
};

const int8_t enc_array_2 [16] =
{
	0,  0,  0,  0,
	-1,  0,  0,  1,
	1,  0,  0, -1,
	0,  0,  0,  0
};

const int8_t enc_array_4 [16] =
{
	0,  1, -1,  0,
	-1,  0,  0,  1,
	1,  0,  0, -1,
	0, -1,  1,  0
};

/* ---------------------------------------------------------------------------
 * Maquina de estados em quadratura (FSM) — usada para ENCODER_CONF_1x e 2x
 *
 * Encoder configurado como button GND: repouso = AB=11 (ambos HIGH)
 * Sequencia CW:  11 -> 01 -> 00 -> 10 -> 11  => pulso ao chegar em 11
 * Sequencia CCW: 11 -> 10 -> 00 -> 01 -> 11  => pulso ao chegar em 11
 *
 * Bounce (11->01->11 ou 11->10->11) descartado automaticamente.
 * Mudanca de direcao imediata detectada sem perder o primeiro step.
 *
 * ENCODER_CONF_4x usa enc_array_4 original — necessario para encoders como
 * o ALPS RKJXT1F42001 que tem detent no meio do ciclo (AB=00), gerando
 * pulso a cada meia-rotacao de quadratura.
 * --------------------------------------------------------------------------- */
#define FSM_R_START     0x00
#define FSM_DIR_CW      0x10
#define FSM_DIR_CCW     0x20
#define FSM_START_M     0x01  /* estado intermediario (00) */
#define FSM_CW_BEGIN    0x02  /* veio de 11, foi para 01 */
#define FSM_CCW_BEGIN   0x03  /* veio de 11, foi para 10 */
#define FSM_CW_BEGIN_M  0x04  /* veio de 00, foi para 10 -> pulso CW */
#define FSM_CCW_BEGIN_M 0x05  /* veio de 00, foi para 01 -> pulso CCW */

/* Tabela [estado & 0x0F][AB] -> proximo estado
 * AB: bit1=B, bit0=A. Colunas: 00=0, 01=1, 10=2, 11=3 */
static const uint8_t fsm_quadrature[6][4] =
{
	/*  00               01                10                11                       */
	{FSM_START_M,    FSM_CW_BEGIN,    FSM_CCW_BEGIN,    FSM_R_START             },  /* R_START       */
	{FSM_START_M,    FSM_CCW_BEGIN_M, FSM_CW_BEGIN_M,   FSM_R_START             },  /* START_M  (00) */
	{FSM_START_M,    FSM_CW_BEGIN,    FSM_R_START,      FSM_R_START             },  /* CW_BEGIN (01) */
	{FSM_START_M,    FSM_R_START,     FSM_CCW_BEGIN,    FSM_R_START             },  /* CCW_BEGIN(10) */
	{FSM_START_M,    FSM_R_START,     FSM_CW_BEGIN_M,   FSM_R_START|FSM_DIR_CW  },  /* CW_BEGIN_M    */
	{FSM_START_M,    FSM_CCW_BEGIN_M, FSM_R_START,      FSM_R_START|FSM_DIR_CCW },  /* CCW_BEGIN_M   */
};

encoder_state_t encoders_state[MAX_ENCODERS_NUM];

static void EncoderFastInit(dev_config_t * p_dev_config)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;	
	RCC_ClocksTypeDef RCC_Clocks;
	
	RCC_GetClocksFreq(&RCC_Clocks);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);		
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
	TIM_TimeBaseInitStructure.TIM_Period = 65535;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up | TIM_CounterMode_Down;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	
	switch (p_dev_config->encoders[0])
	{
		default:	
		case ENCODER_CONF_2x:
			TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI1, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
			break;
		
		case ENCODER_CONF_4x:
			TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
			break;
	}
	
	TIM1->CNT = 0;
	TIM_Cmd(TIM1, ENABLE);
}

static void EmitEncoderPulse(int i, int8_t stt,
	logical_buttons_state_t * button_state_buf,
	dev_config_t * p_dev_config,
	int8_t * ignore_a, int8_t * ignore_b,
	int32_t millis)
{
	if (stt > 0)
	{
		if ((p_dev_config->buttons[encoders_state[i].pin_a].shift_modificator > 0 && 
				shifts_state & 1<<(p_dev_config->buttons[encoders_state[i].pin_a].shift_modificator-1)))
		{
			button_state_buf[encoders_state[i].pin_a].current_state = 1;
		}
		else if (p_dev_config->buttons[encoders_state[i].pin_a].shift_modificator == 0)
		{
			uint8_t tmp_ignore = 0;
			for (int k = 0; k < MAX_ENCODERS_NUM; k++)
			{
				if (p_dev_config->buttons[encoders_state[i].pin_a].physical_num == ignore_a[k])
				{
					tmp_ignore = 1;
					break;
				}
			}
			if (tmp_ignore == 0)
			{
				button_state_buf[encoders_state[i].pin_a].current_state = 1;
			}
		}
	}
	else
	{
		if ((p_dev_config->buttons[encoders_state[i].pin_b].shift_modificator > 0 && 
				shifts_state & 1<<(p_dev_config->buttons[encoders_state[i].pin_b].shift_modificator-1)))
		{
			button_state_buf[encoders_state[i].pin_b].current_state = 1;
		}
		else if (p_dev_config->buttons[encoders_state[i].pin_b].shift_modificator == 0)
		{
			uint8_t tmp_ignore = 0;
			for (int k = 0; k < MAX_ENCODERS_NUM; k++)
			{
				if (p_dev_config->buttons[encoders_state[i].pin_b].physical_num == ignore_b[k])
				{
					tmp_ignore = 1;
					break;
				}
			}
			if (tmp_ignore == 0)
			{
				button_state_buf[encoders_state[i].pin_b].current_state = 1;
			}
		}
	}

	encoders_state[i].dir       = stt > 0 ? 1 : -1;
	encoders_state[i].last_dir  = encoders_state[i].dir;
	encoders_state[i].time_last = millis;
	encoders_state[i].cnt      += stt;

	if (encoders_state[i].cnt > AXIS_MAX_VALUE) encoders_state[i].cnt = AXIS_MAX_VALUE;
	if (encoders_state[i].cnt < AXIS_MIN_VALUE) encoders_state[i].cnt = AXIS_MIN_VALUE;
}

void EncoderProcess (logical_buttons_state_t * button_state_buf, dev_config_t * p_dev_config)
{	
	uint8_t encoders_present = 0;
	
	if (encoders_state[0].pin_a >=0 && encoders_state[0].pin_b >=0) 
	{
		encoders_state[0].cnt = (int16_t)(TIM1->CNT);
	}
	
	for (int i=1; i<MAX_ENCODERS_NUM; i++)
	{
		if (encoders_state[i].pin_a >=0 && encoders_state[i].pin_b >=0) 
		{
			encoders_present = 1;
			break;
		}
	}
	if (!encoders_present) return;
	
	int8_t ignore_a[MAX_ENCODERS_NUM]={};
	int8_t ignore_b[MAX_ENCODERS_NUM]={};
	for (int k=0; k<MAX_ENCODERS_NUM; k++)
	{
		ignore_a[k] = -1;
		ignore_b[k] = -1;
	}	
	uint8_t tmp_a = 0;
	uint8_t tmp_b = 0;
	
	int32_t millis = GetMillis();
	
	for (int k = 0; k < MAX_ENCODERS_NUM; k++)
	{
		if (p_dev_config->buttons[encoders_state[k].pin_a].shift_modificator > 0 && 
		 shifts_state & 1<<(p_dev_config->buttons[encoders_state[k].pin_a].shift_modificator-1))
		{
			ignore_a[tmp_a++] = p_dev_config->buttons[encoders_state[k].pin_a].physical_num;
		}
		if (p_dev_config->buttons[encoders_state[k].pin_b].shift_modificator > 0 && 
		 shifts_state & 1<<(p_dev_config->buttons[encoders_state[k].pin_b].shift_modificator-1))
		{
			ignore_b[tmp_b++] = p_dev_config->buttons[encoders_state[k].pin_b].physical_num;
		}
	}
	
	for (int i=1; i<MAX_ENCODERS_NUM; i++)
	{
		if (encoders_state[i].pin_a >=0 && encoders_state[i].pin_b >=0)
		{
			/* Leitura dos pinos A e B
			 * Button GND: repouso=HIGH=1, acionado=LOW=0 */
			uint8_t pin_a = raw_buttons_data[p_dev_config->buttons[encoders_state[i].pin_a].physical_num] ? 1 : 0;
			uint8_t pin_b = raw_buttons_data[p_dev_config->buttons[encoders_state[i].pin_b].physical_num] ? 1 : 0;
			uint8_t ab = (pin_b << 1) | pin_a;

			if (p_dev_config->encoders[i] == ENCODER_CONF_4x)
			{
				/* Modo 4x — enc_array_4 original com leitura de 4 bits (prev+cur).
				 * Necessario para ALPS RKJXT1F42001: detent em AB=00 e AB=11,
				 * gerando pulso a cada meia-rotacao. */
				encoders_state[i].state = ((encoders_state[i].state << 2) & 0x0C) | ab;
				uint8_t cur  = encoders_state[i].state & 0x03;
				uint8_t prev = (encoders_state[i].state >> 2) & 0x03;
				if (cur != prev)
				{
					int8_t stt = enc_array_4[encoders_state[i].state & 0x0F];
					if (stt != 0)
					{
						EmitEncoderPulse(i, stt, button_state_buf, p_dev_config, ignore_a, ignore_b, millis);
					}
				}
			}
			else
			{
				/* Modos 1x e 2x — FSM de quadratura completa.
				 * Pulso so emitido apos 4 transicoes completas.
				 * Bounce descartado automaticamente.
				 * Mudanca de direcao imediata sem perder o primeiro step. */
				uint8_t next_state = fsm_quadrature[encoders_state[i].fsm_state & 0x0F][ab];
				encoders_state[i].fsm_state = next_state & 0x0F;
				uint8_t direction = next_state & 0x30;
				if (direction == FSM_DIR_CW || direction == FSM_DIR_CCW)
				{
					int8_t stt = (direction == FSM_DIR_CW) ? 1 : -1;
					EmitEncoderPulse(i, stt, button_state_buf, p_dev_config, ignore_a, ignore_b, millis);
				}
			}
		}

		/* Unpress encoder button apos press_time */
		if (encoders_state[i].pin_a >=0 && encoders_state[i].pin_b >=0)
		{		
			uint16_t a_press_time;
			uint16_t b_press_time;

			switch (p_dev_config->buttons[encoders_state[i].pin_a].press_timer)
			{	
				case BUTTON_TIMER_1:
					a_press_time = p_dev_config->button_timer1_ms;
					break;
				case BUTTON_TIMER_2:
					a_press_time = p_dev_config->button_timer2_ms;
					break;
				case BUTTON_TIMER_3:
					a_press_time = p_dev_config->button_timer3_ms;
					break;
				default:
					a_press_time = p_dev_config->encoder_press_time_ms;
					break;
			};
			
			switch (p_dev_config->buttons[encoders_state[i].pin_b].press_timer)
			{	
				case BUTTON_TIMER_1:
					b_press_time = p_dev_config->button_timer1_ms;
					break;
				case BUTTON_TIMER_2:
					b_press_time = p_dev_config->button_timer2_ms;
					break;
				case BUTTON_TIMER_3:
					b_press_time = p_dev_config->button_timer3_ms;
					break;
				default:
					b_press_time = p_dev_config->encoder_press_time_ms;
					break;
			};
			
			if (millis - encoders_state[i].time_last > a_press_time)
			{	
				button_state_buf[encoders_state[i].pin_a].current_state = 0;
			}
			if (millis - encoders_state[i].time_last > b_press_time)
			{	
				button_state_buf[encoders_state[i].pin_b].current_state = 0;
			}
		}
	}
}

void EncodersInit(dev_config_t * p_dev_config)
{
	uint8_t pos = 1;
	int8_t prev_a = -1;
	int8_t prev_b = -1;
	
	for (int i=0; i<MAX_ENCODERS_NUM; i++)
	{
		encoders_state[i].pin_a = -1;
		encoders_state[i].pin_b = -1;
		encoders_state[i].state = 0;
		encoders_state[i].fsm_state = FSM_R_START;
		encoders_state[i].time_last = 0;
		encoders_state[i].cycle_started = 0;
	}
	
	if (p_dev_config->pins[8] == FAST_ENCODER &&
			p_dev_config->pins[9] == FAST_ENCODER)
	{
		encoders_state[0].pin_a = 8;
		encoders_state[0].pin_b = 9;
		EncoderFastInit(p_dev_config);
	}
	
	for (int i=0; i<MAX_BUTTONS_NUM; i++)
	{
		if ((p_dev_config->buttons[i].type) == ENCODER_INPUT_A &&  i > prev_a)
		{
			for (int j=0; j<MAX_BUTTONS_NUM; j++)
			{
				if ((p_dev_config->buttons[j].type) == ENCODER_INPUT_B && j > prev_b && pos < MAX_ENCODERS_NUM)
				{
					encoders_state[pos].pin_a = i;
					encoders_state[pos].pin_b = j;
					encoders_state[pos].dir = 1;
					encoders_state[pos].last_dir = 1;
					encoders_state[pos].cycle_started = 0;
					encoders_state[pos].fsm_state = FSM_R_START;
					
					prev_a = i;
					prev_b = j;
					pos++;
					break;
				}
			}
		}	
	}
}
