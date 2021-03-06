// --------------------------------------------------------------------
// Copylight (C) 2021, Tomoya Sato( https://blog.goo.ne.jp/nacci_tomoya )
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
// filename		:	state_control.h
// brief		:	FullMoni rev.C 	Xe[gΗ
// author		:	Tomoya Sato
// update		:	2021/02/21
// version		:	1.06
// --------------------------------------------------------------------

// --------------------------------------------------------------------
// ½dCN[hh~
// --------------------------------------------------------------------
#ifndef _STATECONTROL_H_
#define _STATECONTROL_H_

// --------------------------------------------------------------------
// externιΎ
// --------------------------------------------------------------------
extern volatile char			version[5];
extern volatile unsigned int	g_oneshot_flg;
extern volatile unsigned int	g_drawbuff_flg;
extern volatile long			g_int50mscnt;
extern volatile unsigned int	touch_done_flg;
extern volatile unsigned int	touch_drag_flg;
extern volatile unsigned int	g_PressX, g_PressY;
extern volatile unsigned int	g_DragX, g_DragY;
extern volatile unsigned int	g_refresh_done;
extern volatile unsigned long	gUART_FIFO_Tx_WP;
extern volatile unsigned long	gUART_FIFO_Tx_RP;
extern volatile unsigned long	gUART_FIFO_Tx_DC;
extern volatile unsigned char	sci_rcv[154];
extern volatile unsigned int	sci_rcv_pointer;
extern volatile unsigned int	g_can_error_cnt;

// --------------------------------------------------------------------
// defineιΎ
// --------------------------------------------------------------------
#define Beep_OneShotMin() g_beep_oneshotmin_flg = 1
#define Beep_OneShotMax() g_beep_oneshotmax_flg = 1
#define Beep_TwoShotMin() g_beep_twoshotmin_flg = 1

// --------------------------------------------------------------------
// define}NΦιΎ
// --------------------------------------------------------------------
#define LCD_RefreshFast() if(g_drawbuff_flg==0){g_drawbuff_flg=1;}else{g_drawbuff_flg=0;}

// --------------------------------------------------------------------
// ρ^ιΎ
// --------------------------------------------------------------------
static enum state
{
	S000 =   0,
	S001 =   1,
	S002 =   2,
	S010 =  10,
	S020 =  20,
	S110 = 110,
	S210 = 210,
	S120 = 120,
	S130 = 130,
	S999 = 999
} g_state;

static enum model
{
	CAN_std		= 0,
	MoTeC1		= 1,
	MoTeC2		= 2,
	Haltech1	= 3,
	Haltech2	= 4,
	Freedom1	= 5,
	Freedom2	= 6
//	MSquirt1	= 7
} g_model;

// --------------------------------------------------------------------
// vg^CvιΎ
// --------------------------------------------------------------------
void state_control(void);
void funcS001(void);
void funcS002(void);
void funcS010(void);
void funcS020(void);
void funcS110(void);
void funcS210(void);
void funcS120(void);
void funcS130(void);
void funcS999(void);
unsigned int area_judge(unsigned int X, unsigned int X1, unsigned int X2, unsigned int Y, unsigned int Y1, unsigned int Y2);
unsigned int E001(void);
unsigned int E002(void);
unsigned int E010(void);
unsigned int E020(void);
unsigned int E100(void);
unsigned int E110(void);
unsigned int E130(void);
unsigned int E200(void);
unsigned int E990(void);
void A001(void);
void A002(void);
void A100(void);
void A200(void);
void A011(void);
void A012(void);
void A013(void);
void A014(void);
void A015(void);
void A016(void);
void A017(void);
void A018(void);
void A019(void);
void A020(void);
void A021(void);
void A022(void);
void A023(void);
void A024(void);
void A101(void);
void A102(void);
void A103(void);
void A104(void);
void A121(void);
void A122(void);
void A123(void);
void A130(void);
void A131(void);
void A132(void);
void A133(void);
void A134(void);
void A201(void);
void A202(void);
void A990(void);
void A991(void);
static void Init_CAN(void);
static void Init_MoTeC1(void);
static void Init_MoTeC2(void);
static void Init_Haltech1(void);
static void Init_Haltech2(void);
static void Init_Freedom2(void);
//static void Init_MSquirt1(void);
static void Preset_load_CAN(void);
static void Preset_load_MoTeC1(void);
//static void Preset_load_MoTeC2(void);
static void Preset_load_Haltech1(void);
static void Preset_load_Haltech2(void);
static void Preset_load_Freedom1(void);
static void Preset_load_Freedom2(void);
//static void Preset_load_MegaSquirt1(void);
static void Save_EEPROM_ALL(void);
static int num_calc(unsigned char num_calc_data_select, int num_calc_bias, int num_calc_gain);

// --------------------------------------------------------------------
// ½dCN[hh~
// --------------------------------------------------------------------
#endif /* _STATECONTROL_H_ */
