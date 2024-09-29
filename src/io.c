/*
	SHARP PC-G800 series Emulator
	���o�̓G�~�����[�g
*/

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "g800.h"

/*
	VRAM�̃I�t�Z�b�g(PC-E200)
*/
static inline int e200vramoff(uint8 x, uint8 row, uint8 begin)
{
	row = (row - begin + 8) % 8;

	if(x == 0x3c)
		return (int )row * E200_VRAM_WIDTH + (E200_VRAM_WIDTH - 1);

	if(row < 4)
		return (int )(row - 0) * E200_VRAM_WIDTH + x;
	else
		return (int )(row - 4) * E200_VRAM_WIDTH + (E200_VRAM_WIDTH - x - 2);
}

/*
	VRAM�ւ̃|�C���^(PC-E200)
*/
static inline uint8 *e200vram(uint8 x, uint8 row, uint8 begin)
{
	return &vram[e200vramoff(x, row, begin)];
}

/*
	�\���J�n�ʒu��ݒ肷��(PC-E200)
*/
static inline int sete200begin(uint8 begin)
{
	uint8 oldvram[166 * 9];
	int row, x;

	memcpy(oldvram, vram, sizeof(vram));
	for(row = 0; row != 8; row++)
		for(x = 0; x != 0x3d; x++)
			*e200vram(x, row, begin) = oldvram[e200vramoff(x, row, lcdBegin)];
	lcdBegin = begin;

	return begin;
}

/*
	VRAM�̃I�t�Z�b�g(PC-G815)
*/
static inline int g815vramoff(uint8 x, uint8 row, uint8 begin)
{
	row = (row - begin + 8) % 8;

	if(x == 0x7b)
		return (int )row * G815_VRAM_WIDTH + (G815_VRAM_WIDTH - 1);
	if(row < 4)
		return (int )(row - 0) * G815_VRAM_WIDTH + x;
	else
		return (int )(row - 4) * G815_VRAM_WIDTH + (G815_VRAM_WIDTH - x - 2);
}

/*
	VRAM�ւ̃|�C���^(PC-G815)
*/
static inline uint8 *g815vram(uint8 x, uint8 row, uint8 begin)
{
	return &vram[g815vramoff(x, row, begin)];
}

/*
	�\���J�n�ʒu��ݒ肷��(PC-G815)
*/
static inline int setg815begin(uint8 begin)
{
	uint8 oldvram[166 * 9];
	int row, x;

	memcpy(oldvram, vram, sizeof(vram));
	for(row = 0; row != 8; row++)
		for(x = 0; x != 0x49; x++)
			*g815vram(x, row, begin) = oldvram[g815vramoff(x, row, lcdBegin)];
	lcdBegin = begin;

	return begin;
}

/*
	VRAM�ւ̃|�C���^(PC-G850)
*/
static inline uint8 *g850vram(uint8 x, uint8 row)
{
	return &vram[row * G850_VRAM_WIDTH + x];
}

/*
	ROM�o���N��؂�ւ���
*/
static inline void swrom(uint8 page)
{
	if(romBanks == 0)
		return;

	if(page < romBanks)
		memmove(&memory[0xc000], ROM(page), 0x4000);
	else
		memset(&memory[0xc000], 0xff, 0x4000);
}

/*
	EXROM�o���N��؂�ւ���
*/
static inline void swexrom(uint8 page)
{
	if(page & 0xfc) {
		if((page & 0x03) < exBanks)
			memcpy(&memory[0x8000], EXROM(page & 0x03), 0x4000);
		else
			memset(&memory[0x8000], 0x2d, 0x4000);
	} else {
		if(romBanks > 0)
			memmove(&memory[0x8000], ROM(0), 0x4000);
	}
}

/*
	11pin�̏o�͐M���𓾂�
*/
uint8 pin11out(void)
{
	switch(pin11If) {
	case PIN11IF_3IO:
		return (io3Out & 0x03) | ((io3Out >> 4) & 0x08);
	case PIN11IF_8PIO:
		return ~pio8Io & pio8Out;
	case PIN11IF_UART:
		return 0;
	default:
		return 0;
	}
}

