/*
	SHARP PC-G800 series emulator
	���C��
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define DEF_GLOBAL
#include "g800.h"

/* �A�h���X0000~003f�̏����l */
static uint8 base[] = {
	0xc3, 0xf4, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc3, 0x03, 0xbd, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* �A�h���X0038~003a�̏����l (PC-G850) */
const static uint8 base_g850[] = {
	0xc3, 0x37, 0xbc
};

/*
	ROM��1�y�[�W�ǂݍ��� (������)
*/
static int loadROM1page(uint8 *p, const char *dir, int page, int size)
{
	int result;
	char path[FILENAME_MAX];

	if(page < 0)
		sprintf(path, "%s/base.bin", dir);
	else
		sprintf(path, "%s/rom%02x.bin", dir, page);
	if((result = readBin(path, p, size)) > 0)
		return result;

	if(page < 0)
		sprintf(path, "%s/base.txt", dir);
	else
		sprintf(path, "%s/rom%02x.txt", dir, page);
	if((result = readHexAbs(path, p, NULL, size, FALSE)) > 0)
		return result;
	return 0;
}

/*
	ROM��ǂݍ��� (������)
*/
static int loadROM(const char *dir)
{
	int page;

	if(useROM) {
		/* 0000-003f��ǂݍ��� */
		if(loadROM1page(base, dir, -1, 0x40) < 0x40)
			return FALSE;

		/* ROM�̃y�[�W���𒲂ׂ� */
		for(romBanks = 0; loadROM1page(NULL, dir, romBanks, 0x4000) == 0x4000; romBanks++)
			;
		if(romBanks == 0)
			return FALSE;

		/* ROM��ǂݍ��� */
		rom = malloc(0x4000 * romBanks);
		for(page = 0; page != romBanks; page++)
			loadROM1page(ROM(page), dir, page, 0x4000);
	} else {
		int font_off;
		uint8 *p;

		/* 0000-003f��ݒ肷�� */
		if(machine == MACHINE_G850)
			memcpy(&base[0x0038], base_g850, sizeof(base_g850));

		/* �[��ROM�̃y�[�W����ݒ肷�� */
		if(machine == MACHINE_E200) {
			romBanks = 5;
			font_off = 0x4000 * 1 + 0x3886;
		} else if(machine == MACHINE_G815) {
			romBanks = 14;
			font_off = 0x4000 * 4 + 0x1720;
		} else if(machine == MACHINE_G850) {
			romBanks = 22;
			font_off = 0x4000 * 4 + 0x2016;
		} else {
			romBanks = 2;
			font_off = 0x4000 * 1;
		}

		/* �[��ROM��ݒ肷�� */
		rom = malloc(0x4000 * romBanks);
		for(p = rom; p < rom + 0x4000 * romBanks; p++)
			*p = rand() & 0xff;
		memcpy(rom + font_off, font, sizeof(font));
	}

	/* BANK������������ */
	romBank = exBank = 0;
	memcpy(&memory[0x8000], ROM(0), 0x4000);
	memcpy(&memory[0xc000], ROM(0), 0x4000);
	return TRUE;
}

/*
	EXROM��1�y�[�W�ǂݍ��� (������)
*/
static int loadExROM1page(uint8 *p, const char *dir, int page, int size)
{
	int result;
	char path[FILENAME_MAX];

	sprintf(path, "%s/exrom%02x.bin", dir, page);
	if((result = readBin(path, p, size)) > 0)
		return result;

	sprintf(path, "%s/exrom%02x.txt", dir, page);
	if((result = readHexAbs(path, p, NULL, size, FALSE)) > 0)
		return result;
	return 0;
}

/*
	EXROM��ǂݍ��� (������)
*/
static int loadExROM(const char *dir)
{
	int page;

	/* EXROM�̃y�[�W���𒲂ׂ� */
	for(exBanks = 0; loadExROM1page(NULL, dir, exBanks, 0x4000) == 0x4000; exBanks++)
		;
	if(exBanks == 0)
		return FALSE;

	/* ROM��ǂݍ��� */
	exrom = malloc(0x4000 * exBanks);
	for(page = 0; page != exBanks; page++)
		loadExROM1page(EXROM(page), dir, page, 0x4000);
}

/*
	RAM��ǂݍ��� (������)
*/
static int loadRAM(const char *path)
{
	if(readHex(path, memory, NULL, 0x8000, FALSE) > 0)
		return TRUE;

	if(!useROM) {
		memset(memory, 0, 0x8000);
		memory[0x7901] = 0x02;
		memory[0x7903] = 0x60;
	}
	return FALSE;
}

/*
	RAM��ۑ�����
*/
int storeRAM(const char *path)
{
	if(exram != NULL && ramBank > 0)
		memcpy(memory, exram, 0x8000);
	return (writeHex(path, memory, 0x40, 0x8000 - 0x40) > 0);
}

/*
	���������G�~�����[�g����
*/
void boot(void)
{
	z80reset(&z80);

	z80outport(NULL, 0x11, 0);
	z80outport(NULL, 0x12, 0);
	z80outport(NULL, 0x14, 0);
	z80outport(NULL, 0x15, 1);
	z80outport(NULL, 0x16, 0xff);
	z80outport(NULL, 0x17, 0xf);
	z80outport(NULL, 0x18, 0);
	z80outport(NULL, 0x19, 0);
	z80outport(NULL, 0x1b, 0);
	z80outport(NULL, 0x1c, 1);
	timerInterval = 388643;
	z80.r.im = 1;

	memcpy(memory, base, sizeof(base));
	memory[0x790d] = 0;

	switch(machine) {
	case MACHINE_E200:
		z80outport(NULL, 0x58, 0xc0);
		break;
	case MACHINE_G815:
		z80outport(NULL, 0x50, 0xc0);
		break;
	case MACHINE_G850:
		memory[0x779c] = (memory[0x779c] >= 0x07 && memory[0x779c] <= 0x1f ? memory[0x779c]: 0x0f);
		z80outport(NULL, 0x40, 0x24);
		z80outport(NULL, 0x40, memory[0x790d] + 0x40);
		z80outport(NULL, 0x40, memory[0x779c] + 0x80);
		z80outport(NULL, 0x40, 0xa0);
		z80outport(NULL, 0x40, 0xa4);
		z80outport(NULL, 0x40, 0xa6);
		z80outport(NULL, 0x40, 0xa9);
		z80outport(NULL, 0x40, 0xaf);
		z80outport(NULL, 0x40, 0xc0);
		z80outport(NULL, 0x40, 0x25);
		z80outport(NULL, 0x60, 0);
		z80outport(NULL, 0x61, 0xff);
		z80outport(NULL, 0x62, 0);
		z80outport(NULL, 0x64, 0);
		z80outport(NULL, 0x65, 1);
		z80outport(NULL, 0x66, 1);
		z80outport(NULL, 0x67, 0);
		z80outport(NULL, 0x6b, 4);
		z80outport(NULL, 0x6c, 0);
		z80outport(NULL, 0x6d, 0);
		z80outport(NULL, 0x6e, 4);
		break;
	}
}

/*
	�d����~������?
*/
int isoff(void)
{
	return z80.r.halt && z80.r.iff == 0 && ioReset == 0;
}

/*
	�v���O���������s����
*/
int exec(int exit_when_underflow)
{
	int wait, states_update_io;
	uint8 itype;

	wait = 1000 / freqUpdateIO;
	z80.i.stack_under = (exit_when_underflow ? 0x7ff6: 0xffff);
	z80.i.states = 0;

	for(;;) {
		/* �E�F�C�g */
		states_update_io = freqCPU / (csClk ? 2: 1) / freqUpdateIO;
		do {
			z80.i.states        += states_update_io;
			statesKeyStrobeLast += states_update_io;
			delay(wait);
		} while(z80.i.states < 0);

		/* �T�E���h�o�b�t�@��؂�ւ��� */
		if(buzzer != BUZZER_NONE)
			flipSoundBuffer();

		/* �R�[�h���s */
		switch(z80exec(&z80)) {
		case Z80_RUN:
			break;
		case Z80_HALT:
			if(!isoff())
				break;
		case Z80_UNDERFLOW:
			updateLCD();
			return 0;
		}

		/* �L�[�X�V�E�L�[���荞�� */
		itype = updateKey();
		if(itype & interruptMask) {
			interruptType |= itype;
			z80int1(&z80);
		}

		/* �^�C�}�X�V�E�^�C�}���荞�� */
		if(timerCount-- == 0) {
			timerCount = freqUpdateIO * timerInterval / 1000 / 1000;
			if(interruptMask & INTERRUPT_1S) {
				timer ^= TIMER_1S;
				interruptType |= INTERRUPT_1S;
				z80int1(&z80);
			}
		}

		/* ���Z�b�g�L�[ */
		if(keyReset) {
			keyReset = FALSE;

			if(exit_when_underflow)
				return 0;
			boot();
		}

		/* LCD�X�V */
		updateLCD();
	}
}

/*
	�V�~�����[�^�����s����
*/
int execSim(void)
{
	/* �V�X�e���������� */
	initBasic(&bas);

	/* �V�X�e�����N�� */
	memory[0x7902] = 0x40;

	while(!isoff()) {
		if(memory[0x7902] == 0x20)
			basPro(&bas);
		else if(memory[0x7902] == 0x40)
			basRun(&bas);
		else
			monitor();
	}
	return 0;
}

int main(int argc, char *argv[])
{
#if defined(_WIN32) && SDL_MAJOR_VERSION == 1
	/* win32��SDL1.2�Ȃ�Έ�����UTF-8�ɕϊ����� */
	argv = argvToUTF8(argc, argv);
#endif

	/* ������ */
	if(!init(argc, argv))
		popup("!", "CANNOT OPEN CONFIG FILE");
	if(!loadROM(dirROM))
		popup("!", "CANNOT OPEN ROM FILES [%s]", dirROM);
	loadExROM(dirROM);
	loadRAM(pathRAM);
	if(pathProg != NULL && strcmp(pathProg, "") != 0)
		if(loadProg(NULL, pathProg) < 0) {
			popup("!", "CANNOT OPEN %s", pathProg);
			beginProg = 0;
		}
	z80srand(time(NULL));

	memset(keyMatrix, 0, sizeof(keyMatrix));
	keyBreak = keyShift = keyReset = FALSE;

	/* ���s */
	boot();

	if(beginProg != 0) {
		/* �A�h���X���w�肵�Ď��s */
		go(beginProg);
		exec(TRUE);
	} else if(useROM) {
		/* �擪������s */
		exec(FALSE);
	} else if(useBasic) {
		/* BASIC�V�~�����[�^�����s */
		execSim();
	} else {
		/* ���j�^�V�~�����[�^�����s */
		monitor();
	}

	/*
	printf("Total states = %d\n", z80.i.total_states);
	*/

	/* �㏈�� */
	storeRAM(pathRAM);
	return 0;
}

/*
	Copyright 2005 ~ 2017 maruhiro
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
