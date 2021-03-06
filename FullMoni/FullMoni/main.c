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
// filename		:	main.c
// brief		:	FullMoni rev.C メインルーチン
// author		:	Tomoya Sato
// update		:	2021/02/21
// version		:	1.06
// --------------------------------------------------------------------

// --------------------------------------------------------------------
// プロトタイプ宣言
// --------------------------------------------------------------------
void main(void);
#ifdef __cplusplus
extern "C" {
void abort(void);
}
#endif

// --------------------------------------------------------------------
// システムヘッダファイル
// --------------------------------------------------------------------
//#include <machine.h>

// --------------------------------------------------------------------
// ユーザーヘッダファイル
// --------------------------------------------------------------------
#include "dataregister.h"
#include "iodefine.h"
#include "init_system.h"
#include "TFTLCD35.h"
#include "CAN2515.h"
#include "spi.h"
#include "state_control.h"

// --------------------------------------------------------------------
// グローバル変数宣言
// --------------------------------------------------------------------
volatile char version[5] = "v1.06";

// --------------------------------------------------------------------
// グローバル構造体宣言
// --------------------------------------------------------------------
volatile e2p_data_t			g_e2p_data;
volatile CAN_data_t			g_CAN_data;
volatile MoTeC1_data_t		g_MoTeC1_data;
volatile Haltech1_data_t	g_Haltech1_data;
volatile Haltech2_data_t	g_Haltech2_data;
volatile Freedom2_data_t	g_Freedom2_data;
volatile Megasquirt1_data_t	g_Megasquirt1_data;

// --------------------------------------------------------------------
// メインルーチン
// --------------------------------------------------------------------
void main(void)
{
	// --------------------------------------------------------------------
	// 各種機能初期化
	// --------------------------------------------------------------------
	Init_CPG();
	Init_INTC();
	Init_MSTP();
	Init_GPIO();
	Init_BSC();
	Init_EXDMAC();
	Init_DMAC();
	Init_TPU();
//	Init_UART();	//モデル別対応の為state_controlで実行
	Init_TFTLCD();
	Init_SPI();
//	Init_CAN();		//モデル別対応の為state_controlで実行
	Init_ADS7843();
	Init_IIC();
	
	// --------------------------------------------------------------------
	// データ初期化
	// --------------------------------------------------------------------
	Init_CAN_data();
	Init_MoTeC1_data();
	Init_Haltech1_data();
	Init_Haltech2_data();
	Init_Freedom2_data();
//	Init_MSquirt1_data();

	// --------------------------------------------------------------------
	// FONTデータ展開(Flash→内部RAM)
	// --------------------------------------------------------------------
	FONT_open();
	
	// --------------------------------------------------------------------
	// 初期ステート定義
	// --------------------------------------------------------------------
	A001();
	
	// --------------------------------------------------------------------
	// メインループ
	// --------------------------------------------------------------------
	while(1)
	{
		state_control();		// 状態別処理実行
	}
}

// --------------------------------------------------------------------
// インライン関数宣言
// --------------------------------------------------------------------
#pragma inline (Touch_Decision)
#pragma inline (Beep_OneShotMin_control)
#pragma inline (Beep_OneShotMax_control)
#pragma inline (Beep_TwoShotMin_control)

// --------------------------------------------------------------------
// インライン関数プロトタイプ宣言
// --------------------------------------------------------------------
static void Touch_Decision(void);
static void Beep_OneShotMin_control(void);
static void Beep_OneShotMax_control(void);
static void Beep_TwoShotMin_control(void);

// --------------------------------------------------------------------
// グローバル変数宣言
// --------------------------------------------------------------------
volatile long			g_int50mscnt;
volatile unsigned int	touch_done_flg;
volatile unsigned int	touch_drag_flg;
volatile unsigned int	g_PressX, g_PressY;
volatile unsigned int	g_DragX, g_DragY;
volatile unsigned int	g_refresh_done;
volatile unsigned int	g_led_o_max_flg;
volatile unsigned int	g_led_o_min_flg;
volatile unsigned int	g_led_r_max_flg;
volatile unsigned int	g_led_r_min_flg;
volatile unsigned int	g_can_rcv_timer;
volatile unsigned int	g_can_error_cnt;
static unsigned int		touch_cnt;
static unsigned int		Beep_OneShotMin_cnt;
static unsigned int		Beep_OneShotMax_cnt;
static unsigned int		Beep_TwoShotMin_cnt;
static unsigned int		master_warning_cnt;
static unsigned int		g_shift_timing_cnt;
static unsigned int		LEDonoff_cnt;

