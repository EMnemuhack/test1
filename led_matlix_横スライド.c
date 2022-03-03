/***********
**  LED_MATLIX
**  �ܓ�����
************/

#include <stm32f10x.h>		//GPIOA��N���b�N�̒�`���L�ڂ��ꂽ�w�b�_�[�t�@�C��	
#include <string.h>

#define COL 16
#define ROW 64

#define ENABLE	0x08	//PB3
#define LATCH		0x04	//PB2
#define S_IN		0x02	//PB1
#define CLOCK		0x01	//PB0

//�v���g�^�C�v�錾
void PortInit(void);
void wait(int time);


//�O���[�o���ϐ�
static int ledDataCount = 0;
static int byteCount = 0;
static int moveUpCount = 0;
static int moveShakeCount = 0;

//���M�p�z��(8Byte * 16 = 128Byte)
static unsigned long long ledData[COL];

//�f�[�^�i�[�p�z��(4 * 32Byte = 128Byte)
static unsigned char dispData[4][32] = {
{0x01,0x00,0x01,0x00,0x01,0x00,0xFF,0xFF,0x08,0x10,0x08,0x10,0x0C,0x30,0x04,0x20,0x06,0x60,0x02,0xC0,0x01,0x80,0x03,0xC0,0x06,0x60,0x0C,0x30,0x38,0x1C,0xE0,0x07},
{0x00,0x80,0x7C,0x80,0x45,0xFC,0x4D,0x0C,0x4A,0x98,0x58,0x70,0x50,0xDC,0x4B,0x87,0x48,0xA0,0x49,0xFE,0x4A,0x20,0x49,0xFC,0x58,0x20,0x40,0x20,0x47,0xFF,0x40,0x00},
{0x00,0x20,0x20,0x40,0x21,0xFC,0xA9,0x04,0xA9,0xFC,0xA9,0x04,0xA9,0xFC,0xA9,0x00,0xA9,0xFF,0xA9,0x00,0xA9,0xFE,0xF8,0x02,0x02,0xAA,0x06,0xAA,0x04,0x06,0x00,0x0C},
{0x00,0x00,0x7F,0xFE,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x3F,0xF0,0x04,0x10,0x04,0x10,0x04,0x10,0x04,0x10,0x0C,0x10,0x08,0x10,0x08,0x10,0xFF,0xFF,0x00,0x00}
};
																	//test�p
																	/*static unsigned char dispData[4][32] = {
																	{15	,	0	,	1	,	14	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0},
																	{0	,	0	,	2	,	14	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0},
																	{0	,	0	,	3	,	14	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0},
																	{0x00,0x00,0x7F,0xFE,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x3F,0xF0,0x04,0x10,0x04,0x10,0x04,0x10,0x04,0x10,0x0C,0x10,0x08,0x10,0x08,0x10,0xFF,0xFF,0x00,0x00}
																		};*/
																		
/*
*	LED�}�g���N�X�T�v
*	�}�g���N�X���͐��ʂ��猩�ā@4�����ځ@3�����ځ@2�����ځ@1�����ځ@�̏��ŕ���ł���
*	�}�g���N�X�P���͐��ʂ��猩�ā@����2�o�C�g�ځ@�E��1�o�C�g�� �𑗂�
*	GPIOA>-ODR�͕\������s�������Ă���
*
*
*/

	
	
	
int main (void){
	int i, j, flg = 0;
	unsigned long long buf;
	
	PortInit();
	
	//�z�񏉊���
	memset(ledData, 0, COL * sizeof(ledData[0]));
	
	//�z�񐬌`
	for(i = 0; i < 32; i=i+2){
		byteCount = 0;
		for(j = 0; j < 4; j++){
			
			//�f�[�^�̍�(0~3)���i�[
			ledData[ledDataCount] |= ((long long)dispData[j][i+1]) << (8*byteCount);
			//�f�[�^�̉E(4~7)���i�[	8bit���V�t�g
			ledData[ledDataCount] |= ((long long)(dispData[j][i]) << (8*(byteCount+1)));
			
			//�i�[Byte�J�E���g
			byteCount+=2;
		}
		//8Byte�i�[����
		ledDataCount++;
	}
	
	//�s���߁@�e�X�g�Ȃ̂�1�s�ڌŒ� PA0~PA2 = L	PA3 = L
	//GPIOA->ODR |=0x01 ;
	
	
	
	//ENABLE�L��
	GPIOB->ODR &= (~ENABLE);	//(NOT)0x02 �Ƃ̃A���h���Ƃ�
	
	while(1){
		for(i = 0; i < COL; i++){
			//�z��̃R�s�[�Ɖ��X���C�h
			buf = ledData[i] << moveShakeCount;
			
			//���X���C�h�ŃI�[�o�[�t���[����������t������
			buf |= ledData[i] >> (64 - moveShakeCount);
			
			
			//�s�̌���(�����for�̍Ō�Ɏ����Ă����ƁA�����l��15�ɂȂ�)
			GPIOA->ODR = i + (moveUpCount/ 2);
			
			//�f�[�^�̑��o
			for(j = 0; j < ROW; j++){
				if(buf & 0x01){
					GPIOB->ODR |= S_IN;
				}
				else{
					GPIOB->ODR &= (~S_IN);
				}
				
				//PB�o��?
				
				//�N���b�N�L��
				GPIOB->ODR |= CLOCK;
				GPIOB->ODR &= (~CLOCK);
				
				//1bit�V�t�g
				buf = buf >> 1;
			}
			//�f�[�^�̑��o����
			
			//LATCH����u�����L��
			GPIOB->ODR &= (~LATCH);
			wait(6500);
			GPIOB->ODR |= LATCH;
		}
		
		/*//�t���O�Ǘ�
		if(flg == 1)
			moveUpCount--;
		else
			moveUpCount++;
		
		if(moveUpCount == 100)
			flg = 1;
		else if(moveUpCount == 0)
			flg = 0;*/
		
		//�t���O�Ǘ�
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
	//		��������
	//�P�@�N���b�N�𑗂�
	
	//		A�|�[�g�̂V����W
	RCC->APB2ENR=RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;	// 0x0C �Ɠ��`�@EN�̓C�l�[�u��
	
	//�Q	�|�[�g�ݒ�@
	
	GPIOA->CRL=0x33333333;		//A�|�[�g	 0����7�܂ł�7SEG�̃A�m�[�h�Ɍq�����Ă��邽��3(�o��)
	GPIOA->CRH=0x44444444;		//A�|�[�g�@8����11�܂ł�SW���q�����Ă��邽��4�i���́j
	GPIOB->CRL=0x44333333;		//B�|�[�g�@CRL�̂R�r�b�g�ڂ܂ł͂VSEG�J�\�[�h�ɂȂ����Ă��邽�� 3(�o��) ����ȊO��4(�o��)
	AFIO->MAPR=0x02000000;		//AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

	//ENABLE�L��
	GPIOB->ODR = 0x0000;
	GPIOA->ODR = 0x0000;
}