/*
        SHARP PC-G800 series emulator
        その他の機能
*/

#include "g800.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if !defined(O_BINARY)
#define O_BINARY 0
#endif

static char autoKeyText[0x8000];        /* 自動入力するテキスト */
static char *autoKeyTextPointer = NULL; /* 自動入力するテキストの読み込み位置 */
static uint8 autoKeyBuffer[8];          /* 自動キー入力バッファ */
static uint8 *autoKeyBufferPointer =
    NULL;                    /* 自動キー入力バッファの読み込み位置 */
static uint8 autoKeyMod = 0; /* 自動キー入力前のキーモデファイア */

#if defined(Z80_TRACE)
/*
        バンク番号を得る
*/
int z80bank(const Z80stat *z, uint16 address) {
  if (address < 0xc000)
    return 0;
  else
    return romBank;
}

/*
        トレースを出力する
*/
void z80log(const Z80stat *z) {
  char buf[256];
  int states = Z80_STATES(z);

  printf("%s%dclocks (%dmsec)\n\n", z80regs(buf, z), states,
         states / (freqCPU / 1000));
}
#endif

#define MOD_CAPSUNLOCK 0xfc /* 仮想キー: CAPSロックを解除する */
#define MOD_CAPSLOCK 0xfd   /* 仮想キー: CAPSロック */
#define MOD_KANA 0xfe       /* 仮想キー: カナロック */

