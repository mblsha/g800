/*
        Zilog Z80 Emulator Header
*/
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

#if !defined(Z80_H)
#define Z80_H

#include <stdio.h>
#if defined(Z80_USE_SDL)
#include "SDL.h"
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define Z80_LITTLEENDIAN 1
#elif SDL_BYTEORDER == SDL_BIG_ENDIAN
#define Z80_BIGENDIAN 1
#endif
#endif

/* z80exec()�̖߂�l */
#define Z80_RUN 0       /* ���쒆 */
#define Z80_HALT 1      /* HALT�� */
#define Z80_UNDERFLOW 2 /* �X�^�b�N�A���_�[�t���[ */

#define Z80_STATES(z) ((z)->i.total_states - (z)->i.states)
#define Z80_RESET_STATES(z) ((z)->i.total_states = (z)->i.states)

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long uint64;
typedef long long int64;

/* 8bits���W�X�^ */
typedef struct {
  uint8 *m;
#if defined(Z80_LITTLEENDIAN)
  uint8 f, a;     /* �t���O, �A�L�������[�^ */
  uint8 c, b;     /* �ėp���W�X�^C, B */
  uint8 e, d;     /* �ėp���W�X�^E, D */
  uint8 l, h;     /* �ėp���W�X�^L, H */
  uint8 ixl, ixh; /* �C���f�b�N�X���W�X�^IXl, IXh */
  uint8 iyl, iyh; /* �C���f�b�N�X���W�X�^IYl, IYh */
  uint8 i;        /* �C���^���v�g���W�X�^I */
  uint8 pad1;
  uint8 f_d, a_d; /* �⏕���W�X�^F', A' */
#elif defined(Z80_BIGENDIAN)
  uint8 a, f;     /* �A�L�������[�^, �t���O */
  uint8 b, c;     /* �ėp���W�X�^B, C */
  uint8 d, e;     /* �ėp���W�X�^D, E */
  uint8 h, l;     /* �ėp���W�X�^H, L */
  uint8 ixh, ixl; /* �C���f�b�N�X���W�X�^IXh, IXl */
  uint8 iyh, iyl; /* �C���f�b�N�X���W�X�^IYh, IYl */
  uint8 pad2;
  uint8 i;        /* �C���^���v�g���W�X�^I */
  uint8 a_d, f_d; /* �⏕���W�X�^A', F' */
#else
  ERROR: MUST DEFINE Z80_LITTLEENDIAN OR Z80_BIGENDIAN
#endif
  uint8 pad3, pad4;
  uint8 pad5, pad6;
  uint8 pad7, pad8;
  uint8 pad9, pad10;
  uint8 pad11, pad12;
  uint8 iff;  /* IFF1, IFF2 */
  uint8 im;   /* ���荞�݃��[�h */
  uint8 halt; /* HALT��? */
  uint8 pad13;
  uint8 pad14, pad15;
} Z80regs;

/* 16bits���W�X�^ */
typedef struct {
  uint8 *m;
  uint16 af; /* �y�A���W�X�^AF */
  uint16 bc; /* �y�A���W�X�^BC */
  uint16 de; /* �y�A���W�X�^DE */
  uint16 hl; /* �y�A���W�X�^HL */
  uint16 ix; /* �C���f�b�N�X���W�X�^IX */
  uint16 iy; /* �C���f�b�N�X���W�X�^IY */
  uint16 pad2;
  uint16 af_d; /* �⏕���W�X�^AF' */
  uint16 bc_d; /* �⏕���W�X�^BC' */
  uint16 de_d; /* �⏕���W�X�^DE' */
  uint16 hl_d; /* �⏕���W�X�^HL' */
  uint16 sp;   /* �X�^�b�N�|�C���^SP */
  uint16 pc;   /* �v���O�����J�E���^PC */
} Z80regs16;

#if defined(Z80_TRACE)
/* �V���{�� */
typedef struct {
  uint16 bank;    /* �o���N�ԍ� */
  uint16 address; /* �A�h���X */
  char name[32];  /* �V���{���� */
} Z80symbol;
#endif

