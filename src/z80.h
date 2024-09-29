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

/* z80exec()の戻り値 */
#define Z80_RUN 0       /* 動作中 */
#define Z80_HALT 1      /* HALT中 */
#define Z80_UNDERFLOW 2 /* スタックアンダーフロー */

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

/* 8bitsレジスタ */
typedef struct {
  uint8 *m;
#if defined(Z80_LITTLEENDIAN)
  uint8 f, a;     /* フラグ, アキュムレータ */
  uint8 c, b;     /* 汎用レジスタC, B */
  uint8 e, d;     /* 汎用レジスタE, D */
  uint8 l, h;     /* 汎用レジスタL, H */
  uint8 ixl, ixh; /* インデックスレジスタIXl, IXh */
  uint8 iyl, iyh; /* インデックスレジスタIYl, IYh */
  uint8 i;        /* インタラプトレジスタI */
  uint8 pad1;
  uint8 f_d, a_d; /* 補助レジスタF', A' */
#elif defined(Z80_BIGENDIAN)
  uint8 a, f;     /* アキュムレータ, フラグ */
  uint8 b, c;     /* 汎用レジスタB, C */
  uint8 d, e;     /* 汎用レジスタD, E */
  uint8 h, l;     /* 汎用レジスタH, L */
  uint8 ixh, ixl; /* インデックスレジスタIXh, IXl */
  uint8 iyh, iyl; /* インデックスレジスタIYh, IYl */
  uint8 pad2;
  uint8 i;        /* インタラプトレジスタI */
  uint8 a_d, f_d; /* 補助レジスタA', F' */
#else
  ERROR: MUST DEFINE Z80_LITTLEENDIAN OR Z80_BIGENDIAN
#endif
  uint8 pad3, pad4;
  uint8 pad5, pad6;
  uint8 pad7, pad8;
  uint8 pad9, pad10;
  uint8 pad11, pad12;
  uint8 iff;  /* IFF1, IFF2 */
  uint8 im;   /* 割り込みモード */
  uint8 halt; /* HALTか? */
  uint8 pad13;
  uint8 pad14, pad15;
} Z80regs;

/* 16bitsレジスタ */
typedef struct {
  uint8 *m;
  uint16 af; /* ペアレジスタAF */
  uint16 bc; /* ペアレジスタBC */
  uint16 de; /* ペアレジスタDE */
  uint16 hl; /* ペアレジスタHL */
  uint16 ix; /* インデックスレジスタIX */
  uint16 iy; /* インデックスレジスタIY */
  uint16 pad2;
  uint16 af_d; /* 補助レジスタAF' */
  uint16 bc_d; /* 補助レジスタBC' */
  uint16 de_d; /* 補助レジスタDE' */
  uint16 hl_d; /* 補助レジスタHL' */
  uint16 sp;   /* スタックポインタSP */
  uint16 pc;   /* プログラムカウンタPC */
} Z80regs16;

#if defined(Z80_TRACE)
/* シンボル */
typedef struct {
  uint16 bank;    /* バンク番号 */
  uint16 address; /* アドレス */
  char name[32];  /* シンボル名 */
} Z80symbol;
#endif

#if defined(Z80_PROF)
/* コールスタック */
typedef struct {
  uint16 bank;    /* サブルーチンのバンク */
  uint16 address; /* サブルーチンのアドレス */
  uint16 sp;      /* 呼び出し時スタックポインタ */
  int64 states;   /* 呼び出し時総ステート数 */
} Z80stack;

/* サブルーチンの呼び出し記録 */
typedef struct {
  uint16 bank;    /* サブルーチンのバンク */
  uint16 address; /* サブルーチンのアドレス */
  int count;      /* 呼び出し回数 */
  int64 states;   /* 総ステート数 */
} Z80record;

/* コードの実行記録 */
typedef struct {
  int count;        /* 実行回数 */
  int cond;         /* 条件成立回数 */
  uint8 code[4];    /* コード */
  int64 states;     /* 総ステート数 */
  int64 sub_states; /* サブルーチンの総ステート数 */
} Z80path;

/* プロファイラ */
typedef struct {
  Z80record *record; /* サブルーチンの呼び出し記録 */
  Z80stack *stack;   /* コールスタック */
  Z80stack *cur;     /* 現在のスタック位置 */
  Z80path **path;    /* コード実行記録 */
  Z80path *pos;      /* 現在の実行位置 */
} Z80prof;
#endif

/* オプション・その他の情報 */
typedef struct {
  Z80regs pad1;
  int states;         /* 実行するステート数 */
  uint16 stack_under; /* スタック下限 */
  uint16 pad2;
  int total_states;       /* 累積ステート数 */
  int emulate_subroutine; /* サブルーチンをエミュレートするか? */
  void *user_data;        /* その他の情報 */
#if defined(Z80_TRACE)
  int trace;         /* トレースモードか? */
  Z80symbol *symbol; /* シンボル */
#endif
#if defined(Z80_PROF)
  Z80prof prof; /* プロファイラ */
#endif
} Z80info;

/* レジスタ */
typedef union {
  uint8 *m;      /* メモリ */
  Z80regs r;     /* 8bitsレジスタ */
  Z80regs16 r16; /* 16bitsレジスタ */
  Z80info i;     /* オプション・その他の状態 */
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

/* ユーザ定義 */
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
