/***********
**  LED_MATLIX
**  五嶋隆文
************/

#include <stm32f10x.h>		//GPIOAやクロックの定義が記載されたヘッダーファイル	
#include <string.h>

#define COL 16
#define ROW 64
#define DISPBLANK 64
#define DISPLINE 64
#define ONELINE 64

#define ENABLE	0x08	//PB3
#define LATCH		0x04	//PB2
#define S_IN		0x02	//PB1
#define CLOCK		0x01	//PB0

//プロトタイプ宣言
void PortInit(void);
void wait(int time);
unsigned long long createOneLineData(unsigned long long orgData,unsigned long long addData, int shiftCount);


//グローバル変数
static int ledDataCount = 0;
static int byteCount = 0;
static int moveUpCount = 0;
static int moveShakeCount = 0;

//送信用配列(8Byte * 16 = 128Byte)
static unsigned long long ledData[COL];
static unsigned long long ledAddData[COL];


//データ格納用配列(4 * 32Byte = 128Byte)
static unsigned char dispData[4][32] = {
{0x01,0x00,0x01,0x00,0x01,0x00,0xFF,0xFF,0x08,0x10,0x08,0x10,0x0C,0x30,0x04,0x20,0x06,0x60,0x02,0xC0,0x01,0x80,0x03,0xC0,0x06,0x60,0x0C,0x30,0x38,0x1C,0xE0,0x07},
{0x00,0x80,0x7C,0x80,0x45,0xFC,0x4D,0x0C,0x4A,0x98,0x58,0x70,0x50,0xDC,0x4B,0x87,0x48,0xA0,0x49,0xFE,0x4A,0x20,0x49,0xFC,0x58,0x20,0x40,0x20,0x47,0xFF,0x40,0x00},
{0x00,0x20,0x20,0x40,0x21,0xFC,0xA9,0x04,0xA9,0xFC,0xA9,0x04,0xA9,0xFC,0xA9,0x00,0xA9,0xFF,0xA9,0x00,0xA9,0xFE,0xF8,0x02,0x02,0xAA,0x06,0xAA,0x04,0x06,0x00,0x0C},
{0x00,0x00,0x7F,0xFE,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x3F,0xF0,0x04,0x10,0x04,0x10,0x04,0x10,0x04,0x10,0x0C,0x10,0x08,0x10,0x08,0x10,0xFF,0xFF,0x00,0x00}
};

static unsigned char dispAddData[4][32] = {
{0x08,0x00,0x08,0x00,0x1F,0xFE,0x30,0x80,0x60,0x80,0xC0,0x80,0x1F,0xFC,0x10,0x80,0x10,0x80,0x10,0x80,0x10,0x80,0xFF,0xFF,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80},
{0x00,0x00,0x00,0x00,0x7F,0xFE,0x44,0x42,0x44,0x42,0x44,0x42,0x44,0x42,0x4C,0x42,0x48,0x62,0x58,0x3E,0x70,0x02,0x40,0x02,0x40,0x02,0x7F,0xFE,0x00,0x00,0x00,0x00},
{0x03,0x00,0x0E,0x00,0xF8,0x7E,0x08,0x42,0x08,0x42,0xFF,0x42,0x08,0x42,0x1C,0x42,0x1A,0x42,0x29,0x42,0x68,0x42,0xC8,0x42,0x08,0x42,0x08,0x7E,0x08,0x00,0x08,0x00},
{0x01,0x00,0x03,0x80,0x06,0xC0,0x0C,0x60,0x18,0x30,0x30,0x1C,0xEF,0xE7,0x00,0x00,0x00,0x00,0x7F,0xFC,0x01,0x04,0x01,0x04,0x01,0x04,0x01,0x1C,0x01,0x00,0x01,0x00}
};

static unsigned char blankData[4][32] = {
{0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0},
{0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0},
{0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0},
{0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0},
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
*	スライドの方法を考える 
* 複数文字を出力する方法を考える
*	プログラムをきれいに書く
*/

	
	
unsigned long long data;
unsigned long long buf;	

int main (void){
int i, j, flg = 0, blankFlg = 0;		
	
	PortInit();
	
	//配列初期化
	memset(ledData, 0, COL * sizeof(ledData[0]));
	memset(ledAddData, 0, COL * sizeof(ledAddData[0]));
	
// ====================================================================================				
// 配列成形			
// ====================================================================================			
	for(i = 0; i < 32; i=i+2){
		byteCount = 0;
		for(j = 0; j < 4; j++){
			
			//データの左(0~3)を格納
			ledData[ledDataCount] |= ((long long)dispData[j][i+1]) << (8*byteCount);
			ledAddData[ledDataCount] |= ((long long)dispAddData[j][i+1]) << (8*byteCount);
			
			//データの右(4~7)を格納	8bit左シフト
			ledData[ledDataCount] |= ((long long)(dispData[j][i]) << (8*(byteCount+1)));
			ledAddData[ledDataCount] |= ((long long)dispAddData[j][i]) << (8*(byteCount+1));
			
			//格納Byteカウント
			byteCount+=2;
		}
		//8Byte格納完了
		ledDataCount++;
	}
	
// ====================================================================================				
// LEDマトリクス処理			
// ====================================================================================			
	
	
	//ENABLE有効
	GPIOB->ODR &= (~ENABLE);	//(NOT)0x02 とのアンドをとる
	
	while(1){
		
		for(i = 0; i < COL; i++){
			//ひとつ前のデータを保持
			
			
			//配列の空白の部分に次の文字を足す
			if(moveShakeCount < 64)
				data = createOneLineData(ledData[i], ledAddData[i], moveShakeCount);
			else if(moveShakeCount < 128)
				data = createOneLineData(ledAddData[i], ledData[i], moveShakeCount);
			else{
				data = data >> (moveShakeCount % 128);
			}
			
			buf = data;
			//行の決定(これをforの最後に持っていくと、初期値が15になる)
			GPIOA->ODR = i;
			
			//データの送出
			for(j = 0; j < ROW; j++){
				if(data & 0x01){
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
				data = data >> 1;
			}
			//データの送出完了
			
			//LATCHを一瞬だけ有効
			GPIOB->ODR &= (~LATCH);
			wait(6500);
			GPIOB->ODR |= LATCH;
		}
		
		if(moveShakeCount == 140){
			moveShakeCount = 0;
		}
		else{
			moveShakeCount++;
		}
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

//------------------------------------------------
//  概　要：配列変換
//  引  数：fontデータ
//  戻り値：行配列
//------------------------------------------------
/*unsigned long long font_to_array(unsigned long long fontData[], int row){
	fontData[] 
	
}*/

//------------------------------------------------
//  概　要：1行の配列データを作成する
//  引  数：1:元データ 2:追加データ 3:シフト数
//  戻り値：追加後のデータ (シフト数分シフトした元データ) + (シフト数分)追加データ
//------------------------------------------------
unsigned long long createOneLineData(unsigned long long orgData,unsigned long long addData, int shiftCount){
	unsigned long long buf = 0;
	
	//元データ 左シフト
		buf |= orgData << shiftCount % 64;
		buf |= addData >> (ONELINE - (shiftCount % 64));
	
	return buf;
}