/*
        文字からキーストロークを得る (getAutoKeyの下請け)
*/
static int ankToKey(char ch, uint8 mod, uint8 *key) {
  const uint8 key_08[] = {GKEY_BACKSPACE, 0}; /* BS */
  const uint8 key_09[] = {GKEY_TAB, 0};       /* HT */
  const uint8 key_0a[] = {GKEY_RETURN, 0};    /* LF */
  const uint8 key_1c[] = {GKEY_RIGHT, 0};
  const uint8 key_1d[] = {GKEY_LEFT, 0};
  const uint8 key_1e[] = {GKEY_UP, 0};
  const uint8 key_1f[] = {GKEY_DOWN, 0};
  const uint8 key_20[] = {GKEY_SPACE, 0};                           /* SPACE */
  const uint8 key_21[] = {GKEY_2NDF, GKEY_Q, 0};                    /* ! */
  const uint8 key_22[] = {GKEY_2NDF, GKEY_W, 0};                    /* " */
  const uint8 key_23[] = {GKEY_2NDF, GKEY_E, 0};                    /* # */
  const uint8 key_24[] = {GKEY_2NDF, GKEY_R, 0};                    /* $ */
  const uint8 key_25[] = {GKEY_2NDF, GKEY_T, 0};                    /* % */
  const uint8 key_26[] = {GKEY_2NDF, GKEY_Y, 0};                    /* & */
  const uint8 key_27[] = {GKEY_2NDF, GKEY_U, 0};                    /* ' */
  const uint8 key_28[] = {GKEY_LKAKKO, 0};                          /* ( */
  const uint8 key_29[] = {GKEY_RKAKKO, 0};                          /* ) */
  const uint8 key_2a[] = {GKEY_ASTER, 0};                           /* * */
  const uint8 key_2b[] = {GKEY_PLUS, 0};                            /* + */
  const uint8 key_2c[] = {GKEY_COMMA, 0};                           /* , */
  const uint8 key_2d[] = {GKEY_MINUS, 0};                           /* - */
  const uint8 key_2e[] = {GKEY_PERIOD, 0};                          /* . */
  const uint8 key_2f[] = {GKEY_SLASH, 0};                           /* / */
  const uint8 key_30[] = {GKEY_0, 0};                               /* 0 */
  const uint8 key_31[] = {GKEY_1, 0};                               /* 1 */
  const uint8 key_32[] = {GKEY_2, 0};                               /* 2 */
  const uint8 key_33[] = {GKEY_3, 0};                               /* 3 */
  const uint8 key_34[] = {GKEY_4, 0};                               /* 4 */
  const uint8 key_35[] = {GKEY_5, 0};                               /* 5 */
  const uint8 key_36[] = {GKEY_6, 0};                               /* 6 */
  const uint8 key_37[] = {GKEY_7, 0};                               /* 7 */
  const uint8 key_38[] = {GKEY_8, 0};                               /* 8 */
  const uint8 key_39[] = {GKEY_9, 0};                               /* 9 */
  const uint8 key_3a[] = {GKEY_2NDF, GKEY_SEMICOLON, 0};            /* : */
  const uint8 key_3b[] = {GKEY_SEMICOLON, 0};                       /* ; */
  const uint8 key_3c[] = {GKEY_2NDF, GKEY_I, 0};                    /* < */
  const uint8 key_3d[] = {GKEY_2NDF, GKEY_L, 0};                    /* = */
  const uint8 key_3e[] = {GKEY_2NDF, GKEY_O, 0};                    /* > */
  const uint8 key_3f[] = {GKEY_2NDF, GKEY_COMMA, 0};                /* ? */
  const uint8 key_40[] = {GKEY_2NDF, GKEY_P, 0};                    /* @ */
  const uint8 key_41[] = {MOD_CAPSLOCK, GKEY_A, 0};                 /* A */
  const uint8 key_42[] = {MOD_CAPSLOCK, GKEY_B, 0};                 /* B */
  const uint8 key_43[] = {MOD_CAPSLOCK, GKEY_C, 0};                 /* C */
  const uint8 key_44[] = {MOD_CAPSLOCK, GKEY_D, 0};                 /* D */
  const uint8 key_45[] = {MOD_CAPSLOCK, GKEY_E, 0};                 /* E */
  const uint8 key_46[] = {MOD_CAPSLOCK, GKEY_F, 0};                 /* F */
  const uint8 key_47[] = {MOD_CAPSLOCK, GKEY_G, 0};                 /* G */
  const uint8 key_48[] = {MOD_CAPSLOCK, GKEY_H, 0};                 /* H */
  const uint8 key_49[] = {MOD_CAPSLOCK, GKEY_I, 0};                 /* I */
  const uint8 key_4a[] = {MOD_CAPSLOCK, GKEY_J, 0};                 /* J */
  const uint8 key_4b[] = {MOD_CAPSLOCK, GKEY_K, 0};                 /* K */
  const uint8 key_4c[] = {MOD_CAPSLOCK, GKEY_L, 0};                 /* L */
  const uint8 key_4d[] = {MOD_CAPSLOCK, GKEY_M, 0};                 /* M */
  const uint8 key_4e[] = {MOD_CAPSLOCK, GKEY_N, 0};                 /* N */
  const uint8 key_4f[] = {MOD_CAPSLOCK, GKEY_O, 0};                 /* O */
  const uint8 key_50[] = {MOD_CAPSLOCK, GKEY_P, 0};                 /* P */
  const uint8 key_51[] = {MOD_CAPSLOCK, GKEY_Q, 0};                 /* Q */
  const uint8 key_52[] = {MOD_CAPSLOCK, GKEY_R, 0};                 /* R */
  const uint8 key_53[] = {MOD_CAPSLOCK, GKEY_S, 0};                 /* S */
  const uint8 key_54[] = {MOD_CAPSLOCK, GKEY_T, 0};                 /* T */
  const uint8 key_55[] = {MOD_CAPSLOCK, GKEY_U, 0};                 /* U */
  const uint8 key_56[] = {MOD_CAPSLOCK, GKEY_V, 0};                 /* V */
  const uint8 key_57[] = {MOD_CAPSLOCK, GKEY_W, 0};                 /* W */
  const uint8 key_58[] = {MOD_CAPSLOCK, GKEY_X, 0};                 /* X */
  const uint8 key_59[] = {MOD_CAPSLOCK, GKEY_Y, 0};                 /* Y */
  const uint8 key_5a[] = {MOD_CAPSLOCK, GKEY_Z, 0};                 /* Z */
  const uint8 key_5b[] = {GKEY_2NDF, GKEY_A, 0};                    /* [ */
  const uint8 key_5c[] = {GKEY_2NDF, GKEY_G, 0};                    /* \ */
  const uint8 key_5d[] = {GKEY_2NDF, GKEY_S, 0};                    /* ] */
  const uint8 key_5e[] = {GKEY_HAT, 0};                             /* ^ */
  const uint8 key_5f[] = {GKEY_2NDF, GKEY_K, 0};                    /* _ */
  const uint8 key_60[] = {GKEY_SPACE, 0};                           /* ` */
  const uint8 key_61[] = {MOD_CAPSUNLOCK, GKEY_A, 0};               /* A */
  const uint8 key_62[] = {MOD_CAPSUNLOCK, GKEY_B, 0};               /* B */
  const uint8 key_63[] = {MOD_CAPSUNLOCK, GKEY_C, 0};               /* C */
  const uint8 key_64[] = {MOD_CAPSUNLOCK, GKEY_D, 0};               /* D */
  const uint8 key_65[] = {MOD_CAPSUNLOCK, GKEY_E, 0};               /* E */
  const uint8 key_66[] = {MOD_CAPSUNLOCK, GKEY_F, 0};               /* F */
  const uint8 key_67[] = {MOD_CAPSUNLOCK, GKEY_G, 0};               /* G */
  const uint8 key_68[] = {MOD_CAPSUNLOCK, GKEY_H, 0};               /* H */
  const uint8 key_69[] = {MOD_CAPSUNLOCK, GKEY_I, 0};               /* I */
  const uint8 key_6a[] = {MOD_CAPSUNLOCK, GKEY_J, 0};               /* J */
  const uint8 key_6b[] = {MOD_CAPSUNLOCK, GKEY_K, 0};               /* K */
  const uint8 key_6c[] = {MOD_CAPSUNLOCK, GKEY_L, 0};               /* L */
  const uint8 key_6d[] = {MOD_CAPSUNLOCK, GKEY_M, 0};               /* M */
  const uint8 key_6e[] = {MOD_CAPSUNLOCK, GKEY_N, 0};               /* N */
  const uint8 key_6f[] = {MOD_CAPSUNLOCK, GKEY_O, 0};               /* O */
  const uint8 key_70[] = {MOD_CAPSUNLOCK, GKEY_P, 0};               /* P */
  const uint8 key_71[] = {MOD_CAPSUNLOCK, GKEY_Q, 0};               /* Q */
  const uint8 key_72[] = {MOD_CAPSUNLOCK, GKEY_R, 0};               /* R */
  const uint8 key_73[] = {MOD_CAPSUNLOCK, GKEY_S, 0};               /* S */
  const uint8 key_74[] = {MOD_CAPSUNLOCK, GKEY_T, 0};               /* T */
  const uint8 key_75[] = {MOD_CAPSUNLOCK, GKEY_U, 0};               /* U */
  const uint8 key_76[] = {MOD_CAPSUNLOCK, GKEY_V, 0};               /* V */
  const uint8 key_77[] = {MOD_CAPSUNLOCK, GKEY_W, 0};               /* W */
  const uint8 key_78[] = {MOD_CAPSUNLOCK, GKEY_X, 0};               /* X */
  const uint8 key_79[] = {MOD_CAPSUNLOCK, GKEY_Y, 0};               /* Y */
  const uint8 key_7a[] = {MOD_CAPSUNLOCK, GKEY_Z, 0};               /* Z */
  const uint8 key_7b[] = {GKEY_2NDF, GKEY_D, 0};                    /* { */
  const uint8 key_7c[] = {GKEY_2NDF, GKEY_H, 0};                    /* | */
  const uint8 key_7d[] = {GKEY_2NDF, GKEY_F, 0};                    /* } */
  const uint8 key_7e[] = {GKEY_2NDF, GKEY_J, 0};                    /* ‾ */
  const uint8 key_a1[] = {MOD_KANA, GKEY_PERIOD, 0};                /* 。 */
  const uint8 key_a2[] = {MOD_KANA, GKEY_LKAKKO, 0};                /* 「 */
  const uint8 key_a3[] = {MOD_KANA, GKEY_RKAKKO, 0};                /* 」 */
  const uint8 key_a4[] = {MOD_KANA, GKEY_COMMA, 0};                 /* 、 */
  const uint8 key_a5[] = {MOD_KANA, GKEY_PLUS, 0};                  /* ・ */
  const uint8 key_a6[] = {MOD_KANA, GKEY_W, GKEY_O, 0};             /* ヲ */
  const uint8 key_a7[] = {MOD_KANA, GKEY_CAPS, GKEY_A, 0};          /* ァ */
  const uint8 key_a8[] = {MOD_KANA, GKEY_CAPS, GKEY_I, 0};          /* ィ */
  const uint8 key_a9[] = {MOD_KANA, GKEY_CAPS, GKEY_U, 0};          /* ゥ */
  const uint8 key_aa[] = {MOD_KANA, GKEY_CAPS, GKEY_E, 0};          /* ェ */
  const uint8 key_ab[] = {MOD_KANA, GKEY_CAPS, GKEY_O, 0};          /* ォ */
  const uint8 key_ac[] = {MOD_KANA, GKEY_CAPS, GKEY_Y, GKEY_A, 0};  /* ャ */
  const uint8 key_ad[] = {MOD_KANA, GKEY_CAPS, GKEY_Y, GKEY_U, 0};  /* ュ */
  const uint8 key_ae[] = {MOD_KANA, GKEY_CAPS, GKEY_Y, GKEY_O, 0};  /* ョ */
  const uint8 key_af[] = {MOD_KANA, GKEY_CAPS, GKEY_T, GKEY_U, 0};  /* ッ */
  const uint8 key_b0[] = {MOD_KANA, GKEY_2NDF, GKEY_KANA, 0};       /* ー */
  const uint8 key_b1[] = {MOD_KANA, GKEY_A, 0};                     /* ア */
  const uint8 key_b2[] = {MOD_KANA, GKEY_I, 0};                     /* イ */
  const uint8 key_b3[] = {MOD_KANA, GKEY_U, 0};                     /* ウ */
  const uint8 key_b4[] = {MOD_KANA, GKEY_E, 0};                     /* エ */
  const uint8 key_b5[] = {MOD_KANA, GKEY_O, 0};                     /* オ */
  const uint8 key_b6[] = {MOD_KANA, GKEY_K, GKEY_A, 0};             /* カ */
  const uint8 key_b7[] = {MOD_KANA, GKEY_K, GKEY_I, 0};             /* キ */
  const uint8 key_b8[] = {MOD_KANA, GKEY_K, GKEY_U, 0};             /* ク */
  const uint8 key_b9[] = {MOD_KANA, GKEY_K, GKEY_E, 0};             /* ケ */
  const uint8 key_ba[] = {MOD_KANA, GKEY_K, GKEY_O, 0};             /* コ */
  const uint8 key_bb[] = {MOD_KANA, GKEY_S, GKEY_A, 0};             /* サ */
  const uint8 key_bc[] = {MOD_KANA, GKEY_S, GKEY_I, 0};             /* シ */
  const uint8 key_bd[] = {MOD_KANA, GKEY_S, GKEY_U, 0};             /* ス */
  const uint8 key_be[] = {MOD_KANA, GKEY_S, GKEY_E, 0};             /* セ */
  const uint8 key_bf[] = {MOD_KANA, GKEY_S, GKEY_O, 0};             /* ソ */
  const uint8 key_c0[] = {MOD_KANA, GKEY_T, GKEY_A, 0};             /* タ */
  const uint8 key_c1[] = {MOD_KANA, GKEY_T, GKEY_I, 0};             /* チ */
  const uint8 key_c2[] = {MOD_KANA, GKEY_T, GKEY_U, 0};             /* ツ */
  const uint8 key_c3[] = {MOD_KANA, GKEY_T, GKEY_E, 0};             /* テ */
  const uint8 key_c4[] = {MOD_KANA, GKEY_T, GKEY_O, 0};             /* ト */
  const uint8 key_c5[] = {MOD_KANA, GKEY_N, GKEY_A, 0};             /* ナ */
  const uint8 key_c6[] = {MOD_KANA, GKEY_N, GKEY_I, 0};             /* ニ */
  const uint8 key_c7[] = {MOD_KANA, GKEY_N, GKEY_U, 0};             /* ヌ */
  const uint8 key_c8[] = {MOD_KANA, GKEY_N, GKEY_E, 0};             /* ネ */
  const uint8 key_c9[] = {MOD_KANA, GKEY_N, GKEY_O, 0};             /* ノ */
  const uint8 key_ca[] = {MOD_KANA, GKEY_H, GKEY_A, 0};             /* ハ */
  const uint8 key_cb[] = {MOD_KANA, GKEY_H, GKEY_I, 0};             /* ヒ */
  const uint8 key_cc[] = {MOD_KANA, GKEY_H, GKEY_U, 0};             /* フ */
  const uint8 key_cd[] = {MOD_KANA, GKEY_H, GKEY_E, 0};             /* ヘ */
  const uint8 key_ce[] = {MOD_KANA, GKEY_H, GKEY_O, 0};             /* ホ */
  const uint8 key_cf[] = {MOD_KANA, GKEY_M, GKEY_A, 0};             /* マ */
  const uint8 key_d0[] = {MOD_KANA, GKEY_M, GKEY_I, 0};             /* ミ */
  const uint8 key_d1[] = {MOD_KANA, GKEY_M, GKEY_U, 0};             /* ム */
  const uint8 key_d2[] = {MOD_KANA, GKEY_M, GKEY_E, 0};             /* メ */
  const uint8 key_d3[] = {MOD_KANA, GKEY_M, GKEY_O, 0};             /* モ */
  const uint8 key_d4[] = {MOD_KANA, GKEY_Y, GKEY_A, 0};             /* ヤ */
  const uint8 key_d5[] = {MOD_KANA, GKEY_Y, GKEY_U, 0};             /* ユ */
  const uint8 key_d6[] = {MOD_KANA, GKEY_Y, GKEY_O, 0};             /* ヨ */
  const uint8 key_d7[] = {MOD_KANA, GKEY_R, GKEY_A, 0};             /* ラ */
  const uint8 key_d8[] = {MOD_KANA, GKEY_R, GKEY_I, 0};             /* リ */
  const uint8 key_d9[] = {MOD_KANA, GKEY_R, GKEY_U, 0};             /* ル */
  const uint8 key_da[] = {MOD_KANA, GKEY_R, GKEY_E, 0};             /* レ */
  const uint8 key_db[] = {MOD_KANA, GKEY_R, GKEY_O, 0};             /* ロ */
  const uint8 key_dc[] = {MOD_KANA, GKEY_W, GKEY_A, 0};             /* ワ */
  const uint8 key_dd[] = {MOD_KANA, GKEY_N, GKEY_N, GKEY_RIGHT, 0}; /* ン */
  const uint8 key_de[] = {MOD_KANA,       GKEY_B,     GKEY_A, GKEY_LEFT,
                          GKEY_BACKSPACE, GKEY_RIGHT, 0}; /* ゛ */
  const uint8 key_df[] = {MOD_KANA,       GKEY_P,     GKEY_A, GKEY_LEFT,
                          GKEY_BACKSPACE, GKEY_RIGHT, 0}; /* ゜ */
  const uint8 key_f8[] = {GKEY_2NDF, GKEY_9, 0};          /* ″ */
  const uint8 *key_table[] = {
      /* 00 */
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, key_08, key_09, key_0a,
      NULL, NULL, key_0a, NULL, NULL,
      /* 10 */
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      key_1c, key_1d, key_1e, key_1f,
      /* 20 */
      key_20, key_21, key_22, key_23, key_24, key_25, key_26, key_27, key_28,
      key_29, key_2a, key_2b, key_2c, key_2d, key_2e, key_2f,
      /* 20 */
      key_30, key_31, key_32, key_33, key_34, key_35, key_36, key_37, key_38,
      key_39, key_3a, key_3b, key_3c, key_3d, key_3e, key_3f,
      /* 40 */
      key_40, key_41, key_42, key_43, key_44, key_45, key_46, key_47, key_48,
      key_49, key_4a, key_4b, key_4c, key_4d, key_4e, key_4f,
      /* 20 */
      key_50, key_51, key_52, key_53, key_54, key_55, key_56, key_57, key_58,
      key_59, key_5a, key_5b, key_5c, key_5d, key_5e, key_5f,
      /* 20 */
      key_60, key_61, key_62, key_63, key_64, key_65, key_66, key_67, key_68,
      key_69, key_6a, key_6b, key_6c, key_6d, key_6e, key_6f,
      /* 20 */
      key_70, key_71, key_72, key_73, key_74, key_75, key_76, key_77, key_78,
      key_79, key_7a, key_7b, key_7c, key_7d, key_7e, NULL,
      /* 80 */
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL,
      /* 90 */
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL,
      /* a0 */
      NULL, key_a1, key_a2, key_a3, key_a4, key_a5, key_a6, key_a7, key_a8,
      key_a9, key_aa, key_ab, key_ac, key_ad, key_ae, key_af,
      /* b0 */
      key_b0, key_b1, key_b2, key_b3, key_b4, key_b5, key_b6, key_b7, key_b8,
      key_b9, key_ba, key_bb, key_bc, key_bd, key_be, key_bf,
      /* c0 */
      key_c0, key_c1, key_c2, key_c3, key_c4, key_c5, key_c6, key_c7, key_c8,
      key_c9, key_ca, key_cb, key_cc, key_cd, key_ce, key_cf,
      /* d0 */
      key_d0, key_d1, key_d2, key_d3, key_d4, key_d5, key_d6, key_d7, key_d8,
      key_d9, key_da, key_db, key_dc, key_dd, key_de, key_df,
      /* e0 */
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL,
      /* f0 */
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, key_f8, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL};
  const uint8 *p = key_table[(unsigned char)ch];
  uint8 *k = key;

  if (p == NULL)
    return 0;

  switch (*p) {
  case MOD_CAPSUNLOCK:
    p++;
    if (mod & 0x02)
      *k++ = GKEY_CAPS;
    if (mod & 0x04)
      *k++ = GKEY_KANA;
    break;
  case MOD_CAPSLOCK:
    p++;
    if (!(mod & 0x02))
      *k++ = GKEY_CAPS;
    if (mod & 0x04)
      *k++ = GKEY_KANA;
    break;
  case MOD_KANA:
    p++;
    if (!(mod & 0x04))
      *k++ = GKEY_KANA;
    break;
  default:
    if (mod & 0x04)
      *k++ = GKEY_KANA;
    break;
  }

  while (*p != 0)
    *k++ = *p++;
  *k = 0;

  return (int)(k - key);
}

