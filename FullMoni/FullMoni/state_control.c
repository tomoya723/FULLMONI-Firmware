// --------------------------------------------------------------------
// Copylight (C) 2013, Tomoya Sato( http://pub.ne.jp/nacci_tomoya )
//
// This file is part of FullMoni firmwere.
//
// FullMoni is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option ) any later version.
//
// FullMoni is distributed in the hope that it will be useful,
// but WITHIOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with FullMoni. if not, see <http:/www.gnu.org/licenses/>.
//
// filename		:	state_control.c
// brief		:	FullMoni rev.B �X�e�[�g�Ǘ�
// author		:	Tomoya Sato
// update		:	2013/03/20
// version		:	1.01
// --------------------------------------------------------------------

// --------------------------------------------------------------------
// ���[�U�[�w�b�_�t�@�C��
// --------------------------------------------------------------------
#include "dataregister.h"
#include "iodefine.h"
#include "state_control.h"
#include "init_system.h"
#include "TFTLCD35.h"
#include "CAN2515.h"
#include "iic.h"

// --------------------------------------------------------------------
// define�錾
// --------------------------------------------------------------------
#define ChartBufNum 201

// --------------------------------------------------------------------
// �O���[�o���ϐ��錾
// --------------------------------------------------------------------
volatile unsigned int	g_fps;
volatile unsigned int	g_fps_cnt;
volatile unsigned int	g_fps_max;
volatile unsigned int	g_beep_oneshotmin_flg;
volatile unsigned int	g_beep_oneshotmax_flg;
volatile unsigned int	g_beep_twoshotmin_flg;
volatile unsigned int	g_master_warning_flg1;
volatile unsigned int	g_master_warning_flg2;
volatile unsigned int	g_master_warning_flg3;
volatile unsigned int	g_master_warning_flg4;
volatile unsigned int	g_master_warning_flg5;
volatile unsigned int	g_master_warning_flg6;
volatile unsigned int	g_shift_timing_flg;
volatile unsigned int	backlight_dimmer_flg;
volatile unsigned int	Onetime_Peakclear_cnt;
static unsigned int		Onetime_Peakclear_done;
static unsigned long	chart_buf1[ChartBufNum];
static unsigned int		chart_buf2[ChartBufNum];
static unsigned int		chart_onoff;
static int				rev_peak, num1_peak, num2_peak, num3_peak, num4_peak, num5_peak, num6_peak;
static unsigned int		Acc;
static unsigned int		num_page;
static unsigned int		num_control;
static unsigned int		preset_no;

// --------------------------------------------------------------------
// ��ԕʏ������s�y�я�ԑJ�ڏ���
// --------------------------------------------------------------------
void state_control(void)
{
	switch(g_state)
	{
		default:
		case S001:	funcS001();
								switch(E001())
								{
									case   1:	A002();	break;
									case 999:	A990();	break;
								}
		case S002:	funcS002();
								switch(E002())
								{
									case   1:	A100();	break;
									case 999:	A990();	break;
								}
								break;
		case S010:	funcS010();
								switch(E010())
								{
									case  1:	A011();	break;
									case  2:	A012();	break;
									case  3:	A013();	break;
									case  4:	A014();	break;
									case  5:	A015();	break;
									case  6:	A016();	break;
									case  7:	A017();	break;
									case  8:	A018();	break;
									case  9:	A019();	break;
									case 10:	A020();	break;
									case 11:	A021();	break;
								}
								break;
		case S110:	funcS110();
								switch(E110())
								{
									case  1:	A101();	break;
									case  2:	A102();	break;
									case  3:	A103();	break;
									case  4:	A104();	break;
								}
								break;
		case S120:	funcS120();
								switch(E100())
								{
									case 1:		A121();	break;
									case 2:		A122();	break;
									case 3:		A133();	break;
								}
								break;
		case S130:	funcS130();
								switch(E100())
								{
									case 1:		A131();	break;
									case 2:		A132();	break;
									case 3:		A123();	break;
								}
								break;
		case S210:	funcS210();
								switch(E200())
								{
									case 1:		A201();	break;
									case 2:		A202();	break;
								}
								break;
		case S999:	funcS999();
								switch(E990())
								{
									case 1:		A991();	break;
								}
								break;
	}
}

// --------------------------------------------------------------------
// �C�x���g�����F�X�e�[�g�J�ڏ������`
// --------------------------------------------------------------------
unsigned int E001(void)
{
	static unsigned int init_cnt;
	
	// --------------------------------------------------------------------
	// �^�b�`���쏈��
	// --------------------------------------------------------------------
	if(touch_done_flg)			// �^�b�`�p�l���������肠��
	{
		touch_done_flg = 0;		// �t���O�N���A
		
		init_cnt ++;
	}
	
	if(init_cnt >= 5)
	{
		return 999;
	}
	
	if(g_int50mscnt >= 0)
	{
		return 1;	// ������ʕ`��ҋ@�I�����f
	}
	else
	{
		return 0;
	}
}

unsigned int E002(void)
{
	if(g_int50mscnt >= 0)
	{
		switch(g_e2p_data.E2P_1.model)
		{
			case 0xFF:		return 999; break;
			default:		return   1; break;
		}
	}
	else
	{
		return 0;
	}
}

unsigned int E010(void)
{
	// --------------------------------------------------------------------
	// �^�b�`���쏈��
	// --------------------------------------------------------------------
	if(touch_done_flg)			// �^�b�`�p�l���������肠��
	{
		touch_done_flg = 0;		// �t���O�N���A
		
		// --------------------------------------------------------------------
		// �ۑ��A�C�R����������
		// --------------------------------------------------------------------
		if		((g_PressX >= 130) && (g_PressX <= 193) && (g_PressY >= 215) && (g_PressY <= 238))
		{
			return 1;
		}
		// --------------------------------------------------------------------
		// ���J�[�\���A�C�R����������
		// --------------------------------------------------------------------
		else if	((g_PressX >=  46) && (g_PressX <= 139) && (g_PressY >= 215) && (g_PressY <= 238))
		{
			return 2;
		}
		// --------------------------------------------------------------------
		// �E�J�[�\���A�C�R����������
		// --------------------------------------------------------------------
		else if	((g_PressX >= 214) && (g_PressX <= 277) && (g_PressY >= 215) && (g_PressY <= 238))
		{
			return 3;
		}
		// --------------------------------------------------------------------
		// ���f���ύX��A�C�R����������
		// --------------------------------------------------------------------
		else if	((g_PressX >= 230) && (g_PressX <= (230 + 32 -1 )) && (g_PressY >= ( 24 * 2)) && (g_PressY <= ( 24 * 2 + 24 - 1)))
		{
			return 4;
		}
		// --------------------------------------------------------------------
		// ���f���ύX���A�C�R����������
		// --------------------------------------------------------------------
		else if	((g_PressX >= 270) && (g_PressX <= (270 + 32 - 1)) && (g_PressY >= ( 24 * 2)) && (g_PressY <= ( 24 * 2 + 24 - 1)))
		{
			return 5;
		}
		// --------------------------------------------------------------------
		// �u�U�[�ύX��A�C�R����������
		// --------------------------------------------------------------------
		else if	((g_PressX >= 230) && (g_PressX <= (230 + 32 -1 )) && (g_PressY >= ( 24 * 3)) && (g_PressY <= ( 24 * 3 + 24 - 1)))
		{
			return 6;
		}
		// --------------------------------------------------------------------
		// �u�U�[�ύX���A�C�R����������
		// --------------------------------------------------------------------
		else if	((g_PressX >= 270) && (g_PressX <= (270 + 32 - 1)) && (g_PressY >= ( 24 * 3)) && (g_PressY <= ( 24 * 3 + 24 - 1)))
		{
			return 7;
		}
		// --------------------------------------------------------------------
		// �}�X�^�[���[�j���O�ύX��A�C�R����������
		// --------------------------------------------------------------------
		else if	((g_PressX >= 230) && (g_PressX <= (230 + 32 -1 )) && (g_PressY >= ( 24 * 4)) && (g_PressY <= ( 24 * 4 + 24 - 1)))
		{
			return 8;
		}
		// --------------------------------------------------------------------
		// �}�X�^�[���[�j���O�ύX���A�C�R����������
		// --------------------------------------------------------------------
		else if	((g_PressX >= 270) && (g_PressX <= (270 + 32 - 1)) && (g_PressY >= ( 24 * 4)) && (g_PressY <= ( 24 * 4 + 24 - 1)))
		{
			return 9;
		}
		// --------------------------------------------------------------------
		// �v���Z�b�g�ύX��A�C�R����������
		// --------------------------------------------------------------------
		else if	((g_PressX >= 230) && (g_PressX <= (230 + 32 -1 )) && (g_PressY >= ( 24 * 6)) && (g_PressY <= ( 24 * 6 + 24 - 1)))
		{
			return 10;
		}
		// --------------------------------------------------------------------
		// �v���Z�b�g�ύX���A�C�R����������
		// --------------------------------------------------------------------
		else if	((g_PressX >= 270) && (g_PressX <= (270 + 32 - 1)) && (g_PressY >= ( 24 * 6)) && (g_PressY <= ( 24 * 6 + 24 - 1)))
		{
			return 11;
		}
	}
	return 0;
}

unsigned int E110(void)
{
	// --------------------------------------------------------------------
	// ���[�J���ϐ��錾
	// --------------------------------------------------------------------
	static unsigned int drag_cnt;
	
	// --------------------------------------------------------------------
	// �^�b�`���쏈��
	// --------------------------------------------------------------------
	if(touch_done_flg)			// �^�b�`�p�l���������肠��
	{
		touch_done_flg = 0;		// �t���O�N���A
		
		// --------------------------------------------------------------------
		// �����R�}���h�A�C�R����������
		// --------------------------------------------------------------------
		if		((g_PressX >= 130) && (g_PressX <= 193) && (g_PressY >= 215) && (g_PressY <= 238))
		{
			return 1;
		}
		// --------------------------------------------------------------------
		// ���R�}���h�A�C�R����������
		// --------------------------------------------------------------------
		else if	((g_PressX >=  46) && (g_PressX <= 139) && (g_PressY >= 215) && (g_PressY <= 238))
		{
			return 2;
		}
		// --------------------------------------------------------------------
		// �E�R�}���h�A�C�R����������
		// --------------------------------------------------------------------
		else if	((g_PressX >= 214) && (g_PressX <= 277) && (g_PressY >= 215) && (g_PressY <= 238))
		{
			return 3;
		}
	}
	if(touch_drag_flg)			// �^�b�`�p�l���������肠��
	{
		touch_drag_flg = 0;		// �t���O�N���A
		
		// --------------------------------------------------------------------
		// ���l�\��1����������
		// --------------------------------------------------------------------
		if		((g_DragX >=   1) && (g_DragX <= 140) && (g_DragY >= 144) && (g_DragY <= 167))
		{
			drag_cnt ++;
			if(drag_cnt > 20)
			{
				Beep_TwoShotMin();
				num_page = 1;
				return 4;
			}
			else
			{
				//
			}
		}
		// --------------------------------------------------------------------
		// ���l�\��2����������
		// --------------------------------------------------------------------
		else if	((g_DragX >=   1) && (g_DragX <= 140) && (g_DragY >= 168) && (g_DragY <= 191))
		{
			drag_cnt ++;
			if(drag_cnt > 20)
			{
				Beep_TwoShotMin();
				num_page = 2;
				return 4;
			}
			else
			{
				//
			}
		}
		// --------------------------------------------------------------------
		// ���l�\��3����������
		// --------------------------------------------------------------------
		else if	((g_DragX >=   1) && (g_DragX <= 140) && (g_DragY >= 192) && (g_DragY <= 215))
		{
			drag_cnt ++;
			if(drag_cnt > 20)
			{
				Beep_TwoShotMin();
				num_page = 3;
				return 4;
			}
			else
			{
				//
			}
		}
		// --------------------------------------------------------------------
		// ���l�\��4����������
		// --------------------------------------------------------------------
		else if	((g_DragX >= 161) && (g_DragX <= 300) && (g_DragY >= 144) && (g_DragY <= 167))
		{
			drag_cnt ++;
			if(drag_cnt > 20)
			{
				Beep_TwoShotMin();
				num_page = 4;
				return 4;
			}
			else
			{
				//
			}
		}
		// --------------------------------------------------------------------
		// ���l�\��5����������
		// --------------------------------------------------------------------
		else if	((g_DragX >= 161) && (g_DragX <= 300) && (g_DragY >= 168) && (g_DragY <= 191))
		{
			drag_cnt ++;
			if(drag_cnt > 20)
			{
				Beep_TwoShotMin();
				num_page = 5;
				return 4;
			}
			else
			{
				//
			}
		}
		// --------------------------------------------------------------------
		// ���l�\��6����������
		// --------------------------------------------------------------------
		else if	((g_DragX >= 161) && (g_DragX <= 300) && (g_DragY >= 192) && (g_DragY <= 215))
		{
			drag_cnt ++;
			if(drag_cnt > 20)
			{
				Beep_TwoShotMin();
				num_page = 6;
				return 4;
			}
			else
			{
				//
			}
		}
		// --------------------------------------------------------------------
		// ��]���\������������
		// --------------------------------------------------------------------
		else if	((g_DragX >= 194) && (g_DragX <= 309) && (g_DragY >=  72) && (g_DragY <=  95))
		{
			drag_cnt ++;
			if(drag_cnt > 20)
			{
				Beep_TwoShotMin();
				num_page = 7;
				return 4;
			}
			else
			{
				//
			}
		}
		// --------------------------------------------------------------------
		// ��R��\������������
		// --------------------------------------------------------------------
		else if	((g_DragX >=  92) && (g_DragX <= 219) && (g_DragY >=  96) && (g_DragY <= 143))
		{
			drag_cnt ++;
			if(drag_cnt > 20)
			{
				Beep_TwoShotMin();
				num_page = 8;
				return 4;
			}
			else
			{
				//
			}
		}
		else
		{
			drag_cnt = 0;
		}
	}
	else
	{
		drag_cnt = 0;
	}
	return 0;
}

unsigned int E100(void)
{
	// --------------------------------------------------------------------
	// �^�b�`���쏈��
	// --------------------------------------------------------------------
	if(touch_done_flg)			// �^�b�`�p�l���������肠��
	{
		touch_done_flg = 0;		// �t���O�N���A
		
		// --------------------------------------------------------------------
		// �����R�}���h�A�C�R����������
		// --------------------------------------------------------------------
		if		((g_PressX >= 130) && (g_PressX <= 193) && (g_PressY >= 215) && (g_PressY <= 238))
		{
			return 1;
		}
		// --------------------------------------------------------------------
		// ���R�}���h�A�C�R����������
		// --------------------------------------------------------------------
		else if	((g_PressX >=  46) && (g_PressX <= 139) && (g_PressY >= 215) && (g_PressY <= 238))
		{
			return 2;
		}
		// --------------------------------------------------------------------
		// �E�R�}���h�A�C�R����������
		// --------------------------------------------------------------------
		else if	((g_PressX >= 214) && (g_PressX <= 277) && (g_PressY >= 215) && (g_PressY <= 238))
		{
			return 3;
		}
	}
	return 0;
}