// --------------------------------------------------------------------
// MCP2515 CAN IRQ 外部割り込み
// --------------------------------------------------------------------
void Int_CAN2515(void)
{
	// --------------------------------------------------------------------
	// ローカル変数宣言
	// --------------------------------------------------------------------
	unsigned char CAN_rcv_INTF, can_rcv[14];
	signed short msgID;
	static unsigned int can_rcv_cnt;
	
	// --------------------------------------------------------------------
	// 割り込みフラグクリア
	// --------------------------------------------------------------------
	INTC.ISR.BIT.IRQ3F = 0;
	
	// --------------------------------------------------------------------
	// MCP2515割り込み要因別処理
	// --------------------------------------------------------------------
	CAN_rcv_INTF = CANReadReg(CANINTF);
	
//	if((CAN_rcv_INTF & 0x20) == 0x20)	// エラー割り込みフラグビット(EFLGレジスタの複数要因)確定
//	{
		g_can_error_cnt = CANReadReg(REC);	// 受信エラーカウンターストア
//	}
//	else
//	{
//		g_can_error_cnt = 0;
//	}
	
	if((CAN_rcv_INTF & 0x02) == 0x02)	// 受信バッファ1フル割り込みフラグビット確定
	{
		CANRxB1Read(&can_rcv[1], 8);
		
		msgID = MAKE_SID(can_rcv[1], can_rcv[2]);		// ID作成
		
		if(Onetime_Peakclear_cnt > 0) Onetime_Peakclear_cnt --;
		
		if			(g_e2p_data.E2P_1.model == CAN_std)
		{
				if		((msgID == g_e2p_data.E2P_1.CAN_ID1) && (g_e2p_data.E2P_1.CANcontrol.BIT.CAN_ch1 == 1))
				{
							g_CAN_data.ID1.DATA1				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_CAN_data.ID1.DATA2				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_CAN_data.ID1.DATA3				= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_CAN_data.ID1.DATA4				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
				}
				else if	((msgID == g_e2p_data.E2P_1.CAN_ID2) && (g_e2p_data.E2P_1.CANcontrol.BIT.CAN_ch2 == 1))
				{
							g_CAN_data.ID2.DATA1				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_CAN_data.ID2.DATA2				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_CAN_data.ID2.DATA3				= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_CAN_data.ID2.DATA4				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
				}
				else if	((msgID == g_e2p_data.E2P_1.CAN_ID3) && (g_e2p_data.E2P_1.CANcontrol.BIT.CAN_ch3 == 1))
				{
							g_CAN_data.ID3.DATA1				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_CAN_data.ID3.DATA2				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_CAN_data.ID3.DATA3				= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_CAN_data.ID3.DATA4				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
				}
				else if	((msgID == g_e2p_data.E2P_1.CAN_ID4) && (g_e2p_data.E2P_1.CANcontrol.BIT.CAN_ch4 == 1))
				{
							g_CAN_data.ID4.DATA1				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_CAN_data.ID4.DATA2				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_CAN_data.ID4.DATA3				= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_CAN_data.ID4.DATA4				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
				}
				else if	((msgID == g_e2p_data.E2P_1.CAN_ID5) && (g_e2p_data.E2P_1.CANcontrol.BIT.CAN_ch5 == 1))
				{
							g_CAN_data.ID5.DATA1				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_CAN_data.ID5.DATA2				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_CAN_data.ID5.DATA3				= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_CAN_data.ID5.DATA4				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
				}
				else if	((msgID == g_e2p_data.E2P_1.CAN_ID6) && (g_e2p_data.E2P_1.CANcontrol.BIT.CAN_ch6 == 1))
				{
							g_CAN_data.ID6.DATA1				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_CAN_data.ID6.DATA2				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_CAN_data.ID6.DATA3				= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_CAN_data.ID6.DATA4				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
				}
		
		}
		else if	(g_e2p_data.E2P_1.model == MoTeC1)
		{
			switch(msgID)
			{
				case 0x5F0:
							if(g_can_rcv_timer >= 10)		// 前回受信時から10mSec以上経過している場合
							{
									can_rcv_cnt = 0;		//  0 データ先頭
									g_MoTeC1_data.RPM					= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									g_MoTeC1_data.ThrottlePosition		= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									g_MoTeC1_data.ManifoldPressure		= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									g_MoTeC1_data.InletAirTemp			= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
							}
							else
							{
								if		(can_rcv_cnt ==  1)	//  1
								{
									g_MoTeC1_data.EngineTemp			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									g_MoTeC1_data.Lambda1				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									g_MoTeC1_data.Lambda2				= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									g_MoTeC1_data.ExhaustPressure		= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  2)	//  2
								{
									g_MoTeC1_data.MassAirFlow			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									g_MoTeC1_data.FuelTemp				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									g_MoTeC1_data.FuelPressure			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									g_MoTeC1_data.OilTemp				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  3)	//  3
								{
									g_MoTeC1_data.OilPressure			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  4)	//  4
								{
									g_MoTeC1_data.ExhaustTemp1			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									g_MoTeC1_data.ExhaustTemp2			= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  5)	//  5
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									g_MoTeC1_data.BatteryVoltage		= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									g_MoTeC1_data.ECUTemp				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  6)	//  6
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  7)	//  7
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									g_MoTeC1_data.GroundSpeed			= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  8)	//  8
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  9)	//  9
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									g_MoTeC1_data.AimLambda1			= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									g_MoTeC1_data.AimLambda2			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 10)	// 10
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									g_MoTeC1_data.IgnitionAdvance		= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 11)	// 11
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 12)	// 12
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 13)	// 13
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									g_MoTeC1_data.InjectorDuty			= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 14)	// 14
								{
									g_MoTeC1_data.Gear					= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 15)	// 15
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 16)	// 16
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 17)	// 17
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
							}
							
							g_can_rcv_timer = 0;	// タイマーリセット
							default: break;
			}
		}
		else if	(g_e2p_data.E2P_1.model == MoTeC2)
		{
			switch(msgID)
			{
				case 0x0E8:
							if((can_rcv[6] == 0x82) && (can_rcv[7] == 0x81) && (can_rcv[8] == 0x80))
							{
								can_rcv_cnt = 0;
							}
							else
							{
								//
							}
							if		(can_rcv_cnt ==  0)	//  0
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								g_MoTeC1_data.RPM					= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								g_MoTeC1_data.ThrottlePosition		= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  1)	//  1
							{
								g_MoTeC1_data.ManifoldPressure		= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								g_MoTeC1_data.InletAirTemp			= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								g_MoTeC1_data.EngineTemp			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								g_MoTeC1_data.Lambda1				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  2)	//  2
							{
								g_MoTeC1_data.Lambda2				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								g_MoTeC1_data.ExhaustPressure		= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								g_MoTeC1_data.MassAirFlow			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								g_MoTeC1_data.FuelTemp				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  3)	//  3
							{
								g_MoTeC1_data.FuelPressure			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								g_MoTeC1_data.OilTemp				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								g_MoTeC1_data.OilPressure			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  4)	//  4
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								g_MoTeC1_data.ExhaustTemp1			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								g_MoTeC1_data.ExhaustTemp2			= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  5)	//  5
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  6)	//  6
							{
								g_MoTeC1_data.BatteryVoltage		= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								g_MoTeC1_data.ECUTemp				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  7)	//  7
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								g_MoTeC1_data.GroundSpeed			= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  8)	//  8
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  9)	//  9
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								g_MoTeC1_data.AimLambda1			= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 10)	// 10
							{
								g_MoTeC1_data.AimLambda2			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								g_MoTeC1_data.IgnitionAdvance		= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 11)	// 11
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 12)	// 12
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 13)	// 13
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 14)	// 14
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								g_MoTeC1_data.InjectorDuty			= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								g_MoTeC1_data.Gear					= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 15)	// 15
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 16)	// 16
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 17)	// 17
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 18)	// 18
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 19)	// 19
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 20)	// 20
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 21)	// 21
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							default: break;
			}
		}
		else if	(g_e2p_data.E2P_1.model == Haltech1)
		{
			switch(msgID)
			{
				case 0x010:
							g_Haltech1_data.RPM					= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech1_data.GroundSpeed			= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_Haltech1_data.OilPressure			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x011:
							g_Haltech1_data.EngineTemp			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech1_data.FuelPressure		= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_Haltech1_data.BatteryVoltage		= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_Haltech1_data.ThrottlePosition	= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x012:
							g_Haltech1_data.ManifoldPressure	= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech1_data.InletAirTemp		= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_Haltech1_data.Lambda				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x013:
							g_Haltech1_data.IgnitionAdvance		= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech1_data.Gear				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_Haltech1_data.InjectorDuty		= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							default: break;
			}
		}
		else if	(g_e2p_data.E2P_1.model == Haltech2)
		{
			switch(msgID)
			{
				case 0x360:
							g_Haltech2_data.RPM					= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech2_data.ManifoldPressure	= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_Haltech2_data.ThrottlePosition	= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x361:
							g_Haltech2_data.FuelPressure		= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech2_data.OilPressure			= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x362:
							g_Haltech2_data.InjectorDuty		= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_Haltech2_data.IgnitionAdvance1	= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_Haltech2_data.IgnitionAdvance2	= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x368:
							g_Haltech2_data.Lambda				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x370:
							g_Haltech2_data.GroundSpeed			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech2_data.Gear				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x372:
							g_Haltech2_data.BatteryVoltage		= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech2_data.InletAirTemp2		= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_Haltech2_data.BaroPressure		= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x373:
							g_Haltech2_data.EGT					= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x3E0:
							g_Haltech2_data.EngineTemp			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech2_data.InletAirTemp1		= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_Haltech2_data.FuelTemp			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_Haltech2_data.OilTemp				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							default: break;
			}
		}
	}
	if((CAN_rcv_INTF & 0x01) == 0x01)	// 受信バッファ0フル割り込みフラグビット確定
	{
		CANRxB0Read(&can_rcv[1], 8);
		
		msgID = MAKE_SID(can_rcv[1], can_rcv[2]);		// ID作成
		
		if(Onetime_Peakclear_cnt > 0) Onetime_Peakclear_cnt --;
		
		if			(g_e2p_data.E2P_1.model == CAN_std)
		{
				if		((msgID == g_e2p_data.E2P_1.CAN_ID1) && (g_e2p_data.E2P_1.CANcontrol.BIT.CAN_ch1 == 1))
				{
							g_CAN_data.ID1.DATA1				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_CAN_data.ID1.DATA2				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_CAN_data.ID1.DATA3				= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_CAN_data.ID1.DATA4				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
				}
				else if	((msgID == g_e2p_data.E2P_1.CAN_ID2) && (g_e2p_data.E2P_1.CANcontrol.BIT.CAN_ch2 == 1))
				{
							g_CAN_data.ID2.DATA1				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_CAN_data.ID2.DATA2				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_CAN_data.ID2.DATA3				= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_CAN_data.ID2.DATA4				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
				}
				else if	((msgID == g_e2p_data.E2P_1.CAN_ID3) && (g_e2p_data.E2P_1.CANcontrol.BIT.CAN_ch3 == 1))
				{
							g_CAN_data.ID3.DATA1				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_CAN_data.ID3.DATA2				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_CAN_data.ID3.DATA3				= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_CAN_data.ID3.DATA4				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
				}
				else if	((msgID == g_e2p_data.E2P_1.CAN_ID4) && (g_e2p_data.E2P_1.CANcontrol.BIT.CAN_ch4 == 1))
				{
							g_CAN_data.ID4.DATA1				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_CAN_data.ID4.DATA2				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_CAN_data.ID4.DATA3				= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_CAN_data.ID4.DATA4				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
				}
				else if	((msgID == g_e2p_data.E2P_1.CAN_ID5) && (g_e2p_data.E2P_1.CANcontrol.BIT.CAN_ch5 == 1))
				{
							g_CAN_data.ID5.DATA1				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_CAN_data.ID5.DATA2				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_CAN_data.ID5.DATA3				= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_CAN_data.ID5.DATA4				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
				}
				else if	((msgID == g_e2p_data.E2P_1.CAN_ID6) && (g_e2p_data.E2P_1.CANcontrol.BIT.CAN_ch6 == 1))
				{
							g_CAN_data.ID6.DATA1				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_CAN_data.ID6.DATA2				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_CAN_data.ID6.DATA3				= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_CAN_data.ID6.DATA4				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
				}
		}
		else if	(g_e2p_data.E2P_1.model == MoTeC1)
		{
			switch(msgID)
			{
				case 0x5F0:
							if(g_can_rcv_timer >= 10)		// 前回受信時から10mSec以上経過している場合
							{
									can_rcv_cnt = 0;		//  0 データ先頭
									g_MoTeC1_data.RPM					= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									g_MoTeC1_data.ThrottlePosition		= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									g_MoTeC1_data.ManifoldPressure		= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									g_MoTeC1_data.InletAirTemp			= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
							}
							else
							{
								if		(can_rcv_cnt ==  1)	//  1
								{
									g_MoTeC1_data.EngineTemp			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									g_MoTeC1_data.Lambda1				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									g_MoTeC1_data.Lambda2				= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									g_MoTeC1_data.ExhaustPressure		= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  2)	//  2
								{
									g_MoTeC1_data.MassAirFlow			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									g_MoTeC1_data.FuelTemp				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									g_MoTeC1_data.FuelPressure			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									g_MoTeC1_data.OilTemp				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  3)	//  3
								{
									g_MoTeC1_data.OilPressure			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  4)	//  4
								{
									g_MoTeC1_data.ExhaustTemp1			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									g_MoTeC1_data.ExhaustTemp2			= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  5)	//  5
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									g_MoTeC1_data.BatteryVoltage		= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									g_MoTeC1_data.ECUTemp				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  6)	//  6
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  7)	//  7
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									g_MoTeC1_data.GroundSpeed			= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  8)	//  8
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt ==  9)	//  9
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									g_MoTeC1_data.AimLambda1			= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									g_MoTeC1_data.AimLambda2			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 10)	// 10
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									g_MoTeC1_data.IgnitionAdvance		= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 11)	// 11
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 12)	// 12
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 13)	// 13
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									g_MoTeC1_data.InjectorDuty			= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 14)	// 14
								{
									g_MoTeC1_data.Gear					= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 15)	// 15
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 16)	// 16
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
								else if	(can_rcv_cnt == 17)	// 17
								{
									// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
									// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
									// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
									// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
									can_rcv_cnt ++;
								}
							}
							
							g_can_rcv_timer = 0;	// タイマーリセット
							default: break;
			}
		}
		else if	(g_e2p_data.E2P_1.model == MoTeC2)
		{
			switch(msgID)
			{
				case 0x0E8:
							if((can_rcv[6] == 0x82) && (can_rcv[7] == 0x81) && (can_rcv[8] == 0x80))
							{
								can_rcv_cnt = 0;
							}
							else
							{
								//
							}
							if		(can_rcv_cnt ==  0)	//  0
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								g_MoTeC1_data.RPM					= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								g_MoTeC1_data.ThrottlePosition		= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  1)	//  1
							{
								g_MoTeC1_data.ManifoldPressure		= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								g_MoTeC1_data.InletAirTemp			= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								g_MoTeC1_data.EngineTemp			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								g_MoTeC1_data.Lambda1				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  2)	//  2
							{
								g_MoTeC1_data.Lambda2				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								g_MoTeC1_data.ExhaustPressure		= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								g_MoTeC1_data.MassAirFlow			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								g_MoTeC1_data.FuelTemp				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  3)	//  3
							{
								g_MoTeC1_data.FuelPressure			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								g_MoTeC1_data.OilTemp				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								g_MoTeC1_data.OilPressure			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  4)	//  4
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								g_MoTeC1_data.ExhaustTemp1			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								g_MoTeC1_data.ExhaustTemp2			= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  5)	//  5
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  6)	//  6
							{
								g_MoTeC1_data.BatteryVoltage		= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								g_MoTeC1_data.ECUTemp				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  7)	//  7
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								g_MoTeC1_data.GroundSpeed			= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  8)	//  8
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt ==  9)	//  9
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								g_MoTeC1_data.AimLambda1			= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 10)	// 10
							{
								g_MoTeC1_data.AimLambda2			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								g_MoTeC1_data.IgnitionAdvance		= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 11)	// 11
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 12)	// 12
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 13)	// 13
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 14)	// 14
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								g_MoTeC1_data.InjectorDuty			= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								g_MoTeC1_data.Gear					= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 15)	// 15
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 16)	// 16
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 17)	// 17
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 18)	// 18
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 19)	// 19
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 20)	// 20
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							else if	(can_rcv_cnt == 21)	// 21
							{
								// N/A								= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
								// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
								// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
								// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
								can_rcv_cnt ++;
							}
							default: break;
			}
		}
		else if	(g_e2p_data.E2P_1.model == Haltech1)
		{
			switch(msgID)
			{
				case 0x010:
							g_Haltech1_data.RPM					= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech1_data.GroundSpeed			= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_Haltech1_data.OilPressure			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x011:
							g_Haltech1_data.EngineTemp			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech1_data.FuelPressure		= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_Haltech1_data.BatteryVoltage		= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_Haltech1_data.ThrottlePosition	= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x012:
							g_Haltech1_data.ManifoldPressure	= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech1_data.InletAirTemp		= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_Haltech1_data.Lambda				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x013:
							g_Haltech1_data.IgnitionAdvance		= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech1_data.Gear				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_Haltech1_data.InjectorDuty		= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							default: break;
			}
		}
		else if	(g_e2p_data.E2P_1.model == Haltech2)
		{
			switch(msgID)
			{
				case 0x360:
							g_Haltech2_data.RPM					= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech2_data.ManifoldPressure	= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_Haltech2_data.ThrottlePosition	= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x361:
							g_Haltech2_data.FuelPressure		= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech2_data.OilPressure			= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x362:
							g_Haltech2_data.InjectorDuty		= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_Haltech2_data.IgnitionAdvance1	= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_Haltech2_data.IgnitionAdvance2	= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x368:
							g_Haltech2_data.Lambda				= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x370:
							g_Haltech2_data.GroundSpeed			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech2_data.Gear				= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x372:
							g_Haltech2_data.BatteryVoltage		= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech2_data.InletAirTemp2		= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_Haltech2_data.BaroPressure		= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x373:
							g_Haltech2_data.EGT					= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							// N/A								= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							// N/A								= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							// N/A								= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							break;
				case 0x3E0:
							g_Haltech2_data.EngineTemp			= ((((unsigned int) can_rcv[6] ) << 8) + can_rcv[7] );
							g_Haltech2_data.InletAirTemp1		= ((((unsigned int) can_rcv[8] ) << 8) + can_rcv[9] );
							g_Haltech2_data.FuelTemp			= ((((unsigned int) can_rcv[10]) << 8) + can_rcv[11]);
							g_Haltech2_data.OilTemp				= ((((unsigned int) can_rcv[12]) << 8) + can_rcv[13]);
							default: break;
			}
		}
	}
	
	CANWriteReg(CANINTF, 0x00);
}