/*
        自動入力を1ストローク得る
*/
uint8 getAutoKey(void) {
  uint8 *p;

  if (autoKeyTextPointer == NULL)
    return GKEY_NONE;

  if (autoKeyBufferPointer != NULL && *autoKeyBufferPointer != 0)
    return *autoKeyBufferPointer++;

  switch (*autoKeyTextPointer) {
  case 0x00: /* NUL */
    if ((memory[0x7901] ^ autoKeyMod) == 0) {
      autoKeyTextPointer = NULL;
      noWait = FALSE;
      return GKEY_NONE;
    }

    p = autoKeyBuffer;
    if ((memory[0x7901] ^ autoKeyMod) & 0x02)
      *p++ = GKEY_CAPS;
    if ((memory[0x7901] ^ autoKeyMod) & 0x04)
      *p++ = GKEY_KANA;
    *p++ = 0;
    break;
  case 0x1a: /* EOF */
    autoKeyTextPointer = NULL;
    noWait = FALSE;
    return GKEY_NONE;
  case 0x1b: /* ESC */
    autoKeyTextPointer++;
    autoKeyBuffer[0] = *autoKeyTextPointer++;
    autoKeyBuffer[1] = 0;
    break;
  default: /* 文字 */
    if (ankToKey(*autoKeyTextPointer++, memory[0x7901], autoKeyBuffer) == 0) {
      autoKeyTextPointer = NULL;
      noWait = FALSE;
      return GKEY_NONE;
    }
    break;
  }

  autoKeyBufferPointer = autoKeyBuffer;
  return *autoKeyBufferPointer++;
}

