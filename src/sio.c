/*
	SHARP PC-G800 series emulator
	SIO
*/

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "g800.h"

#define M	10
#define N	14

/*
	�p���e�B(�)�����߂�(getBit�̉�����)
*/
static inline int oddparity(char x)
{
	const static int bits[] = {
		0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
	};

	return (bits[x & 0x0f] + bits[x >> 4]) & 1;
}

/*
	�o�b�t�@�����M����(sioRead�̉�����)
*/
static inline uint8 getBit(void)
{
	uint8 pin11_out;

	if(sioCount / N >= sioBufferSize)
		return 0;

	switch(sioCount % N) {
	case 0:
	case 1:
	case 2:
		/* �X�^�[�g */
		pin11_out = PIN11_XIN | PIN11_ACK;
		break;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		/* �f�[�^ */
		pin11_out = (sioBuffer[sioCount / N] & (1 << (sioCount % N - 3)) ? 0: PIN11_XIN);
		break;
	case 11:
		/* �p���e�B */
		pin11_out = (oddparity(sioBuffer[sioCount / N]) ? PIN11_XIN: 0);
		break;
	case 12:
	case 13:
		/* �G���h */
		pin11_out = PIN11_ACK;
		break;
	default:
		pin11_out = 0;
		break;
	}
	sioCount++;

	return pin11_out;
}

/*
	�t�@�C������SIO�Ɏ�M����
*/
uint8 sioRead(uint8 pin11_in)
{
	int pin11_out;

	if(sioMode != SIO_MODE_IN)
		return 0;

	if(pin11_in & PIN11_BUSY) {
		if(sioCount == 0) {
			/* ��M�J�n */
			sioBufferSize = readBin(pathSioIn, sioBuffer, sizeof(sioBuffer));
			if(sioBufferSize <= 0 || sioBufferSize >= sizeof(sioBuffer)) {
				sioBufferSize = 0;
				sioMode = SIO_MODE_STOP;
				return 0;
			}
			if(sioBuffer[sioBufferSize - 1] != 0x1a)
				sioBuffer[sioBufferSize++] = 0x1a;
			noWait = TRUE;
		}
		/* ��M�� */
		pin11_out = getBit();
	} else if(pin11_in & PIN11_DOUT) {
		/* ��M�ꎞ��~�� */
		pin11_out = 0;
	} else {
		/* ��M��~�� */
		pin11_out = 0;
		sioCount = 0;
		noWait = FALSE;
	}

	return pin11_out;
}

/*
	�f�B�X�N����SIO�o�b�t�@�ɏ�������
*/
int sioLoad(const char *path)
{
	/* �t�@�C���̃T�C�Y�𓾂� */
	sioBufferSize = readBin(path, sioBuffer, sizeof(sioBuffer));
	if(sioBufferSize <= 0 || sioBufferSize >= sizeof(sioBuffer)) {
		/* �t�@�C�������݂��Ȃ� */
		sioBufferSize = 0;
		strcpy(pathSioIn, "");
		sioMode = SIO_MODE_STOP;
		return 0;
	}

	/* ���͏�Ԃ����������� */
	strcpy(pathSioIn, path);
	sioMode = SIO_MODE_IN;
	sioCount = 0;
	return sioBufferSize;
}

/*
	���M����bit���o�b�t�@�ɏ�������(sioWrite�̉�����)
*/
static inline void putBit(uint8 pin11_in)
{
	switch(sioCount % M) {
	case 0:
		/* �X�^�[�g */
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		/* �f�[�^ */
		if(!(pin11_in & PIN11_XOUT))
			sioBuffer[sioCount / M] |= (1 << (sioCount % M - 1));
		else
			sioBuffer[sioCount / M] &= ~(1 << (sioCount % M - 1));
		break;
	case 9:
		/* �G���h */
		break;
	}
	sioCount++;
}

/*
	SIO�o�b�t�@����t�@�C���ɏ�������
*/
uint8 sioWrite(uint8 pin11_in)
{
	if(sioMode != SIO_MODE_OUT)
		return 0;

	if(sioBusy) {
		if(pin11_in & PIN11_DOUT) {
			/* ���M�� */
			putBit(pin11_in);
		} else {
			/* ���M�I�� */
			sioBusy = FALSE;
			if(sioCount > 3)
				writeBin(pathSioOut, sioBuffer, sioCount / M);
			noWait = FALSE;
		}
	} else {
		if(pin11_in & PIN11_DOUT) {
			/* ���M�J�n */
			sioBusy = TRUE;
			sioCount = 0;
			memset(sioBuffer, 0, sizeof(sioBuffer));
			noWait = TRUE;
		}
	}

	return (sioBusy ? PIN11_ACK: 0);
}

/*
	SIO�o�b�t�@���f�B�X�N�ɏ�������
*/
int sioSave(const char *path)
{
	/* �o�̓t�@�C�����`�F�b�N���� */
	if(readBin(path, NULL, 0) >= 0) {
		/* OK:�t�@�C�������݂��� */
	} else if(writeBin(path, NULL, 0) >= 0) {
		/* OK:�t�@�C���𐶐��ł��� */
		removeFile(path);
	} else {
		/* NG:�t�@�C���𐶐��ł��Ȃ� */
		sioMode = SIO_MODE_STOP;
		strcpy(pathSioOut, "");
		return FALSE;
	}

	/* �o�͏�Ԃ����������� */
	strcpy(pathSioOut, path);
	sioMode = SIO_MODE_OUT;
	sioBusy = FALSE;
	sioCount = 0;
	noWait = FALSE;
	return TRUE;
}

/*
	Copyright 2006 ~ 2008 maruhiro
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