/*
	�L�[�̏��
*/
static inline int in10(uint8 *x)
{
	if(statesKeyStrobeLast - z80.i.states > statesKeyStrobeClear)
		keyStrobe = keyStrobeLast;
	*x = (keyStrobe & 0x001 ? keyMatrix[0]: 0) |
	     (keyStrobe & 0x002 ? keyMatrix[1]: 0) |
	     (keyStrobe & 0x004 ? keyMatrix[2]: 0) |
	     (keyStrobe & 0x008 ? keyMatrix[3]: 0) |
	     (keyStrobe & 0x010 ? keyMatrix[4]: 0) |
	     (keyStrobe & 0x020 ? keyMatrix[5]: 0) |
	     (keyStrobe & 0x040 ? keyMatrix[6]: 0) |
	     (keyStrobe & 0x080 ? keyMatrix[7]: 0) |
	     (keyStrobe & 0x100 ? keyMatrix[8]: 0) |
	     (keyStrobe & 0x200 ? keyMatrix[9]: 0);
	return 1100;
}
static inline int out10(int x)
{
	return 0;
}

/*
	�L�[�X�g���[�u
*/
static inline int in11(uint8 *x)
{
	*x = 0;
	return 0;
}
static inline int out11(uint8 x)
{
	if(statesKeyStrobeLast - z80.i.states > statesKeyStrobeClear)
		keyStrobe = keyStrobeLast;
	keyStrobeLast = x;
	keyStrobe |= keyStrobeLast;
	statesKeyStrobeLast = z80.i.states;
	if(x & 0x10)
		interruptType |= INTERRUPT_IA;
	return 0;
}
static inline int in12(uint8 *x)
{
	*x = 0;
	return 0;
}
static inline int out12(uint8 x)
{
	if(statesKeyStrobeLast - z80.i.states > statesKeyStrobeClear)
		keyStrobe = keyStrobeLast;
	keyStrobeLast = x << 8U;
	keyStrobe |= keyStrobeLast;
	statesKeyStrobeLast = z80.i.states;
	return 0;
}

/*
	�V�t�g�L�[�̏��
*/
static inline int in13(uint8 *x)
{
	*x = (keyStrobe & 0x08 ? keyShift: 0);
	return 0;
}
static inline int out13(uint8 x)
{
	return 0;
}

/*
	�^�C�}
*/
static inline int in14(uint8 *x)
{
	*x = timer;
	return 0;
}
static inline int out14(uint8 x)
{
	timer = 0;
	return 0;
}

/*
	Xin���͒[�q�̓��͉ۏ��
*/
static inline int in15(uint8 *x)
{
	*x = xinEnabled;
	return 0;
}
static inline int out15(uint8 x)
{
	xinEnabled = x & 0x80;
	return 0;
}

/*
	���荞�ݗv��
*/
static inline int in16(uint8 *x)
{
	*x = interruptType;
	return 0;
}
static inline int out16(uint8 x)
{
	interruptType &= ~x;
	return 0;
}

/*
	���荞�݃}�X�N
*/
static inline int in17(uint8 *x)
{
	*x = interruptMask;
	return 0;
}
static inline int out17(uint8 x)
{
	interruptMask = x;
	return 0;
}

/*
	11pinI/F�̏o�͐���
*/
static inline int in18(uint8 *x)
{
	*x = io3Out;
	return 0;
}
static inline int out18(uint8 x)
{
	/*
	int interval;
	static int prev_states = 0;
	*/

	io3Out = x & 0xc3;

	updateSerial();
	if(sioMode == SIO_MODE_OUT) {
		pin11In = sioWrite(pin11out());
		updateSerial();
	}
	if(buzzer != BUZZER_NONE && !sioBusy)
		writeSound(pin11out() & PIN11_XOUT);

	/*
	if((interval = Z80_STATES(&z80) - prev_states) < 0)
		interval = INT_MAX - prev_states + Z80_STATES(&z80);
	prev_states = Z80_STATES(&z80) + states;
	printf("interval=%d (%d)\n", interval, pin11out() & PIN11_XOUT);
	*/
	return 18;

	/*
	if(machine == MACHINE_G850) {
		if(freqCPU > 7356960)
			return (freqCPU - 7356960) / 20960;
	} else {
		if(freqCPU > 2891248)
			return (freqCPU - 2891248) / 5276;
	}
	return 0;
	*/
}