/*
        UTF-8をUTF-16に変換する (utf8ToAnkの下請け)
*/
static int utf8ToUtf16(const char *utf8, int *utf16) {
  if ((utf8[0] & 0x80) == 0) { /* 1バイト */
    *utf16 = *utf8;
    return 1;
  } else if ((utf8[0] & 0xe0) == 0xc0) { /* 2バイト */
    *utf16 = (utf8[0] & 0x1f) << 6U | (utf8[1] & 0x3f);
    return 2;
  } else if ((utf8[0] & 0xf0) == 0xe0) { /* 3バイト */
    *utf16 =
        (utf8[0] & 0x0fU) << 12U | (utf8[1] & 0x3fU) << 6U | (utf8[2] & 0x3fU);
    return 3;
  } else if ((utf8[0] & 0xf8) == 0xf0) { /* 4バイト */
    *utf16 = 0x20;
    return 4;
  } else if ((utf8[0] & 0xfc) == 0xf8) { /* 5バイト */
    *utf16 = 0x20;
    return 5;
  } else if ((utf8[0] & 0xfe) == 0xfc) { /* 6バイト */
    *utf16 = 0x20;
    return 6;
  } else { /* ? */
    *utf16 = 0;
    return 1;
  }
}

/*
        UTF-16をANK文字に変換する (utf8ToAnkの下請け)
*/
static int utf16ToAnk(int utf16, char *ank) {
  const char *kana[] = {
      "\xde",     /* 0x309b ゛ */
      "\xdf",     /* 0x309c ゜ */
      "",         /* 0x309d */
      "",         /* 0x309e */
      "",         /* 0x309f */
      "",         /* 0x30a0 */
      "\xa7",     /* 0x30a1 ァ */
      "\xb1",     /* 0x30a2 ア */
      "\xa8",     /* 0x30a3 ィ */
      "\xb2",     /* 0x30a4 イ */
      "\xa9",     /* 0x30a5 ゥ */
      "\xb3",     /* 0x30a6 ウ */
      "\xaa",     /* 0x30a7 ェ */
      "\xb4",     /* 0x30a8 エ */
      "\xab",     /* 0x30a9 ォ */
      "\xb5",     /* 0x30aa オ */
      "\xb6",     /* 0x30ab カ */
      "\xb6\xde", /* 0x30ac ガ */
      "\xb7",     /* 0x30ad キ */
      "\xb7\xde", /* 0x30ae ギ */
      "\xb8",     /* 0x30af ク */
      "\xb8\xde", /* 0x30b0 グ */
      "\xb9",     /* 0x30b1 ケ */
      "\xb9\xde", /* 0x30b2 ゲ */
      "\xba",     /* 0x30b3 コ */
      "\xba\xde", /* 0x30b4 ゴ */
      "\xbb",     /* 0x30b5 サ */
      "\xbb\xde", /* 0x30b6 ザ */
      "\xbc",     /* 0x30b7 シ */
      "\xbc\xde", /* 0x30b8 ジ */
      "\xbd",     /* 0x30b9 ス */
      "\xbd\xde", /* 0x30ba ズ */
      "\xbe",     /* 0x30bb セ */
      "\xbe\xde", /* 0x30bc ゼ */
      "\xbf",     /* 0x30bd ソ */
      "\xbf\xde", /* 0x30be ゾ */
      "\xc0",     /* 0x30bf タ */
      "\xc0\xde", /* 0x30c0 ダ */
      "\xc1",     /* 0x30c1 チ */
      "\xc1\xde", /* 0x30c2 ヂ */
      "\xaf",     /* 0x30c3 ッ */
      "\xc2",     /* 0x30c4 ツ */
      "\xc2\xde", /* 0x30c5 ヅ */
      "\xc3",     /* 0x30c6 テ */
      "\xc3\xde", /* 0x30c7 デ */
      "\xc4",     /* 0x30c8 ト */
      "\xc4\xde", /* 0x30c9 ド */
      "\xc5",     /* 0x30ca ナ */
      "\xc6",     /* 0x30cb ニ */
      "\xc7",     /* 0x30cc ヌ */
      "\xc8",     /* 0x30cd ネ */
      "\xc9",     /* 0x30ce ノ */
      "\xca",     /* 0x30cf ハ */
      "\xca\xde", /* 0x30d0 バ */
      "\xca\xdf", /* 0x30d1 パ */
      "\xcb",     /* 0x30d2 ヒ */
      "\xcb\xde", /* 0x30d3 ビ */
      "\xcb\xdf", /* 0x30d4 ピ */
      "\xcc",     /* 0x30d5 フ */
      "\xcc\xde", /* 0x30d6 ブ */
      "\xcc\xdf", /* 0x30d7 プ */
      "\xcd",     /* 0x30d8 ヘ */
      "\xcd\xde", /* 0x30d9 ベ */
      "\xcd\xdf", /* 0x30da ペ */
      "\xce",     /* 0x30db ホ */
      "\xce\xde", /* 0x30dc ボ */
      "\xce\xdf", /* 0x30dd ポ */
      "\xcf",     /* 0x30de マ */
      "\xd0",     /* 0x30df ミ */
      "\xd1",     /* 0x30e0 ム */
      "\xd2",     /* 0x30e1 メ */
      "\xd3",     /* 0x30e2 モ */
      "\xac",     /* 0x30e3 ャ */
      "\xd4",     /* 0x30e4 ヤ */
      "\xad",     /* 0x30e5 ュ */
      "\xd5",     /* 0x30e6 ユ */
      "\xae",     /* 0x30e7 ョ */
      "\xd6",     /* 0x30e8 ヨ */
      "\xd7",     /* 0x30e9 ラ */
      "\xd8",     /* 0x30ea リ */
      "\xd9",     /* 0x30eb ル */
      "\xda",     /* 0x30ec レ */
      "\xdb",     /* 0x30ed ロ */
      "",         /* 0x30ee */
      "\xdc",     /* 0x30ef ワ */
      "",         /* 0x30f0 */
      "",         /* 0x30f1 */
      "\xa6",     /* 0x30f2 ヲ */
      "\xdd",     /* 0x30f3 ン */
      "\xb3\xde", /* 0x30f4 ヴ */
      "",         /* 0x30f5 */
      "",         /* 0x30f6 */
      "",         /* 0x30f7 */
      "",         /* 0x30f8 */
      "",         /* 0x30f9 */
      "",         /* 0x30fa */
      "\xa5",     /* 0x30fb ・ */
      "\xb0"      /* 0x30fc ー */
  };
  char *a = ank;
  const char *p;

  if (utf16 == 0)
    ;
  else if (utf16 <= 0x7f) /* ASCII */
    *a++ = utf16;
  else if (utf16 == 0x0091 || utf16 == 0x0092 || utf16 == 0x2018 ||
           utf16 == 0x2019) /* ' */
    *a++ = 0x27;
  else if (utf16 == 0x0093 || utf16 == 0x0094 || utf16 == 0x201c ||
           utf16 == 0x201d) /* " */
    *a++ = 0x22;
  else if (utf16 == 0x00a5 || utf16 == 0xffe5) /* \ */
    *a++ = 0x5c;
  else if (utf16 == 0x00b0) /* ° */
    *a++ = 0xdf;
  else if (utf16 == 0x2032) /* ′ */
    *a++ = 0x27;
  else if (utf16 == 0x2033) /* ″ */
    *a++ = 0xf8;
  else if (utf16 == 0x3000) /*   */
    *a++ = 0x20;
  else if (utf16 == 0x3001) /* 、 */
    *a++ = 0xa4;
  else if (utf16 == 0x3002) /* 。 */
    *a++ = 0xa1;
  else if (utf16 == 0x300c) /* 「 */
    *a++ = 0xa2;
  else if (utf16 == 0x300d) /* 」 */
    *a++ = 0xa3;
  else if (0x309b <= utf16 &&
           utf16 <= 0x309b + sizeof(kana) / sizeof(kana[0])) /* カナ */
    for (p = kana[utf16 - 0x309b]; *p != 0; p++)
      *a++ = *p;
  else if (0xff00 <= utf16 && utf16 <= 0xff9f) /* 全角英数記号・半角カナ */
    *a++ = utf16 - 0xff00U + 0x40;
  else if (utf16 == 0xffe3) /* ‾ */
    *a++ = 0x7e;

  *a = 0;
  return (int)(a - ank);
}