volatile unsigned long dmy;

// --------------------------------------------------------------------
// 定期割り込みルーチン 50mSec毎
// --------------------------------------------------------------------
void Int_50msFunc(void)
{
	// --------------------------------------------------------------------
	// ローカル変数宣言
	// --------------------------------------------------------------------
	unsigned long i;
	
	// --------------------------------------------------------------------
	// 割り込みフラグクリア
	// --------------------------------------------------------------------
	TPU0.TSR.BIT.TGFA = 0;
	
	// --------------------------------------------------------------------
	// TFT LCD リフレッシュ処理
	// --------------------------------------------------------------------
	Display_Home();
	g_refresh_done = 1;
	
	// --------------------------------------------------------------------
	// 50msecカウンタインクリメント
	// --------------------------------------------------------------------
	g_int50mscnt ++;
	
	// --------------------------------------------------------------------
	// マスターワーニング処理
	// --------------------------------------------------------------------
	if ((g_e2p_data.E2P_1.control.BIT.ms_on == 0)
	&& ((g_master_warning_flg1 == 1)
	||  (g_master_warning_flg2 == 1)
	||  (g_master_warning_flg3 == 1)
	||  (g_master_warning_flg4 == 1)
	||  (g_master_warning_flg5 == 1)
	||  (g_master_warning_flg6 == 1)))
	{
		master_warning_cnt ++;
		if(master_warning_cnt == 1)
		{
			Beep_OneShotMax();
		}
		else if(master_warning_cnt >= 10)
		{
			master_warning_cnt = 0;
		}
	}
	// --------------------------------------------------------------------
	// シフトタイミングランプ処理
	// --------------------------------------------------------------------
	else if(g_shift_timing_flg == 3)
	{
		g_shift_timing_cnt ++;
		
		if(g_shift_timing_cnt == 1)
		{
			Led_R_Min_ON();
			Led_O_Min_ON();
		}
		else if(g_shift_timing_cnt == 2)
		{
			Led_R_Min_OFF();
			Led_O_Min_OFF();
			g_shift_timing_cnt = 0;
		}
	}
	else if(g_shift_timing_flg == 2)
	{
		Led_R_Min_ON();
		Led_O_Min_ON();
	}
	else if(g_shift_timing_flg == 1)
	{
		Led_R_Min_OFF();
		Led_O_Min_ON();
	}
	else
	{
		master_warning_cnt = 0;
		Led_R_Min_OFF();
		Led_O_Min_OFF();
	}
	
	// --------------------------------------------------------------------
	// タッチパネル入力処理
	// --------------------------------------------------------------------
	Touch_Read();
	
	// --------------------------------------------------------------------
	// タッチパネル押下判断
	// --------------------------------------------------------------------
	Touch_Decision();
	
	// --------------------------------------------------------------------
	// ブザー制御
	// --------------------------------------------------------------------
	if(g_e2p_data.E2P_1.control.BIT.beep_on == 0)	// ブザー on/off 設定により有効の時
	{
		Beep_OneShotMin_control();					// 小ブザー 1ショット 処理
		Beep_OneShotMax_control();					// 大ブザー 1ショット 処理
		Beep_TwoShotMin_control();					// 小ブザー 2ショット 処理
	}
	else											// ブザー on/off 設定により無効の時
	{
		TPU3.TGRD = 0x0000;
		g_beep_oneshotmin_flg = 0;
		Beep_OneShotMin_cnt = 0;
		g_beep_oneshotmax_flg = 0;
		Beep_OneShotMax_cnt = 0;
		g_beep_twoshotmin_flg = 0;
		Beep_TwoShotMin_cnt = 0;
	}
/*
	// --------------------------------------------------------------------
	// FPS計測カウンタインクリメント
	// --------------------------------------------------------------------
	g_fps_cnt ++;
	if(g_fps_cnt == 20)
	{
		g_fps_max = g_fps;
		g_fps = 0;
		g_fps_cnt = 0;
	}
	else
	{
		//
*/
	
	// --------------------------------------------------------------------
	// EXDMA転送時間待機
	// --------------------------------------------------------------------
	for(i = 0; i <= 60000; i++)	dmy = i;
}


