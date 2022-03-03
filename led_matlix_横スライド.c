/***********
**  LED_MATLIX
**  五嶋隆文
************/

#include <stm32f10x.h>		//GPIOAやクロックの定義が記載されたヘッダーファイル	
#include <string.h>

#define COL 16
#define ROW 64

#define ENABLE	0x08	//PB3
#define LATCH		0x04	//PB2
#define S_IN		0x02	//PB1
#define CLOCK		0x01	//PB0

//プロトタイプ宣言
void PortInit(void);
void wait(int time);


//グローバル変数
static int ledDataCount = 0;
static int byteCount = 0;
static int moveUpCount = 0;
static int moveShakeCount = 0;

//送信用配列(8Byte * 16 = 128Byte)
static unsigned long long ledData[COL];

//データ格納用配列(4 * 32Byte = 128Byte)
static unsigned char dispData[4][32] = {
{0x01,0x00,0x01,0x00,0x01,0x00,0xFF,0xFF,0x08,0x10,0x08,0x10,0x0C,0x30,0x04,0x20,0x06,0x60,0x02,0xC0,0x01,0x80,0x03,0xC0,0x06,0x60,0x0C,0x30,0x38,0x1C,0xE0,0x07},
{0x00,0x80,0x7C,0x80,0x45,0xFC,0x4D,0x0C,0x4A,0x98,0x58,0x70,0x50,0xDC,0x4B,0x87,0x48,0xA0,0x49,0xFE,0x4A,0x20,0x49,0xFC,0x58,0x20,0x40,0x20,0x47,0xFF,0x40,0x00},
{0x00,0x20,0x20,0x40,0x21,0xFC,0xA9,0x04,0xA9,0xFC,0xA9,0x04,0xA9,0xFC,0xA9,0x00,0xA9,0xFF,0xA9,0x00,0xA9,0xFE,0xF8,0x02,0x02,0xAA,0x06,0xAA,0x04,0x06,0x00,0x0C},
{0x00,0x00,0x7F,0xFE,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x3F,0xF0,0x04,0x10,0x04,0x10,0x04,0x10,0x04,0x10,0x0C,0x10,0x08,0x10,0x08,0x10,0xFF,0xFF,0x00,0x00}
};
																	//test用
																	/*static unsigned char dispData[4][32] = {
																	{15	,	0	,	1	,	14	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0},
																	{0	,	0	,	2	,	14	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0},
																	{0	,	0	,	3	,	14	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0},
																	{0x00,0x00,0x7F,0xFE,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x3F,0xF0,0x04,0x10,0x04,0x10,0x04,0x10,0x04,0x10,0x0C,0x10,0x08,0x10,0x08,0x10,0xFF,0xFF,0x00,0x00}
																		};*/
																		
/*
*	LEDマトリクス概要
*	マトリクス順は正面から見て　4文字目　3文字目　2文字目　1文字目　の順で並んでいる
*	マトリクス１枚は正面から見て　左に2バイト目　右に1バイト目 を送る
*	GPIOA>-ODRは表示する行を示している
*
*
*/

	
	
	
int main (void){
	int i, j, flg = 0;
	unsigned long long buf;
	
	PortInit();
	
	//配列初期化
	memset(ledData, 0, COL * sizeof(ledData[0]));
	
	//配列成形
	for(i = 0; i < 32; i=i+2){
		byteCount = 0;
		for(j = 0; j < 4; j++){
			
			//データの左(0~3)を格納
			ledData[ledDataCount] |= ((long long)dispData[j][i+1]) << (8*byteCount);
			//データの右(4~7)を格納	8bit左シフト
			ledData[ledDataCount] |= ((long long)(dispData[j][i]) << (8*(byteCount+1)));
			
			//格納Byteカウント
			byteCount+=2;
		}
		//8Byte格納完了
		ledDataCount++;
	}
	
	//行決め　テストなので1行目固定 PA0~PA2 = L	PA3 = L
	//GPIOA->ODR |=0x01 ;
	
	
	
	//ENABLE有効
	GPIOB->ODR &= (~ENABLE);	//(NOT)0x02 とのアンドをとる
	
	while(1){
		for(i = 0; i < COL; i++){
			//配列のコピーと横スライド
			buf = ledData[i] << moveShakeCount;
			
			//横スライドでオーバーフローした部分を付け足す
			buf |= ledData[i] >> (64 - moveShakeCount);
			
			
			//行の決定(これをforの最後に持っていくと、初期値が15になる)
			GPIOA->ODR = i + (moveUpCount/ 2);
			
			//データの送出
			for(j = 0; j < ROW; j++){
				if(buf & 0x01){
					GPIOB->ODR |= S_IN;
				}
				else{
					GPIOB->ODR &= (~S_IN);
				}
				
				//PB出力?
				
				//クロック有効
				GPIOB->ODR |= CLOCK;
				GPIOB->ODR &= (~CLOCK);
				
				//1bitシフト
				buf = buf >> 1;
			}
			//データの送出完了
			
			//LATCHを一瞬だけ有効
			GPIOB->ODR &= (~LATCH);
			wait(6500);
			GPIOB->ODR |= LATCH;
		}
		
		/*//フラグ管理
		if(flg == 1)
			moveUpCount--;
		else
			moveUpCount++;
		
		if(moveUpCount == 100)
			flg = 1;
		else if(moveUpCount == 0)
			flg = 0;*/
		
		//フラグ管理
		/*if(flg == 1)
			moveShakeCount--;
		else
			moveShakeCount++;
		
		if(moveShakeCount == 64)
			flg = 1;
		else if(moveShakeCount == 0)
			flg = 0;*/
		
		if(moveShakeCount == 64)
			moveShakeCount = 0;
		
		moveShakeCount++;
			
			
	}
}

void wait(int time){
	int i=0;
	for(i=0;i <= time;i++){
	}
}

void PortInit(void){
	//		初期処理
	//１　クロックを送る
	
	//		Aポートの７から８
	RCC->APB2ENR=RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;	// 0x0C と同義　ENはイネーブル
	
	//２	ポート設定　
	
	GPIOA->CRL=0x33333333;		//Aポート	 0から7までは7SEGのアノードに繋がっているため3(出力)
	GPIOA->CRH=0x44444444;		//Aポート　8から11まではSWが繋がっているため4（入力）
	GPIOB->CRL=0x44333333;		//Bポート　CRLの３ビット目までは７SEGカソードにつながっているため 3(出力) それ以外は4(出力)
	AFIO->MAPR=0x02000000;		//AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

	//ENABLE有効
	GPIOB->ODR = 0x0000;
	GPIOA->ODR = 0x0000;
}