/*
	ROM�o���N�؂�ւ�
*/
static inline int in19(uint8 *x)
{
	*x = ((exBank & 0x07) << 4) | (romBank & 0x0f);
	return 0;
}
static inline int out19(uint8 x)
{
	uint8 tmp;

	if(romBanks == 0)
		return 0;

	if((tmp = (x & 0x0f) % romBanks) != romBank)
		swrom(romBank = tmp);
	if((tmp = (x & 0x70) >> 4) != exBank)
		swexrom(exBank = tmp);
	return 0;
}

/*
	BOOT ROM ON/OFF
*/
static inline int in1a(uint8 *x)
{
	*x = 0;
	return 0;
}
static inline int out1a(uint8 x)
{
	return 0;
}

/*
	RAM�o���N�؂�ւ�
*/
static inline int in1b(uint8 *x)
{
	*x = ramBank;
	return 0;
}
static inline int out1b(uint8 x)
{
	uint8 *tmp;

	if(exram == NULL)
		return 0;
	if(ramBank == (x & 0x04))
		return 0;

	tmp = alloca(0x8000);
	memcpy(tmp, memory, 0x8000);
	memcpy(memory, exram, 0x8000);
	memcpy(exram, tmp, 0x8000);

	ramBank = x & 0x04;
	return 0;
}

/*
	I/O���Z�b�g
*/
static inline int in1c(uint8 *x)
{
	*x = 0;
	return 0;
}
static inline int out1c(uint8 x)
{
	ioReset = x;
	return 0;
}

/*
	�o�b�e���[���?
*/
static inline int in1d(uint8 *x)
{
	switch(battChk) {
	case 1: /* LOW? */
		*x = 0x00;
		break;
	case 3: /* EMPTY? */
		*x = 0x00;
		break;
	default /* ??? */:
		*x = 0x00;
		break;
	}
	return 0;
}
static inline int out1d(uint8 x)
{
	return 0;
}

/*
	�o�b�e���[�`�F�b�N���[�h?
*/
static inline int in1e(uint8 *x)
{
	*x = 0;
	return 0;
}
static inline int out1e(uint8 x)
{
	battChk = x & 0x03;
	return 0;
}

/*
	11pinI/F�̓���
*/
static inline int in1f(uint8 *x)
{
	/*updateSerial();*/
	if(sioMode == SIO_MODE_IN) {
		pin11In = sioRead(pin11out());
		/*updateSerial();*/
	}

	*x = keyBreak | (xinEnabled ? pin11In & 0x04: 0) | (pin11In & 0x20 ? 0x02: 0) | (pin11In & 0x10 ? 0x01: 0);
	return 0;
}
static inline int out1f(uint8 x)
{
	return 0;
}

/*
	�f�B�X�v���C�R���g���[�� (PC-E200)
*/
static inline int out58_e200(uint8 x)
{
	lcdRead = FALSE;

	switch(x & 0xc0) {
	case 0x00:
		break;
	case 0x40:
		lcdX = x & 0x3f;
		break;
	case 0x80:
		lcdY = x & 0x07;
		break;
	case 0xc0:
		sete200begin((x >> 3) & 0x07);
		break;
	}
	return 0;
}
static inline int in59_e200(uint8 *x)
{
	*x = 0;
	return 0;
}

/*
	�f�B�X�v���C READ/WRITE (PC-E200)
*/
static inline int out5a_e200(uint8 x)
{
	lcdRead = FALSE;

	if(lcdX < 0x3d && lcdY < 8)
		*e200vram(lcdX++, lcdY, lcdBegin) = x & 0x7f;
	return 0;
}
static inline int in5b_e200(uint8 *x)
{
	if(!lcdRead) {
		lcdRead = TRUE;
		*x = 0;
		return 0;
	}

	if(0 <= lcdX && lcdX < 0x3d && lcdY < 8)
		*x = *e200vram(lcdX++, lcdY, lcdBegin);
	else
		*x = 0;
	return 0;
}