// --------------------------------------------------------------------
// タッチパネル押下判断
// --------------------------------------------------------------------
static void Touch_Decision(void)
{
	if((g_TouchX != 0) || (g_TouchY != 0))				// タッチ無しでは無い場合
	{
		if((g_TouchX != 999) || (g_TouchY != 999))		// 半押し判定では無い場合
		{
			touch_cnt ++;		// 押下時カウントアップ
			if(touch_cnt > 10)
			touch_cnt = 10;		// リミット
		}
		else
		{
			touch_cnt = 0;		// リリースクリア
		}
	}
	else
	{
		touch_cnt = 0;			// リリースクリア
	}
	
	if(touch_cnt == 1)			// 押下1回につき
	{
 		// マスターワーニング解除
 		if(g_master_warning_flg1 == 1)g_master_warning_flg1 = 2;
 		if(g_master_warning_flg2 == 1)g_master_warning_flg2 = 2;
 		if(g_master_warning_flg3 == 1)g_master_warning_flg3 = 2;
 		if(g_master_warning_flg4 == 1)g_master_warning_flg4 = 2;
 		if(g_master_warning_flg5 == 1)g_master_warning_flg5 = 2;
 		if(g_master_warning_flg6 == 1)g_master_warning_flg6 = 2;
		Beep_OneShotMin();		// ブザー小
		touch_done_flg = 1;		// タッチ押下判定発行
		g_PressX = g_TouchX;	// 押下X座標
		g_PressY = g_TouchY;
	}
	else
	{
		//
	}
	if(touch_cnt >= 2)			// 連続押下の時
	{
	//	Beep_OneShotMin();		// ブザー小
		touch_drag_flg = 1;		// ドラッグ判定発行
		g_DragX = g_TouchX;		// 押下X座標
		g_DragY = g_TouchY;
	}
	else
	{
		//
	}
}

