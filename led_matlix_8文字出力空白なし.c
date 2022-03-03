/***********
**  LED_MATLIX
**  �ܓ�����
************/

#include <stm32f10x.h>		//GPIOA��N���b�N�̒�`���L�ڂ��ꂽ�w�b�_�[�t�@�C��	
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

//�v���g�^�C�v�錾
void PortInit(void);
void wait(int time);
unsigned long long createOneLineData(unsigned long long orgData,unsigned long long addData, int shiftCount);


//�O���[�o���ϐ�
static int ledDataCount = 0;
static int byteCount = 0;
static int moveUpCount = 0;
static int moveShakeCount = 0;

//���M�p�z��(8Byte * 16 = 128Byte)
static unsigned long long ledData[COL];
static unsigned long long ledAddData[COL];


//�f�[�^�i�[�p�z��(4 * 32Byte = 128Byte)
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
*	�X���C�h�̕��@���l���� 
* �����������o�͂�����@���l����
*	�v���O���������ꂢ�ɏ���
*/

	
	
unsigned long long data;
unsigned long long buf;	

int main (void){
int i, j, flg = 0, blankFlg = 0;		
	
	PortInit();
	
	//�z�񏉊���
	memset(ledData, 0, COL * sizeof(ledData[0]));
	memset(ledAddData, 0, COL * sizeof(ledAddData[0]));
	
// ====================================================================================				
// �z�񐬌`			
// ====================================================================================			
	for(i = 0; i < 32; i=i+2){
		byteCount = 0;
		for(j = 0; j < 4; j++){
			
			//�f�[�^�̍�(0~3)���i�[
			ledData[ledDataCount] |= ((long long)dispData[j][i+1]) << (8*byteCount);
			ledAddData[ledDataCount] |= ((long long)dispAddData[j][i+1]) << (8*byteCount);
			
			//�f�[�^�̉E(4~7)���i�[	8bit���V�t�g
			ledData[ledDataCount] |= ((long long)(dispData[j][i]) << (8*(byteCount+1)));
			ledAddData[ledDataCount] |= ((long long)dispAddData[j][i]) << (8*(byteCount+1));
			
			//�i�[Byte�J�E���g
			byteCount+=2;
		}
		//8Byte�i�[����
		ledDataCount++;
	}
	
// ====================================================================================				
// LED�}�g���N�X����			
// ====================================================================================			
	
	
	//ENABLE�L��
	GPIOB->ODR &= (~ENABLE);	//(NOT)0x02 �Ƃ̃A���h���Ƃ�
	
	while(1){
		
		for(i = 0; i < COL; i++){
			//�ЂƂO�̃f�[�^��ێ�
			
			
			//�z��̋󔒂̕����Ɏ��̕����𑫂�
			if(moveShakeCount < 64)
				data = createOneLineData(ledData[i], ledAddData[i], moveShakeCount);
			else if(moveShakeCount < 128)
				data = createOneLineData(ledAddData[i], ledData[i], moveShakeCount);
			else{
				data = data >> (moveShakeCount % 128);
			}
			
			buf = data;
			//�s�̌���(�����for�̍Ō�Ɏ����Ă����ƁA�����l��15�ɂȂ�)
			GPIOA->ODR = i;
			
			//�f�[�^�̑��o
			for(j = 0; j < ROW; j++){
				if(data & 0x01){
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
				data = data >> 1;
			}
			//�f�[�^�̑��o����
			
			//LATCH����u�����L��
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

//------------------------------------------------
//  �T�@�v�F�z��ϊ�
//  ��  ���Ffont�f�[�^
//  �߂�l�F�s�z��
//------------------------------------------------
/*unsigned long long font_to_array(unsigned long long fontData[], int row){
	fontData[] 
	
}*/

//------------------------------------------------
//  �T�@�v�F1�s�̔z��f�[�^���쐬����
//  ��  ���F1:���f�[�^ 2:�ǉ��f�[�^ 3:�V�t�g��
//  �߂�l�F�ǉ���̃f�[�^ (�V�t�g�����V�t�g�������f�[�^) + (�V�t�g����)�ǉ��f�[�^
//------------------------------------------------
unsigned long long createOneLineData(unsigned long long orgData,unsigned long long addData, int shiftCount){
	unsigned long long buf = 0;
	
	//���f�[�^ ���V�t�g
		buf |= orgData << shiftCount % 64;
		buf |= addData >> (ONELINE - (shiftCount % 64));
	
	return buf;
}