/*
	�f�B�X�v���C�R���g���[�� (PC-G815)
*/
static inline void g815lcdctrl(uint8 *lcd_x, uint8 *lcd_y, uint8 x)
{
	lcdRead = FALSE;

	switch(x & 0xc0) {
	case 0x00:
		break;
	case 0x40:
		*lcd_x = x & 0x3f;
		break;
	case 0x80:
		*lcd_y = x & 0x07;
		break;
	case 0xc0:
		setg815begin((x >> 3) & 0x07);
		break;
	}
}
static inline int out50_g815(uint8 x)
{
	g815lcdctrl(&lcdX2, &lcdY2, x);
	g815lcdctrl(&lcdX,  &lcdY,  x);
	return 0;
}
static inline int in51_g815(uint8 *x)
{
	*x = 0;
	return 0;
}
static inline int out54_g815(uint8 x)
{
	g815lcdctrl(&lcdX2, &lcdY2, x);
	return 0;
}
static inline int in55_g815(uint8 *x)
{
	*x = 0;
	return 0;
}
static inline int out58_g815(uint8 x)
{
	g815lcdctrl(&lcdX, &lcdY, x);
	return 0;
}
static inline int in59_g815(uint8 *x)
{
	*x = 0;
	return 0;
}

/*
	�f�B�X�v���C READ/WRITE (PC-G815)
*/
static inline int out56_g815(uint8 x)
{
	lcdRead = FALSE;

	if(lcdX2 < 0x3c && lcdY2 < 8)
		*g815vram(lcdX2++, lcdY2, lcdBegin) = x;
	return 0;
}
static inline int in57_g815(uint8 *x)
{
	if(!lcdRead) {
		lcdRead = TRUE;
		*x = 0;
		return 0;
	}

	if(0 <= lcdX2 && lcdX2 < 0x3c && lcdY2 < 8)
		*x = *g815vram(lcdX2++, lcdY2, lcdBegin);
	else
		*x = 0;
	return 0;
}
static inline int out5a_g815(uint8 x)
{
	lcdRead = FALSE;

	if((0x3c + lcdX < 0x49 || 0x3c + lcdX == 0x7b) && lcdY < 8)
		*g815vram(0x3c + lcdX++, lcdY, lcdBegin) = x;
	return 0;
}
static inline int in5b_g815(uint8 *x)
{
	if(!lcdRead) {
		lcdRead = TRUE;
		*x = 0;
		return 0;
	}

	if(0x3c + lcdX < 0x49 && lcdY < 8)
		*x = *g815vram(0x3c + lcdX++, lcdY, lcdBegin);
	else
		*x = 0;
	return 0;
}
static inline int out52_g815(uint8 x)
{
	out56_g815(x);
	out5a_g815(x);
	return 0;
}

/*
	�f�B�X�v���C�R���g���[�� (PC-G850)
*/
static inline int in40_g850(uint8 *x)
{
	*x = 0;
	return 0;
}
static inline int out40_g850(uint8 x)
{
	lcdRead = FALSE;

	switch(x & 0xf0) {
	case 0x00:
		if(!lcdMod)
			lcdX = (lcdX & 0xf0) | (x & 0x0f);
		break;
	case 0x10:
		if(!lcdMod)
			lcdX = (x << 4) | (lcdX & 0x0f);
		break;
	case 0x20:
		if(x == 0x24)
			lcdDisabled = TRUE;
		else if(x == 0x25)
			lcdDisabled = FALSE;
		updateLCDContrast();
		break;
	case 0x30:
		timerInterval = 16192 * ((x & 0x0f) + 1);
		break;
	case 0x40:
	case 0x50:
	case 0x60:
	case 0x70:
		lcdTop = x - 0x40;
		break;
	case 0x80:
	case 0x90:
		lcdContrast = x - 0x80;
		updateLCDContrast();
		break;
	case 0xa0:
		switch(x) {
		case 0xa0:
			lcdEffectMirror = FALSE;
			break;
		case 0xa1:
			lcdEffectMirror = TRUE;
			break;
		case 0xa4:
			lcdEffectBlack = FALSE;
			break;
		case 0xa5:
			lcdEffectBlack = TRUE;
			break;
		case 0xa6:
			lcdEffectReverse = FALSE;
			break;
		case 0xa7:
			lcdEffectReverse = TRUE;
			break;
		case 0xa8:
			lcdEffectDark = TRUE;
			break;
		case 0xa9:
			lcdEffectDark = FALSE;
			break;
		case 0xae:
			lcdEffectWhite = TRUE;
			break;
		case 0xaf:
			lcdEffectWhite = FALSE;
			break;
		}
		updateLCDContrast();
		break;
	case 0xb0:
		lcdY = x & 0x0f;
		break;
	case 0xc0:
		lcdTrim = x & 0x0f;
		break;
	case 0xd0:
		break;
	case 0xe0:
		if(x == 0xe0) {
			lcdMod = TRUE;
			lcdX2 = lcdX;
		} else if(x == 0xe2) {
			lcdContrast = lcdMod = 0;
			updateLCDContrast();
		} else if(x == 0xee) {
			lcdMod = FALSE;
			lcdX = lcdX2;
		}
		break;
	case 0xf0:
		break;
	}
	return 0;
}