unsigned int E200(void)
{
	// --------------------------------------------------------------------
	// ���[�J���ϐ��錾
	// --------------------------------------------------------------------
	static unsigned int drag_cnt;
	
	// --------------------------------------------------------------------
	// �^�b�`���쏈��
	// --------------------------------------------------------------------
	if(touch_done_flg | touch_drag_flg)			// �^�b�`�p�l���������肠��
	{
		// --------------------------------------------------------------------
		// �ۑ��A�C�R����������
		// --------------------------------------------------------------------
		if((touch_done_flg) && ((g_PressX >= 130) && (g_PressX <= 193) && (g_PressY >= 215) && (g_PressY <= 238)))
		{
			touch_done_flg = 0;		// �t���O�N���A
			return 1;
		}
		// --------------------------------------------------------------------
		// �f�[�^�ύX��A�C�R����������
		// --------------------------------------------------------------------
		else if	(((touch_done_flg) || (touch_drag_flg)) && ((g_PressX >= 230) && (g_PressX <= (230 + 32 - 1)) && (g_PressY >= ( 24 * 1)) && (g_PressY <= ( 24 * 1 + 24 - 1))))
		{
			if(touch_done_flg)
			{
				touch_done_flg = 0;		// �t���O�N���A
				num_control = 1;
				return 2;
			}
			else if(touch_drag_flg)
			{
				touch_drag_flg = 0;		// �t���O�N���A
				drag_cnt ++;
				if(drag_cnt > 5)
				{
					drag_cnt = 4;
					num_control = 1;
					return 2;
				}
			}
		}
		// --------------------------------------------------------------------
		// �f�[�^�ύX���A�C�R����������
		// --------------------------------------------------------------------
		else if	(((touch_done_flg) || (touch_drag_flg)) && ((g_PressX >= 270) && (g_PressX <= (270 + 32 - 1)) && (g_PressY >= ( 24 * 1)) && (g_PressY <= ( 24 * 1 + 24 - 1))))
		{
			if(touch_done_flg)
			{
				touch_done_flg = 0;		// �t���O�N���A
				num_control = 2;
				return 2;
			}
			else if(touch_drag_flg)
			{
				touch_drag_flg = 0;		// �t���O�N���A
				drag_cnt ++;
				if(drag_cnt > 5)
				{
					drag_cnt = 4;
					num_control = 2;
					return 2;
				}
			}
		}
		// --------------------------------------------------------------------
		// �Q�C���ύX��A�C�R����������
		// --------------------------------------------------------------------
		else if	(((touch_done_flg) || (touch_drag_flg)) && ((g_PressX >= 230) && (g_PressX <= (230 + 32 - 1)) && (g_PressY >= ( 24 * 3)) && (g_PressY <= ( 24 * 3 + 24 - 1))))
		{
			if(touch_done_flg)
			{
				touch_done_flg = 0;		// �t���O�N���A
				num_control = 3;
				return 2;
			}
			else if(touch_drag_flg)
			{
				touch_drag_flg = 0;		// �t���O�N���A
				drag_cnt ++;
				if(drag_cnt > 5)
				{
					drag_cnt = 4;
					num_control = 3;
					return 2;
				}
			}
		}
		// --------------------------------------------------------------------
		// �Q�C���ύX���A�C�R����������
		// --------------------------------------------------------------------
		else if	(((touch_done_flg) || (touch_drag_flg)) && ((g_PressX >= 270) && (g_PressX <= (270 + 32 - 1)) && (g_PressY >= ( 24 * 3)) && (g_PressY <= ( 24 * 3 + 24 - 1))))
		{
			if(touch_done_flg)
			{
				touch_done_flg = 0;		// �t���O�N���A
				num_control = 4;
				return 2;
			}
			else if(touch_drag_flg)
			{
				touch_drag_flg = 0;		// �t���O�N���A
				drag_cnt ++;
				if(drag_cnt > 5)
				{
					drag_cnt = 4;
					num_control = 4;
					return 2;
				}
			}
		}
		// --------------------------------------------------------------------
		// �o�C�A�X�ύX��A�C�R����������
		// --------------------------------------------------------------------
		else if	(((touch_done_flg) || (touch_drag_flg)) && ((g_PressX >= 230) && (g_PressX <= (230 + 32 - 1)) && (g_PressY >= ( 24 * 4)) && (g_PressY <= ( 24 * 4 + 24 - 1))))
		{
			if(touch_done_flg)
			{
				touch_done_flg = 0;		// �t���O�N���A
				num_control = 5;
				return 2;
			}
			else if(touch_drag_flg)
			{
				touch_drag_flg = 0;		// �t���O�N���A
				drag_cnt ++;
				if(drag_cnt > 5)
				{
					drag_cnt = 4;
					num_control = 5;
					return 2;
				}
			}	
		}
		// --------------------------------------------------------------------
		// �o�C�A�X�ύX���A�C�R����������
		// --------------------------------------------------------------------
		else if	(((touch_done_flg) || (touch_drag_flg)) && ((g_PressX >= 270) && (g_PressX <= (270 + 32 - 1)) && (g_PressY >= ( 24 * 4)) && (g_PressY <= ( 24 * 4 + 24 - 1))))
		{
			if(touch_done_flg)
			{
				touch_done_flg = 0;		// �t���O�N���A
				num_control = 6;
				return 2;
			}
			else if(touch_drag_flg)
			{
				touch_drag_flg = 0;		// �t���O�N���A
				drag_cnt ++;
				if(drag_cnt > 5)
				{
					drag_cnt = 4;
					num_control = 6;
					return 2;
				}
			}
		}
		// --------------------------------------------------------------------
		// �����_�ʒu�ύX��A�C�R����������
		// --------------------------------------------------------------------
		else if	(((touch_done_flg) || (touch_drag_flg)) && ((g_PressX >= 230) && (g_PressX <= (230 + 32 - 1)) && (g_PressY >= ( 24 * 5)) && (g_PressY <= ( 24 * 5 + 24 - 1))))
		{
			if(touch_done_flg)
			{
				touch_done_flg = 0;		// �t���O�N���A
				num_control = 7;
				return 2;
			}
			else if(touch_drag_flg)
			{
				touch_drag_flg = 0;		// �t���O�N���A
				drag_cnt ++;
				if(drag_cnt > 5)
				{
					drag_cnt = 4;
					num_control = 7;
					return 2;
				}
			}
		}
		// --------------------------------------------------------------------
		// �����_�ʒu�ύX���A�C�R����������
		// --------------------------------------------------------------------
		else if	(((touch_done_flg) || (touch_drag_flg)) && ((g_PressX >= 270) && (g_PressX <= (270 + 32 - 1)) && (g_PressY >= ( 24 * 5)) && (g_PressY <= ( 24 * 5 + 24 - 1))))
		{
			if(touch_done_flg)
			{
				touch_done_flg = 0;		// �t���O�N���A
				num_control = 8;
				return 2;
			}
			else if(touch_drag_flg)
			{
				touch_drag_flg = 0;		// �t���O�N���A
				drag_cnt ++;
				if(drag_cnt > 5)
				{
					drag_cnt = 4;
					num_control = 8;
					return 2;
				}
			}
		}
		// --------------------------------------------------------------------
		// �^�O�ύX��A�C�R����������
		// --------------------------------------------------------------------
		else if	(((touch_done_flg) || (touch_drag_flg)) && ((g_PressX >= 230) && (g_PressX <= (230 + 32 - 1)) && (g_PressY >= ( 24 * 6)) && (g_PressY <= ( 24 * 6 + 24 - 1))))
		{
			if(touch_done_flg)
			{
				touch_done_flg = 0;		// �t���O�N���A
				num_control = 9;
				return 2;
			}
			else if(touch_drag_flg)
			{
				touch_drag_flg = 0;		// �t���O�N���A
				drag_cnt ++;
				if(drag_cnt > 5)
				{
					drag_cnt = 4;
					num_control = 9;
					return 2;
				}
			}
		}
		// --------------------------------------------------------------------
		// �^�O�ύX���A�C�R����������
		// --------------------------------------------------------------------
		else if	(((touch_done_flg) || (touch_drag_flg)) && ((g_PressX >= 270) && (g_PressX <= (270 + 32 - 1)) && (g_PressY >= ( 24 * 6)) && (g_PressY <= ( 24 * 6 + 24 - 1))))
		{
			if(touch_done_flg)
			{
				touch_done_flg = 0;		// �t���O�N���A
				num_control = 10;
				return 2;
			}
			else if(touch_drag_flg)
			{
				touch_drag_flg = 0;		// �t���O�N���A
				drag_cnt ++;
				if(drag_cnt > 5)
				{
					drag_cnt = 4;
					num_control = 10;
					return 2;
				}
			}
		}
		// --------------------------------------------------------------------
		// �P�ʕύX��A�C�R����������
		// --------------------------------------------------------------------
		else if	(((touch_done_flg) || (touch_drag_flg)) && ((g_PressX >= 230) && (g_PressX <= (230 + 32 - 1)) && (g_PressY >= ( 24 * 7)) && (g_PressY <= ( 24 * 7 + 24 - 1))))
		{
			if(touch_done_flg)
			{
				touch_done_flg = 0;		// �t���O�N���A
				num_control = 11;
				return 2;
			}
			else if(touch_drag_flg)
			{
				touch_drag_flg = 0;		// �t���O�N���A
				drag_cnt ++;
				if(drag_cnt > 5)
				{
					drag_cnt = 4;
					num_control = 11;
					return 2;
				}
			}
		}
		// --------------------------------------------------------------------
		// �P�ʕύX���A�C�R����������
		// --------------------------------------------------------------------
		else if	(((touch_done_flg) || (touch_drag_flg)) && ((g_PressX >= 270) && (g_PressX <= (270 + 32 - 1)) && (g_PressY >= ( 24 * 7)) && (g_PressY <= ( 24 * 7 + 24 - 1))))
		{
			if(touch_done_flg)
			{
				touch_done_flg = 0;		// �t���O�N���A
				num_control = 12;
				return 2;
			}
			else if(touch_drag_flg)
			{
				touch_drag_flg = 0;		// �t���O�N���A
				drag_cnt ++;
				if(drag_cnt > 5)
				{
					drag_cnt = 4;
					num_control = 12;
					return 2;
				}
			}
		}
		// --------------------------------------------------------------------
		// ���~�b�g�ύX��A�C�R����������
		// --------------------------------------------------------------------
		else if	(((touch_done_flg) || (touch_drag_flg)) && ((g_PressX >= 230) && (g_PressX <= (230 + 32 - 1)) && (g_PressY >= ( 24 * 8)) && (g_PressY <= ( 24 * 8 + 24 - 1))))
		{
			if(touch_done_flg)
			{
				touch_done_flg = 0;		// �t���O�N���A
				num_control = 13;
				return 2;
			}
			else if(touch_drag_flg)
			{
				touch_drag_flg = 0;		// �t���O�N���A
				drag_cnt ++;
				if(drag_cnt > 5)
				{
					drag_cnt = 4;
					num_control = 13;
					return 2;
				}
			}
		}
		// --------------------------------------------------------------------
		// ���~�b�g�ύX���A�C�R����������
		// --------------------------------------------------------------------
		else if	(((touch_done_flg) || (touch_drag_flg)) && ((g_PressX >= 270) && (g_PressX <= (270 + 32 - 1)) && (g_PressY >= ( 24 * 8)) && (g_PressY <= ( 24 * 8 + 24 - 1))))
		{
			if(touch_done_flg)
			{
				touch_done_flg = 0;		// �t���O�N���A
				num_control = 14;
				return 2;
			}
			else if(touch_drag_flg)
			{
				touch_drag_flg = 0;		// �t���O�N���A
				drag_cnt ++;
				if(drag_cnt > 5)
				{
					drag_cnt = 4;
					num_control = 14;
					return 2;
				}
			}
		}
		else
		{
			touch_done_flg = 0;
			touch_drag_flg = 0;
			drag_cnt = 0;
		}
	}
	else
	{
		drag_cnt = 0;
		Acc = 1;
	}
	return 0;
}

unsigned int E990(void)
{
	// --------------------------------------------------------------------
	// �^�b�`���쏈��
	// --------------------------------------------------------------------
	if(touch_done_flg)			// �^�b�`�p�l���������肠��
	{
		touch_done_flg = 0;		// �t���O�N���A
		
		// --------------------------------------------------------------------
		// �ۑ��A�C�R����������
		// --------------------------------------------------------------------
		if		((g_PressX >= 130) && (g_PressX <= 193) && (g_PressY >= 215) && (g_PressY <= 238))
		{
			return 1;
		}
	}
	return 0;
}

// --------------------------------------------------------------------
// �A�N�V���������F�X�e�[�g�J�n��1�x�̂ݎ��s
// --------------------------------------------------------------------
// --------------------------------------------------------------------
// ��������������ʕ`��
// --------------------------------------------------------------------
void A001(void)
{
	// --------------------------------------------------------------------
	// ��ԑJ�ڏ���
	// --------------------------------------------------------------------
	g_state = S001;
//	g_int50mscnt = -100;		// �s���I�h50ms x 100 = 5.0s �ҋ@
	g_int50mscnt =  -60;		// �s���I�h50ms x  60 = 3.0s �ҋ@
//	g_int50mscnt =  -20;		// �s���I�h50ms x  20 = 1.0s �ҋ@
//	g_int50mscnt =  -10;		// �s���I�h50ms x  10 = 0.5s �ҋ@
	
	// --------------------------------------------------------------------
	// LCD���t���b�V���ϐ�������
	// --------------------------------------------------------------------
	g_drawbuff_flg = 1;
	g_refresh_done = 1;
	
	// --------------------------------------------------------------------
	// �I�[�v�j���O��ʕ`��
	// --------------------------------------------------------------------
	LCD_CLR(COLOR_WHITE);
	LCD_locate(  0,  0);	LCD_Gcopy(  0,   0, 320,240, (volatile unsigned int    *)TITLE);	// roadster���S
	
	LCD_Refresh();
	
	// --------------------------------------------------------------------
	// �^�C�}�X�^�[�g
	// --------------------------------------------------------------------
	TPU.TSTR.BIT.CST0 = 1;		// �^�C�}�X�^�[�g���W�X�^	Start TPU0 for TFT LCD Refresh Cycle
	TPU.TSTR.BIT.CST3 = 1;		// �^�C�}�X�^�[�g���W�X�^	Start TPU3 for BEEP
	TPU.TSTR.BIT.CST4 = 1;		// �^�C�}�X�^�[�g���W�X�^	Start TPU4 for TFT LCD Back Light PWM
	TPU.TSTRB.BIT.CST0 = 1;		// �^�C�}�X�^�[�g���W�X�^	Start TPU0 for LED Dimmer Cycle
	
	while(g_oneshot_flg <= 1);	// 1��ʕ`�ʊ����܂őҋ@
	
	// --------------------------------------------------------------------
	// �o�b�N���C�g�_��
	// --------------------------------------------------------------------
	LCD_FadeIN();
}

// --------------------------------------------------------------------
// ��ʏ������N����
// --------------------------------------------------------------------
void A002(void)
{
	// --------------------------------------------------------------------
	// ��ԑJ�ڏ���
	// --------------------------------------------------------------------
	g_state = S002;
	g_int50mscnt =   -2;		// �s���I�h50ms x   2 = 0.1s �ҋ@
	
	// --------------------------------------------------------------------
	// ��ʃN���A&�؂�ւ�
	// --------------------------------------------------------------------
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	
	// --------------------------------------------------------------------
	// EEPROM�f�[�^�ǂݍ���
	// --------------------------------------------------------------------
	mrandrd_start((const unsigned char *)&g_e2p_data, 0x00, sizeof(g_e2p_data));
	
	// --------------------------------------------------------------------
	// �N����
	// --------------------------------------------------------------------
	Beep_TwoShotMin();
}

// --------------------------------------------------------------------
// ������
// --------------------------------------------------------------------
void A100(void)
{
	// --------------------------------------------------------------------
	// ���[�J���ϐ��錾
	// --------------------------------------------------------------------
	unsigned int i;
	
	// --------------------------------------------------------------------
	// ��ԑJ�ڏ���
	// --------------------------------------------------------------------
	g_state = S110;
	
	// --------------------------------------------------------------------
	// CAN������
	// --------------------------------------------------------------------
	switch(g_e2p_data.E2P_1.model)
	{
		case MoTeC1:	Init_MoTeC1();		break;
		case Haltech1:	Init_Haltech1();	break;
		case Haltech2:	Init_Haltech2();	break;
	}
	
	// --------------------------------------------------------------------
	// �s�[�N�z�[���h�l�������X�V�N���A�t���O���Z�b�g
	// --------------------------------------------------------------------
	Onetime_Peakclear_done = 0;
	Onetime_Peakclear_cnt = 10;
	
	// --------------------------------------------------------------------
	// �`���[�g�o�b�t�@�N���A
	// --------------------------------------------------------------------
	for(i = 0; i < ChartBufNum; i++ )
	{
		chart_buf1[i] = 1742;
		chart_buf2[i] =    0;
	}
}