// --------------------------------------------------------------------
// 小ブザー 1ショット 処理
// --------------------------------------------------------------------
static void Beep_OneShotMin_control(void)
{
	if(g_beep_oneshotmin_flg == 1)
	{
		g_beep_oneshotmin_flg = 2;
		Beep_OneShotMin_cnt = 0;
	}
	else
	{
		//
	}
	
	if(g_beep_oneshotmin_flg == 2)
	{
		TPU3.TGRD = 0x0100;
		Beep_OneShotMin_cnt ++;
	
		if(Beep_OneShotMin_cnt >= 2)	// 0.1sec経過
		{
			TPU3.TGRD = 0x0000;
			g_beep_oneshotmin_flg = 0;
			Beep_OneShotMin_cnt = 0;
		}
		else
		{
			//
		}
	}
}

// --------------------------------------------------------------------
// 大ブザー 1ショット 処理
// --------------------------------------------------------------------
static void Beep_OneShotMax_control(void)
{
	if(g_beep_oneshotmax_flg == 1)
	{
		g_beep_oneshotmax_flg = 2;
		Beep_OneShotMax_cnt = 0;
	}
	else
	{
		//
	}
	
	if(g_beep_oneshotmax_flg == 2)
	{
		TPU3.TGRD = 0x0BB8;
		Led_R_Min_ON();
		Led_O_Min_OFF();
		Beep_OneShotMax_cnt ++;
	
		if(Beep_OneShotMax_cnt >= 5)	// 0.25sec経過
		{
			TPU3.TGRD = 0x0000;
			Led_R_Min_OFF();
			Led_O_Min_OFF();
			g_beep_oneshotmax_flg = 0;
			Beep_OneShotMax_cnt = 0;
		}
		else
		{
			//
		}
	}
}