/*
        UTF-8文字列をANK文字列に変換する (setAutoKeyTextの下請け)
*/
static int utf8ToAnk(const char *utf8, char *ank, int ank_size) {
  int len = strlen(utf8), size, utf16;
  const char *u = utf8;
  char buf[8], *a = ank;

  while (u < utf8 + len) {
    u += utf8ToUtf16(u, &utf16);

    size = utf16ToAnk(utf16, buf);
    if ((int)(a - ank) + size >= ank_size - 1)
      break;

    memcpy(a, buf, size + 1);
    a += size;
  }

  return (int)(a - ank);
}

/*
        自動キー入力するテキストを設定する
*/
void setAutoKeyText(const char *text, int utf8) {
  memset(autoKeyText, 0, sizeof(autoKeyText));

  if (utf8)
    utf8ToAnk(text, autoKeyText, sizeof(autoKeyText));
  else
    strncpy(autoKeyText, text, sizeof(autoKeyText) - 1);

  autoKeyMod = memory[0x7901];

  autoKeyBufferPointer = NULL;
  autoKeyTextPointer = autoKeyText;
  noWait = TRUE;
}

/*
        自動キー入力するキーを設定する
*/
void setAutoKey(uint8 key) {
  char buf[4];

  buf[0] = 0x1b;
  buf[1] = key;
  buf[2] = 0x1a;
  buf[3] = 0x00;
  setAutoKeyText(buf, FALSE);
}

