/*
        SHARP PC-G800 series emulator
        BASICインタープリタ ヘッダ
*/

#if !defined(BASIC_H)
#define BASIC_H

#include <stdint.h>
#include <stdio.h>

/* サイズ */
#define SIZEOF_NUM 8 /* 数値のサイズ */

/* 中間コード */
#define CODE_MON 0x0f
#define CODE_RUN 0x10
#define CODE_NEW 0x11
#define CODE_CONT 0x12
#define CODE_PASS 0x13
#define CODE_LIST 0x14
#define CODE_LLIST 0x15
#define CODE_BLOAD 0x16
#define CODE_RENUM 0x17
#define CODE_LOAD 0x18
#define CODE_AUTO 0x1a
#define CODE_DELETE 0x1b
#define CODE_FILES 0x1c
#define CODE_LCOPY 0x1f
#define CODE_BSAVE 0x20
#define CODE_OPEN 0x21
#define CODE_CLOSE 0x22
#define CODE_SAVE 0x23
#define CODE_RANDOMIZE 0x25
#define CODE_DEGREE 0x26
#define CODE_RADIAN 0x27
#define CODE_GRAD 0x28
#define CODE_BEEP 0x29
#define CODE_WAIT 0x2a
#define CODE_GOTO 0x2b
#define CODE_TRON 0x2c
#define CODE_TROFF 0x2d
#define CODE_CLEAR 0x2e
#define CODE_USING 0x2f
#define CODE_DIM 0x30
#define CODE_CALL 0x31
#define CODE_POKE 0x32
#define CODE_GPRINT 0x33
#define CODE_PSET 0x34
#define CODE_PRESET 0x35
#define CODE_ERASE 0x3a
#define CODE_LFILES 0x3b
#define CODE_KILL 0x3c
#define CODE_OUT 0x45
#define CODE_PIOSET 0x48
#define CODE_PIOPUT 0x49
#define CODE_SPOUT 0x4a
#define CODE_SPINP 0x4b
#define CODE_HDCOPY 0x4c
#define CODE_ENDIF 0x4d
#define CODE_REPEAT 0x4e
#define CODE_UNTIL 0x4f
#define CODE_CLS 0x50
#define CODE_LOCATE 0x51
#define CODE_TO 0x52
#define CODE_STEP 0x53
#define CODE_THEN 0x54
#define CODE_ON 0x55
#define CODE_IF 0x56
#define CODE_FOR 0x57
#define CODE_LET 0x58
#define CODE_REM 0x59
#define CODE_END 0x5a
#define CODE_NEXT 0x5b
#define CODE_STOP 0x5c
#define CODE_READ 0x5d
#define CODE_DATA 0x5e
#define CODE_PRINT 0x60
#define CODE_INPUT 0x61
#define CODE_GOSUB 0x62
#define CODE_LNINPUT 0x63
#define CODE_LPRINT 0x64
#define CODE_RETURN 0x65
#define CODE_RESTORE 0x66
#define CODE_GCURSOR 0x68
#define CODE_LINE 0x69
#define CODE_CIRCLE 0x6f
#define CODE_PAINT 0x70
#define CODE_OUTPUT 0x71
#define CODE_APPEND 0x72
#define CODE_AS 0x73
#define CODE_ELSE 0x76
#define CODE_WHILE 0x7a
#define CODE_WEND 0x7b
#define CODE_SWITCH 0x7c
#define CODE_CASE 0x7d
#define CODE_DEFAULT 0x7e
#define CODE_ENDSWITCH 0x7f
#define CODE_MDF 0x80
#define CODE_REC 0x81
#define CODE_POL 0x82
#define CODE_TEN 0x86
#define CODE_RCP 0x87
#define CODE_SQU 0x88
#define CODE_CUR 0x89
#define CODE_HSN 0x8a
#define CODE_HCS 0x8b
#define CODE_HTN 0x8c
#define CODE_AHS 0x8d
#define CODE_AHC 0x8e
#define CODE_AHT 0x8f
#define CODE_FACT 0x90
#define CODE_LN 0x91
#define CODE_LOG 0x92
#define CODE_EXP 0x93
#define CODE_SQR 0x94
#define CODE_SIN 0x95
#define CODE_COS 0x96
#define CODE_TAN 0x97
#define CODE_INT 0x98
#define CODE_ABS 0x99
#define CODE_SGN 0x9a
#define CODE_DEG 0x9b
#define CODE_DMS 0x9c
#define CODE_ASN 0x9d
#define CODE_ACS 0x9e
#define CODE_ATN 0x9f
#define CODE_RND 0xa0
#define CODE_AND 0xa1
#define CODE_OR 0xa2
#define CODE_NOT 0xa3
#define CODE_PEEK 0xa4
#define CODE_XOR 0xa5
#define CODE_INP 0xa6
#define CODE_PIOGET 0xa8
#define CODE_POINT 0xad
#define CODE_PI 0xae
#define CODE_FRE 0xaf
#define CODE_EOF 0xb0
#define CODE_LOF 0xb2
#define CODE_NCR 0xb6
#define CODE_NPR 0xb7
#define CODE_CUB 0xbf
#define CODE_MOD 0xc6
#define CODE_FIX 0xc7
#define CODE_ASC 0xd0
#define CODE_VAL 0xd1
#define CODE_LEN 0xd2
#define CODE_VDEG 0xd3
#define CODE_INKEY_S 0xe9
#define CODE_MID_S 0xea
#define CODE_LEFT_S 0xeb
#define CODE_RIGHT_S 0xec
#define CODE_CHR_S 0xf0
#define CODE_STR_S 0xf1
#define CODE_HEX_S 0xf2
#define CODE_DMS_S 0xf3
#define CODE_RESERVED 0xfe

struct ForLoop {
  uint8_t kind;
  int line_no;
  const uint8_t *ret;
  const uint8_t *var;
  uint8_t to[SIZEOF_NUM];
  uint8_t step[SIZEOF_NUM];
};

struct RepeatLoop {
  uint8_t kind;
  int line_no;
  const uint8_t *ret;
};

struct WhileLoop {
  uint8_t kind;
  int line_no;
  const uint8_t *ret;
};

struct GosubReturn {
  uint8_t kind;
  int line_no;
  const uint8_t *ret;
};

struct SwitchBase {
  uint8_t kind;
};

/* BASICシステムの状態 */
struct Basic {
  int prog_size; /* プログラム領域のサイズ */
  uint8_t *prog; /* プログラム領域 */

  const uint8_t *d; /* DATA文の読み込み位置 */
  const uint8_t *p; /* プログラムの実行位置 */
  int line_no;      /* プログラムの実行中行番号 */

  uint8_t (*fixed_var)[SIZEOF_NUM]; /* 固定変数 */
  uint8_t **vars;                   /* 単純変数・配列変数 */

  int auto_line_no; /* AUTO時の行番号 */
  int auto_step;    /* AUTO時のステップ */

  int seed;
  int format_period;
  int format_comma;
  int format_exp;
  int format_ints;
  int format_decs;
  int format_strs;

  FILE *fp[3];

  union Flow {
    uint8_t kind;

    ForLoop for_loop;
    RepeatLoop repeat_loop;
    WhileLoop while_loop;
    GosubReturn gosub_return;
    SwitchBase switch_case;
  } stack[16], *top;
};

#endif

/*
        Copyright 2005 ~ 2017 maruhiro
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