// --------------------------------------------------------------------
// 小ブザー 2ショット 処理
// --------------------------------------------------------------------
static void Beep_TwoShotMin_control(void)
{
	if(g_beep_twoshotmin_flg == 1)
	{
		g_beep_twoshotmin_flg = 2;
		Beep_TwoShotMin_cnt = 0;
	}
	else
	{
		//
	}
	
	if(g_beep_twoshotmin_flg == 2)
	{
		Beep_TwoShotMin_cnt ++;
	
		if     (Beep_TwoShotMin_cnt == 1)	// 0.00sec経過
		{
			TPU3.TGRD = 0x0100;
		}
		else if(Beep_TwoShotMin_cnt == 2)	// 0.05sec経過
		{
			TPU3.TGRD = 0x0000;
		}
		else if(Beep_TwoShotMin_cnt == 3)	// 0.10sec経過
		{
			TPU3.TGRD = 0x0100;
		}
		else if(Beep_TwoShotMin_cnt >= 4)	// 0.20sec経過
		{
			TPU3.TGRD = 0x0000;
			g_beep_twoshotmin_flg = 0;
			Beep_TwoShotMin_cnt = 0;
		}
		else
		{
			//
		}
	}
}

// --------------------------------------------------------------------
// 定期割り込みルーチン 1mSec毎
// --------------------------------------------------------------------
void Int_1msFunc(void)
{
	// --------------------------------------------------------------------
	// ローカル変数宣言
	// --------------------------------------------------------------------
	static unsigned long sci_rcv_cnt;
	
	// --------------------------------------------------------------------
	// 割り込みフラグクリア
	// --------------------------------------------------------------------
	TPU6.TSR.BIT.TGFA = 0;
	
	// --------------------------------------------------------------------
	// LED点灯処理
	// --------------------------------------------------------------------
	if		(LEDonoff_cnt == 0)
	{
		if((g_led_o_max_flg) || (g_led_o_min_flg)) { PM.DR.BIT.B0 = 1;} else { PM.DR.BIT.B0 = 0; }
		if((g_led_r_max_flg) || (g_led_r_min_flg)) { PB.DR.BIT.B3 = 1;} else { PB.DR.BIT.B3 = 0; }
	}
	// --------------------------------------------------------------------
	// LED減光処理
	// --------------------------------------------------------------------
	else if((LEDonoff_cnt >= 1) && (LEDonoff_cnt <= 19))
	{
		if((backlight_dimmer_flg == 1) && ((g_led_o_max_flg) || (g_led_o_min_flg))) PM.DR.BIT.B0 = 0;
		if((backlight_dimmer_flg == 1) && ((g_led_r_max_flg) || (g_led_r_min_flg))) PB.DR.BIT.B3 = 0;
	}
	
	LEDonoff_cnt ++;
	if(LEDonoff_cnt > 19) LEDonoff_cnt = 0;
	
	// --------------------------------------------------------------------
	// SCI5 コマンド処理
	// --------------------------------------------------------------------
	if		(g_e2p_data.E2P_1.model == Freedom2)
	{
		if(g_e2p_data.E2P_1.control.BIT.FC_mode == 0)	// FC_mode FAST
		{
			sci_rcv_cnt ++;
			if		(sci_rcv_cnt ==   1)
			{
				UART_Tx_Char(0x3F);		// ?
				UART_Tx_Char(0x44);		// D
				UART_Tx_Char(0x41);		// A
				UART_Tx_Char(0x54);		// T
				UART_Tx_Char(0x0D);		// [CR]
				sci_rcv_command = 0;
				sci_rcv_pointer = 0;
			}
			else if (sci_rcv_cnt >=  70) //  単一コードで高速化
			{
				sci_rcv_cnt = 0;
			}
			else
			{
				//
			}
		}
		else											// FC_mode ALL
		{
			sci_rcv_cnt ++;
			if		(sci_rcv_cnt ==   1)
			{
				UART_Tx_Char(0x3F);		// ?
				UART_Tx_Char(0x44);		// D
				UART_Tx_Char(0x41);		// A
				UART_Tx_Char(0x54);		// T
				UART_Tx_Char(0x0D);		// [CR]
				sci_rcv_command = 0;
				sci_rcv_pointer = 0;
			}
			else if	(sci_rcv_cnt ==  70)
			{
				UART_Tx_Char(0x3F);		// ?
				UART_Tx_Char(0x48);		// H
				UART_Tx_Char(0x4F);		// O
				UART_Tx_Char(0x53);		// S
				UART_Tx_Char(0x0D);		// [CR]
				sci_rcv_command = 1;
				sci_rcv_pointer = 0;
			}
			else if (sci_rcv_cnt >= 140)
			{
				sci_rcv_cnt = 0;
			}
			else
			{
				//
			}
		}
/*	}
	else if	(g_e2p_data.E2P_1.model == MSquirt1)
	{
		sci_rcv_cnt ++;
//		if		(sci_rcv_cnt ==   1)
//		{
//			UART_Tx_Char(0x41);		// A
//			sci_rcv_pointer = 0;
//		}
//		else if (sci_rcv_cnt >=  50) //  50mSec周期 20Hz
//		{
//			sci_rcv_cnt = 0;
//		}
//		else
//		{
//			//
//		}*/
	}
	else
	{
		//
	}
	
	// --------------------------------------------------------------------
	// CAN 時分割処理用タイマー
	// --------------------------------------------------------------------
	g_can_rcv_timer ++;
	if(g_can_rcv_timer > 10) g_can_rcv_timer = 10;
}

#ifdef __cplusplus
void abort(void)
{

}
#endif