// --------------------------------------------------------------------
// �ݒ�ۑ�����
// --------------------------------------------------------------------
void A011(void)
{
	// --------------------------------------------------------------------
	// �ۑ��A�C�R���^�b�`�`��
	// --------------------------------------------------------------------
	LCD_locate(130,215);	LCD_Gcopy(256, 120, 64, 24, (volatile unsigned int    *)FONTR);
	LCD_Refresh();
	LCD_locate(130,215);	LCD_Gcopy(256, 120, 64, 24, (volatile unsigned int    *)FONTR);
	LCD_Refresh();
	
	// --------------------------------------------------------------------
	// �f�[�^�v���Z�b�g�ۑ�
	// --------------------------------------------------------------------
	switch(preset_no)
	{
		case 1:	Preset_load_MoTeC1();		break;
		case 2:	Preset_load_MoTeC2();		break;
		case 3:	Preset_load_Haltech1();		break;
		case 4:	Preset_load_Haltech2();		break;
		case 5:	Preset_load_Freedom1();		break;
		case 6:	Preset_load_Freedom2();		break;
		case 7:	Preset_load_MegaSquirt1();	break;
		case 0:
		default:						break;
	}
	
	// --------------------------------------------------------------------
	// EEPROM�f�[�^�ۑ�
	// --------------------------------------------------------------------
	mtrs_start((const unsigned char *)&g_e2p_data + 0x00, 0x00, 16);	// E2P_1
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x10, 0x10, 16);	// E2P_2
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x20, 0x20, 16);	// E2P_3
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x30, 0x30, 16);	// E2P_4
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x40, 0x40, 16);	// E2P_5
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x50, 0x50, 16);	// E2P_6
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x60, 0x60, 16);	// E2P_7
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x70, 0x70, 16);	// E2P_8
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A012(void)
{
	// --------------------------------------------------------------------
	// ���[�J���ϐ��錾
	// --------------------------------------------------------------------
	unsigned int i;
	
	// --------------------------------------------------------------------
	// ��ԑJ�ڏ���
	// --------------------------------------------------------------------
	g_state = S130;
	
	// --------------------------------------------------------------------
	// CAN������
	// --------------------------------------------------------------------
	switch(g_e2p_data.E2P_1.model)
	{
		case MoTeC1:	Init_MoTeC1();		break;
		case Haltech1:	Init_Haltech1();	break;
		case Haltech2:	Init_Haltech2();	break;
	}
	
	// --------------------------------------------------------------------
	// ��ʃN���A&�؂�ւ�
	// --------------------------------------------------------------------
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	
	// --------------------------------------------------------------------
	// �`���[�g�o�b�t�@�N���A
	// --------------------------------------------------------------------
	if(chart_onoff == 0)
	{
		for(i = 0; i < ChartBufNum; i++ )
		{
			chart_buf1[i] = 1742;
			chart_buf2[i] =    0;
		}
	}
	else
	{
		//
	}
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A013(void)
{
	// --------------------------------------------------------------------
	// ��ԑJ�ڏ���
	// --------------------------------------------------------------------
	g_state = S110;
	
	// --------------------------------------------------------------------
	// CAN������
	// --------------------------------------------------------------------
	switch(g_e2p_data.E2P_1.model)
	{
		case MoTeC1:	Init_MoTeC1();		break;
		case Haltech1:	Init_Haltech1();	break;
		case Haltech2:	Init_Haltech2();	break;
	}
	
	// --------------------------------------------------------------------
	// ��ʃN���A&�؂�ւ�
	// --------------------------------------------------------------------
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A014(void)
{
	g_e2p_data.E2P_1.model --;
	g_e2p_data.E2P_1.model = ( g_e2p_data.E2P_1.model >= 255 ) ? 0 : g_e2p_data.E2P_1.model;
	
	LCD_locate(230,24 * 2);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	LCD_locate(230,24 * 2);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	
	// --------------------------------------------------------------------
	// 2�x�����L�����Z��
	// --------------------------------------------------------------------
	touch_done_flg = 0;
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A015(void)
{
	g_e2p_data.E2P_1.model ++;
	g_e2p_data.E2P_1.model = (g_e2p_data.E2P_1.model > 6) ? 6 : g_e2p_data.E2P_1.model;
	
	LCD_locate(270,24 * 2);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	LCD_locate(270,24 * 2);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	
	// --------------------------------------------------------------------
	// 2�x�����L�����Z��
	// --------------------------------------------------------------------
	touch_done_flg = 0;
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A016(void)
{
	if(g_e2p_data.E2P_1.control.BIT.beep_on)
	{
		g_e2p_data.E2P_1.control.BIT.beep_on = 0;
	}
	else
	{
		g_e2p_data.E2P_1.control.BIT.beep_on = 1;
	}
	LCD_locate(230,24 * 3);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	LCD_locate(230,24 * 3);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	
	// --------------------------------------------------------------------
	// 2�x�����L�����Z��
	// --------------------------------------------------------------------
	touch_done_flg = 0;
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A017(void)
{
	if(g_e2p_data.E2P_1.control.BIT.beep_on)
	{
		g_e2p_data.E2P_1.control.BIT.beep_on = 0;
	}
	else
	{
		g_e2p_data.E2P_1.control.BIT.beep_on = 1;
	}
	LCD_locate(270,24 * 3);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	LCD_locate(270,24 * 3);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	
	// --------------------------------------------------------------------
	// 2�x�����L�����Z��
	// --------------------------------------------------------------------
	touch_done_flg = 0;
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A018(void)
{
	if(g_e2p_data.E2P_1.control.BIT.ms_on)
	{
		g_e2p_data.E2P_1.control.BIT.ms_on = 0;
	}
	else
	{
		g_e2p_data.E2P_1.control.BIT.ms_on = 1;
	}
	LCD_locate(230,24 * 4);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	LCD_locate(230,24 * 4);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	
	// --------------------------------------------------------------------
	// 2�x�����L�����Z��
	// --------------------------------------------------------------------
	touch_done_flg = 0;
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A019(void)
{
	if(g_e2p_data.E2P_1.control.BIT.ms_on)
	{
		g_e2p_data.E2P_1.control.BIT.ms_on = 0;
	}
	else
	{
		g_e2p_data.E2P_1.control.BIT.ms_on = 1;
	}
	LCD_locate(270,24 * 4);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	LCD_locate(270,24 * 4);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	
	// --------------------------------------------------------------------
	// 2�x�����L�����Z��
	// --------------------------------------------------------------------
	touch_done_flg = 0;
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A020(void)
{
	preset_no --;
	preset_no = ( preset_no >= 255 ) ? 0 : preset_no;
	
	LCD_locate(230,24 * 6);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	LCD_locate(230,24 * 6);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	
	// --------------------------------------------------------------------
	// 2�x�����L�����Z��
	// --------------------------------------------------------------------
	touch_done_flg = 0;
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A021(void)
{
	preset_no ++;
	preset_no = (preset_no > 7) ? 7 : preset_no;
	
	LCD_locate(270,24 * 6);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	LCD_locate(270,24 * 6);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
	LCD_Refresh();
	
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	
	// --------------------------------------------------------------------
	// 2�x�����L�����Z��
	// --------------------------------------------------------------------
	touch_done_flg = 0;
}

// --------------------------------------------------------------------
// �o�b�N���C�g�f�B�}�[�g�O������
// --------------------------------------------------------------------
void A101(void)
{
	if(backlight_dimmer_flg)
	{
		TPU4.TGRB = 0x0010;
		backlight_dimmer_flg = 0;
	}
	else
	{
		TPU4.TGRB = 0x2000;
		backlight_dimmer_flg = 1;
	}
	// --------------------------------------------------------------------
	// �o�b�N���C�g�f�B�}�[�A�C�R���`��
	// --------------------------------------------------------------------
	if(backlight_dimmer_flg)
	{
		LCD_locate(130,215);	LCD_Gcopy(256,  24, 64, 24, (volatile unsigned int    *)FONTR);
	}
	else
	{
		LCD_locate(130,215);	LCD_Gcopy(256,   0, 64, 24, (volatile unsigned int    *)FONTR);
	}
	LCD_Refresh();
	if(backlight_dimmer_flg)
	{
		LCD_locate(130,215);	LCD_Gcopy(256,  24, 64, 24, (volatile unsigned int    *)FONTR);
	}
	else
	{
		LCD_locate(130,215);	LCD_Gcopy(256,   0, 64, 24, (volatile unsigned int    *)FONTR);
	}
	LCD_Refresh();
	
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
}

// --------------------------------------------------------------------
// �s�[�N�z�[���h�N���A����
// --------------------------------------------------------------------
void A121(void)
{
	// --------------------------------------------------------------------
	// �s�[�N�z�[���h�N���A�A�C�R���^�b�`�`��
	// --------------------------------------------------------------------
	LCD_locate(130,215);	LCD_Gcopy(256,  96, 64, 24, (volatile unsigned int    *)FONTR);
	LCD_Refresh();
	LCD_locate(130,215);	LCD_Gcopy(256,  96, 64, 24, (volatile unsigned int    *)FONTR);
	LCD_Refresh();
	
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	
	rev_peak  = 0;
	num1_peak = 0;
	num2_peak = 0;
	num3_peak = 0;
	num4_peak = 0;
	num5_peak = 0;
	num6_peak = 0;
}

// --------------------------------------------------------------------
// �`���[�g�N���A����
// --------------------------------------------------------------------
void A131(void)
{
	// --------------------------------------------------------------------
	// ���[�J���ϐ��錾
	// --------------------------------------------------------------------
	unsigned int i;
	
	if(chart_onoff == 0)
	{
		chart_onoff = 1;
		
		// --------------------------------------------------------------------
		// �`���[�g��~�A�C�R���^�b�`�`��
		// --------------------------------------------------------------------
		LCD_locate(130,215);	LCD_Gcopy(256, 168, 64, 24, (volatile unsigned int    *)FONTR);	// ||
		LCD_Refresh();
		LCD_locate(130,215);	LCD_Gcopy(256, 168, 64, 24, (volatile unsigned int    *)FONTR);	// ||
		LCD_Refresh();
	}
	else
	{
		chart_onoff = 0;
		
		// --------------------------------------------------------------------
		// �`���[�g�Đ��A�C�R���^�b�`�`��
		// --------------------------------------------------------------------
		LCD_locate(130,215);	LCD_Gcopy(256,  72, 64, 24, (volatile unsigned int    *)FONTR);	// �E
		LCD_Refresh();
		LCD_locate(130,215);	LCD_Gcopy(256,  72, 64, 24, (volatile unsigned int    *)FONTR);	// �E
		LCD_Refresh();
		
		// --------------------------------------------------------------------
		// �`���[�g�o�b�t�@�N���A
		// --------------------------------------------------------------------
		for(i = 0; i < ChartBufNum; i++ )
		{
			chart_buf1[i] = 1742;
			chart_buf2[i] =    0;
		}
	}
	
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A102(void)
{
	// --------------------------------------------------------------------
	// ��ԑJ�ڏ���
	// --------------------------------------------------------------------
	g_state = S010;
	
	// --------------------------------------------------------------------
	// �v���Z�b�g�ύX������
	// --------------------------------------------------------------------
	preset_no = 0;
	
	// --------------------------------------------------------------------
	// ��ʃN���A&�؂�ւ�
	// --------------------------------------------------------------------
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A103(void)
{
	// --------------------------------------------------------------------
	// ��ԑJ�ڏ���
	// --------------------------------------------------------------------
	g_state = S120;
	
	// --------------------------------------------------------------------
	// ��ʃN���A&�؂�ւ�
	// --------------------------------------------------------------------
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A104(void)
{
	// --------------------------------------------------------------------
	// ��ԑJ�ڏ���
	// --------------------------------------------------------------------
	g_state = S210;
	
	// --------------------------------------------------------------------
	// ��ʃN���A&�؂�ւ�
	// --------------------------------------------------------------------
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A133(void)
{
	// --------------------------------------------------------------------
	// ���[�J���ϐ��錾
	// --------------------------------------------------------------------
	unsigned int i;
	
	// --------------------------------------------------------------------
	// ��ԑJ�ڏ���
	// --------------------------------------------------------------------
	g_state = S130;
	
	// --------------------------------------------------------------------
	// ��ʃN���A&�؂�ւ�
	// --------------------------------------------------------------------
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	
	// --------------------------------------------------------------------
	// �`���[�g�o�b�t�@�N���A
	// --------------------------------------------------------------------
	if(chart_onoff == 0)
	{
		for(i = 0; i < ChartBufNum; i++ )
		{
			chart_buf1[i] = 1742;
			chart_buf2[i] =    0;
		}
	}
	else
	{
		//
	}
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A122(void)
{
	// --------------------------------------------------------------------
	// ��ԑJ�ڏ���
	// --------------------------------------------------------------------
	g_state = S110;
	
	// --------------------------------------------------------------------
	// ��ʃN���A&�؂�ւ�
	// --------------------------------------------------------------------
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A132(void)
{
	// --------------------------------------------------------------------
	// ��ԑJ�ڏ���
	// --------------------------------------------------------------------
	g_state = S120;
	
	// --------------------------------------------------------------------
	// ��ʃN���A&�؂�ւ�
	// --------------------------------------------------------------------
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A123(void)
{
	// --------------------------------------------------------------------
	// ��ԑJ�ڏ���
	// --------------------------------------------------------------------
	g_state = S010;
	
	// --------------------------------------------------------------------
	// �v���Z�b�g�ύX������
	// --------------------------------------------------------------------
	preset_no = 0;
	
	// --------------------------------------------------------------------
	// ��ʃN���A&�؂�ւ�
	// --------------------------------------------------------------------
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
}

// --------------------------------------------------------------------
// ���l�ݒ�ۑ�����
// --------------------------------------------------------------------
void A201(void)
{
	// --------------------------------------------------------------------
	// ��ԑJ�ڏ���
	// --------------------------------------------------------------------
	g_state = S110;
	
	// --------------------------------------------------------------------
	// �ۑ��A�C�R���^�b�`�`��
	// --------------------------------------------------------------------
	LCD_locate(130,215);	LCD_Gcopy(256, 120, 64, 24, (volatile unsigned int    *)FONTR);
	LCD_Refresh();
	LCD_locate(130,215);	LCD_Gcopy(256, 120, 64, 24, (volatile unsigned int    *)FONTR);
	LCD_Refresh();
	
	// --------------------------------------------------------------------
	// EEPROM�f�[�^�ۑ�
	// --------------------------------------------------------------------
	mtrs_start((const unsigned char *)&g_e2p_data + 0x10, 0x10, 16);	// E2P_2
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x20, 0x20, 16);	// E2P_3
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x30, 0x30, 16);	// E2P_4
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x40, 0x40, 16);	// E2P_5
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x50, 0x50, 16);	// E2P_6
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x60, 0x60, 16);	// E2P_7
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x70, 0x70, 16);	// E2P_8
	
	// --------------------------------------------------------------------
	// ��ʃN���A&�؂�ւ�
	// --------------------------------------------------------------------
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	
	// --------------------------------------------------------------------
	// 2�x�����L�����Z��
	// --------------------------------------------------------------------
	touch_done_flg = 0;
	touch_drag_flg = 0;
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void A202(void)
{
	if		(num_control == 1)
	{
		switch(num_page)
		{
			case 1:	g_e2p_data.E2P_4.num1_data_select -= Acc; g_e2p_data.E2P_4.num1_data_select = ( g_e2p_data.E2P_4.num1_data_select >= 255 ) ? 0 : g_e2p_data.E2P_4.num1_data_select; break;
			case 2:	g_e2p_data.E2P_4.num2_data_select -= Acc; g_e2p_data.E2P_4.num2_data_select = ( g_e2p_data.E2P_4.num2_data_select >= 255 ) ? 0 : g_e2p_data.E2P_4.num2_data_select; break;
			case 3:	g_e2p_data.E2P_5.num3_data_select -= Acc; g_e2p_data.E2P_5.num3_data_select = ( g_e2p_data.E2P_5.num3_data_select >= 255 ) ? 0 : g_e2p_data.E2P_5.num3_data_select; break;
			case 4:	g_e2p_data.E2P_5.num4_data_select -= Acc; g_e2p_data.E2P_5.num4_data_select = ( g_e2p_data.E2P_5.num4_data_select >= 255 ) ? 0 : g_e2p_data.E2P_5.num4_data_select; break;
			case 5:	g_e2p_data.E2P_6.num5_data_select -= Acc; g_e2p_data.E2P_6.num5_data_select = ( g_e2p_data.E2P_6.num5_data_select >= 255 ) ? 0 : g_e2p_data.E2P_6.num5_data_select; break;
			case 6:	g_e2p_data.E2P_6.num6_data_select -= Acc; g_e2p_data.E2P_6.num6_data_select = ( g_e2p_data.E2P_6.num6_data_select >= 255 ) ? 0 : g_e2p_data.E2P_6.num6_data_select; break;
			case 7:	g_e2p_data.E2P_3.rev_data_select  -= Acc; g_e2p_data.E2P_3.rev_data_select  = ( g_e2p_data.E2P_3.rev_data_select  >= 255 ) ? 0 : g_e2p_data.E2P_3.rev_data_select;  break;
			case 8:	g_e2p_data.E2P_3.afr_data_select  -= Acc; g_e2p_data.E2P_3.afr_data_select  = ( g_e2p_data.E2P_3.afr_data_select  >= 255 ) ? 0 : g_e2p_data.E2P_3.afr_data_select;  break;
		}
		LCD_locate(230,24 * 1);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
		LCD_locate(230,24 * 1);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
	}
	else if	(num_control == 2)
	{
		switch(num_page)
		{
			case 1:	g_e2p_data.E2P_4.num1_data_select += Acc; g_e2p_data.E2P_4.num1_data_select = (g_e2p_data.E2P_4.num1_data_select > num_data_select_value) ? num_data_select_value : g_e2p_data.E2P_4.num1_data_select; break;
			case 2:	g_e2p_data.E2P_4.num2_data_select += Acc; g_e2p_data.E2P_4.num2_data_select = (g_e2p_data.E2P_4.num2_data_select > num_data_select_value) ? num_data_select_value : g_e2p_data.E2P_4.num2_data_select; break;
			case 3:	g_e2p_data.E2P_5.num3_data_select += Acc; g_e2p_data.E2P_5.num3_data_select = (g_e2p_data.E2P_5.num3_data_select > num_data_select_value) ? num_data_select_value : g_e2p_data.E2P_5.num3_data_select; break;
			case 4:	g_e2p_data.E2P_5.num4_data_select += Acc; g_e2p_data.E2P_5.num4_data_select = (g_e2p_data.E2P_5.num4_data_select > num_data_select_value) ? num_data_select_value : g_e2p_data.E2P_5.num4_data_select; break;
			case 5:	g_e2p_data.E2P_6.num5_data_select += Acc; g_e2p_data.E2P_6.num5_data_select = (g_e2p_data.E2P_6.num5_data_select > num_data_select_value) ? num_data_select_value : g_e2p_data.E2P_6.num5_data_select; break;
			case 6:	g_e2p_data.E2P_6.num6_data_select += Acc; g_e2p_data.E2P_6.num6_data_select = (g_e2p_data.E2P_6.num6_data_select > num_data_select_value) ? num_data_select_value : g_e2p_data.E2P_6.num6_data_select; break;
			case 7:	g_e2p_data.E2P_3.rev_data_select  += Acc; g_e2p_data.E2P_3.rev_data_select  = (g_e2p_data.E2P_3.rev_data_select  > rev_data_select_value) ? rev_data_select_value : g_e2p_data.E2P_3.rev_data_select;  break;
			case 8:	g_e2p_data.E2P_3.afr_data_select  += Acc; g_e2p_data.E2P_3.afr_data_select  = (g_e2p_data.E2P_3.afr_data_select  > afr_data_select_value) ? afr_data_select_value : g_e2p_data.E2P_3.afr_data_select;  break;
		}
		LCD_locate(270,24 * 1);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
		LCD_locate(270,24 * 1);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
	}
	else if	(num_control == 3)
	{
		switch(num_page)
		{
			case 1:	g_e2p_data.E2P_4.num1_gain += Acc; Acc ++; g_e2p_data.E2P_4.num1_gain = (g_e2p_data.E2P_4.num1_gain > 10000) ? 10000 : g_e2p_data.E2P_4.num1_gain; break;
			case 2:	g_e2p_data.E2P_4.num2_gain += Acc; Acc ++; g_e2p_data.E2P_4.num2_gain = (g_e2p_data.E2P_4.num2_gain > 10000) ? 10000 : g_e2p_data.E2P_4.num2_gain; break;
			case 3:	g_e2p_data.E2P_5.num3_gain += Acc; Acc ++; g_e2p_data.E2P_5.num3_gain = (g_e2p_data.E2P_5.num3_gain > 10000) ? 10000 : g_e2p_data.E2P_5.num3_gain; break;
			case 4:	g_e2p_data.E2P_5.num4_gain += Acc; Acc ++; g_e2p_data.E2P_5.num4_gain = (g_e2p_data.E2P_5.num4_gain > 10000) ? 10000 : g_e2p_data.E2P_5.num4_gain; break;
			case 5:	g_e2p_data.E2P_6.num5_gain += Acc; Acc ++; g_e2p_data.E2P_6.num5_gain = (g_e2p_data.E2P_6.num5_gain > 10000) ? 10000 : g_e2p_data.E2P_6.num5_gain; break;
			case 6:	g_e2p_data.E2P_6.num6_gain += Acc; Acc ++; g_e2p_data.E2P_6.num6_gain = (g_e2p_data.E2P_6.num6_gain > 10000) ? 10000 : g_e2p_data.E2P_6.num6_gain; break;
			case 7:	g_e2p_data.E2P_3.rev_gain  += Acc; Acc ++; g_e2p_data.E2P_3.rev_gain  = (g_e2p_data.E2P_3.rev_gain  > 10000) ? 10000 : g_e2p_data.E2P_3.rev_gain;  break;
			case 8:	g_e2p_data.E2P_3.afr_gain  += Acc; Acc ++; g_e2p_data.E2P_3.afr_gain  = (g_e2p_data.E2P_3.afr_gain  > 10000) ? 10000 : g_e2p_data.E2P_3.afr_gain;  break;
		}
		LCD_locate(230,24 * 3);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
		LCD_locate(230,24 * 3);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
	}
	else if	(num_control == 4)
	{
		switch(num_page)
		{
			case 1:	g_e2p_data.E2P_4.num1_gain -= Acc; Acc ++; g_e2p_data.E2P_4.num1_gain = (g_e2p_data.E2P_4.num1_gain < -10000) ? -10000 : g_e2p_data.E2P_4.num1_gain; break;
			case 2:	g_e2p_data.E2P_4.num2_gain -= Acc; Acc ++; g_e2p_data.E2P_4.num2_gain = (g_e2p_data.E2P_4.num2_gain < -10000) ? -10000 : g_e2p_data.E2P_4.num2_gain; break;
			case 3:	g_e2p_data.E2P_5.num3_gain -= Acc; Acc ++; g_e2p_data.E2P_5.num3_gain = (g_e2p_data.E2P_5.num3_gain < -10000) ? -10000 : g_e2p_data.E2P_5.num3_gain; break;
			case 4:	g_e2p_data.E2P_5.num4_gain -= Acc; Acc ++; g_e2p_data.E2P_5.num4_gain = (g_e2p_data.E2P_5.num4_gain < -10000) ? -10000 : g_e2p_data.E2P_5.num4_gain; break;
			case 5:	g_e2p_data.E2P_6.num5_gain -= Acc; Acc ++; g_e2p_data.E2P_6.num5_gain = (g_e2p_data.E2P_6.num5_gain < -10000) ? -10000 : g_e2p_data.E2P_6.num5_gain; break;
			case 6:	g_e2p_data.E2P_6.num6_gain -= Acc; Acc ++; g_e2p_data.E2P_6.num6_gain = (g_e2p_data.E2P_6.num6_gain < -10000) ? -10000 : g_e2p_data.E2P_6.num6_gain; break;
			case 7:	g_e2p_data.E2P_3.rev_gain  -= Acc; Acc ++; g_e2p_data.E2P_3.rev_gain  = (g_e2p_data.E2P_3.rev_gain  < -10000) ? -10000 : g_e2p_data.E2P_3.rev_gain;  break;
			case 8:	g_e2p_data.E2P_3.afr_gain  -= Acc; Acc ++; g_e2p_data.E2P_3.afr_gain  = (g_e2p_data.E2P_3.afr_gain  < -10000) ? -10000 : g_e2p_data.E2P_3.afr_gain;  break;
		}
		LCD_locate(270,24 * 3);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
		LCD_locate(270,24 * 3);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
	}
	else if	(num_control == 5)
	{
		switch(num_page)
		{
			case 1:	g_e2p_data.E2P_4.num1_bias += Acc; Acc ++; g_e2p_data.E2P_4.num1_bias = (g_e2p_data.E2P_4.num1_bias > 10000) ? 10000 : g_e2p_data.E2P_4.num1_bias; break;
			case 2:	g_e2p_data.E2P_4.num2_bias += Acc; Acc ++; g_e2p_data.E2P_4.num2_bias = (g_e2p_data.E2P_4.num2_bias > 10000) ? 10000 : g_e2p_data.E2P_4.num2_bias; break;
			case 3:	g_e2p_data.E2P_5.num3_bias += Acc; Acc ++; g_e2p_data.E2P_5.num3_bias = (g_e2p_data.E2P_5.num3_bias > 10000) ? 10000 : g_e2p_data.E2P_5.num3_bias; break;
			case 4:	g_e2p_data.E2P_5.num4_bias += Acc; Acc ++; g_e2p_data.E2P_5.num4_bias = (g_e2p_data.E2P_5.num4_bias > 10000) ? 10000 : g_e2p_data.E2P_5.num4_bias; break;
			case 5:	g_e2p_data.E2P_6.num5_bias += Acc; Acc ++; g_e2p_data.E2P_6.num5_bias = (g_e2p_data.E2P_6.num5_bias > 10000) ? 10000 : g_e2p_data.E2P_6.num5_bias; break;
			case 6:	g_e2p_data.E2P_6.num6_bias += Acc; Acc ++; g_e2p_data.E2P_6.num6_bias = (g_e2p_data.E2P_6.num6_bias > 10000) ? 10000 : g_e2p_data.E2P_6.num6_bias; break;
			case 7:	g_e2p_data.E2P_3.rev_bias  += Acc; Acc ++; g_e2p_data.E2P_3.rev_bias  = (g_e2p_data.E2P_3.rev_bias  > 10000) ? 10000 : g_e2p_data.E2P_3.rev_bias;  break;
			case 8:	g_e2p_data.E2P_3.afr_bias  += Acc; Acc ++; g_e2p_data.E2P_3.afr_bias  = (g_e2p_data.E2P_3.afr_bias  > 10000) ? 10000 : g_e2p_data.E2P_3.afr_bias;  break;
		}
		LCD_locate(230,24 * 4);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
		LCD_locate(230,24 * 4);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
	}
	else if	(num_control == 6)
	{
		switch(num_page)
		{
			case 1:	g_e2p_data.E2P_4.num1_bias -= Acc; Acc ++; g_e2p_data.E2P_4.num1_bias = (g_e2p_data.E2P_4.num1_bias < -10000) ? -10000 : g_e2p_data.E2P_4.num1_bias; break;
			case 2:	g_e2p_data.E2P_4.num2_bias -= Acc; Acc ++; g_e2p_data.E2P_4.num2_bias = (g_e2p_data.E2P_4.num2_bias < -10000) ? -10000 : g_e2p_data.E2P_4.num2_bias; break;
			case 3:	g_e2p_data.E2P_5.num3_bias -= Acc; Acc ++; g_e2p_data.E2P_5.num3_bias = (g_e2p_data.E2P_5.num3_bias < -10000) ? -10000 : g_e2p_data.E2P_5.num3_bias; break;
			case 4:	g_e2p_data.E2P_5.num4_bias -= Acc; Acc ++; g_e2p_data.E2P_5.num4_bias = (g_e2p_data.E2P_5.num4_bias < -10000) ? -10000 : g_e2p_data.E2P_5.num4_bias; break;
			case 5:	g_e2p_data.E2P_6.num5_bias -= Acc; Acc ++; g_e2p_data.E2P_6.num5_bias = (g_e2p_data.E2P_6.num5_bias < -10000) ? -10000 : g_e2p_data.E2P_6.num5_bias; break;
			case 6:	g_e2p_data.E2P_6.num6_bias -= Acc; Acc ++; g_e2p_data.E2P_6.num6_bias = (g_e2p_data.E2P_6.num6_bias < -10000) ? -10000 : g_e2p_data.E2P_6.num6_bias; break;
			case 7:	g_e2p_data.E2P_3.rev_bias  -= Acc; Acc ++; g_e2p_data.E2P_3.rev_bias  = (g_e2p_data.E2P_3.rev_bias  < -10000) ? -10000 : g_e2p_data.E2P_3.rev_bias;  break;
			case 8:	g_e2p_data.E2P_3.afr_bias  -= Acc; Acc ++; g_e2p_data.E2P_3.afr_bias  = (g_e2p_data.E2P_3.afr_bias  < -10000) ? -10000 : g_e2p_data.E2P_3.afr_bias;  break;
		}
		LCD_locate(270,24 * 4);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
		LCD_locate(270,24 * 4);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
	}
	else if	(num_control == 7)
	{
		switch(num_page)
		{
			case 1:	g_e2p_data.E2P_4.num1_dp += Acc; g_e2p_data.E2P_4.num1_dp = (g_e2p_data.E2P_4.num1_dp > num_dp_value) ? num_dp_value : g_e2p_data.E2P_4.num1_dp; break;
			case 2:	g_e2p_data.E2P_4.num2_dp += Acc; g_e2p_data.E2P_4.num2_dp = (g_e2p_data.E2P_4.num2_dp > num_dp_value) ? num_dp_value : g_e2p_data.E2P_4.num2_dp; break;
			case 3:	g_e2p_data.E2P_5.num3_dp += Acc; g_e2p_data.E2P_5.num3_dp = (g_e2p_data.E2P_5.num3_dp > num_dp_value) ? num_dp_value : g_e2p_data.E2P_5.num3_dp; break;
			case 4:	g_e2p_data.E2P_5.num4_dp += Acc; g_e2p_data.E2P_5.num4_dp = (g_e2p_data.E2P_5.num4_dp > num_dp_value) ? num_dp_value : g_e2p_data.E2P_5.num4_dp; break;
			case 5:	g_e2p_data.E2P_6.num5_dp += Acc; g_e2p_data.E2P_6.num5_dp = (g_e2p_data.E2P_6.num5_dp > num_dp_value) ? num_dp_value : g_e2p_data.E2P_6.num5_dp; break;
			case 6:	g_e2p_data.E2P_6.num6_dp += Acc; g_e2p_data.E2P_6.num6_dp = (g_e2p_data.E2P_6.num6_dp > num_dp_value) ? num_dp_value : g_e2p_data.E2P_6.num6_dp; break;
			case 7:	g_e2p_data.E2P_2.rev_timing_rmp1 += Acc; Acc ++; g_e2p_data.E2P_2.rev_timing_rmp1 = (g_e2p_data.E2P_2.rev_timing_rmp1 > (g_e2p_data.E2P_2.rev_timing_rmp2 - 1)) ? (g_e2p_data.E2P_2.rev_timing_rmp2 - 1) : g_e2p_data.E2P_2.rev_timing_rmp1; break;
			case 8:	g_e2p_data.E2P_3.afr_dp  += Acc; g_e2p_data.E2P_3.afr_dp  = (g_e2p_data.E2P_3.afr_dp  > num_dp_value) ? num_dp_value : g_e2p_data.E2P_3.afr_dp;  break;
		}
		LCD_locate(230,24 * 5);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
		LCD_locate(230,24 * 5);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
	}
	else if	(num_control == 8)
	{
		switch(num_page)
		{
			case 1:	g_e2p_data.E2P_4.num1_dp -= Acc; g_e2p_data.E2P_4.num1_dp = (g_e2p_data.E2P_4.num1_dp >= 255) ? 0 : g_e2p_data.E2P_4.num1_dp; break;
			case 2:	g_e2p_data.E2P_4.num2_dp -= Acc; g_e2p_data.E2P_4.num2_dp = (g_e2p_data.E2P_4.num2_dp >= 255) ? 0 : g_e2p_data.E2P_4.num2_dp; break;
			case 3:	g_e2p_data.E2P_5.num3_dp -= Acc; g_e2p_data.E2P_5.num3_dp = (g_e2p_data.E2P_5.num3_dp >= 255) ? 0 : g_e2p_data.E2P_5.num3_dp; break;
			case 4:	g_e2p_data.E2P_5.num4_dp -= Acc; g_e2p_data.E2P_5.num4_dp = (g_e2p_data.E2P_5.num4_dp >= 255) ? 0 : g_e2p_data.E2P_5.num4_dp; break;
			case 5:	g_e2p_data.E2P_6.num5_dp -= Acc; g_e2p_data.E2P_6.num5_dp = (g_e2p_data.E2P_6.num5_dp >= 255) ? 0 : g_e2p_data.E2P_6.num5_dp; break;
			case 6:	g_e2p_data.E2P_6.num6_dp -= Acc; g_e2p_data.E2P_6.num6_dp = (g_e2p_data.E2P_6.num6_dp >= 255) ? 0 : g_e2p_data.E2P_6.num6_dp; break;
			case 7:	g_e2p_data.E2P_2.rev_timing_rmp1 -= Acc; Acc ++; g_e2p_data.E2P_2.rev_timing_rmp1 = (g_e2p_data.E2P_2.rev_timing_rmp1 < 2000) ? 2000 : g_e2p_data.E2P_2.rev_timing_rmp1; break;
			case 8:	g_e2p_data.E2P_3.afr_dp  -= Acc; g_e2p_data.E2P_3.afr_dp  = (g_e2p_data.E2P_3.afr_dp  >= 255) ? 0 : g_e2p_data.E2P_3.afr_dp;  break;
		}
		LCD_locate(270,24 * 5);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
		LCD_locate(270,24 * 5);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
	}
	else if	(num_control == 9)
	{
		switch(num_page)
		{
			case 1:	g_e2p_data.E2P_4.num1_label -= Acc; g_e2p_data.E2P_4.num1_label = (g_e2p_data.E2P_4.num1_label >= 255) ? 0 : g_e2p_data.E2P_4.num1_label; break;
			case 2:	g_e2p_data.E2P_4.num2_label -= Acc; g_e2p_data.E2P_4.num2_label = (g_e2p_data.E2P_4.num2_label >= 255) ? 0 : g_e2p_data.E2P_4.num2_label; break;
			case 3:	g_e2p_data.E2P_5.num3_label -= Acc; g_e2p_data.E2P_5.num3_label = (g_e2p_data.E2P_5.num3_label >= 255) ? 0 : g_e2p_data.E2P_5.num3_label; break;
			case 4:	g_e2p_data.E2P_5.num4_label -= Acc; g_e2p_data.E2P_5.num4_label = (g_e2p_data.E2P_5.num4_label >= 255) ? 0 : g_e2p_data.E2P_5.num4_label; break;
			case 5:	g_e2p_data.E2P_6.num5_label -= Acc; g_e2p_data.E2P_6.num5_label = (g_e2p_data.E2P_6.num5_label >= 255) ? 0 : g_e2p_data.E2P_6.num5_label; break;
			case 6:	g_e2p_data.E2P_6.num6_label -= Acc; g_e2p_data.E2P_6.num6_label = (g_e2p_data.E2P_6.num6_label >= 255) ? 0 : g_e2p_data.E2P_6.num6_label; break;
			case 7:	g_e2p_data.E2P_2.rev_timing_rmp2 += Acc; Acc ++; g_e2p_data.E2P_2.rev_timing_rmp2 = (g_e2p_data.E2P_2.rev_timing_rmp2 > (g_e2p_data.E2P_2.rev_timing_rmp3 - 1)) ? (g_e2p_data.E2P_2.rev_timing_rmp3 - 1) : g_e2p_data.E2P_2.rev_timing_rmp2; break;
			case 8:	g_e2p_data.E2P_3.afr_label  -= Acc; g_e2p_data.E2P_3.afr_label  = (g_e2p_data.E2P_3.afr_label  >= 255) ? 0 : g_e2p_data.E2P_3.afr_label;  break;
		}
		LCD_locate(230,24 * 6);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
		LCD_locate(230,24 * 6);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
	}
	else if	(num_control == 10)
	{
		switch(num_page)
		{
			case 1:	g_e2p_data.E2P_4.num1_label += Acc; g_e2p_data.E2P_4.num1_label = (g_e2p_data.E2P_4.num1_label > num_label_value) ? num_label_value : g_e2p_data.E2P_4.num1_label; break;
			case 2:	g_e2p_data.E2P_4.num2_label += Acc; g_e2p_data.E2P_4.num2_label = (g_e2p_data.E2P_4.num2_label > num_label_value) ? num_label_value : g_e2p_data.E2P_4.num2_label; break;
			case 3:	g_e2p_data.E2P_5.num3_label += Acc; g_e2p_data.E2P_5.num3_label = (g_e2p_data.E2P_5.num3_label > num_label_value) ? num_label_value : g_e2p_data.E2P_5.num3_label; break;
			case 4:	g_e2p_data.E2P_5.num4_label += Acc; g_e2p_data.E2P_5.num4_label = (g_e2p_data.E2P_5.num4_label > num_label_value) ? num_label_value : g_e2p_data.E2P_5.num4_label; break;
			case 5:	g_e2p_data.E2P_6.num5_label += Acc; g_e2p_data.E2P_6.num5_label = (g_e2p_data.E2P_6.num5_label > num_label_value) ? num_label_value : g_e2p_data.E2P_6.num5_label; break;
			case 6:	g_e2p_data.E2P_6.num6_label += Acc; g_e2p_data.E2P_6.num6_label = (g_e2p_data.E2P_6.num6_label > num_label_value) ? num_label_value : g_e2p_data.E2P_6.num6_label; break;
			case 7:	g_e2p_data.E2P_2.rev_timing_rmp2 -= Acc; Acc ++; g_e2p_data.E2P_2.rev_timing_rmp2 = (g_e2p_data.E2P_2.rev_timing_rmp2 < (g_e2p_data.E2P_2.rev_timing_rmp1 + 1)) ? (g_e2p_data.E2P_2.rev_timing_rmp1 + 1) : g_e2p_data.E2P_2.rev_timing_rmp2; break;
			case 8:	g_e2p_data.E2P_3.afr_label  += Acc; g_e2p_data.E2P_3.afr_label  = (g_e2p_data.E2P_3.afr_label  > afr_label_value) ? afr_label_value : g_e2p_data.E2P_3.afr_label;  break;
		}
		LCD_locate(270,24 * 6);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
		LCD_locate(270,24 * 6);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
		LCD_RefreshFast();
	}
	else if	(num_control == 11)
	{
		switch(num_page)
		{
			case 1:	g_e2p_data.E2P_4.num1_unit -= Acc; g_e2p_data.E2P_4.num1_unit = (g_e2p_data.E2P_4.num1_unit >= 255) ? 0 : g_e2p_data.E2P_4.num1_unit; break;
			case 2:	g_e2p_data.E2P_4.num2_unit -= Acc; g_e2p_data.E2P_4.num2_unit = (g_e2p_data.E2P_4.num2_unit >= 255) ? 0 : g_e2p_data.E2P_4.num2_unit; break;
			case 3:	g_e2p_data.E2P_5.num3_unit -= Acc; g_e2p_data.E2P_5.num3_unit = (g_e2p_data.E2P_5.num3_unit >= 255) ? 0 : g_e2p_data.E2P_5.num3_unit; break;
			case 4:	g_e2p_data.E2P_5.num4_unit -= Acc; g_e2p_data.E2P_5.num4_unit = (g_e2p_data.E2P_5.num4_unit >= 255) ? 0 : g_e2p_data.E2P_5.num4_unit; break;
			case 5:	g_e2p_data.E2P_6.num5_unit -= Acc; g_e2p_data.E2P_6.num5_unit = (g_e2p_data.E2P_6.num5_unit >= 255) ? 0 : g_e2p_data.E2P_6.num5_unit; break;
			case 7:	g_e2p_data.E2P_2.rev_timing_rmp3 += Acc; Acc ++; g_e2p_data.E2P_2.rev_timing_rmp3 = (g_e2p_data.E2P_2.rev_timing_rmp3 > 20000) ? 20000 : g_e2p_data.E2P_2.rev_timing_rmp3; break;
			case 6:	g_e2p_data.E2P_6.num6_unit -= Acc; g_e2p_data.E2P_6.num6_unit = (g_e2p_data.E2P_6.num6_unit >= 255) ? 0 : g_e2p_data.E2P_6.num6_unit; break;
		}
		if(num_page != 8)
		{
			LCD_locate(230,24 * 7);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
			LCD_RefreshFast();
			LCD_locate(230,24 * 7);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
			LCD_RefreshFast();
		}
	}
	else if	(num_control == 12)
	{
		switch(num_page)
		{
			case 1:	g_e2p_data.E2P_4.num1_unit += Acc; g_e2p_data.E2P_4.num1_unit = (g_e2p_data.E2P_4.num1_unit > num_unit_value) ? num_unit_value : g_e2p_data.E2P_4.num1_unit; break;
			case 2:	g_e2p_data.E2P_4.num2_unit += Acc; g_e2p_data.E2P_4.num2_unit = (g_e2p_data.E2P_4.num2_unit > num_unit_value) ? num_unit_value : g_e2p_data.E2P_4.num2_unit; break;
			case 3:	g_e2p_data.E2P_5.num3_unit += Acc; g_e2p_data.E2P_5.num3_unit = (g_e2p_data.E2P_5.num3_unit > num_unit_value) ? num_unit_value : g_e2p_data.E2P_5.num3_unit; break;
			case 4:	g_e2p_data.E2P_5.num4_unit += Acc; g_e2p_data.E2P_5.num4_unit = (g_e2p_data.E2P_5.num4_unit > num_unit_value) ? num_unit_value : g_e2p_data.E2P_5.num4_unit; break;
			case 5:	g_e2p_data.E2P_6.num5_unit += Acc; g_e2p_data.E2P_6.num5_unit = (g_e2p_data.E2P_6.num5_unit > num_unit_value) ? num_unit_value : g_e2p_data.E2P_6.num5_unit; break;
			case 7:	g_e2p_data.E2P_2.rev_timing_rmp3 -= Acc; Acc ++; g_e2p_data.E2P_2.rev_timing_rmp3 = (g_e2p_data.E2P_2.rev_timing_rmp3 < (g_e2p_data.E2P_2.rev_timing_rmp2 + 1)) ? (g_e2p_data.E2P_2.rev_timing_rmp2 + 1) : g_e2p_data.E2P_2.rev_timing_rmp3; break;
			case 6:	g_e2p_data.E2P_6.num6_unit += Acc; g_e2p_data.E2P_6.num6_unit = (g_e2p_data.E2P_6.num6_unit > num_unit_value) ? num_unit_value : g_e2p_data.E2P_6.num6_unit; break;
		}
		if(num_page != 8)
		{
			LCD_locate(270,24 * 7);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
			LCD_RefreshFast();
			LCD_locate(270,24 * 7);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
			LCD_RefreshFast();
		}
	}
	else if	(num_control == 13)
	{
		switch(num_page)
		{
			case 1:	g_e2p_data.E2P_7.num1_warning += Acc; Acc ++; g_e2p_data.E2P_7.num1_warning = (g_e2p_data.E2P_7.num1_warning > 30000) ? 30000 : g_e2p_data.E2P_7.num1_warning; break;
			case 2:	g_e2p_data.E2P_7.num2_warning += Acc; Acc ++; g_e2p_data.E2P_7.num2_warning = (g_e2p_data.E2P_7.num2_warning > 30000) ? 30000 : g_e2p_data.E2P_7.num2_warning; break;
			case 3:	g_e2p_data.E2P_7.num3_warning += Acc; Acc ++; g_e2p_data.E2P_7.num3_warning = (g_e2p_data.E2P_7.num3_warning > 30000) ? 30000 : g_e2p_data.E2P_7.num3_warning; break;
			case 4:	g_e2p_data.E2P_7.num4_warning += Acc; Acc ++; g_e2p_data.E2P_7.num4_warning = (g_e2p_data.E2P_7.num4_warning > 30000) ? 30000 : g_e2p_data.E2P_7.num4_warning; break;
			case 5:	g_e2p_data.E2P_7.num5_warning += Acc; Acc ++; g_e2p_data.E2P_7.num5_warning = (g_e2p_data.E2P_7.num5_warning > 30000) ? 30000 : g_e2p_data.E2P_7.num5_warning; break;
			case 6:	g_e2p_data.E2P_7.num6_warning += Acc; Acc ++; g_e2p_data.E2P_7.num6_warning = (g_e2p_data.E2P_7.num6_warning > 30000) ? 30000 : g_e2p_data.E2P_7.num6_warning; break;
		}
		if((num_page != 7) && (num_page != 8))
		{
			LCD_locate(230,24 * 8);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
			LCD_RefreshFast();
			LCD_locate(230,24 * 8);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
			LCD_RefreshFast();
		}
	}
	else if	(num_control == 14)
	{
		switch(num_page)
		{
			case 1:	g_e2p_data.E2P_7.num1_warning -= Acc; Acc ++; g_e2p_data.E2P_7.num1_warning = (g_e2p_data.E2P_7.num1_warning < -30000) ? -30000 : g_e2p_data.E2P_7.num1_warning; break;
			case 2:	g_e2p_data.E2P_7.num2_warning -= Acc; Acc ++; g_e2p_data.E2P_7.num2_warning = (g_e2p_data.E2P_7.num2_warning < -30000) ? -30000 : g_e2p_data.E2P_7.num2_warning; break;
			case 3:	g_e2p_data.E2P_7.num3_warning -= Acc; Acc ++; g_e2p_data.E2P_7.num3_warning = (g_e2p_data.E2P_7.num3_warning < -30000) ? -30000 : g_e2p_data.E2P_7.num3_warning; break;
			case 4:	g_e2p_data.E2P_7.num4_warning -= Acc; Acc ++; g_e2p_data.E2P_7.num4_warning = (g_e2p_data.E2P_7.num4_warning < -30000) ? -30000 : g_e2p_data.E2P_7.num4_warning; break;
			case 5:	g_e2p_data.E2P_7.num5_warning -= Acc; Acc ++; g_e2p_data.E2P_7.num5_warning = (g_e2p_data.E2P_7.num5_warning < -30000) ? -30000 : g_e2p_data.E2P_7.num5_warning; break;
			case 6:	g_e2p_data.E2P_7.num6_warning -= Acc; Acc ++; g_e2p_data.E2P_7.num6_warning = (g_e2p_data.E2P_7.num6_warning < -30000) ? -30000 : g_e2p_data.E2P_7.num6_warning; break;
		}
		if((num_page != 7) && (num_page != 8))
		{
			LCD_locate(270,24 * 8);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
			LCD_RefreshFast();
			LCD_locate(270,24 * 8);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONTR);	// ��
			LCD_RefreshFast();
		}
	}
	else
	{
		//
	}
	
	// --------------------------------------------------------------------
	// 2�x�����L�����Z��
	// --------------------------------------------------------------------
	touch_done_flg = 0;
	touch_drag_flg = 0;
}

// --------------------------------------------------------------------
// EEPROM������ԑJ��
// --------------------------------------------------------------------
void A990(void)
{
	// --------------------------------------------------------------------
	// ��ԑJ�ڏ���
	// --------------------------------------------------------------------
	g_state = S999;
	
	// --------------------------------------------------------------------
	// ��ʃN���A&�؂�ւ�
	// --------------------------------------------------------------------
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	LCD_CLR(COLOR_BLACK);
	LCD_Refresh();
	
	touch_done_flg = 0;
	touch_drag_flg = 0;
}

// --------------------------------------------------------------------
// �ݒ�ۑ�����
// --------------------------------------------------------------------
void A991(void)
{
	// --------------------------------------------------------------------
	// �ۑ��A�C�R���^�b�`�`��
	// --------------------------------------------------------------------
	LCD_locate(130,215);	LCD_Gcopy(256, 120, 64, 24, (volatile unsigned int    *)FONTR);
	LCD_locate( 20, 24 * 5 + 6);	LCD_textout("Processing.......");
	LCD_locate( 20, 24 * 6 + 6);	LCD_textout("                ");
	LCD_Refresh();
	LCD_locate(130,215);	LCD_Gcopy(256, 120, 64, 24, (volatile unsigned int    *)FONTR);
	LCD_locate( 20, 24 * 5 + 6);	LCD_textout("Processing.......");
	LCD_locate( 20, 24 * 6 + 6);	LCD_textout("                ");
	LCD_Refresh();
	
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	
	// --------------------------------------------------------------------
	// EEPROM�f�[�^�ۑ�
	// --------------------------------------------------------------------
	Init_e2p_data();						// EEPROM�f�[�^���W�X�^������
	Preset_load_MoTeC1();					// �����f�[�^�Ƃ���MoTeC1�f�[�^��ǂݍ���
	// EEPROM�����f�[�^�f�[�^��������
	mtrs_start((const unsigned char *)&g_e2p_data + 0x00, 0x00, 16);	// E2P_1
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x10, 0x10, 16);	// E2P_2
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x20, 0x20, 16);	// E2P_3
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x30, 0x30, 16);	// E2P_4
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x40, 0x40, 16);	// E2P_5
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x50, 0x50, 16);	// E2P_6
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x60, 0x60, 16);	// E2P_7
	g_int50mscnt =  -4;		// �s���I�h50ms x   6 = 0.2s �ҋ@
	while(g_int50mscnt < 0);
	mtrs_start((const unsigned char *)&g_e2p_data + 0x70, 0x70, 16);	// E2P_8
	
	LCD_locate( 20, 24 * 5 + 6);	LCD_textout("Save is complete.");
	LCD_locate( 20, 24 * 6 + 6);	LCD_textout("Please restart!!");
	LCD_Refresh();
	LCD_locate(130,215);	LCD_Gcopy(256, 120, 64, 24, (volatile unsigned int    *)FONTR);
	LCD_locate( 20, 24 * 5 + 6);	LCD_textout("Save is complete.");
	LCD_locate( 20, 24 * 6 + 6);	LCD_textout("Please restart!!");
	LCD_Refresh();
}

// --------------------------------------------------------------------
// �X�e�[�g��폈�� �X�e�[�g���͏펞���s
// --------------------------------------------------------------------
// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void funcS001(void)
{
	// ��폈���Ȃ�
}

// --------------------------------------------------------------------
// 
// --------------------------------------------------------------------
void funcS002(void)
{
	// ��폈���Ȃ�
}

// --------------------------------------------------------------------
// �ݒ��ʕ`��
// --------------------------------------------------------------------
void funcS010(void)
{
	// --------------------------------------------------------------------
	// ���[�J���ϐ��錾
	// --------------------------------------------------------------------
	
	// --------------------------------------------------------------------
	// �ݒ荀�ڕ`��
	// --------------------------------------------------------------------
	LCD_locate( 20, 24 * 0 + 6);	LCD_textout("<<SETTINGS>>");
	LCD_locate( 20, 24 * 2 + 6);	LCD_textout("Model   ");
	LCD_locate( 20, 24 * 3 + 6);	LCD_textout("Beep    ");
	LCD_locate( 20, 24 * 4 + 6);	LCD_textout("Mater-W ");
	LCD_locate( 20, 24 * 6 + 6);	LCD_textout("Preset  ");
	LCD_locate(250, 24 * 0 + 6);	LCD_textout(version);
	
	switch(g_e2p_data.E2P_1.model)
	{
		case MoTeC1:	LCD_locate(100, 24 * 2 + 6);	LCD_textout("< MoTeC1   >");	break;
		case MoTeC2:	LCD_locate(100, 24 * 2 + 6);	LCD_textout("< MoTeC2   >");	break;
		case Haltech1:	LCD_locate(100, 24 * 2 + 6);	LCD_textout("< Haltech1 >");	break;
		case Haltech2:	LCD_locate(100, 24 * 2 + 6);	LCD_textout("< Haltech2 >");	break;
		case Freedom1:	LCD_locate(100, 24 * 2 + 6);	LCD_textout("< Freedom1 >");	break;
		case Freedom2:	LCD_locate(100, 24 * 2 + 6);	LCD_textout("< Freedom2 >");	break;
		case MSquirt1:	LCD_locate(100, 24 * 2 + 6);	LCD_textout("< MSquirt1 >");	break;
	}
	LCD_locate(230,24 * 2);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
	LCD_locate(270,24 * 2);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
	
	switch(g_e2p_data.E2P_1.control.BIT.beep_on)
	{
		case 0:			LCD_locate(100, 24 * 3 + 6);	LCD_textout("< ON       >");	break;
		case 1:			LCD_locate(100, 24 * 3 + 6);	LCD_textout("< OFF      >");	break;
	}
	LCD_locate(230,24 * 3);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
	LCD_locate(270,24 * 3);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
	
	switch(g_e2p_data.E2P_1.control.BIT.ms_on)
	{
		case 0:			LCD_locate(100, 24 * 4 + 6);	LCD_textout("< ON       >");	break;
		case 1:			LCD_locate(100, 24 * 4 + 6);	LCD_textout("< OFF      >");	break;
	}
	LCD_locate(230,24 * 4);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
	LCD_locate(270,24 * 4);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
	
	switch(preset_no)
	{
		case 0:			LCD_locate(100, 24 * 6 + 6);	LCD_textout("<-nochange->");	break;
		case 1:			LCD_locate(100, 24 * 6 + 6);	LCD_textout("< MoTeC1   >");	break;
		case 2:			LCD_locate(100, 24 * 6 + 6);	LCD_textout("< MoTeC2   >");	break;
		case 3:			LCD_locate(100, 24 * 6 + 6);	LCD_textout("< Haltech1 >");	break;
		case 4:			LCD_locate(100, 24 * 6 + 6);	LCD_textout("< Haltech2 >");	break;
		case 5:			LCD_locate(100, 24 * 6 + 6);	LCD_textout("< Freedom1 >");	break;
		case 6:			LCD_locate(100, 24 * 6 + 6);	LCD_textout("< Freedom2 >");	break;
		case 7:			LCD_locate(100, 24 * 6 + 6);	LCD_textout("< MSquirt1 >");	break;
	}
	LCD_locate(230,24 * 6);	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
	LCD_locate(270,24 * 6);	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
	
	// --------------------------------------------------------------------
	// �ۑ��A�C�R���`��
	// --------------------------------------------------------------------
	LCD_locate(130,215);	LCD_Gcopy(256, 120, 64, 24, (volatile unsigned int    *)FONT);
	
	// --------------------------------------------------------------------
	// �J�[�\���A�C�R���`��
	// --------------------------------------------------------------------
	LCD_locate( 46,215);	LCD_Gcopy(256,  48, 64, 24, (volatile unsigned int    *)FONT);	// ��
	LCD_locate(214,215);	LCD_Gcopy(256,  72, 64, 24, (volatile unsigned int    *)FONT);	// �E
	
	LCD_Refresh();			// �t���[���o�b�t�@�X�V
}

// --------------------------------------------------------------------
// ���`��
// --------------------------------------------------------------------
void funcS110(void)
{
	// --------------------------------------------------------------------
	// ���[�J���ϐ��錾
	// --------------------------------------------------------------------
	unsigned int			RevPoint;
	int						rev, afr, num1, num2, num3, num4, num5, num6;
	
	// --------------------------------------------------------------------
	// ���l�Z�o
	// --------------------------------------------------------------------
	rev  = ((long)(rev_data_select(g_e2p_data.E2P_3.rev_data_select )) + g_e2p_data.E2P_3.rev_bias ) * g_e2p_data.E2P_3.rev_gain  / 1000;
	afr  = ((long)(afr_data_select(g_e2p_data.E2P_3.afr_data_select )) + g_e2p_data.E2P_3.afr_bias ) * g_e2p_data.E2P_3.afr_gain  / 1000;
	num1 = ((long)(num_data_select(g_e2p_data.E2P_4.num1_data_select)) + g_e2p_data.E2P_4.num1_bias) * g_e2p_data.E2P_4.num1_gain / 1000;
	num2 = ((long)(num_data_select(g_e2p_data.E2P_4.num2_data_select)) + g_e2p_data.E2P_4.num2_bias) * g_e2p_data.E2P_4.num2_gain / 1000;
	num3 = ((long)(num_data_select(g_e2p_data.E2P_5.num3_data_select)) + g_e2p_data.E2P_5.num3_bias) * g_e2p_data.E2P_5.num3_gain / 1000;
	num4 = ((long)(num_data_select(g_e2p_data.E2P_5.num4_data_select)) + g_e2p_data.E2P_5.num4_bias) * g_e2p_data.E2P_5.num4_gain / 1000;
	num5 = ((long)(num_data_select(g_e2p_data.E2P_6.num5_data_select)) + g_e2p_data.E2P_6.num5_bias) * g_e2p_data.E2P_6.num5_gain / 1000;
	num6 = ((long)(num_data_select(g_e2p_data.E2P_6.num6_data_select)) + g_e2p_data.E2P_6.num6_bias) * g_e2p_data.E2P_6.num6_gain / 1000;
	// �Z�o�l���~�b�g
	rev  = (rev  >= +32000) ? +32000 : rev ;	rev  = (rev  <= -32000) ? -32000 : rev ;
	afr  = (afr  >= +32000) ? +32000 : afr ;	afr  = (afr  <= -32000) ? -32000 : afr ;
	num1 = (num1 >= +32000) ? +32000 : num1;	num1 = (num1 <= -32000) ? -32000 : num1;
	num2 = (num2 >= +32000) ? +32000 : num2;	num2 = (num2 <= -32000) ? -32000 : num2;
	num3 = (num3 >= +32000) ? +32000 : num3;	num3 = (num3 <= -32000) ? -32000 : num3;
	num4 = (num4 >= +32000) ? +32000 : num4;	num4 = (num4 <= -32000) ? -32000 : num4;
	num5 = (num5 >= +32000) ? +32000 : num5;	num5 = (num5 <= -32000) ? -32000 : num5;
	num6 = (num6 >= +32000) ? +32000 : num6;	num6 = (num6 <= -32000) ? -32000 : num6;
	
	// --------------------------------------------------------------------
	// �s�[�N�z�[���h�l�X�V
	// --------------------------------------------------------------------
	if(rev_peak  < rev  ){rev_peak  = rev;}
	if(num1_peak < num1 ){num1_peak = num1;}
	if(num2_peak < num2 ){num2_peak = num2;}
	if(num3_peak < num3 ){num3_peak = num3;}
	if(num4_peak < num4 ){num4_peak = num4;}
	if(num5_peak < num5 ){num5_peak = num5;}
	if(num6_peak < num6 ){num6_peak = num6;}
	
	// --------------------------------------------------------------------
	// �s�[�N�z�[���h�l�������X�V
	// --------------------------------------------------------------------
	if( (Onetime_Peakclear_done == 0)
	&&	(Onetime_Peakclear_cnt == 0))
	{
		Onetime_Peakclear_done = 1;
		rev_peak  = rev;
		num1_peak = num1;
		num2_peak = num2;
		num3_peak = num3;
		num4_peak = num4;
		num5_peak = num5;
		num6_peak = num6;
	}
	
	// --------------------------------------------------------------------
	// �}�X�^�[���[�j���O����&����
	// --------------------------------------------------------------------
	if(num1 >= g_e2p_data.E2P_7.num1_warning){ if(g_master_warning_flg1 == 0){g_master_warning_flg1 = 1;}} else { if((g_master_warning_flg1 == 2) && (num1 <= (g_e2p_data.E2P_7.num1_warning - 5))){g_master_warning_flg1 = 0;}}
	if(num2 >= g_e2p_data.E2P_7.num2_warning){ if(g_master_warning_flg2 == 0){g_master_warning_flg2 = 1;}} else { if((g_master_warning_flg2 == 2) && (num2 <= (g_e2p_data.E2P_7.num2_warning - 5))){g_master_warning_flg2 = 0;}}
	if(num3 >= g_e2p_data.E2P_7.num3_warning){ if(g_master_warning_flg3 == 0){g_master_warning_flg3 = 1;}} else { if((g_master_warning_flg3 == 2) && (num3 <= (g_e2p_data.E2P_7.num3_warning - 5))){g_master_warning_flg3 = 0;}}
	if(num4 >= g_e2p_data.E2P_7.num4_warning){ if(g_master_warning_flg4 == 0){g_master_warning_flg4 = 1;}} else { if((g_master_warning_flg4 == 2) && (num4 <= (g_e2p_data.E2P_7.num4_warning - 5))){g_master_warning_flg4 = 0;}}
	if(num5 >= g_e2p_data.E2P_7.num5_warning){ if(g_master_warning_flg5 == 0){g_master_warning_flg5 = 1;}} else { if((g_master_warning_flg5 == 2) && (num5 <= (g_e2p_data.E2P_7.num5_warning - 5))){g_master_warning_flg5 = 0;}}
	if(num6 >= g_e2p_data.E2P_7.num6_warning){ if(g_master_warning_flg6 == 0){g_master_warning_flg6 = 1;}} else { if((g_master_warning_flg6 == 2) && (num6 <= (g_e2p_data.E2P_7.num6_warning - 5))){g_master_warning_flg6 = 0;}}
	
	// --------------------------------------------------------------------
	// �V�t�g�^�C�~���O�����v����
	// --------------------------------------------------------------------
	if		 (rev >= g_e2p_data.E2P_2.rev_timing_rmp3)												{ g_shift_timing_flg = 3;	}
	else if	((rev >= g_e2p_data.E2P_2.rev_timing_rmp2) && (rev < g_e2p_data.E2P_2.rev_timing_rmp3))	{ g_shift_timing_flg = 2;	}
	else if	((rev >= g_e2p_data.E2P_2.rev_timing_rmp1) && (rev < g_e2p_data.E2P_2.rev_timing_rmp2))	{ g_shift_timing_flg = 1;	}
	else																							{ g_shift_timing_flg = 0;	}
	
	// --------------------------------------------------------------------
	// ���u�Q�[�W�`��
	// --------------------------------------------------------------------
	RevPoint = rev * 3 / 100;
	LCD_locate( 0				, 10);	LCD_Gcopy( 0				,   0, (RevPoint + 23)			,119, (volatile unsigned int    *)BASE);
	LCD_locate((RevPoint + 23)	, 10);	LCD_Gcopy((RevPoint + 23)	, 120, 320 - (RevPoint + 23)	,119, (volatile unsigned int    *)BASE);
	
	// --------------------------------------------------------------------
	// ���l���[�^�`��
	// --------------------------------------------------------------------
	LCD_locate(178, 24 * 3);	LCD_INT_drawBN (rev , 5, 0);
	LCD_locate( 60, 24 * 4);	LCD_INT_drawBN2(afr	, 4, g_e2p_data.E2P_3.afr_dp );
	LCD_locate( 45, 24 * 6);	if(num1 >= g_e2p_data.E2P_7.num1_warning){ LCD_INT_drawBNR(num1, 5, g_e2p_data.E2P_4.num1_dp);} else {LCD_INT_drawBN (num1, 5, g_e2p_data.E2P_4.num1_dp);}
	LCD_locate( 45, 24 * 7);	if(num2 >= g_e2p_data.E2P_7.num2_warning){ LCD_INT_drawBNR(num2, 5, g_e2p_data.E2P_4.num2_dp);} else {LCD_INT_drawBN (num2, 5, g_e2p_data.E2P_4.num2_dp);}
	LCD_locate( 45, 24 * 8);	if(num3 >= g_e2p_data.E2P_7.num3_warning){ LCD_INT_drawBNR(num3, 5, g_e2p_data.E2P_5.num3_dp);} else {LCD_INT_drawBN (num3, 5, g_e2p_data.E2P_5.num3_dp);}
	LCD_locate(205, 24 * 6);	if(num4 >= g_e2p_data.E2P_7.num4_warning){ LCD_INT_drawBNR(num4, 5, g_e2p_data.E2P_5.num4_dp);} else {LCD_INT_drawBN (num4, 5, g_e2p_data.E2P_5.num4_dp);}
	LCD_locate(205, 24 * 7);	if(num5 >= g_e2p_data.E2P_7.num5_warning){ LCD_INT_drawBNR(num5, 5, g_e2p_data.E2P_6.num5_dp);} else {LCD_INT_drawBN (num5, 5, g_e2p_data.E2P_6.num5_dp);}
	LCD_locate(205, 24 * 8);	if(num6 >= g_e2p_data.E2P_7.num6_warning){ LCD_INT_drawBNR(num6, 5, g_e2p_data.E2P_6.num6_dp);} else {LCD_INT_drawBN (num6, 5, g_e2p_data.E2P_6.num6_dp);}
	
	// --------------------------------------------------------------------
	// �x�[�X�`��
	// --------------------------------------------------------------------
	LCD_locate(280, 24 * 3 + 6);	LCD_textout("RPM");
	LCD_locate( 20, 24 * 5 + 6);	num_label_draw(g_e2p_data.E2P_3.afr_label);
	LCD_locate(  1, 24 * 6 + 6);	num_label_draw(g_e2p_data.E2P_4.num1_label);
	LCD_locate(  1, 24 * 7 + 6);	num_label_draw(g_e2p_data.E2P_4.num2_label);
	LCD_locate(  1, 24 * 8 + 6);	num_label_draw(g_e2p_data.E2P_5.num3_label);
	LCD_locate(161, 24 * 6 + 6);	num_label_draw(g_e2p_data.E2P_5.num4_label);
	LCD_locate(161, 24 * 7 + 6);	num_label_draw(g_e2p_data.E2P_6.num5_label);
	LCD_locate(161, 24 * 8 + 6);	num_label_draw(g_e2p_data.E2P_6.num6_label);
	
	// --------------------------------------------------------------------
	// �P�ʕ`��
	// --------------------------------------------------------------------
	LCD_locate(145, 24 * 6 + 6);	num_unit_draw(g_e2p_data.E2P_4.num1_unit);
	LCD_locate(145, 24 * 7 + 6);	num_unit_draw(g_e2p_data.E2P_4.num2_unit);
	LCD_locate(145, 24 * 8 + 6);	num_unit_draw(g_e2p_data.E2P_5.num3_unit);
	LCD_locate(305, 24 * 6 + 6);	num_unit_draw(g_e2p_data.E2P_5.num4_unit);
	LCD_locate(305, 24 * 7 + 6);	num_unit_draw(g_e2p_data.E2P_6.num5_unit);
	LCD_locate(305, 24 * 8 + 6);	num_unit_draw(g_e2p_data.E2P_6.num6_unit);
	
	// --------------------------------------------------------------------
	// �o�b�N���C�g�f�B�}�[�A�C�R���`��
	// --------------------------------------------------------------------
	if(backlight_dimmer_flg)
	{
		LCD_locate(130,215);	LCD_Gcopy(256,  24, 64, 24, (volatile unsigned int    *)FONT);
	}
	else
	{
		LCD_locate(130,215);	LCD_Gcopy(256,   0, 64, 24, (volatile unsigned int    *)FONT);
	}
	
	// --------------------------------------------------------------------
	// �J�[�\���A�C�R���`��
	// --------------------------------------------------------------------
	LCD_locate( 46,215);	LCD_Gcopy(256,  48, 64, 24, (volatile unsigned int    *)FONT);	// ��
	LCD_locate(214,215);	LCD_Gcopy(256,  72, 64, 24, (volatile unsigned int    *)FONT);	// �E
	
	// --------------------------------------------------------------------
	// FPS�`��
	// --------------------------------------------------------------------
	g_fps ++;
	LCD_locate(  0,  0);	LCD_INT_drawBN (g_fps_max, 3, 0);
	LCD_locate( 64,  6);	LCD_textout("fps");
	
	LCD_Refresh();			// �t���[���o�b�t�@�X�V
}

// --------------------------------------------------------------------
// �s�[�N�z�[���h�`��
// --------------------------------------------------------------------
void funcS120(void)
{
	// --------------------------------------------------------------------
	// ���[�J���ϐ��錾
	// --------------------------------------------------------------------
	unsigned int			RevPoint;
	int						rev, afr, num1, num2, num3, num4, num5, num6;
	
	// --------------------------------------------------------------------
	// ���l�Z�o
	// --------------------------------------------------------------------
	rev  = ((long)(rev_data_select(g_e2p_data.E2P_3.rev_data_select )) + g_e2p_data.E2P_3.rev_bias ) * g_e2p_data.E2P_3.rev_gain  / 1000;
	afr  = ((long)(afr_data_select(g_e2p_data.E2P_3.afr_data_select )) + g_e2p_data.E2P_3.afr_bias ) * g_e2p_data.E2P_3.afr_gain  / 1000;
	num1 = ((long)(num_data_select(g_e2p_data.E2P_4.num1_data_select)) + g_e2p_data.E2P_4.num1_bias) * g_e2p_data.E2P_4.num1_gain / 1000;
	num2 = ((long)(num_data_select(g_e2p_data.E2P_4.num2_data_select)) + g_e2p_data.E2P_4.num2_bias) * g_e2p_data.E2P_4.num2_gain / 1000;
	num3 = ((long)(num_data_select(g_e2p_data.E2P_5.num3_data_select)) + g_e2p_data.E2P_5.num3_bias) * g_e2p_data.E2P_5.num3_gain / 1000;
	num4 = ((long)(num_data_select(g_e2p_data.E2P_5.num4_data_select)) + g_e2p_data.E2P_5.num4_bias) * g_e2p_data.E2P_5.num4_gain / 1000;
	num5 = ((long)(num_data_select(g_e2p_data.E2P_6.num5_data_select)) + g_e2p_data.E2P_6.num5_bias) * g_e2p_data.E2P_6.num5_gain / 1000;
	num6 = ((long)(num_data_select(g_e2p_data.E2P_6.num6_data_select)) + g_e2p_data.E2P_6.num6_bias) * g_e2p_data.E2P_6.num6_gain / 1000;
	// �Z�o�l���~�b�g
	rev  = (rev  >= +32767) ? +32767 : rev ;	rev  = (rev  <= -32767) ? -32767 : rev ;
	afr  = (afr  >= +32767) ? +32767 : afr ;	afr  = (afr  <= -32767) ? -32767 : afr ;
	num1 = (num1 >= +32767) ? +32767 : num1;	num1 = (num1 <= -32767) ? -32767 : num1;
	num2 = (num2 >= +32767) ? +32767 : num2;	num2 = (num2 <= -32767) ? -32767 : num2;
	num3 = (num3 >= +32767) ? +32767 : num3;	num3 = (num3 <= -32767) ? -32767 : num3;
	num4 = (num4 >= +32767) ? +32767 : num4;	num4 = (num4 <= -32767) ? -32767 : num4;
	num5 = (num5 >= +32767) ? +32767 : num5;	num5 = (num5 <= -32767) ? -32767 : num5;
	num6 = (num6 >= +32767) ? +32767 : num6;	num6 = (num6 <= -32767) ? -32767 : num6;
	
	// --------------------------------------------------------------------
	// �s�[�N�z�[���h�l�X�V
	// --------------------------------------------------------------------
	if(rev_peak  < rev  ){rev_peak  = rev;}
	if(num1_peak < num1 ){num1_peak = num1;}
	if(num2_peak < num2 ){num2_peak = num2;}
	if(num3_peak < num3 ){num3_peak = num3;}
	if(num4_peak < num4 ){num4_peak = num4;}
	if(num5_peak < num5 ){num5_peak = num5;}
	if(num6_peak < num6 ){num6_peak = num6;}
	
	// --------------------------------------------------------------------
	// �s�[�N�z�[���h�l�������X�V
	// --------------------------------------------------------------------
	if( (Onetime_Peakclear_done == 0)
	&&	(Onetime_Peakclear_cnt == 0))
	{
		Onetime_Peakclear_done = 1;
		rev_peak  = rev;
		num1_peak = num1;
		num2_peak = num2;
		num3_peak = num3;
		num4_peak = num4;
		num5_peak = num5;
		num6_peak = num6;
	}
	
	// --------------------------------------------------------------------
	// �}�X�^�[���[�j���O����&����
	// --------------------------------------------------------------------
	if(num1 >= g_e2p_data.E2P_7.num1_warning){ if(g_master_warning_flg1 == 0){g_master_warning_flg1 = 1;}} else { if((g_master_warning_flg1 == 2) && (num1 <= (g_e2p_data.E2P_7.num1_warning - 5))){g_master_warning_flg1 = 0;}}
	if(num2 >= g_e2p_data.E2P_7.num2_warning){ if(g_master_warning_flg2 == 0){g_master_warning_flg2 = 1;}} else { if((g_master_warning_flg2 == 2) && (num2 <= (g_e2p_data.E2P_7.num2_warning - 5))){g_master_warning_flg2 = 0;}}
	if(num3 >= g_e2p_data.E2P_7.num3_warning){ if(g_master_warning_flg3 == 0){g_master_warning_flg3 = 1;}} else { if((g_master_warning_flg3 == 2) && (num3 <= (g_e2p_data.E2P_7.num3_warning - 5))){g_master_warning_flg3 = 0;}}
	if(num4 >= g_e2p_data.E2P_7.num4_warning){ if(g_master_warning_flg4 == 0){g_master_warning_flg4 = 1;}} else { if((g_master_warning_flg4 == 2) && (num4 <= (g_e2p_data.E2P_7.num4_warning - 5))){g_master_warning_flg4 = 0;}}
	if(num5 >= g_e2p_data.E2P_7.num5_warning){ if(g_master_warning_flg5 == 0){g_master_warning_flg5 = 1;}} else { if((g_master_warning_flg5 == 2) && (num5 <= (g_e2p_data.E2P_7.num5_warning - 5))){g_master_warning_flg5 = 0;}}
	if(num6 >= g_e2p_data.E2P_7.num6_warning){ if(g_master_warning_flg6 == 0){g_master_warning_flg6 = 1;}} else { if((g_master_warning_flg6 == 2) && (num6 <= (g_e2p_data.E2P_7.num6_warning - 5))){g_master_warning_flg6 = 0;}}
	
	// --------------------------------------------------------------------
	// �V�t�g�^�C�~���O�����v����
	// --------------------------------------------------------------------
	if		 (rev >= g_e2p_data.E2P_2.rev_timing_rmp3)												{ g_shift_timing_flg = 3;	}
	else if	((rev >= g_e2p_data.E2P_2.rev_timing_rmp2) && (rev < g_e2p_data.E2P_2.rev_timing_rmp3))	{ g_shift_timing_flg = 2;	}
	else if	((rev >= g_e2p_data.E2P_2.rev_timing_rmp1) && (rev < g_e2p_data.E2P_2.rev_timing_rmp2))	{ g_shift_timing_flg = 1;	}
	else																							{ g_shift_timing_flg = 0;	}
	
	// --------------------------------------------------------------------
	// ���u�Q�[�W�`��
	// --------------------------------------------------------------------
	RevPoint = rev_peak * 3 / 100;
	LCD_locate( 0				, 10);	LCD_Gcopy( 0				,   0, (RevPoint + 23)			,119, (volatile unsigned int    *)BASE);
	LCD_locate((RevPoint + 23)	, 10);	LCD_Gcopy((RevPoint + 23)	, 120, 320 - (RevPoint + 23)	,119, (volatile unsigned int    *)BASE);
	
	// --------------------------------------------------------------------
	// ���l���[�^�`��
	// --------------------------------------------------------------------
	LCD_locate(178, 24 * 3);	LCD_INT_drawBN (rev_peak , 5, 0);
	LCD_locate( 45, 24 * 6);	if(num1_peak >= g_e2p_data.E2P_7.num1_warning){ LCD_INT_drawBNR(num1_peak, 5, g_e2p_data.E2P_4.num1_dp);} else {LCD_INT_drawBN (num1_peak, 5, g_e2p_data.E2P_4.num1_dp);}
	LCD_locate( 45, 24 * 7);	if(num2_peak >= g_e2p_data.E2P_7.num2_warning){ LCD_INT_drawBNR(num2_peak, 5, g_e2p_data.E2P_4.num2_dp);} else {LCD_INT_drawBN (num2_peak, 5, g_e2p_data.E2P_4.num2_dp);}
	LCD_locate( 45, 24 * 8);	if(num3_peak >= g_e2p_data.E2P_7.num3_warning){ LCD_INT_drawBNR(num3_peak, 5, g_e2p_data.E2P_5.num3_dp);} else {LCD_INT_drawBN (num3_peak, 5, g_e2p_data.E2P_5.num3_dp);}
	LCD_locate(205, 24 * 6);	if(num4_peak >= g_e2p_data.E2P_7.num4_warning){ LCD_INT_drawBNR(num4_peak, 5, g_e2p_data.E2P_5.num4_dp);} else {LCD_INT_drawBN (num4_peak, 5, g_e2p_data.E2P_5.num4_dp);}
	LCD_locate(205, 24 * 7);	if(num5_peak >= g_e2p_data.E2P_7.num5_warning){ LCD_INT_drawBNR(num5_peak, 5, g_e2p_data.E2P_6.num5_dp);} else {LCD_INT_drawBN (num5_peak, 5, g_e2p_data.E2P_6.num5_dp);}
	LCD_locate(205, 24 * 8);	if(num6_peak >= g_e2p_data.E2P_7.num6_warning){ LCD_INT_drawBNR(num6_peak, 5, g_e2p_data.E2P_6.num6_dp);} else {LCD_INT_drawBN (num6_peak, 5, g_e2p_data.E2P_6.num6_dp);}
	
	// --------------------------------------------------------------------
	// �x�[�X�`��
	// --------------------------------------------------------------------
	LCD_locate( 20, 24 * 1    );	LCD_textout("PEAK");
	LCD_locate(280, 24 * 3 + 6);	LCD_textout("RPM");
	LCD_locate(  1, 24 * 6 + 6);	num_label_draw(g_e2p_data.E2P_4.num1_label);
	LCD_locate(  1, 24 * 7 + 6);	num_label_draw(g_e2p_data.E2P_4.num2_label);
	LCD_locate(  1, 24 * 8 + 6);	num_label_draw(g_e2p_data.E2P_5.num3_label);
	LCD_locate(161, 24 * 6 + 6);	num_label_draw(g_e2p_data.E2P_5.num4_label);
	LCD_locate(161, 24 * 7 + 6);	num_label_draw(g_e2p_data.E2P_6.num5_label);
	LCD_locate(161, 24 * 8 + 6);	num_label_draw(g_e2p_data.E2P_6.num6_label);
	
	// --------------------------------------------------------------------
	// �P�ʕ`��
	// --------------------------------------------------------------------
	LCD_locate(145, 24 * 6 + 6);	num_unit_draw(g_e2p_data.E2P_4.num1_unit);
	LCD_locate(145, 24 * 7 + 6);	num_unit_draw(g_e2p_data.E2P_4.num2_unit);
	LCD_locate(145, 24 * 8 + 6);	num_unit_draw(g_e2p_data.E2P_5.num3_unit);
	LCD_locate(305, 24 * 6 + 6);	num_unit_draw(g_e2p_data.E2P_5.num4_unit);
	LCD_locate(305, 24 * 7 + 6);	num_unit_draw(g_e2p_data.E2P_6.num5_unit);
	LCD_locate(305, 24 * 8 + 6);	num_unit_draw(g_e2p_data.E2P_6.num6_unit);
	
	// --------------------------------------------------------------------
	// �s�[�N�z�[���h�N���A�A�C�R���`��
	// --------------------------------------------------------------------
	LCD_locate(130,215);	LCD_Gcopy(256,  96, 64, 24, (volatile unsigned int    *)FONT);
	
	// --------------------------------------------------------------------
	// �J�[�\���A�C�R���`��
	// --------------------------------------------------------------------
	LCD_locate( 46,215);	LCD_Gcopy(256,  48, 64, 24, (volatile unsigned int    *)FONT);	// ��
	LCD_locate(214,215);	LCD_Gcopy(256,  72, 64, 24, (volatile unsigned int    *)FONT);	// �E
	
	// --------------------------------------------------------------------
	// FPS�`��
	// --------------------------------------------------------------------
	g_fps ++;
	LCD_locate(  0,  0);	LCD_INT_drawBN (g_fps_max, 3, 0);
	LCD_locate( 64,  6);	LCD_textout("fps");
	
	LCD_Refresh();			// �t���[���o�b�t�@�X�V
}

// --------------------------------------------------------------------
// A/F�`���[�g�`��
// --------------------------------------------------------------------
void funcS130(void)
{
	// --------------------------------------------------------------------
	// ���[�J���ϐ��錾
	// --------------------------------------------------------------------
	unsigned int			i;
	static unsigned int		ChartX = 314, ChartY = 0;
	int						chart_line0, chart_line1;
	int						rev, afr, num1, num2, num3, num4, num5, num6;
	
	// --------------------------------------------------------------------
	// �`���[�g���@��ʃN���A&�x�[�X�`��
	// --------------------------------------------------------------------
	//�`���[�g�ʃN���A
	LCD_PAINT( 113,   4, 203, 207, COLOR_WHITE);
	//�c�r��
	LCD_line ( 114,   5, 114, 209, 0xCE59);
	LCD_line ( 164,   5, 164, 209, 0xCE59);
	LCD_line ( 214,   5, 214, 209, 0xCE59);
	LCD_line ( 264,   5, 264, 209, 0xCE59);
	LCD_line ( 314,   5, 314, 209, 0xCE59);
	//���r��
	LCD_line ( 114,   5, 314,   5, 0xCE59);
	LCD_line ( 114,  22, 314,  22, 0xCE59);
	LCD_line ( 114,  39, 314,  39, 0xCE59);
	LCD_line ( 114,  56, 314,  56, 0xCE59);
	LCD_line ( 114,  73, 314,  73, 0xCE59);
	LCD_line ( 114,  90, 314,  90, 0xCE59);
	LCD_line ( 114, 107, 314, 107, 0xCE59);
	LCD_line ( 114, 124, 314, 124, 0xCE59);
	LCD_line ( 114, 141, 314, 141, 0xCE59);
	LCD_line ( 114, 158, 314, 158, 0xCE59);
	LCD_line ( 114, 175, 314, 175, 0xCE59);
	LCD_line ( 114, 192, 314, 192, 0xCE59);
	LCD_line ( 114, 209, 314, 209, 0xCE59);
	//��R��r��
	LCD_line2( 114,  95, 314,  95, 0x865F);	// 14.7
	LCD_line2( 114, 132, 314, 132, 0xFE50);	// 12.5
	
	// --------------------------------------------------------------------
	// ���l�Z�o
	// --------------------------------------------------------------------
	rev  = ((long)(rev_data_select(g_e2p_data.E2P_3.rev_data_select )) + g_e2p_data.E2P_3.rev_bias ) * g_e2p_data.E2P_3.rev_gain  / 1000;
	afr  = ((long)(afr_data_select(g_e2p_data.E2P_3.afr_data_select )) + g_e2p_data.E2P_3.afr_bias ) * g_e2p_data.E2P_3.afr_gain  / 1000;
	num1 = ((long)(num_data_select(g_e2p_data.E2P_4.num1_data_select)) + g_e2p_data.E2P_4.num1_bias) * g_e2p_data.E2P_4.num1_gain / 1000;
	num2 = ((long)(num_data_select(g_e2p_data.E2P_4.num2_data_select)) + g_e2p_data.E2P_4.num2_bias) * g_e2p_data.E2P_4.num2_gain / 1000;
	num3 = ((long)(num_data_select(g_e2p_data.E2P_5.num3_data_select)) + g_e2p_data.E2P_5.num3_bias) * g_e2p_data.E2P_5.num3_gain / 1000;
	num4 = ((long)(num_data_select(g_e2p_data.E2P_5.num4_data_select)) + g_e2p_data.E2P_5.num4_bias) * g_e2p_data.E2P_5.num4_gain / 1000;
	num5 = ((long)(num_data_select(g_e2p_data.E2P_6.num5_data_select)) + g_e2p_data.E2P_6.num5_bias) * g_e2p_data.E2P_6.num5_gain / 1000;
	num6 = ((long)(num_data_select(g_e2p_data.E2P_6.num6_data_select)) + g_e2p_data.E2P_6.num6_bias) * g_e2p_data.E2P_6.num6_gain / 1000;
	// �Z�o�l���~�b�g
	rev  = (rev  >= +32767) ? +32767 : rev ;	rev  = (rev  <= -32767) ? -32767 : rev ;
	afr  = (afr  >= +32767) ? +32767 : afr ;	afr  = (afr  <= -32767) ? -32767 : afr ;
	num1 = (num1 >= +32767) ? +32767 : num1;	num1 = (num1 <= -32767) ? -32767 : num1;
	num2 = (num2 >= +32767) ? +32767 : num2;	num2 = (num2 <= -32767) ? -32767 : num2;
	num3 = (num3 >= +32767) ? +32767 : num3;	num3 = (num3 <= -32767) ? -32767 : num3;
	num4 = (num4 >= +32767) ? +32767 : num4;	num4 = (num4 <= -32767) ? -32767 : num4;
	num5 = (num5 >= +32767) ? +32767 : num5;	num5 = (num5 <= -32767) ? -32767 : num5;
	num6 = (num6 >= +32767) ? +32767 : num6;	num6 = (num6 <= -32767) ? -32767 : num6;
	
	// --------------------------------------------------------------------
	// �s�[�N�z�[���h�l�X�V
	// --------------------------------------------------------------------
	if(rev_peak  < rev  ){rev_peak  = rev;}
	if(num1_peak < num1 ){num1_peak = num1;}
	if(num2_peak < num2 ){num2_peak = num2;}
	if(num3_peak < num3 ){num3_peak = num3;}
	if(num4_peak < num4 ){num4_peak = num4;}
	if(num5_peak < num5 ){num5_peak = num5;}
	if(num6_peak < num6 ){num6_peak = num6;}
	
	// --------------------------------------------------------------------
	// �s�[�N�z�[���h�l�������X�V
	// --------------------------------------------------------------------
	if( (Onetime_Peakclear_done == 0)
	&&	(Onetime_Peakclear_cnt == 0))
	{
		Onetime_Peakclear_done = 1;
		rev_peak  = rev;
		num1_peak = num1;
		num2_peak = num2;
		num3_peak = num3;
		num4_peak = num4;
		num5_peak = num5;
		num6_peak = num6;
	}
	
	// --------------------------------------------------------------------
	// �}�X�^�[���[�j���O����&����
	// --------------------------------------------------------------------
	if(num1 >= g_e2p_data.E2P_7.num1_warning){ if(g_master_warning_flg1 == 0){g_master_warning_flg1 = 1;}} else { if((g_master_warning_flg1 == 2) && (num1 <= (g_e2p_data.E2P_7.num1_warning - 5))){g_master_warning_flg1 = 0;}}
	if(num2 >= g_e2p_data.E2P_7.num2_warning){ if(g_master_warning_flg2 == 0){g_master_warning_flg2 = 1;}} else { if((g_master_warning_flg2 == 2) && (num2 <= (g_e2p_data.E2P_7.num2_warning - 5))){g_master_warning_flg2 = 0;}}
	if(num3 >= g_e2p_data.E2P_7.num3_warning){ if(g_master_warning_flg3 == 0){g_master_warning_flg3 = 1;}} else { if((g_master_warning_flg3 == 2) && (num3 <= (g_e2p_data.E2P_7.num3_warning - 5))){g_master_warning_flg3 = 0;}}
	if(num4 >= g_e2p_data.E2P_7.num4_warning){ if(g_master_warning_flg4 == 0){g_master_warning_flg4 = 1;}} else { if((g_master_warning_flg4 == 2) && (num4 <= (g_e2p_data.E2P_7.num4_warning - 5))){g_master_warning_flg4 = 0;}}
	if(num5 >= g_e2p_data.E2P_7.num5_warning){ if(g_master_warning_flg5 == 0){g_master_warning_flg5 = 1;}} else { if((g_master_warning_flg5 == 2) && (num5 <= (g_e2p_data.E2P_7.num5_warning - 5))){g_master_warning_flg5 = 0;}}
	if(num6 >= g_e2p_data.E2P_7.num6_warning){ if(g_master_warning_flg6 == 0){g_master_warning_flg6 = 1;}} else { if((g_master_warning_flg6 == 2) && (num6 <= (g_e2p_data.E2P_7.num6_warning - 5))){g_master_warning_flg6 = 0;}}
	
	// --------------------------------------------------------------------
	// �V�t�g�^�C�~���O�����v����
	// --------------------------------------------------------------------
	if		 (rev >= g_e2p_data.E2P_2.rev_timing_rmp3)												{ g_shift_timing_flg = 3;	}
	else if	((rev >= g_e2p_data.E2P_2.rev_timing_rmp2) && (rev < g_e2p_data.E2P_2.rev_timing_rmp3))	{ g_shift_timing_flg = 2;	}
	else if	((rev >= g_e2p_data.E2P_2.rev_timing_rmp1) && (rev < g_e2p_data.E2P_2.rev_timing_rmp2))	{ g_shift_timing_flg = 1;	}
	else																							{ g_shift_timing_flg = 0;	}
	
	// --------------------------------------------------------------------
	// AF�`���[�g�f�[�^�X�V
	// --------------------------------------------------------------------
	if(g_int50mscnt >= 1)
	{
		if(chart_onoff == 0)
		{
			for(i = 1; i < ChartBufNum; i++ )
			{
				chart_buf1[i - 1] = chart_buf1[i];
				chart_buf2[i - 1] = chart_buf2[i];
			}
			chart_buf1[ChartBufNum - 1] = afr_data_select(g_e2p_data.E2P_3.afr_data_select);
			chart_buf2[ChartBufNum - 1] = rev;
		}
		
		g_int50mscnt = 0;
	}
	else
	{
		//
	}
	
	// --------------------------------------------------------------------
	// AF�`���[�g�`��
	// --------------------------------------------------------------------
	for(i = 1; i < ChartBufNum; i++ )
	{
		chart_line0 = ((chart_buf1[i - 1] * 16377) >> 16) * (-1) + 340;
		chart_line0 = (chart_line0 > 204) ? 204 : chart_line0;
		chart_line0 = (chart_line0 <   1) ?   1 : chart_line0;
		
		chart_line1 = ((chart_buf1[i    ] * 16377) >> 16) * (-1) + 340;
		chart_line1 = (chart_line1 > 204) ? 204 : chart_line1;
		chart_line1 = (chart_line1 <   1) ?   1 : chart_line1;
		
		LCD_line2((i - 1 + 114), chart_line0      - 1 + 5,( i + 114), chart_line1  - 1 + 5, COLOR_BLUE);
	}
	
	// --------------------------------------------------------------------
	// �x�[�X�`��
	// --------------------------------------------------------------------
	LCD_locate(  0, 24 * 1 + 6);	num_label_draw(g_e2p_data.E2P_3.afr_label);
	LCD_locate(  0, 24 * 3 + 6);	LCD_textout("RPM");
	LCD_locate( 93,   0);			LCD_textout("20");
	LCD_locate( 93,  34);			LCD_textout("18");
	LCD_locate( 93,  68);			LCD_textout("16");
	LCD_locate( 93, 102);			LCD_textout("14");
	LCD_locate( 93, 136);			LCD_textout("12");
	LCD_locate( 93, 170);			LCD_textout("10");
	LCD_locate( 12, 24 * 8 + 6);	LCD_textout("2.5s/DIV");
	
	if(chart_onoff == 0)
	{
		// --------------------------------------------------------------------
		// ���l���[�^�`��
		// --------------------------------------------------------------------
		LCD_locate(  0, 24 * 2);	LCD_INT_drawBN (afr	, 5, g_e2p_data.E2P_3.afr_dp );
		LCD_locate(  0, 24 * 4);	LCD_INT_drawBN (rev , 5, 0);
		
		// --------------------------------------------------------------------
		// �`���[�g��~�A�C�R���`��
		// --------------------------------------------------------------------
		LCD_locate(130,215);	LCD_Gcopy(256, 168, 64, 24, (volatile unsigned int    *)FONT);	// ||
	}
	else
	{
		// --------------------------------------------------------------------
		// ���l���[�^�`��
		// --------------------------------------------------------------------
		LCD_locate(  0, 24 * 2);		LCD_INT_drawBNR((chart_buf1[ChartX - 114] * 147 + 500) /1000,5, 1);
		LCD_locate(  0, 24 * 4);		LCD_INT_drawBNR( chart_buf2[ChartX - 114]					,5, 0);
		
		// --------------------------------------------------------------------
		// �^�b�`���쏈��
		// --------------------------------------------------------------------
		if(touch_drag_flg)			// �^�b�`�p�l���������肠��
		{
			touch_drag_flg = 0;		// �t���O�N���A
			
			if((g_DragX >= 114) && (g_DragX <= 314) && (g_DragY >=   5) && (g_DragY <= 209))
			{
				ChartX = g_DragX;
				ChartY = g_DragY;
			}
			else
			{
				//
			}
		}
		else
		{
			//
		}
		LCD_line ( ChartX,   5,  ChartX, 209, COLOR_RED);
		// --------------------------------------------------------------------
		// �`���[�g�Đ��A�C�R���`��
		// --------------------------------------------------------------------
		LCD_locate(130,215);	LCD_Gcopy(256,  72, 64, 24, (volatile unsigned int    *)FONT);	// �E
	}
		
	// --------------------------------------------------------------------
	// �J�[�\���A�C�R���`��
	// --------------------------------------------------------------------
	LCD_locate( 46,215);	LCD_Gcopy(256,  48, 64, 24, (volatile unsigned int    *)FONT);	// ��
	LCD_locate(214,215);	LCD_Gcopy(256,  72, 64, 24, (volatile unsigned int    *)FONT);	// �E
	
	// --------------------------------------------------------------------
	// FPS�`��
	// --------------------------------------------------------------------
	g_fps ++;
	LCD_locate(  0,  0);	LCD_INT_drawBN (g_fps_max, 3, 0);
	LCD_locate( 64,  6);	LCD_textout("fps");
	
	LCD_Refresh();			// �t���[���o�b�t�@�X�V
}

// --------------------------------------------------------------------
// ���l�`��ݒ��ʕ`��
// --------------------------------------------------------------------
void funcS210(void)
{
	// --------------------------------------------------------------------
	// �ݒ荀�ڕ`��
	// --------------------------------------------------------------------
	LCD_locate( 20, 24 * 0 + 6);
	switch(num_page)
	{
		case 1:		LCD_textout("<<NUM 1 SETTINGS>>");	break;
		case 2:		LCD_textout("<<NUM 2 SETTINGS>>");	break;
		case 3:		LCD_textout("<<NUM 3 SETTINGS>>");	break;
		case 4:		LCD_textout("<<NUM 4 SETTINGS>>");	break;
		case 5:		LCD_textout("<<NUM 5 SETTINGS>>");	break;
		case 6:		LCD_textout("<<NUM 6 SETTINGS>>");	break;
		case 7:		LCD_textout("<<REV SETTINGS>>");	break;
		case 8:		LCD_textout("<<AFR SETTINGS>>");	break;
	}
	
							LCD_locate( 20, 24 * 1 + 6);	LCD_textout("Data Select");
							LCD_locate( 20, 24 * 3 + 6);	LCD_textout("Gain  ");
							LCD_locate( 20, 24 * 4 + 6);	LCD_textout("Bias  ");
	if( num_page != 7) {	LCD_locate( 20, 24 * 5 + 6);	LCD_textout("DP    ");
							LCD_locate( 20, 24 * 6 + 6);	LCD_textout("Label ");}
	if((num_page != 7)
	&&( num_page != 8)){	LCD_locate( 20, 24 * 7 + 6);	LCD_textout("Unit  ");
							LCD_locate( 20, 24 * 8 + 6);	LCD_textout("Limit ");}
	
	if( num_page == 7) {	LCD_locate( 20, 24 * 5 + 6);	LCD_textout("REV-1 ");
							LCD_locate( 20, 24 * 6 + 6);	LCD_textout("REV-2 ");
							LCD_locate( 20, 24 * 7 + 6);	LCD_textout("REV-3 ");}
	
	LCD_locate( 20, 24 * 2 + 6);
	switch(num_page)
	{
		case 1:		num_data_select_draw(g_e2p_data.E2P_4.num1_data_select);	break;
		case 2:		num_data_select_draw(g_e2p_data.E2P_4.num2_data_select);	break;
		case 3:		num_data_select_draw(g_e2p_data.E2P_5.num3_data_select);	break;
		case 4:		num_data_select_draw(g_e2p_data.E2P_5.num4_data_select);	break;
		case 5:		num_data_select_draw(g_e2p_data.E2P_6.num5_data_select);	break;
		case 6:		num_data_select_draw(g_e2p_data.E2P_6.num6_data_select);	break;
		case 7:		rev_data_select_draw(g_e2p_data.E2P_3.rev_data_select );	break;
		case 8:		afr_data_select_draw(g_e2p_data.E2P_3.afr_data_select );	break;
	}
	LCD_locate(230, 24 * 1    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
	LCD_locate(270, 24 * 1    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
	
	LCD_locate( 80, 24 * 3 + 6);	LCD_textout("< x ");
	switch(num_page)
	{
		case 1:		LCD_INT_draw(g_e2p_data.E2P_4.num1_gain, 6, 3);	break;
		case 2:		LCD_INT_draw(g_e2p_data.E2P_4.num2_gain, 6, 3);	break;
		case 3:		LCD_INT_draw(g_e2p_data.E2P_5.num3_gain, 6, 3);	break;
		case 4:		LCD_INT_draw(g_e2p_data.E2P_5.num4_gain, 6, 3);	break;
		case 5:		LCD_INT_draw(g_e2p_data.E2P_6.num5_gain, 6, 3);	break;
		case 6:		LCD_INT_draw(g_e2p_data.E2P_6.num6_gain, 6, 3);	break;
		case 7:		LCD_INT_draw(g_e2p_data.E2P_3.rev_gain , 6, 3);	break;
		case 8:		LCD_INT_draw(g_e2p_data.E2P_3.afr_gain , 6, 3);	break;
	}
	LCD_locate(200, 24 * 3 + 6);	LCD_textout(">");
	LCD_locate(230, 24 * 3    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
	LCD_locate(270, 24 * 3    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
	
	LCD_locate( 80, 24 * 4 + 6);	LCD_textout("< + ");
	switch(num_page)
	{
		case 1:		LCD_INT_draw(g_e2p_data.E2P_4.num1_bias, 6, 0);	break;
		case 2:		LCD_INT_draw(g_e2p_data.E2P_4.num2_bias, 6, 0);	break;
		case 3:		LCD_INT_draw(g_e2p_data.E2P_5.num3_bias, 6, 0);	break;
		case 4:		LCD_INT_draw(g_e2p_data.E2P_5.num4_bias, 6, 0);	break;
		case 5:		LCD_INT_draw(g_e2p_data.E2P_6.num5_bias, 6, 0);	break;
		case 6:		LCD_INT_draw(g_e2p_data.E2P_6.num6_bias, 6, 0);	break;
		case 7:		LCD_INT_draw(g_e2p_data.E2P_3.rev_bias , 6, 0);	break;
		case 8:		LCD_INT_draw(g_e2p_data.E2P_3.afr_bias , 6, 0);	break;
	}
	LCD_locate(200, 24 * 4 + 6);	LCD_textout(">");
	LCD_locate(230, 24 * 4    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
	LCD_locate(270, 24 * 4    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
	
	LCD_locate( 80, 24 * 5 + 6);
	switch(num_page)
	{
		case 1:		num_dp_draw(g_e2p_data.E2P_4.num1_dp);
					LCD_locate(230, 24 * 5    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 5    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 2:		num_dp_draw(g_e2p_data.E2P_4.num2_dp);
					LCD_locate(230, 24 * 5    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 5    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 3:		num_dp_draw(g_e2p_data.E2P_5.num3_dp);
					LCD_locate(230, 24 * 5    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 5    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 4:		num_dp_draw(g_e2p_data.E2P_5.num4_dp);
					LCD_locate(230, 24 * 5    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 5    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 5:		num_dp_draw(g_e2p_data.E2P_6.num5_dp);
					LCD_locate(230, 24 * 5    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 5    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 6:		num_dp_draw(g_e2p_data.E2P_6.num6_dp);
					LCD_locate(230, 24 * 5    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 5    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 7:		LCD_textout("<   ");	LCD_INT_draw(g_e2p_data.E2P_2.rev_timing_rmp1, 6, 0);	LCD_textout(">");
					LCD_locate(230, 24 * 5    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 5    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 8:		num_dp_draw(g_e2p_data.E2P_3.afr_dp );
					LCD_locate(230, 24 * 5    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 5    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
	}
	
	LCD_locate( 80, 24 * 6 + 6);	
	switch(num_page)
	{
		case 1:		LCD_textout("< ");	num_label_draw(g_e2p_data.E2P_4.num1_label);	LCD_locate(200, 24 * 6 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 6    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 6    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 2:		LCD_textout("< ");	num_label_draw(g_e2p_data.E2P_4.num2_label);	LCD_locate(200, 24 * 6 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 6    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 6    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 3:		LCD_textout("< ");	num_label_draw(g_e2p_data.E2P_5.num3_label);	LCD_locate(200, 24 * 6 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 6    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 6    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 4:		LCD_textout("< ");	num_label_draw(g_e2p_data.E2P_5.num4_label);	LCD_locate(200, 24 * 6 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 6    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 6    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 5:		LCD_textout("< ");	num_label_draw(g_e2p_data.E2P_6.num5_label);	LCD_locate(200, 24 * 6 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 6    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 6    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 6:		LCD_textout("< ");	num_label_draw(g_e2p_data.E2P_6.num6_label);	LCD_locate(200, 24 * 6 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 6    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 6    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 7:		LCD_textout("<   ");	LCD_INT_draw(g_e2p_data.E2P_2.rev_timing_rmp2, 6, 0);	LCD_textout(">");
					LCD_locate(230, 24 * 6    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 6    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 8:		LCD_textout("< ");	num_label_draw(g_e2p_data.E2P_3.afr_label );	LCD_locate(200, 24 * 6 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 6    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 6    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
	}
	
	LCD_locate( 80, 24 * 7 + 6);
	switch(num_page)
	{
		case 1:		LCD_textout("< ");	num_unit_draw(g_e2p_data.E2P_4.num1_unit);	LCD_locate(200, 24 * 7 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 7    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 7    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 2:		LCD_textout("< ");	num_unit_draw(g_e2p_data.E2P_4.num2_unit);	LCD_locate(200, 24 * 7 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 7    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 7    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 3:		LCD_textout("< ");	num_unit_draw(g_e2p_data.E2P_5.num3_unit);	LCD_locate(200, 24 * 7 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 7    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 7    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 4:		LCD_textout("< ");	num_unit_draw(g_e2p_data.E2P_5.num4_unit);	LCD_locate(200, 24 * 7 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 7    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 7    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 5:		LCD_textout("< ");	num_unit_draw(g_e2p_data.E2P_6.num5_unit);	LCD_locate(200, 24 * 7 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 7    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 7    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 6:		LCD_textout("< ");	num_unit_draw(g_e2p_data.E2P_6.num6_unit);	LCD_locate(200, 24 * 7 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 7    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 7    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 7:		LCD_textout("<   ");	LCD_INT_draw(g_e2p_data.E2P_2.rev_timing_rmp3, 6, 0);	LCD_textout(">");
					LCD_locate(230, 24 * 7    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 7    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
	}
	
	LCD_locate( 80, 24 * 8 + 6);
	switch(num_page)
	{
		case 1:		LCD_textout("<   ");	LCD_INT_draw(g_e2p_data.E2P_7.num1_warning, 6, g_e2p_data.E2P_4.num1_dp);	LCD_locate(200, 24 * 8 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 8    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 8    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 2:		LCD_textout("<   ");	LCD_INT_draw(g_e2p_data.E2P_7.num2_warning, 6, g_e2p_data.E2P_4.num2_dp);	LCD_locate(200, 24 * 8 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 8    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 8    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 3:		LCD_textout("<   ");	LCD_INT_draw(g_e2p_data.E2P_7.num3_warning, 6, g_e2p_data.E2P_5.num3_dp);	LCD_locate(200, 24 * 8 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 8    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 8    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 4:		LCD_textout("<   ");	LCD_INT_draw(g_e2p_data.E2P_7.num4_warning, 6, g_e2p_data.E2P_5.num4_dp);	LCD_locate(200, 24 * 8 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 8    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 8    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 5:		LCD_textout("<   ");	LCD_INT_draw(g_e2p_data.E2P_7.num5_warning, 6, g_e2p_data.E2P_6.num5_dp);	LCD_locate(200, 24 * 8 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 8    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 8    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
		case 6:		LCD_textout("<   ");	LCD_INT_draw(g_e2p_data.E2P_7.num6_warning, 6, g_e2p_data.E2P_6.num6_dp);	LCD_locate(200, 24 * 8 + 6);	LCD_textout(">");
					LCD_locate(230, 24 * 8    );	LCD_Gcopy(256, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					LCD_locate(270, 24 * 8    );	LCD_Gcopy(288, 144, 32, 24, (volatile unsigned int    *)FONT);	// ��
					break;
	}
	
	// --------------------------------------------------------------------
	// �ۑ��A�C�R���`��
	// --------------------------------------------------------------------
	LCD_locate(130,215);	LCD_Gcopy(256, 120, 64, 24, (volatile unsigned int    *)FONT);
	
	LCD_Refresh();			// �t���[���o�b�t�@�X�V
}

// --------------------------------------------------------------------
// �����ݒ��ʕ`��
// --------------------------------------------------------------------
void funcS999(void)
{
	// --------------------------------------------------------------------
	// �����ݒ荀�ڕ`��
	// --------------------------------------------------------------------
	LCD_locate( 20, 24 * 0 + 6);	LCD_textout("<<INITIALIZATION>>");
	LCD_locate(250, 24 * 0 + 6);	LCD_textout(version);
	LCD_locate( 20, 24 * 2 + 6);	LCD_textout("Initialize the SETUP DATA.");
	LCD_locate( 20, 24 * 3 + 6);	LCD_textout("Press to SAVE.");
	
	// --------------------------------------------------------------------
	// �ۑ��A�C�R���`��
	// --------------------------------------------------------------------
	LCD_locate(130,215);	LCD_Gcopy(256, 120, 64, 24, (volatile unsigned int    *)FONT);
	
	LCD_Refresh();			// �t���[���o�b�t�@�X�V
}

// --------------------------------------------------------------------
// MoTeC1������
// --------------------------------------------------------------------
static void Init_MoTeC1(void)
{
	// --------------------------------------------------------------------
	// CAN���荞�݋֎~
	// --------------------------------------------------------------------
	RX0IE_Disable;
	RX1IE_Disable;
//	ERRIE_Disable;
//	WAKIE_Disable;
//	MERRE_Disable;
	
	// --------------------------------------------------------------------
	// CAN������
	// --------------------------------------------------------------------
	CANInit(CAN_BRP_20MHz_1MBPS);
	
	// --------------------------------------------------------------------
	// CAN�t�B���^�[�ݒ�
	// --------------------------------------------------------------------
	CANSetSidFilter0(0x0010);
	CANSetSidFilter1(0x0010);
	CANSetSidFilter2(0x0010);
	CANSetSidFilter3(0x0010);
	CANSetSidFilter4(0x0010);
	CANSetSidFilter5(0x0010);
	CANSetSidMask0(0xFFF0);
	CANSetSidMask1(0xFFF0);
	
	CANSetFilterRxB0(1);
	CANSetFilterRxB1(1);
	
	// --------------------------------------------------------------------
	// CAN���[�h�ؑ�
	// --------------------------------------------------------------------
	CANSetOpMode(CAM_MODE_NORMAL);
//	CANSetOpMode(CAM_MODE_LISTEN);
	
	// --------------------------------------------------------------------
	// CAN���荞�݋���
	// --------------------------------------------------------------------
	RX0IE_Enable;
	RX1IE_Enable;
//	ERRIE_Enable;
//	WAKIE_Enable;
//	MERRE_Enable;
}

// --------------------------------------------------------------------
// Haltech1������
// --------------------------------------------------------------------
static void Init_Haltech1(void)
{
	// --------------------------------------------------------------------
	// CAN���荞�݋֎~
	// --------------------------------------------------------------------
	RX0IE_Disable;
	RX1IE_Disable;
//	ERRIE_Disable;
//	WAKIE_Disable;
//	MERRE_Disable;
	
	// --------------------------------------------------------------------
	// CAN������
	// --------------------------------------------------------------------
	CANInit(CAN_BRP_20MHz_1MBPS);
	
	// --------------------------------------------------------------------
	// CAN�t�B���^�[�ݒ�
	// --------------------------------------------------------------------
	CANSetSidFilter0(0x0010);
	CANSetSidFilter1(0x0010);
	CANSetSidFilter2(0x0010);
	CANSetSidFilter3(0x0010);
	CANSetSidFilter4(0x0010);
	CANSetSidFilter5(0x0010);
	CANSetSidMask0(0xFFF0);
	CANSetSidMask1(0xFFF0);
	
	CANSetFilterRxB0(1);
	CANSetFilterRxB1(1);
	
	// --------------------------------------------------------------------
	// CAN���[�h�ؑ�
	// --------------------------------------------------------------------
	CANSetOpMode(CAM_MODE_NORMAL);
//	CANSetOpMode(CAM_MODE_LISTEN);
	
	// --------------------------------------------------------------------
	// CAN���荞�݋���
	// --------------------------------------------------------------------
	RX0IE_Enable;
	RX1IE_Enable;
//	ERRIE_Enable;
//	WAKIE_Enable;
//	MERRE_Enable;
}

// --------------------------------------------------------------------
// Haltech2������
// --------------------------------------------------------------------
static void Init_Haltech2(void)
{
	// --------------------------------------------------------------------
	// CAN���荞�݋֎~
	// --------------------------------------------------------------------
	RX0IE_Disable;
	RX1IE_Disable;
//	ERRIE_Disable;
//	WAKIE_Disable;
//	MERRE_Disable;
	
	// --------------------------------------------------------------------
	// CAN������
	// --------------------------------------------------------------------
	CANInit(CAN_BRP_20MHz_1MBPS);
	
	// --------------------------------------------------------------------
	// CAN�t�B���^�[�ݒ�
	// --------------------------------------------------------------------
	CANSetSidFilter0(0x0300);
	CANSetSidFilter1(0x0300);
	CANSetSidFilter2(0x0300);
	CANSetSidFilter3(0x0300);
	CANSetSidFilter4(0x0300);
	CANSetSidFilter5(0x0300);
	CANSetSidMask0(0xFF00);
	CANSetSidMask1(0xFF00);
	
	CANSetFilterRxB0(1);
	CANSetFilterRxB1(1);
	
	// --------------------------------------------------------------------
	// CAN���[�h�ؑ�
	// --------------------------------------------------------------------
	CANSetOpMode(CAM_MODE_NORMAL);
//	CANSetOpMode(CAM_MODE_LISTEN);
	
	// --------------------------------------------------------------------
	// CAN���荞�݋���
	// --------------------------------------------------------------------
	RX0IE_Enable;
	RX1IE_Enable;
//	ERRIE_Enable;
//	WAKIE_Enable;
//	MERRE_Enable;
}

// --------------------------------------------------------------------
// MoTeC1�v���Z�b�g�ǂݍ���
// --------------------------------------------------------------------
static void Preset_load_MoTeC1(void)
{
	g_e2p_data.E2P_2.rev_timing_rmp1			=   6250;
	g_e2p_data.E2P_2.rev_timing_rmp2			=   6500;
	g_e2p_data.E2P_2.rev_timing_rmp3			=   6750;
	g_e2p_data.E2P_3.rev_data_select			=      0;
	g_e2p_data.E2P_3.rev_gain					=   1000;
	g_e2p_data.E2P_3.rev_bias					=      0;
	g_e2p_data.E2P_3.afr_data_select			=      0;
	g_e2p_data.E2P_3.afr_gain					=    147;
	g_e2p_data.E2P_3.afr_bias					=      0;
	g_e2p_data.E2P_3.afr_dp						=      1;
	g_e2p_data.E2P_3.afr_label					=      1;
	g_e2p_data.E2P_4.num1_data_select			=      3;
	g_e2p_data.E2P_4.num1_gain					=   1000;
	g_e2p_data.E2P_4.num1_bias					=      0;
	g_e2p_data.E2P_4.num1_dp					=      1;
	g_e2p_data.E2P_4.num1_label					=      3;
	g_e2p_data.E2P_4.num1_unit					=      1;
	g_e2p_data.E2P_4.num2_data_select			=      8;
	g_e2p_data.E2P_4.num2_gain					=   1000;
	g_e2p_data.E2P_4.num2_bias					=      0;
	g_e2p_data.E2P_4.num2_dp					=      1;
	g_e2p_data.E2P_4.num2_label					=      5;
	g_e2p_data.E2P_4.num2_unit					=      1;
	g_e2p_data.E2P_5.num3_data_select			=      5;
	g_e2p_data.E2P_5.num3_gain					=   1000;
	g_e2p_data.E2P_5.num3_bias					=      0;
	g_e2p_data.E2P_5.num3_dp					=      2;
	g_e2p_data.E2P_5.num3_label					=      8;
	g_e2p_data.E2P_5.num3_unit					=      2;
	g_e2p_data.E2P_5.num4_data_select			=      7;
	g_e2p_data.E2P_5.num4_gain					=   1000;
	g_e2p_data.E2P_5.num4_bias					=      0;
	g_e2p_data.E2P_5.num4_dp					=      0;
	g_e2p_data.E2P_5.num4_label					=     12;
	g_e2p_data.E2P_5.num4_unit					=      4;
	g_e2p_data.E2P_6.num5_data_select			=      6;
	g_e2p_data.E2P_6.num5_gain					=   1000;
	g_e2p_data.E2P_6.num5_bias					=      0;
	g_e2p_data.E2P_6.num5_dp					=      1;
	g_e2p_data.E2P_6.num5_label					=     10;
	g_e2p_data.E2P_6.num5_unit					=      3;
	g_e2p_data.E2P_6.num6_data_select			=     12;
	g_e2p_data.E2P_6.num6_gain					=   1000;
	g_e2p_data.E2P_6.num6_bias					=      0;
	g_e2p_data.E2P_6.num6_dp					=      1;
	g_e2p_data.E2P_6.num6_label					=     15;
	g_e2p_data.E2P_6.num6_unit					=      3;
	g_e2p_data.E2P_7.num1_warning				=   1050;
	g_e2p_data.E2P_7.num2_warning				=    700;
	g_e2p_data.E2P_7.num3_warning				=   1600;
	g_e2p_data.E2P_7.num4_warning				=   1500;
	g_e2p_data.E2P_7.num5_warning				=   1100;
	g_e2p_data.E2P_7.num6_warning				=    900;
}

// --------------------------------------------------------------------
// MoTeC2�v���Z�b�g�ǂݍ���
// --------------------------------------------------------------------
static void Preset_load_MoTeC2(void)
{
	//
}

// --------------------------------------------------------------------
// Haltech1�v���Z�b�g�ǂݍ���
// --------------------------------------------------------------------
static void Preset_load_Haltech1(void)
{
	g_e2p_data.E2P_2.rev_timing_rmp1			=   6250;
	g_e2p_data.E2P_2.rev_timing_rmp2			=   6500;
	g_e2p_data.E2P_2.rev_timing_rmp3			=   6750;
	g_e2p_data.E2P_3.rev_data_select			=      1;
	g_e2p_data.E2P_3.rev_gain					=   1000;
	g_e2p_data.E2P_3.rev_bias					=      0;
	g_e2p_data.E2P_3.afr_data_select			=      1;
	g_e2p_data.E2P_3.afr_gain					=    147;
	g_e2p_data.E2P_3.afr_bias					=      0;
	g_e2p_data.E2P_3.afr_dp						=      1;
	g_e2p_data.E2P_3.afr_label					=      1;
	g_e2p_data.E2P_4.num1_data_select			=     16;
	g_e2p_data.E2P_4.num1_gain					=   1000;
	g_e2p_data.E2P_4.num1_bias					=      0;
	g_e2p_data.E2P_4.num1_dp					=      1;
	g_e2p_data.E2P_4.num1_label					=      3;
	g_e2p_data.E2P_4.num1_unit					=      1;
	g_e2p_data.E2P_4.num2_data_select			=     21;
	g_e2p_data.E2P_4.num2_gain					=   1000;
	g_e2p_data.E2P_4.num2_bias					=      0;
	g_e2p_data.E2P_4.num2_dp					=      1;
	g_e2p_data.E2P_4.num2_label					=      5;
	g_e2p_data.E2P_4.num2_unit					=      1;
	g_e2p_data.E2P_5.num3_data_select			=     18;
	g_e2p_data.E2P_5.num3_gain					=   1000;
	g_e2p_data.E2P_5.num3_bias					=      0;
	g_e2p_data.E2P_5.num3_dp					=      2;
	g_e2p_data.E2P_5.num3_label					=      8;
	g_e2p_data.E2P_5.num3_unit					=      2;
	g_e2p_data.E2P_5.num4_data_select			=     20;
	g_e2p_data.E2P_5.num4_gain					=   1000;
	g_e2p_data.E2P_5.num4_bias					=      0;
	g_e2p_data.E2P_5.num4_dp					=      0;
	g_e2p_data.E2P_5.num4_label					=     12;
	g_e2p_data.E2P_5.num4_unit					=      4;
	g_e2p_data.E2P_6.num5_data_select			=     19;
	g_e2p_data.E2P_6.num5_gain					=   1000;
	g_e2p_data.E2P_6.num5_bias					=      0;
	g_e2p_data.E2P_6.num5_dp					=      1;
	g_e2p_data.E2P_6.num5_label					=     10;
	g_e2p_data.E2P_6.num5_unit					=      3;
	g_e2p_data.E2P_6.num6_data_select			=     25;
	g_e2p_data.E2P_6.num6_gain					=   1000;
	g_e2p_data.E2P_6.num6_bias					=      0;
	g_e2p_data.E2P_6.num6_dp					=      1;
	g_e2p_data.E2P_6.num6_label					=     15;
	g_e2p_data.E2P_6.num6_unit					=      3;
	g_e2p_data.E2P_7.num1_warning				=   1050;
	g_e2p_data.E2P_7.num2_warning				=    700;
	g_e2p_data.E2P_7.num3_warning				=   1600;
	g_e2p_data.E2P_7.num4_warning				=   1500;
	g_e2p_data.E2P_7.num5_warning				=   1100;
	g_e2p_data.E2P_7.num6_warning				=    900;
}

// --------------------------------------------------------------------
// Haltech2�v���Z�b�g�ǂݍ���
// --------------------------------------------------------------------
static void Preset_load_Haltech2(void)
{
	g_e2p_data.E2P_2.rev_timing_rmp1			=   6250;
	g_e2p_data.E2P_2.rev_timing_rmp2			=   6500;
	g_e2p_data.E2P_2.rev_timing_rmp3			=   6750;
	g_e2p_data.E2P_3.rev_data_select			=      2;
	g_e2p_data.E2P_3.rev_gain					=   1000;
	g_e2p_data.E2P_3.rev_bias					=      0;
	g_e2p_data.E2P_3.afr_data_select			=      2;
	g_e2p_data.E2P_3.afr_gain					=    147;
	g_e2p_data.E2P_3.afr_bias					=      0;
	g_e2p_data.E2P_3.afr_dp						=      1;
	g_e2p_data.E2P_3.afr_label					=      1;
	g_e2p_data.E2P_4.num1_data_select			=     42;
	g_e2p_data.E2P_4.num1_gain					=   1000;
	g_e2p_data.E2P_4.num1_bias					=      0;
	g_e2p_data.E2P_4.num1_dp					=      1;
	g_e2p_data.E2P_4.num1_label					=      3;
	g_e2p_data.E2P_4.num1_unit					=      1;
	g_e2p_data.E2P_4.num2_data_select			=     43;
	g_e2p_data.E2P_4.num2_gain					=   1000;
	g_e2p_data.E2P_4.num2_bias					=      0;
	g_e2p_data.E2P_4.num2_dp					=      1;
	g_e2p_data.E2P_4.num2_label					=      5;
	g_e2p_data.E2P_4.num2_unit					=      1;
	g_e2p_data.E2P_5.num3_data_select			=     38;
	g_e2p_data.E2P_5.num3_gain					=   1000;
	g_e2p_data.E2P_5.num3_bias					=      0;
	g_e2p_data.E2P_5.num3_dp					=      2;
	g_e2p_data.E2P_5.num3_label					=      8;
	g_e2p_data.E2P_5.num3_unit					=      2;
	g_e2p_data.E2P_5.num4_data_select			=     27;
	g_e2p_data.E2P_5.num4_gain					=   1000;
	g_e2p_data.E2P_5.num4_bias					=      0;
	g_e2p_data.E2P_5.num4_dp					=      0;
	g_e2p_data.E2P_5.num4_label					=     12;
	g_e2p_data.E2P_5.num4_unit					=      4;
	g_e2p_data.E2P_6.num5_data_select			=     28;
	g_e2p_data.E2P_6.num5_gain					=   1000;
	g_e2p_data.E2P_6.num5_bias					=      0;
	g_e2p_data.E2P_6.num5_dp					=      1;
	g_e2p_data.E2P_6.num5_label					=     10;
	g_e2p_data.E2P_6.num5_unit					=      3;
	g_e2p_data.E2P_6.num6_data_select			=     31;
	g_e2p_data.E2P_6.num6_gain					=   1000;
	g_e2p_data.E2P_6.num6_bias					=      0;
	g_e2p_data.E2P_6.num6_dp					=      1;
	g_e2p_data.E2P_6.num6_label					=     15;
	g_e2p_data.E2P_6.num6_unit					=      3;
	g_e2p_data.E2P_7.num1_warning				=   1050;
	g_e2p_data.E2P_7.num2_warning				=    700;
	g_e2p_data.E2P_7.num3_warning				=   1600;
	g_e2p_data.E2P_7.num4_warning				=   1500;
	g_e2p_data.E2P_7.num5_warning				=   1100;
	g_e2p_data.E2P_7.num6_warning				=    900;
}

// --------------------------------------------------------------------
// Freedom1�v���Z�b�g�ǂݍ���
// --------------------------------------------------------------------
static void Preset_load_Freedom1(void)
{
	//
}

// --------------------------------------------------------------------
// Freedom2�v���Z�b�g�ǂݍ���
// --------------------------------------------------------------------
static void Preset_load_Freedom2(void)
{
	//
}

// --------------------------------------------------------------------
// MegaSquirt1�v���Z�b�g�ǂݍ���
// --------------------------------------------------------------------
static void Preset_load_MegaSquirt1(void)
{
	//
}