/*
	�f�B�X�v���C READ/WRITE (PC-G850)
*/
static inline int in41_g850(uint8 *x)
{
	if(!lcdRead) {
		lcdRead = TRUE;
		*x = 0;
		return 0;
	}

	if(lcdX < 166 && lcdY < 8)
		*x = *g850vram(lcdX, lcdY);
	else
		*x = 0xff;
	if(!lcdMod)
		lcdX++;
	return 0;
}
static inline int out41_g850(uint8 x)
{
	lcdRead = FALSE;

	if(lcdX < 166 && lcdY < 8)
		*g850vram(lcdX++, lcdY) = x;
	return 0;
}

/*
	11pin I/F�̓���
*/
static inline int in60_g850(uint8 *x)
{
	*x = pin11If;
	return 0;
}
static inline int out60_g850(uint8 x)
{
	pin11If = x & 0x03;
	updateSerial();
	return 0;
}

/*
	�p������I/O�̓��o�͕���
*/
static inline int in61_g850(uint8 *x)
{
	*x = 0;
	return 0;
}
static inline int out61_g850(uint8 x)
{
	pio8Io = x;
	updateSerial();
	return 0;
}

/*
	�p������I/O�̃f�[�^���W�X�^
*/
static inline int in62_g850(uint8 *x)
{
	*x = pin11In & ~pio8Io;
	return 0;
}
static inline int out62_g850(uint8 x)
{
	pio8Out = x;
	updateSerial();
	if(buzzer != BUZZER_NONE)
		writeSound(pin11out() & PIN11_XOUT);
	return 0;
}

/*
	UART�t���[����
*/
static inline int in63_g850(uint8 *x)
{
	*x = 0;
	return 0;
}
static inline int out63_g850(uint8 x)
{
	return 0;
}

/*
	CD�M���ɂ��ON����
*/
static inline int in64_g850(uint8 *x)
{
	*x = onCd;
	return 0;
}
static inline int out64_g850(uint8 x)
{
	onCd = x & 1;
	return 0;
}

/*
	M1�M����wait����
*/
static inline int in65_g850(uint8 *x)
{
	*x = m1Wait;
	return 0;
}
static inline int out65_g850(uint8 x)
{
	m1Wait = x & 1;
	return 0;
}

/*
	I/O wait
*/
static inline int in66_g850(uint8 *x)
{
	*x = ioWait;
	return 0;
}
static inline int out66_g850(uint8 x)
{
	ioWait = x & 1;
	return 0;
}

/*
	CPU�N���b�N����/�ᑬ�؂�ւ� (PC-G850)
*/
static inline int in67_g850(uint8 *x)
{
	*x = csClk;
	return 0;
}
static inline int out67_g850(uint8 x)
{
	csClk = x & 1;
	return 0;
}

/*
	�^�C�}�M��/LCD�h���C�o����
*/
static inline int in68_g850(uint8 *x)
{
	*x = 0;
	return 0;
}
static inline int out68_g850(uint8 x)
{
	return 0;
}

/*
	ROM�o���N�؂�ւ� (PC-G850)
*/
static inline int in69_g850(uint8 *x)
{
	*x = romBank;
	return 0;
}
static inline int out69_g850(uint8 x)
{
	if(x != romBank)
		swrom(romBank = x);
	return 0;
}

/*
	?
*/
static inline int in6a_g850(uint8 *x)
{
	*x = 0;
	return 0;
}
static inline int out6a_g850(uint8 x)
{
	return 0;
}