#if defined(Z80_PROF)
/* �R�[���X�^�b�N */
typedef struct {
  uint16 bank;    /* �T�u���[�`���̃o���N */
  uint16 address; /* �T�u���[�`���̃A�h���X */
  uint16 sp;      /* �Ăяo�����X�^�b�N�|�C���^ */
  int64 states;   /* �Ăяo�������X�e�[�g�� */
} Z80stack;

/* �T�u���[�`���̌Ăяo���L�^ */
typedef struct {
  uint16 bank;    /* �T�u���[�`���̃o���N */
  uint16 address; /* �T�u���[�`���̃A�h���X */
  int count;      /* �Ăяo���� */
  int64 states;   /* ���X�e�[�g�� */
} Z80record;

/* �R�[�h�̎��s�L�^ */
typedef struct {
  int count;        /* ���s�� */
  int cond;         /* ���������� */
  uint8 code[4];    /* �R�[�h */
  int64 states;     /* ���X�e�[�g�� */
  int64 sub_states; /* �T�u���[�`���̑��X�e�[�g�� */
} Z80path;

/* �v���t�@�C�� */
typedef struct {
  Z80record *record; /* �T�u���[�`���̌Ăяo���L�^ */
  Z80stack *stack;   /* �R�[���X�^�b�N */
  Z80stack *cur;     /* ���݂̃X�^�b�N�ʒu */
  Z80path **path;    /* �R�[�h���s�L�^ */
  Z80path *pos;      /* ���݂̎��s�ʒu */
} Z80prof;
#endif

/* �I�v�V�����E���̑��̏�� */
typedef struct {
  Z80regs pad1;
  int states;         /* ���s����X�e�[�g�� */
  uint16 stack_under; /* �X�^�b�N���� */
  uint16 pad2;
  int total_states;       /* �ݐσX�e�[�g�� */
  int emulate_subroutine; /* �T�u���[�`�����G�~�����[�g���邩? */
  void *user_data;        /* ���̑��̏�� */
#if defined(Z80_TRACE)
  int trace;         /* �g���[�X���[�h��? */
  Z80symbol *symbol; /* �V���{�� */
#endif
#if defined(Z80_PROF)
  Z80prof prof; /* �v���t�@�C�� */
#endif
} Z80info;

/* ���W�X�^ */
typedef union {
  uint8 *m;      /* ������ */
  Z80regs r;     /* 8bits���W�X�^ */
  Z80regs16 r16; /* 16bits���W�X�^ */
  Z80info i;     /* �I�v�V�����E���̑��̏�� */
} Z80stat;

/* z80.c */
void z80srand(uint32);
int z80reset(Z80stat *);
int z80nmi(Z80stat *);
int z80int0chk(const Z80stat *);
int z80int0(Z80stat *, uint8);
int z80int1chk(const Z80stat *);
int z80int1(Z80stat *);
int z80int2chk(const Z80stat *);
int z80int2(Z80stat *, uint8);
int z80exec(Z80stat *);
void z80init(Z80stat *);

/* z80disasm.c */
const char *z80symbol(const Z80symbol *, int, uint16);
void *z80disasm(char *, uint8 *, int, uint16, const Z80symbol *);
char *z80regs(char *, const Z80stat *);

/* z80prof.c */
#if defined(Z80_PROF)
void z80prof_init(Z80stat *, int);
void z80prof_clear(Z80stat *);
void z80prof_call(Z80stat *, int);
void z80prof_ret(Z80stat *, int);
void z80prof_path(Z80stat *);
void z80prof_exec(Z80stat *, int);
void z80prof_cond(Z80stat *, int);
#endif

/* ���[�U��` */
#include "z80memory.h"
int z80inport(Z80stat *, uint8 *, uint16);
int z80outport(Z80stat *, uint16, uint8);
int z80subroutine(Z80stat *, uint16);
#if defined(Z80_TRACE)
int z80bank(const Z80stat *, uint16);
void z80log(const Z80stat *);
#endif

#endif

/*
        Copyright 2005 ~ 2024 maruhiro
        All rights reserved.

        Redistribution and use in source and binary forms,
        with or without modification, are permitted provided that
        the following conditions are met:

         1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

         2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

        THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
   EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
   OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* eof */
