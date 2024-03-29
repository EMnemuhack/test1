/***********
**  LED_MATLIX
**  五嶋隆文
************/

#include <stm32f10x.h>		//GPIOAやクロックの定義が記載されたヘッダーファイル	


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

//送信用配列
static unsigned long long ledData[COL];

//データ格納用配列	米印 星　星　星
static unsigned char dispData[4][32] = {
																	{0x00,0x00,0x00,0x00,0x21,0x84,0x11,0x88,0x08,0x10,0x04,0x20,0x02,0x40,0x31,0x8C,0x31,0x8C,0x02,0x40,0x04,0x20,0x08,0x10,0x11,0x88,0x21,0x84,0x00,0x00,0x00,0x00},
																	{0x00,0x00,0x01,0x00,0x01,0x00,0x03,0x80,0x03,0x80,0x07,0xC0,0x7F,0xFC,0x3F,0xF8,0x1F,0xF0,0x0F,0xE0,0x0F,0xE0,0x1E,0xF0,0x1C,0x70,0x38,0x38,0x20,0x08,0x00,0x00},
																	{0x00,0x00,0x01,0x00,0x01,0x00,0x03,0x80,0x03,0x80,0x07,0xC0,0x7F,0xFC,0x3F,0xF8,0x1F,0xF0,0x0F,0xE0,0x0F,0xE0,0x1E,0xF0,0x1C,0x70,0x38,0x38,0x20,0x08,0x00,0x00},
																	{0x00,0x00,0x01,0x00,0x01,0x00,0x03,0x80,0x03,0x80,0x07,0xC0,0x7F,0xFC,0x3F,0xF8,0x1F,0xF0,0x0F,0xE0,0x0F,0xE0,0x1E,0xF0,0x1C,0x70,0x38,0x38,0x20,0x08,0x00,0x00}
																		};

	
	
	
int main (void){
	int i, j;
	
	//配列成形
	for(i = 0; i < 16; i=i+2){
		for(j = 0; j < 4; i++){
			ledData[ledDataCount] = dispData[i][j];
			//データの右(4~7)を格納	8bit左シフト
			ledData[ledDataCount] |= (dispData[i+(j%2)][j] << (8*j)); 
		}
		//データ格納カウントを増やす
		ledDataCount++;
	}
	
	
	//ENABLE有効
	GPIOB->ODR &= (~ENABLE);	//(NOT)0x02 とのアンドをとる
	
	for(i = 0; i < COL; i++){
		//最下位bitの判定
		for(j = 0; j < ROW; j++){
			if(ledData[i] & 0x01 == 1){
				GPIOB->ODR |= S_IN;
			}
			else{
				GPIOB->ODR &= (~S_IN);
			}
			
			//PB出力?
			
			//クロック有効
			//ENABLE有効
			GPIOB->ODR |= CLOCK;
			
			//1bitシフト
			ledData[i] = ledData[i] >> 1;
		}
		//LATCHを一瞬だけ有効
		GPIOB->ODR |= LATCH;
		wait(10000);
		GPIOB->ODR &= (~LATCH);
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
	GPIOB->ODR &= (ENABLE);	//(NOT)0x02 とのアンドをとる
	
	
}