/*
	UART�̓��͑I��
*/
static inline int in6b_g850(uint8 *x)
{
	*x = uartIo;
	return 0;
}
static inline int out6b_g850(uint8 x)
{
	uartIo = x & 0x87;
	return 0;
}

/*
	UART���[�h���W�X�^
*/
static inline int in6c_g850(uint8 *x)
{
	*x = 0;
	return 0;
}
static inline int out6c_g850(uint8 x)
{
	uartMode = x;
	return 0;
}

/*
	UART�R�}���h���W�X�^
*/
static inline int in6d_g850(uint8 *x)
{
	*x = 0;
	return 0;
}
static inline int out6d_g850(uint8 x)
{
	uartCommand = x;
	return 0;
}

/*
	UART�X�e�[�^�X���W�X�^
*/
static inline int in6e_g850(uint8 *x)
{
	*x = uartStatus;
	return 0;
}
static inline int out6e_g850(uint8 x)
{
	return 0;
}

/*
	UART����M���W�X�^
*/
static inline int in6f_g850(uint8 *x)
{
	*x = 0xff;
	return 0;
}
static inline int out6f_g850(uint8 x)
{
	return 0;
}

/*
	Inport���G�~�����[�g����
*/
int z80inport(Z80stat *z, uint8 *x, uint16 address)
{
	/*
	printf("in %02x\n", address);
	*/
	
	/* �f�B�X�v���C�p�E���̑�(�@��ˑ�) */
	switch(machine) {
	case MACHINE_E200:
		switch(address & 0xff) {
		case 0x51:
		case 0x59:
			return in59_e200(x);
		case 0x57:
		case 0x5b:
			return in5b_e200(x);
		}
		break;
	case MACHINE_G815:
		switch(address & 0xff) {
		case 0x51:
			return in51_g815(x);
		case 0x55:
			return in55_g815(x);
		case 0x57:
			return in57_g815(x);
		case 0x59:
			return in59_g815(x);
		case 0x5b:
			return in5b_g815(x);
		}
		break;
	case MACHINE_G850:
		switch(address & 0xff) {
		case 0x40:
		case 0x42:
		case 0x44:
		case 0x46:
		case 0x48:
		case 0x4a:
		case 0x4c:
		case 0x4e:
		case 0x50:
		case 0x52:
		case 0x54:
		case 0x56:
		case 0x58:
		case 0x5a:
		case 0x5c:
		case 0x5e:
			return in40_g850(x);
		case 0x41:
		case 0x43:
		case 0x45:
		case 0x47:
		case 0x49:
		case 0x4b:
		case 0x4d:
		case 0x4f:
		case 0x51:
		case 0x53:
		case 0x55:
		case 0x57:
		case 0x59:
		case 0x5b:
		case 0x5d:
		case 0x5f:
			return in41_g850(x);
		case 0x60:
			return in60_g850(x);
		case 0x61:
			return in61_g850(x);
		case 0x62:
			return in62_g850(x);
		case 0x63:
			return in63_g850(x);
		case 0x64:
			return in64_g850(x);
		case 0x65:
			return in65_g850(x);
		case 0x66:
			return in66_g850(x);
		case 0x67:
			return in67_g850(x);
		case 0x68:
			return in68_g850(x);
		case 0x69:
			return in69_g850(x);
		case 0x6a:
			return in6a_g850(x);
		case 0x6b:
			return in6b_g850(x);
		case 0x6c:
			return in6c_g850(x);
		case 0x6d:
			return in6d_g850(x);
		case 0x6e:
			return in6e_g850(x);
		case 0x6f:
			return in6f_g850(x);
#if 0
		case 0x74:
			*x = 1; /* ??? */
			return 0;
#endif
		}
		break;
	}

	/* �V�X�e���|�[�g(����) */
	switch(address & 0xff) {
	case 0x10:
		return in10(x);
	case 0x11:
		return in11(x);
	case 0x12:
		return in12(x);
	case 0x13:
		return in13(x);
	case 0x14:
		return in14(x);
	case 0x15:
		return in15(x);
	case 0x16:
		return in16(x);
	case 0x17:
		return in17(x);
	case 0x18:
		return in18(x);
	case 0x19:
		return in19(x);
	case 0x1a:
		return in1a(x);
	case 0x1b:
		return in1b(x);
	case 0x1c:
		return in1c(x);
	case 0x1d:
		return in1d(x);
	case 0x1e:
		return in1e(x);
	case 0x1f:
		return in1f(x);
	}

#if defined(WARN_UNKOWN_IO)
	printf("UNKNOWN in (%02xh)\n", address & 0xff);
	fflush(stdout);
#endif
	*x = 0x78;
	return 0;
}