/*
        ANK文字列をUTF-8文字列に変換する
*/
char *ankToUtf8(const uint8 *ank, char *utf8) {
  char *u = utf8;
  const uint8 *a;

  for (a = ank; *a != 0; a++)
    if (*a == 0xdf) { /* ° */
      *u++ = 0xc2;
      *u++ = 0xb0;
    } else if (*a == 0x27) { /* ′ */
      *u++ = 0xe2;
      *u++ = 0x80;
      *u++ = 0xb2;
    } else if (*a == 0xf8) { /* ″ */
      *u++ = 0xe2;
      *u++ = 0x80;
      *u++ = 0xb3;
    } else
      *u++ = *a;

  *u = 0;
  return utf8;
}

#if defined(Z80_TRACE)
/*
        ファイル名からシンボルファイル名を得る (loadSymの下請け)
*/
static char *getSymFileName(char *sym, const char *file) {
  char *p;

  strcpy(sym, file);

  for (p = sym + strlen(sym) - 1; p > sym && *p != '.'; p--)
    ;
  strcpy(p + 1, "sym");
  return sym;
}

/*
        シンボルレコードを書き込む (loadSymの下請け)
*/
static int writeSymRecord(Z80symbol *sym, const char *record) {
  memset(sym, 0, sizeof(*sym));
  return sscanf(record, "%hx:%hx %31s", &sym->bank, &sym->address,
                &sym->name) == 3;
}

/*
        シンボルファイルを読み込む
*/
int loadSym(const char *path) {
  FILE *fp;
  int n = 0;
  char buf[256];

  if (z80.i.symbol != NULL) {
    free(z80.i.symbol);
    z80.i.symbol = NULL;
  }

  if ((fp = fopen(getSymFileName(buf, path), "r"
#if defined(WIN32)
                                             "t"
#endif
                  )) == NULL)
    return -1;

  z80.i.symbol = malloc(sizeof(*z80.i.symbol));

  while (!feof(fp)) {
    fgets(buf, sizeof(buf), fp);
    if (writeSymRecord(&z80.i.symbol[n], buf))
      z80.i.symbol = realloc(z80.i.symbol, (++n + 1) * sizeof(*z80.i.symbol));
  }

  memset(&z80.i.symbol[n], 0, sizeof(z80.i.symbol[n]));

  fclose(fp);
  return 0;
}
#endif

#if defined(Z80_PROF)
/*
        プロファイラの結果のバンク・アドレスを比較する(qsort用) (下請け)
*/
static int cmpProfRecord(const void *a, const void *b) {
  const Z80record *x = a, *y = b;
  int result;

  if ((result = (int)x->bank - (int)y->bank) != 0)
    return result;
  return (int)x->address - (int)y->address;
}