/*
	Outport���G�~�����[�g����
*/
int z80outport(Z80stat *z, uint16 address, uint8 x)
{
	/*
	printf("out (%02x), %02x\n", address, x);
	fflush(stdout);
	*/

	/* �f�B�X�v���C�p�E���̑�(�@��ˑ�) */
	switch(machine) {
	case MACHINE_E200:
		switch(address & 0xff) {
		case 0x50:
		case 0x58:
			return out58_e200(x);
		case 0x56:
		case 0x5a:
			return out5a_e200(x);
		}
		break;
	case MACHINE_G815:
		switch(address & 0xff) {
		case 0x50:
			return out50_g815(x);
		case 0x52:
			return out52_g815(x);
		case 0x54:
			return out54_g815(x);
		case 0x56:
			return out56_g815(x);
		case 0x58:
			return out58_g815(x);
		case 0x5a:
			return out5a_g815(x);
		}
		break;
	case MACHINE_G850:
		switch(address & 0xff) {
		case 0x40:
		case 0x42:
		case 0x44:
		case 0x46:
		case 0x48:
		case 0x4a:
		case 0x4c:
		case 0x4e:
		case 0x50:
		case 0x52:
		case 0x54:
		case 0x56:
		case 0x58:
		case 0x5a:
		case 0x5c:
		case 0x5e:
			return out40_g850(x);
		case 0x41:
		case 0x43:
		case 0x45:
		case 0x47:
		case 0x49:
		case 0x4b:
		case 0x4d:
		case 0x4f:
		case 0x51:
		case 0x53:
		case 0x55:
		case 0x57:
		case 0x59:
		case 0x5b:
		case 0x5d:
		case 0x5f:
			return out41_g850(x);
		case 0x60:
			return out60_g850(x);
		case 0x61:
			return out61_g850(x);
		case 0x62:
			return out62_g850(x);
		case 0x63:
			return out63_g850(x);
		case 0x64:
			return out64_g850(x);
		case 0x65:
			return out65_g850(x);
		case 0x66:
			return out66_g850(x);
		case 0x67:
			return out67_g850(x);
		case 0x68:
			return out68_g850(x);
		case 0x69:
			return out69_g850(x);
		case 0x6a:
			return out6a_g850(x);
		case 0x6b:
			return out6b_g850(x);
		case 0x6c:
			return out6c_g850(x);
		case 0x6d:
			return out6d_g850(x);
		case 0x6e:
			return out6e_g850(x);
		case 0x6f:
			return out6f_g850(x);
		}
		break;
	}

	/* �V�X�e���|�[�g(����) */
	switch(address & 0xff) {
	case 0x11:
		return out11(x);
	case 0x12:
		return out12(x);
	case 0x13:
		return out13(x);
	case 0x14:
		return out14(x);
	case 0x15:
		return out15(x);
	case 0x16:
		return out16(x);
	case 0x17:
		return out17(x);
	case 0x18:
		return out18(x);
	case 0x19:
		return out19(x);
	case 0x1a:
		return out1a(x);
	case 0x1b:
		return out1b(x);
	case 0x1c:
		return out1c(x);
	case 0x1e:
		return out1e(x);
	case 0x1f:
		return out1f(x);
	}

#if defined(WARN_UNKOWN_IO)
	printf("UNKNOWN out (%02xh), %02xh\n", address & 0xff, x);
	fflush(stdout);
#endif
	return 0;
}

/*
	Copyright 2005 ~ 2023 maruhiro
	All rights reserved. 

	Redistribution and use in source and binary forms, 
	with or without modification, are permitted provided that 
	the following conditions are met: 

	 1. Redistributions of source code must retain the above copyright notice, 
	    this list of conditions and the following disclaimer. 

	 2. Redistributions in binary form must reproduce the above copyright notice, 
	    this list of conditions and the following disclaimer in the documentation 
	    and/or other materials provided with the distribution. 

	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, 
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
	THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* eof */