/*
        サブルーチン呼び出し回数の結果をファイルに出力する
*/
int writeProfFile(const char *path) {
  FILE *fp;
  Z80record *p;
  int n;
  const char *sym;

  if (z80.i.prof.stack == NULL)
    return 0;

  if ((fp = fopen(path, "w"
#if defined(WIN32)
                        "t"
#endif
                  )) == NULL)
    return -1;

  for (p = z80.i.prof.record, n = 0; p->count != 0; p++, n++)
    ;
  qsort(z80.i.prof.record, n, sizeof(*z80.i.prof.record), cmpProfRecord);

  for (p = z80.i.prof.record; p->count != 0; p++)
    if (p->bank == 0 && p->address <= 0x7fff) {
      sym = z80symbol(z80.i.symbol, p->bank, p->address);
      fprintf(fp, "%02x:%04x\t%s\t%d\t%I64d\t%I64d\n", p->bank, p->address,
              (sym != NULL ? sym : ""), p->count, p->states,
              p->states / p->count);
    }

  fclose(fp);
  return 0;
}

/*
        命令呼び出し回数の結果をファイルに出力する
*/
int writePathFile(const char *path) {
  FILE *fp;
  Z80path *p;
  int bank;
  uint16 address;
  char disasm[32];
  const char *sym;

  if (z80.i.prof.path == NULL)
    return 0;

  if ((fp = fopen(path, "w"
#if defined(WIN32)
                        "t"
#endif
                  )) == NULL)
    return -1;

  for (bank = 0; bank <= 0; bank++)
    if (z80.i.prof.path[bank] != NULL)
      for (p = &z80.i.prof.path[bank][0]; p <= &z80.i.prof.path[bank][0x7fff];
           p++)
        if (p->count > 0) {
          address = (uint16)(p - &z80.i.prof.path[bank][0]);
          z80disasm(disasm, p->code, bank, address, z80.i.symbol);
          sym = z80symbol(z80.i.symbol, bank, address);
          if (p->code[0] == 0x10 || /* djnz e */
              p->code[0] == 0x20 || /* jr NZ, e */
              p->code[0] == 0x28 || /* jr Z, e */
              p->code[0] == 0x30 || /* jr NC, e */
              p->code[0] == 0x38 || /* jr C, e */
              p->code[0] == 0xc0 || /* ret NZ */
              p->code[0] == 0xc2 || /* jp NZ, mn */
              p->code[0] == 0xc4 || /* call NZ, mn */
              p->code[0] == 0xc8 || /* ret Z */
              p->code[0] == 0xca || /* jp Z, mn */
              p->code[0] == 0xcc || /* call Z, mn */
              p->code[0] == 0xd0 || /* ret NC */
              p->code[0] == 0xd2 || /* jp NC, mn */
              p->code[0] == 0xd4 || /* call NC, mn */
              p->code[0] == 0xd8 || /* ret C */
              p->code[0] == 0xda || /* jp C, mn */
              p->code[0] == 0xdc || /* call C, mn */
              p->code[0] == 0xe0 || /* ret PO */
              p->code[0] == 0xe2 || /* jp PO, mn */
              p->code[0] == 0xe4 || /* call PO, mn */
              p->code[0] == 0xe8 || /* ret PE */
              p->code[0] == 0xea || /* jp PE, mn */
              p->code[0] == 0xec || /* call PE, mn */
              p->code[0] == 0xf0 || /* ret P */
              p->code[0] == 0xf2 || /* jp P, mn */
              p->code[0] == 0xf4 || /* call P, mn */
              p->code[0] == 0xf8 || /* ret M */
              p->code[0] == 0xfa || /* jp M, mn */
              p->code[0] == 0xfc    /* call M, mn */
          )
            fprintf(fp, "%02x:%04x\t%s\t%s\t%d\t%I64d\t%f\t%d\t%f", bank,
                    address, (sym != NULL ? sym : ""), disasm, p->count,
                    p->states, (double)p->states / p->count, p->cond,
                    ((double)p->cond / p->count));
          else
            fprintf(fp, "%02x:%04x\t%s\t%s\t%d\t%I64d\t%f\t\t", bank, address,
                    (sym != NULL ? sym : ""), disasm, p->count, p->states,
                    (double)p->states / p->count);
          if (p->code[0] == 0xcd /* call mn */
          )
            fprintf(fp, "\t%I64d\t%f\n", p->sub_states,
                    ((double)p->sub_states / p->count));
          else if (p->code[0] == 0xc4 || /* call NZ, mn */
                   p->code[0] == 0xc7 || /* rst 00H */
                   p->code[0] == 0xcc || /* call Z, mn */
                   p->code[0] == 0xcf || /* rst 08H */
                   p->code[0] == 0xd4 || /* call NC, mn */
                   p->code[0] == 0xd7 || /* rst 10H */
                   p->code[0] == 0xdc || /* call C, mn */
                   p->code[0] == 0xdf || /* rst 18H */
                   p->code[0] == 0xe4 || /* call PO, mn */
                   p->code[0] == 0xe7 || /* rst 20H */
                   p->code[0] == 0xec || /* call PE, mn */
                   p->code[0] == 0xef || /* rst 28H */
                   p->code[0] == 0xf4 || /* call P, mn */
                   p->code[0] == 0xf7 || /* rst 30H */
                   p->code[0] == 0xfc || /* call M, mn */
                   p->code[0] == 0xff    /* rst 38H */
          )
            fprintf(fp, "\t%I64d\t%f\t%f\n", p->sub_states,
                    ((double)p->sub_states / p->count),
                    (p->cond > 0 ? (double)p->sub_states / p->cond : .0));
          else
            fprintf(fp, "\n");

          fflush(fp);
        }

  fclose(fp);
  return 0;
}
#endif

/*
        Copyright 2005 ‾ 2024 maruhiro
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
