/*
	SHARP PC-G800 series emulator
	BASIC�C���^�[�v���^
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "g800.h"

/* �^�U */
#define FALSE	0	/* �U */
#define TRUE	1	/* �^ */

/* ���z�R�[�h */
#define CODE_ADD	0x100	/* + */
#define CODE_SUB	0x101	/* - */
#define CODE_MUL	0x102	/* * */
#define CODE_DIV	0x103	/* / */
#define CODE_IDIV	0x104	/* \ */
#define CODE_POW	0x105	/* ^ */
#define CODE_EQ	0x106	/* = */
#define CODE_NE	0x107	/* <> */
#define CODE_GT	0x108	/* > */
#define CODE_GE	0x109	/* >= */
#define CODE_LT	0x10a	/* < */
#define CODE_LE	0x10b	/* <= */
#define CODE_POS	0x10c	/* + */
#define CODE_NEG	0x10d	/* - */

/* �G���[ */
#define ERR_OK	0	/* ���� */
#define ERR_OK_NEXT	0	/* ����E���̖��߂����s */
#define ERR_OK_JUMP	1	/* ����E�W�����v */
#define ERR_OK_CONT	2	/* ����E�Ď��s */
#define ERR_10	-10	/* �\���G���[ */
#define ERR_11	-11
#define ERR_12	-12	/* ���[�h���������Ȃ� */
#define ERR_13	-13	/* �Ď��s�s�� */
#define ERR_14	-14	/* �p�X���[�h�ݒ�s�� */
#define ERR_15	-15	/* �s���ȃA�h���X */
#define ERR_16	-16
#define ERR_17	-17
#define ERR_18	-18
#define ERR_19	-19
#define ERR_20	-20	/* �I�[�o�[�t���[ */
#define ERR_21	-21	/* 0�ŏ��Z */
#define ERR_22	-22	/* �s�\�ȉ��Z */
#define ERR_23	-23
#define ERR_24	-24
#define ERR_25	-25
#define ERR_26	-26
#define ERR_27	-27
#define ERR_28	-28
#define ERR_29	-29
#define ERR_30	-30	/* �z��ϐ����錾�ς� */
#define ERR_31	-31	/* �z��ϐ����錾����Ă��Ȃ� */
#define ERR_32	-32	/* �z��ϐ��̓Y�������͈͊O */
#define ERR_33	-33	/* �l���͈͊O */
#define ERR_34	-34
#define ERR_35	-35
#define ERR_36	-36
#define ERR_37	-37
#define ERR_38	-38
#define ERR_39	-39
#define ERR_40	-40	/* �s�ԍ��܂��̓��x�������݂��Ȃ� */
#define ERR_41	-41	/* �s�ԍ����͈͊O */
#define ERR_42	-42
#define ERR_43	-43	/* �s�ԍ����s�� */
#define ERR_44	-44	/* �I���s���J�n�s���O */
#define ERR_45	-45
#define ERR_46	-46
#define ERR_47	-47
#define ERR_48	-48
#define ERR_49	-49
#define ERR_50	-50	/* �l�X�g���[������ */
#define ERR_51	-51	/* GOSUB�̂Ȃ�RETURN */
#define ERR_52	-52	/* FOR�̂Ȃ�NEXT */
#define ERR_53	-53	/* DATA�̂Ȃ�READ */
#define ERR_54	-54	/* ���Z�����G������ */
#define ERR_55	-55	/* �����񂪒������� */
#define ERR_56	-56
#define ERR_57	-57
#define ERR_58	-58
#define ERR_59	-59
#define ERR_60	-60	/* �������s�� */
#define ERR_61	-61	/* ENDIF�̂Ȃ�IF�܂���ELSE */
#define ERR_62	-62	/* REPEAT�̂Ȃ�UNTIL */ /* ??? */
#define ERR_63	-63	/* WEND�̂Ȃ�WHILE */ /* ??? */
#define ERR_64	-64	/* WHILE�̂Ȃ�WEND */
#define ERR_65	-65
#define ERR_66	-66	/* DEFAULT����CASE�܂���DEFAULT */
#define ERR_67	-67
#define ERR_68	-68	/* ENDSWITCH�̂Ȃ�SWITCH */
#define ERR_69	-69	/* SWITCH�̂Ȃ�CASE�܂���DEFAULT�܂���ENDSWITCH */
#define ERR_70	-70	/* USING�Ŏw�肵���`���ŏo�͂ł��Ȃ� */
#define ERR_71	-71	/* USING�̎w�肪�s�� */
#define ERR_72	-72	/* ���o�͑��u�̃G���[ */
#define ERR_73	-73
#define ERR_74	-74
#define ERR_75	-75
#define ERR_76	-76
#define ERR_77	-77	/* �t�@�C�����������ޗe�ʂ��s�� */
#define ERR_78	-78
#define ERR_79	-79
#define ERR_80	-80	/* SIO�ǂݍ��݃G���[ */
#define ERR_81	-81	/* SIO�܂��̓~�jI/O�̃^�C���A�E�g */
#define ERR_82	-82	/* �ƍ��s��v */
#define ERR_83	-83	/* �^���s��v */
#define ERR_84	-84	/* �v�����^�G���[ */
#define ERR_85	-85	/* �I�[�v������Ă��Ȃ� */
#define ERR_86	-86	/* ���ɃI�[�v������Ă��� */
#define ERR_87	-87	/* �������ݍς� */
#define ERR_88	-88
#define ERR_89	-89
#define ERR_90	-90	/* �^���s��v */
#define ERR_91	-91	/* �Œ�ϐ��ڌ^���s��v */
#define ERR_92	-92	/* �p�X���[�h���s��v */
#define ERR_93	-93	/* ���b�N����Ă��� */
#define ERR_94	-94	/* �t�@�C�������݂��Ȃ� */
#define ERR_95	-95	/* �t�@�C�������s�� */
#define ERR_96	-96	/* �t�@�C�����s�� */
#define ERR_97	-97	/* �t�@�C������������ */
#define ERR_98	-98
#define ERR_99	-99
#define ERR_BREAK	-256	/* �v���O�����𒆒f���� */
#define ERR_END	-257	/* �v���O�������I������ */
#define ERR_LIST	-258	/* �v���O�����̃��X�g��\�� */
#define ERR_AUTO	-259	/* AUTO���J�n */
#define IS_ERROR(err)	(-255 <= (err) && (err) <= -1)

/* �D��x */
#define PRIORITY_OPE	0	/* ���Z�q */
#define PRIORITY_VAL	100	/* �l */
#define PRIORITY_FUNC	200	/* �֐� */
#define PRIORITY_ARRAY	200	/* �z�� */

/* �v�f�̎�� */
#define ELE_TYPE_NUM	0	/* ���e�������l */
#define ELE_TYPE_STR	1	/* ���e���������� */
#define ELE_TYPE_FIXED	2	/* �Œ�ϐ� */
#define ELE_TYPE_SIMPLE	3	/* �P���ϐ� */
#define ELE_TYPE_ARRAY	4	/* �z��ϐ� */
#define ELE_TYPE_OPE	5	/* ���Z�q�E�֐� */

/* �ϐ��̎�� */
#define KIND_FIXED	ELE_TYPE_FIXED	/* �Œ�ϐ� */
#define KIND_SIMPLE	ELE_TYPE_SIMPLE	/* �P���ϐ� */
#define KIND_ARRAY	ELE_TYPE_ARRAY	/* �z��ϐ� */

/* �l�̌^ */
#define TYPE_NUM	ELE_TYPE_NUM	/* ���l */
#define TYPE_STR	ELE_TYPE_STR	/* ������ */

/* �p�x */
#define ANGLE_RADIAN	0x10	/* ���W�A�� */
#define ANGLE_GRAD	0x30	/* �O���[�h */
#define ANGLE_DEGREE	0x60	/* �x���@ */

/* ���[�h */
#define MODE_MON	0x00
#define MODE_TEXT	0x08
#define MODE_CASL	0x10
#define MODE_PRO	0x20
#define MODE_RUN	0x40

/* ���z���[�h */
#define MODE_MAN	0x01

/* �̈�̖�����? */
#define IS_LAST(p)	((p)[0] == 0xff)

/* �s�ԍ� */
#define LINE_NO(p)	((p)[0] << 8U | (p)[1])

/* �s�̒��� */
#define LINE_SIZE(p)	((p)[2] + 3)

/* �^ */
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;

/* �v�f */
struct Element {
	int priority;	/* �D��x */
	int ele_type;	/* �v�f�̎�� */
	union {
		uint8 num[SIZEOF_NUM];	/* ���e����(���l) */
		const uint8 *str;	/* ���e����(������) */
		int ope;	/* ���Z�q�E�֐� */
		uint8 *var;	/* �ϐ� */
	} x;
};

/* �\���\ */
struct KeywordTable {
	const uint8 *name;
	uint8 code;
	int level;
};

/* ���Z�q�E�֐��\ */
struct Operator {
	int (*func)();	/* ��������֐� */
	int params;	/* �����̐� */
	int type[4];	/* �����̌^ */
	int ret;	/* �߂�l�̌^ */
	const struct Operator *next;	/* ��������v���Ȃ��ꍇ, ���Ƀ`�F�b�N���鉉�Z�q�E�֐��ւ̃|�C���^ */
};

/* �X�e�[�g�����g�\ */
struct Statement {
	int (*sta)(struct Basic *, const uint8 **);
	int mode;
};

extern struct Statement staTable[];

/* �L�[���b�N */
/*static uint8 *keylock = &memory[0x7901];*/
/* ���[�h */
static uint8 *mode = &memory[0x7902];
/* �p�x */
static uint8 *angle = &memory[0x7903];
/* ��� */
/*static uint8 *status = &memory[0x7904];*/
/* ���ɕ\�����镶���̈ʒu */
static uint8 *curCol = &memory[0x7920], *curRow = &memory[0x7921];
/* �Ō�ɕ\�����������̈ʒu */
/*static uint8 *lastCol = &memory[0x7922], *lastRow = &memory[0x7923];*/
/* ���s���Ȃ���? */
static uint8 *noWrap = &memory[0x797d];
/* �v�Z���� */
static uint8 *answer = &memory[0x79a0];
/* PRINT����RETURN�L�[���͂�҂�? */
static uint8 *pauseWhenPrint = &memory[0x79d8]; /* 4bit��OFF:�|�[�Y�Ȃ�, ON:���� */
/* WAIT���� */
static uint8 *waitTimeL = &memory[0x79e5], *waitTimeH = &memory[0x79e6];
/* ���������� */
/*static uint8 *intPart = &memory[0x79f9];*/
/* ���������� */
/*static uint8 *fraPart = &memory[0x79fa];*/
/* �t���[�G���A */
const uint8 *freeLow = &memory[0x7ffe], *freeHigh = &memory[0x7fff];

/* ���l */
uint8 NUM_0[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 NUM_1[] = { 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 NUM_2[] = { 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 NUM_3[] = { 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 NUM_10[] = { 0x00, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 NUM_100[] = { 0x00, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 NUM_60[] = { 0x00, 0x10, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 NUM_3600[] = { 0x00, 0x30, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 NUM_360000[] = { 0x00, 0x50, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 NUM_3600000[] = { 0x00, 0x60, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 NUM_MINUS1[] = { 0x00, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 NUM_0_5[] = { 0x99, 0x90, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 NUM_PI[] = { 0x00, 0x00, 0x31, 0x41, 0x59, 0x26, 0x53, 0x59 };

/* ���ԃR�[�h�\ */
const struct KeywordTable keywordTable[] = {
	{ (const uint8 *)"ABS", CODE_ABS },
	{ (const uint8 *)"AND", CODE_AND },
	{ (const uint8 *)"ASC", CODE_ASC },
	{ (const uint8 *)"ATN", CODE_ATN },
	{ (const uint8 *)"ASN", CODE_ASN },
	{ (const uint8 *)"ACS", CODE_ACS },
	{ (const uint8 *)"AHS", CODE_AHS },
	{ (const uint8 *)"AHC", CODE_AHC },
	{ (const uint8 *)"AHT", CODE_AHT },
	{ (const uint8 *)"AS", CODE_AS },
	{ (const uint8 *)"APPEND", CODE_APPEND },
	{ (const uint8 *)"AUTO", CODE_AUTO, MACHINE_SUB_PCG850V },
	{ (const uint8 *)"BEEP", CODE_BEEP },
	{ (const uint8 *)"BLOAD", CODE_BLOAD },
	{ (const uint8 *)"CLOAD", CODE_BLOAD },
	{ (const uint8 *)"BSAVE", CODE_BSAVE, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"CSAVE", CODE_BSAVE },
	{ (const uint8 *)"CONT", CODE_CONT },
	{ (const uint8 *)"CLEAR", CODE_CLEAR },
	{ (const uint8 *)"COS", CODE_COS },
	{ (const uint8 *)"CUB", CODE_CUB },
	{ (const uint8 *)"CUR", CODE_CUR },
	{ (const uint8 *)"CHR$", CODE_CHR_S },
	{ (const uint8 *)"CLOSE", CODE_CLOSE },
	{ (const uint8 *)"CALL", CODE_CALL },
	{ (const uint8 *)"CLS", CODE_CLS },
	{ (const uint8 *)"CASE", CODE_CASE, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"CIRCLE", CODE_CIRCLE, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"DIM", CODE_DIM },
	{ (const uint8 *)"DEGREE", CODE_DEGREE },
	{ (const uint8 *)"DEG", CODE_DEG },
	{ (const uint8 *)"DATA", CODE_DATA },
	{ (const uint8 *)"DMS$", CODE_DMS_S, MACHINE_SUB_PCG850S },
	{ (const uint8 *)"DMS", CODE_DMS, MACHINE_SUB_PCG850S },
	{ (const uint8 *)"DELETE", CODE_DELETE, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"DEFAULT", CODE_DEFAULT, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"ENDIF", CODE_ENDIF, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"ENDSWITCH", CODE_ENDSWITCH, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"END", CODE_END },
	{ (const uint8 *)"EXP", CODE_EXP },
	{ (const uint8 *)"ELSE", CODE_ELSE, MACHINE_SUB_PCE220 },
	{ (const uint8 *)"EOF", CODE_EOF },
	{ (const uint8 *)"ERASE", CODE_ERASE },
	{ (const uint8 *)"FOR", CODE_FOR },
	{ (const uint8 *)"FRE", CODE_FRE },
	{ (const uint8 *)"FACT", CODE_FACT },
	{ (const uint8 *)"FILES", CODE_FILES },
	{ (const uint8 *)"FIX", CODE_FIX },
	{ (const uint8 *)"GOTO", CODE_GOTO },
	{ (const uint8 *)"GOSUB", CODE_GOSUB },
	{ (const uint8 *)"GCURSOR", CODE_GCURSOR },
	{ (const uint8 *)"GPRINT", CODE_GPRINT },
	{ (const uint8 *)"GRAD", CODE_GRAD },
	{ (const uint8 *)"HEX$", CODE_HEX_S },
	{ (const uint8 *)"HSN", CODE_HSN },
	{ (const uint8 *)"HCS", CODE_HCS },
	{ (const uint8 *)"HTN", CODE_HTN },
	{ (const uint8 *)"HDCOPY", CODE_HDCOPY },
	{ (const uint8 *)"INPUT", CODE_INPUT },
	{ (const uint8 *)"IF", CODE_IF },
	{ (const uint8 *)"INT", CODE_INT },
	{ (const uint8 *)"INKEY$", CODE_INKEY_S },
	{ (const uint8 *)"INP", CODE_INP, MACHINE_SUB_PCG811 },
	{ (const uint8 *)"KILL", CODE_KILL },
	{ (const uint8 *)"LIST", CODE_LIST },
	{ (const uint8 *)"LINE", CODE_LINE, MACHINE_SUB_PCG815 },
	{ (const uint8 *)"LLIST", CODE_LLIST },
	{ (const uint8 *)"LPRINT", CODE_LPRINT },
	{ (const uint8 *)"LOAD", CODE_LOAD },
	{ (const uint8 *)"LOG", CODE_LOG },
	{ (const uint8 *)"LNINPUT", CODE_LNINPUT },
	{ (const uint8 *)"LN", CODE_LN },
	{ (const uint8 *)"LET", CODE_LET },
	{ (const uint8 *)"LEN", CODE_LEN },
	{ (const uint8 *)"LEFT$", CODE_LEFT_S },
	{ (const uint8 *)"LFILES", CODE_LFILES },
	{ (const uint8 *)"LOCATE", CODE_LOCATE },
	{ (const uint8 *)"LOF", CODE_LOF },
	{ (const uint8 *)"LCOPY", CODE_LCOPY },
	{ (const uint8 *)"MDF", CODE_MDF },
	{ (const uint8 *)"MID$", CODE_MID_S },
	{ (const uint8 *)"MON", CODE_MON },
	{ (const uint8 *)"MOD", CODE_MOD },
	{ (const uint8 *)"NEXT", CODE_NEXT },
	{ (const uint8 *)"NEW", CODE_NEW },
	{ (const uint8 *)"NOT", CODE_NOT },
	{ (const uint8 *)"NCR", CODE_NCR },
	{ (const uint8 *)"NPR", CODE_NPR },
	{ (const uint8 *)"ON", CODE_ON },
	{ (const uint8 *)"OPEN", CODE_OPEN },
	{ (const uint8 *)"OR", CODE_OR },
	{ (const uint8 *)"OUTPUT", CODE_OUTPUT },
	{ (const uint8 *)"OUT", CODE_OUT },
	{ (const uint8 *)"PRINT", CODE_PRINT },
	{ (const uint8 *)"PRESET", CODE_PRESET, MACHINE_SUB_PCG815 },
	{ (const uint8 *)"PASS", CODE_PASS },
	{ (const uint8 *)"POL", CODE_POL },
	{ (const uint8 *)"PEEK", CODE_PEEK },
	{ (const uint8 *)"POKE", CODE_POKE },
	{ (const uint8 *)"POINT", CODE_POINT, MACHINE_SUB_PCG815 },
	{ (const uint8 *)"PSET", CODE_PSET, MACHINE_SUB_PCG815 },
	{ (const uint8 *)"PAINT", CODE_PAINT, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"PIOSET", CODE_PIOSET, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"PIOPUT", CODE_PIOPUT, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"PIOGET", CODE_PIOGET, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"PI", CODE_PI },
	{ (const uint8 *)"RUN", CODE_RUN },
	{ (const uint8 *)"RETURN", CODE_RETURN },
	{ (const uint8 *)"READ", CODE_READ },
	{ (const uint8 *)"RESTORE", CODE_RESTORE },
	{ (const uint8 *)"RND", CODE_RND },
	{ (const uint8 *)"RANDOMIZE", CODE_RANDOMIZE },
	{ (const uint8 *)"RIGHT$", CODE_RIGHT_S },
	{ (const uint8 *)"RADIAN", CODE_RADIAN },
	{ (const uint8 *)"REM", CODE_REM },
	{ (const uint8 *)"REC", CODE_REC },
	{ (const uint8 *)"RCP", CODE_RCP },
	{ (const uint8 *)"RENUM", CODE_RENUM },
	{ (const uint8 *)"REPEAT",CODE_REPEAT, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"STOP", CODE_STOP },
	{ (const uint8 *)"SQR", CODE_SQR },
	{ (const uint8 *)"SIN", CODE_SIN },
	{ (const uint8 *)"SGN", CODE_SGN },
	{ (const uint8 *)"STR$", CODE_STR_S },
	{ (const uint8 *)"STEP", CODE_STEP },
	{ (const uint8 *)"SAVE", CODE_SAVE },
	{ (const uint8 *)"SQU", CODE_SQU },
	{ (const uint8 *)"SWITCH", CODE_SWITCH, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"SPOUT", CODE_SPOUT, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"SPINP", CODE_SPINP, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"THEN", CODE_THEN },
	{ (const uint8 *)"TAN", CODE_TAN },
	{ (const uint8 *)"TRON", CODE_TRON },
	{ (const uint8 *)"TROFF", CODE_TROFF },
	{ (const uint8 *)"TO", CODE_TO },
	{ (const uint8 *)"TEN", CODE_TEN },
	{ (const uint8 *)"USING", CODE_USING },
	{ (const uint8 *)"UNTIL", CODE_UNTIL, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"VAL", CODE_VAL },
	{ (const uint8 *)"VDEG", CODE_VDEG, MACHINE_SUB_PCG850S },
	{ (const uint8 *)"WAIT", CODE_WAIT },
	{ (const uint8 *)"WHILE", CODE_WHILE, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"WEND", CODE_WEND, MACHINE_SUB_PCG850 },
	{ (const uint8 *)"XOR", CODE_XOR },
	{ NULL }
};

/* �\����͗p�X�^�b�N */
static struct Element stack[32];

/* �\����͗p�X�^�b�N�̐擪 */
static struct Element *top;

/* ���Z�p�X�^�b�N(�l) */
static uint8 *valueStack[32], **valueSp;

/* ���Z�p�X�^�b�N(�^) */
static int typeStack[32], *typeSp;

/* �v���g�^�C�v�錾 */
static int clearBas(struct Basic *);
static int loadBas(struct Basic *);
static int saveBas(struct Basic *);
static int insertProg(struct Basic *, const uint8 *, int *, uint8 **);
int encodeNum(uint8 *, const uint8 *);
int encodeNum_f(uint8 *, double);
int encodeNum_i(uint8 *, int);
int encodeNum_r(const struct Basic *, uint8 *, double);
int decodeNum(uint8 *, const uint8 *);
int decodeNum_f(double *, const uint8 *);
int decodeNum_i(int *, const uint8 *);
int decodeNum_r(const struct Basic *, double *, const uint8 *);
int opeAnd(struct Basic *, uint8 *, uint8 *);
int opeAdd(struct Basic *, uint8 *, uint8 *);
int opeCat(struct Basic *, uint8 *, uint8 *);
int opeDiv(struct Basic *, uint8 *, uint8 *);
int opeEq(struct Basic *, uint8 *, uint8 *);
int opeEqStr(struct Basic *, uint8 *, uint8 *);
int opeGe(struct Basic *, uint8 *, uint8 *);
int opeGeStr(struct Basic *, uint8 *, uint8 *);
int opeGt(struct Basic *, uint8 *, uint8 *);
int opeGtStr(struct Basic *, uint8 *, uint8 *);
int opeIdiv(struct Basic *, uint8 *, uint8 *);
int opeLe(struct Basic *, uint8 *, uint8 *);
int opeLeStr(struct Basic *, uint8 *, uint8 *);
int opeLt(struct Basic *, uint8 *, uint8 *);
int opeLtStr(struct Basic *, uint8 *, uint8 *);
int opeMod(struct Basic *, uint8 *, uint8 *);
int opeMul(struct Basic *, uint8 *, uint8 *);
int opeNe(struct Basic *, uint8 *, uint8 *);
int opeNeStr(struct Basic *, uint8 *, uint8 *);
int opeNeg(struct Basic *, uint8 *);
int opeNot(struct Basic *, uint8 *);
int opeOr(struct Basic *, uint8 *, uint8 *);
int opePos(struct Basic *, uint8 *);
int opePow(struct Basic *, uint8 *, uint8 *);
int opeSub(struct Basic *, uint8 *, uint8 *);
int opeXor(struct Basic *, uint8 *, uint8 *);
int funcAbs(struct Basic *, uint8 *);
int funcAcs(struct Basic *, uint8 *);
int funcAhc(struct Basic *, uint8 *);
int funcAhs(struct Basic *, uint8 *);
int funcAht(struct Basic *, uint8 *);
int funcAsc(struct Basic *, uint8 *);
int funcAsn(struct Basic *, uint8 *);
int funcAtn(struct Basic *, uint8 *);
int funcChrS(struct Basic *, uint8 *);
int funcCos(struct Basic *, uint8 *);
int funcCub(struct Basic *, uint8 *);
int funcCur(struct Basic *, uint8 *);
int funcDeg(struct Basic *, uint8 *);
int funcDms(struct Basic *, uint8 *);
int funcDmsS(struct Basic *, uint8 *);
int funcEof(struct Basic *, uint8 *);
int funcExp(struct Basic *, uint8 *);
int funcFact(struct Basic *, uint8 *);
int funcFix(struct Basic *, uint8 *);
int funcHcs(struct Basic *, uint8 *);
int funcHexS(struct Basic *, uint8 *);
int funcHsn(struct Basic *, uint8 *);
int funcHtn(struct Basic *, uint8 *);
int funcInkeyS(struct Basic *, uint8 *);
int funcInt(struct Basic *, uint8 *);
int funcInp(struct Basic *, uint8 *);
int funcLeftS(struct Basic *, uint8 *, uint8 *);
int funcLen(struct Basic *, uint8 *);
int funcLn(struct Basic *, uint8 *);
int funcLof(struct Basic *, uint8 *);
int funcLog(struct Basic *, uint8 *);
int funcMdf(struct Basic *, uint8 *);
int funcMidS(struct Basic *, uint8 *, uint8 *, uint8 *);
int funcNcr(struct Basic *, uint8 *, uint8 *);
int funcNpr(struct Basic *, uint8 *, uint8 *);
int funcPeek(struct Basic *, uint8 *);
int funcPi(struct Basic *, uint8 *);
int funcPioget(struct Basic *, uint8 *);
int funcPoint(struct Basic *, uint8 *, uint8 *);
int funcPol(struct Basic *, uint8 *, uint8 *);
int funcRcp(struct Basic *, uint8 *);
int funcRec(struct Basic *, uint8 *, uint8 *);
int funcRightS(struct Basic *, uint8 *, uint8 *);
int funcRnd(struct Basic *, uint8 *);
int funcRound(struct Basic *, uint8 *);
int funcSgn(struct Basic *, uint8 *);
int funcSin(struct Basic *, uint8 *);
int funcSqr(struct Basic *, uint8 *);
int funcSqu(struct Basic *, uint8 *);
int funcStrS(struct Basic *, uint8 *);
int funcTan(struct Basic *, uint8 *);
int funcTen(struct Basic *, uint8 *);
int funcVal(struct Basic *, uint8 *);
int funcVdeg(struct Basic *, uint8 *);

/*
	�������
*/
static int numLet(uint8 *x, const uint8 *y)
{
	memcpy(x, y, SIZEOF_NUM);
	return ERR_OK;
}

/*
	������?
*/
static int numIsInt(const uint8 *num)
{
	int pos, exp = (num[0] >> 4) * 100 + (num[0] & 0x0f) * 10 + (num[1] >> 4);

	if(exp >= 10)
		return FALSE;
	for(pos = exp + 1; pos < 12; pos++)
		if(num[2 + pos / 2] & (0xf0 >> (pos % 2 ? 4: 0)))
			return FALSE;
	return TRUE;
}

/*
	���̐���?
*/
static int numIsNeg(const uint8 *num)
{
	return num[1] & 0x08;
}

/*
	0��?
*/
static int numIsZero(const uint8 *num)
{
	return memcmp(num, "\0\0\0\0\0\0\0", 7) == 0;
}

/*
	�����𓾂�
*/
static int numSgn(const uint8 *num)
{
	if(numIsZero(num))
		return 0;
	else if(numIsNeg(num))
		return -1;
	else
		return 1;
}

/*
	���l���r����
*/
static int numCmp(const uint8 *x, const uint8 *y)
{
	uint8 a[SIZEOF_NUM], b[SIZEOF_NUM];

	numLet(a, x);
	numLet(b, y);
	opeSub(NULL, a, b);
	return numSgn(a);
}

/*
	�␳���� (10��)
*/
static int numCorrect(uint8 *x)
{
	int err;
	uint8 y[SIZEOF_NUM];

	if(numIsZero(x))
		return ERR_OK;

	y[0] = x[0];
	y[1] = x[1];
	y[2] = 0x00;
	y[3] = 0x00;
	y[4] = 0x00;
	y[5] = 0x00;
	y[6] = 0x00;
	y[7] = 0x50;

	if((err = opeAdd(NULL, x, y)) < 0)
		return err;
	x[7] = 0x00;
	return ERR_OK;
}

/*
	�␳���� (9��)
*/
static int numCorrect9(uint8 *x)
{
	int err;

	if((err = numCorrect(x)) < 0)
		return err;
	x[6] &= 0xf0;
	x[7] = 0x00;
	return ERR_OK;
}

/*
	������̈���m�ۂ���
*/
/*
static uint8 *allocStr(uint8 *p_val, int size)
{
	if(size < 8)
		size = 8;
	else if(size > 256)
		size = 256;
	return realloc(p_val, size + 1);
}
*/

/*
	����
*/
/*
static int strCat(uint8 *x, const uint8 *y)
{
	x = allocStr(x, strlen((const char *)x) + strlen((const char *)y));
	strcat((char *)x, (const char *)y);
	return ERR_OK;
}
*/

/*
	������𐔒l�ɕϊ����� (������)
*/
static int encodeNumDms(uint8 *num, const uint8 *str, int dms)
{
	int pos = 0, exp = -1;
	uint8 sign, fra[] = { 0, 0, 0, 0, 0, 0 };
	const uint8 *p = str;

	/* ���͕�����̉������̕����𓾂� */
	if(*p == '-') {
		sign = 0x08;
		p++;
	} else
		sign = 0x00;

	if(strnicmp((const char *)p, "&H", 2) == 0) { /* 16�i�\�L */
		int i;
		uint32 x = 0;
		uint8 dec[8 + 1], *q;

		/* 10�i���ɕϊ����� */
		p += 2;
		for(i = 0; i < 8 && isxdigit(*p); i++, p++)
			if(isdigit(*p))
				x = (x << 4) | (*p - '0');
			else
				x = (x << 4) | (toupper(*p) - 'A' + 0x0a);
		if(isxdigit(*p))
			return ERR_20;
		sprintf((char *)dec, "%u", x);

		/* �������𓾂� */
		for(q = dec; isdigit(*q) && pos < 12; q++, pos++) {
			fra[pos / 2] |= ((*q - '0') << 4) >> ((pos % 2) * 4);
			exp++;
		}
	} else if(isdigit(*p) || *p == '.') { /* 10�i�܂���60�i�\�L */
		/* ���͕�����̉������𓾂� */
		for(; *p == '0'; p++)
			;
		if(*p == '.') {
			p++;

			for(; *p == '0'; p++)
				exp--;
		} else {
			for(; pos < 12 && isdigit(*p); pos++, p++) {
				fra[pos / 2] |= ((*p - '0') << 4) >> ((pos % 2) * 4);
				exp++;
			}
			for(; isdigit(*p); p++)
				exp++;
			if(*p == '.')
				p++;
		}
		for(; pos < 12 && isdigit(*p); pos++, p++)
			fra[pos / 2] |= ((*p - '0') << 4) >> ((pos % 2) * 4);
		for(; isdigit(*p); p++)
			;
		for(; *p == ' '; p++)
			;

		/* ���͕�����̎w�����𓾂� */
		if(toupper(*p) == 'E') {
			int s, e = 0;

			/* ���͕�����̎w�����̕����𓾂� */
			p++;
			for(; *p == ' '; p++)
				;
			if(*p == '-') {
				p++;
				s = -1;
			} else if(*p == '+') {
				p++;
				s = 1;
			} else
				s = 1;

			/* ���͕�����̎w�����𓾂� */
			for(pos = 0; pos < 2 && isdigit(*p); pos++, p++)
				e = e * 10 + (*p - '0');
			for(; isdigit(*p); p++)
				;

			/* �w���������߂� */
			exp += s * e;
		}

		/*
		printf("%02X%02X%02X%02X%02X%02X E%d\n",
		fra[0], fra[1], fra[2], fra[3], fra[4], fra[5], exp);
		*/

		if(dms)
			if(*p == 0xdf || *p == 0x27 || *p == 0xf8) { /* 60�i�\�L */
				int err, len, sign, count = 0;
				uint8 val[SIZEOF_NUM];
				const uint8 *p = str;

				numLet(num, NUM_0);

				/* �����𓾂� */
				if(*p == '-') {
					p++;
					sign = -1;
				} else
					sign = 1;

				for(;;) {
					/* ���l�𓾂� */
					if(*p == '-')
						break;
					if((len = encodeNumDms(val, p, FALSE)) < 0)
						break;
					p += len;

					if(toupper(*p) == 0xdf) {
						/* �x��10�i�ɂ��� */
						if(count > 0)
							return ERR_10;
						count = 1;

						if((err = numLet(num, val)) < 0)
							return err;
					} else if(toupper(*p) == 0x27) {
						/* ����10�i�ɂ��ĉ��Z���� */
						if(count > 1)
							return ERR_10;
						count = 2;

						if((err = opeDiv(NULL, val, NUM_60)) < 0)
							return err;
						if((err = opeAdd(NULL, num, val)) < 0)
							return err;
					} else if(toupper(*p) == 0xf8) {
						/* �b��10�i�ɂ��ĉ��Z���� */
						if(count > 2)
							return ERR_10;
						count = 3;

						if((err = opeDiv(NULL, val, NUM_3600)) < 0)
							return err;
						if((err = opeAdd(NULL, num, val)) < 0)
							return err;
					} else
						return ERR_10;
					p++;
				}
				if(sign < 0)
					opeNeg(NULL, num);

				num[1] |= 0x04;
				return (int )(p - str);
			}
	} else /* ���l�ł͂Ȃ� */
		return ERR_90;

	/* 0��? */
	if(memcmp(fra, "\0\0\0\0\0\0", 6) == 0) {
		exp = 0;
		sign = 0x00;
	}

	/* �w�������������� */
	if(exp > 99 || exp < -99)
		return ERR_20;
	if(exp < 0)
		exp = 1000 + exp;
	num[0] = (exp / 100) << 4 | (((exp / 10) % 10) & 0x0f);
	num[1] = (exp % 10) << 4 | sign;

	/* ���������������� */
	memcpy(&num[2], fra, sizeof(fra));
	/*
	printf("%02X%02X%02X%02X%02X%02X%02X%02X\n",
	num[0], num[1], num[2], num[3], num[4], num[5], num[6], num[7]);
	*/
	return (int )(p - str);
}

/*
	������𐔒l�ɕϊ�����
*/
int encodeNum(uint8 *num, const uint8 *str)
{
	return encodeNumDms(num, str, FALSE);
}

/*
	���������_�𐔒l�ɕϊ�����
*/
int encodeNum_f(uint8 *dst, double f)
{
	char buf[32];

	sprintf(buf, "%13.13f", f);
	return encodeNum(dst, (uint8 *)buf);
}

/*
	�����𐔒l�ɕϊ�����
*/
int encodeNum_i(uint8 *dst, int i)
{
	char buf[16];

	sprintf(buf, "%d", i);
	return encodeNumDms(dst, (uint8 *)buf, FALSE);
}

/*
	���W�A���𐔒l�ɕϊ�����
*/
int encodeNum_r(const struct Basic *bas, uint8 *num, double r)
{
	if(*angle == ANGLE_RADIAN)
		return encodeNum_f(num, r);
	else if(*angle == ANGLE_GRAD)
		return encodeNum_f(num, r * 400.0 / (2.0 * M_PI));
	else
		return encodeNum_f(num, r * 360.0 / (2.0 * M_PI));
}

/*
	���l�𕶎���ɕϊ�����(�����w�肠��)
*/
static int decodeNumFormat(const struct Basic *bas, uint8 *str, const uint8 *num, int dms)
{
	int exp, ints, decs, digits, format_exp;
	char fra[12 + 1], *p;
	const uint8 *sign;

	/* ���l�ł͂Ȃ���? */
	if(num[0] == 0xf5)
		return ERR_90;

	/* �w�����𓾂� */
	exp = (num[0] >> 4) * 100 + (num[0] & 0x0f) * 10 + (num[1] >> 4);
	if(exp > 99)
		exp -= 1000;

	/* �������̕����𓾂� */
	sign = (const uint8 *)(num[1] & 0x08 ? "-": "");

	/* 60�i��? */
	if(dms && (num[1] & 0x04)) {
		int err, d, m, s;
		uint8 angle[SIZEOF_NUM], deg[SIZEOF_NUM];

		/* 10^10�ȏォ? */
		if(exp >= 10) {
			int len;

			if((len = decodeNumFormat(bas, str, num, FALSE)) < 0)
				return len;
			str[len++] = 0xdf;
			str[len++] = 0;
			return len;
		}

		/* �p�x */
		if((err = numLet(angle, num)) < 0)
			return err;
		if((err = funcAbs(NULL, angle)) < 0)
			return err;

		/* �x */
		if((err = numLet(deg, angle)) < 0)
			return err;
		if((err = funcFix(NULL, deg)) < 0)
			return err;
		if((err = opeSub(NULL, angle, deg)) < 0)
			return err;
		if((err = decodeNum_i(&d, deg)) < 0)
			return err;

		/* ���E�b */
		if((err = opeMul(NULL, angle, NUM_3600000)) < 0)
			return err;
		if((err = opeIdiv(NULL, angle, NUM_10)) < 0)
			return err;
		if((err = decodeNum_i(&s, angle)) < 0)
			return err;
		m = s / 6000;
		s -= m * 6000;

		if(m == 0 && s == 0)
			sprintf((char *)str, "%s%d\xdf", sign, d);
		else if(s == 0)
			sprintf((char *)str, "%s%d\xdf" "%02d\x27", sign, d, m);
		else if(s % 100 == 0)
			sprintf((char *)str, "%s%d\xdf" "%02d\x27" "%02d\xf8", sign, d, m, s / 100);
		else
			sprintf((char *)str, "%s%d\xdf" "%02d\x27" "%02d.%02d\xf8", sign, d, m, s / 100, s % 100);
		return strlen(str);
	}

	/* �������𓾂� */
	sprintf((char *)fra, "%02X%02X%02X%02X%02X%02X", num[2], num[3], num[4], num[5], num[6], num[7]);

	/* �����𓾂� */
	for(p = fra + 11, digits = 12; digits > 0 && *p == '0'; p--, digits--)
		;

	/* �������̌����𓾂� */
	if((ints = exp + 1) < 0)
		ints = 0;

	/* �������̌����𓾂� */
	if((decs = (exp < 0 ? -exp - 1: 0) + digits - ints) < 0)
		decs = 0;

	format_exp = !(-9 <= exp && exp <= 9);

	if(bas != NULL && (bas->format_ints > 0 || bas->format_decs > 0)) {
		if(ints > bas->format_ints - 1)
			return ERR_70;
		decs = bas->format_decs;
		format_exp = bas->format_exp;
	}

	/* ������ɕϊ����� */
	if(format_exp)
		sprintf((char *)str, "%s%c.%.*sE %02d", sign, fra[0], digits - 1, &fra[1], exp);
	else {
		char ints_str[16], decs_str[16];

		/* �������𕶎���ɂ��� */
		if(bas != NULL && bas->format_ints > 0) {
			if(exp >= 0)
				sprintf(ints_str, "%*.*s", bas->format_ints, ints, fra);
			else
				sprintf(ints_str, "%*.*s", bas->format_ints, bas->format_ints, "0");

			if(*sign != 0) {
				for(p = ints_str + strlen(ints_str) - 1; *p != ' '; p--)
					;
				*p = *sign;
			}
		} else {
			if(exp >= 0)
				sprintf(ints_str, "%s%.*s", sign, ints, fra);
			else
				sprintf(ints_str, "%s0", sign);
		}

		/* �R���}��t���� */
		if(bas != NULL && bas->format_comma)
			for(p = ints_str + strlen(ints_str) - 3; p > ints_str && isdigit(*(p - 1)); p -= 3) {
				memmove(p + 1, p, strlen(p) + 1);
				*p = ',';
			}

		/* �������𕶎���ɂ��� */
		if(exp < 0)
			sprintf(decs_str, "%.*s%.*s", -exp - 1, "0000000000", digits, fra);
		else
			sprintf(decs_str, "%.*s", digits - ints, fra + ints);

		/* �������� */
		if(bas == NULL || bas->format_ints == 0) {
			for(p = decs_str + strlen(decs_str) - 1; p >= decs_str && *p == '0'; p--)
				*p = 0;
			sprintf((char *)str, "%s.%s", ints_str, decs_str);
		} else if(bas->format_decs == 0)
			sprintf((char *)str, "%s", ints_str);
		else
			sprintf((char *)str, "%s.%.*s", ints_str, bas->format_decs, decs_str);
	}

	return strlen((char *)str);
}

/*
	���l�𕶎���ɕϊ�����
*/
int decodeNum(uint8 *str, const uint8 *num)
{
	return decodeNumFormat(NULL, str, num, TRUE);
}

/*
	���l�𕂓������_�ɕϊ�����
*/
int decodeNum_f(double *f, const uint8 *num)
{
	int err;
	char tmp[32], *p, *q;

	if((err = decodeNumFormat(NULL, (uint8 *)tmp, num, FALSE)) < 0)
		return err;

	p = tmp;
	for(q = tmp; *q != 0; q++) {
		if(*q != ' ')
			*p++ = *q;
	}
	*p = 0;

	*f = atof(tmp);
	return ERR_OK;
}

/*
	���l�𐮐��ɕϊ�����
*/
int decodeNum_i(int *i, const uint8 *num)
{
	double f;
	int err;
	uint8 buf[SIZEOF_NUM];

	if((err = numLet(buf, num)) < 0)
		return err;
	if((err = numCorrect(buf)) < 0)
		return err;
	if((err = decodeNum_f(&f, buf)) < 0)
		return err;
	if(f <= -2147483649.0 || f >= 4294967296.0)
		return ERR_33;

	*i = (unsigned int )f;
	return ERR_OK;
}

/*
	���l�����W�A���ɕϊ�����
*/
int decodeNum_r(const struct Basic *bas, double *r, const uint8 *num)
{
	int err;

	if((err = decodeNum_f(r, num)) < 0)
		return err;

	if(*angle == ANGLE_RADIAN)
		;
	else if(*angle == ANGLE_GRAD)
		*r = *r * 2.0 * M_PI / 400.0;
	else
		*r = *r * 2.0 * M_PI / 360.0;
	return ERR_OK;
}

/*
	�w��̃X�e�[�g�����҂�
*/
static void ssleep(int states)
{
	int f = freqCPU / (csClk ? 2: 1);

	z80.i.total_states += states;
	z80.i.states += states;
	if(z80.i.states < f / freqUpdateIO)
		return;

	timerCount--;
	if(timerCount <= 0) {
		timerCount += freqUpdateIO * timerInterval / 1000 / 1000;
		timer ^= TIMER_1S;
	}

	delay(z80.i.states / (f / 1000));
	z80.i.states %= (f / 1000);
}

/*
	AND
*/
int opeAnd(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err, a, b;

	if(bas != NULL)
		ssleep(26564);

	if((err = decodeNum_i(&a, x)) < 0)
		return err;
	if((err = decodeNum_i(&b, y)) < 0)
		return err;
	if(a < -32768 || a > 32767 || b < -32768 || b > 32767)
		return ERR_33;

	return encodeNum_i(x, a & b);
}

/*
	+ (���Z)
*/
int opeAdd(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err, dms;
	double a, b;

	if(bas != NULL)
		ssleep(7003);

	if((err = decodeNum_f(&a, x)) < 0)
		return err;
	if((err = decodeNum_f(&b, y)) < 0)
		return err;
	dms = x[1] & y[1] & 0x04;

	if((err = encodeNum_f(x, a + b)) < 0)
		return err;
	x[1] = (x[1] & ~0x04) | dms;
	return ERR_OK;
}

/*
	+ (����)
*/
int opeCat(struct Basic *bas, uint8 *x, uint8 *y)
{
	if(bas != NULL)
		ssleep(4525);

	if(strlen((char *)x) + strlen((char *)y) >= 255)
		return ERR_55;

	strcat((char *)x, (const char *)y);
	return ERR_OK;
}

/*
	/ (���Z)
*/
int opeDiv(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err, dms;
	double a, b;

	if(bas != NULL)
		ssleep(25324);

	if((err = decodeNum_f(&a, x)) < 0)
		return err;
	if((err = decodeNum_f(&b, y)) < 0)
		return err;
	if(b == .0)
		return ERR_21;
	dms = (x[1] | y[1]) & 0x04;

	if((err = encodeNum_f(x, a / b)) < 0)
		return err;
	x[1] = (x[1] & ~0x04) | dms;
	return ERR_OK;
}

/*
	= (��������?)
*/
int opeEq(struct Basic *bas, uint8 *x, uint8 *y)
{
	if(bas != NULL)
		ssleep(8481);

	return encodeNum_i(x, numCmp(x, y) == 0 ? -1: 0);
}

/*
	= (��������?) (������)
*/
int opeEqStr(struct Basic *bas, uint8 *x, uint8 *y)
{
	if(bas != NULL)
		ssleep(5133);

	return encodeNum_i(x, strcmp(x, y) == 0 ? -1: 0);
}

/*
	>= (�ȏォ?)
*/
int opeGe(struct Basic *bas, uint8 *x, uint8 *y)
{
	if(bas != NULL)
		ssleep(8672);

	return encodeNum_i(x, numCmp(x, y) >= 0 ? -1: 0);
}

/*
	>= (�ȏォ?) (������)
*/
int opeGeStr(struct Basic *bas, uint8 *x, uint8 *y)
{
	if(bas != NULL)
		ssleep(5398);

	return encodeNum_i(x, strcmp(x, y) >= 0 ? -1: 0);
}

/*
	> (���傫����?)
*/
int opeGt(struct Basic *bas, uint8 *x, uint8 *y)
{
	if(bas != NULL)
		ssleep(8586);

	return encodeNum_i(x, numCmp(x, y) > 0 ? -1: 0);
}

/*
	> (���傫����?) (������)
*/
int opeGtStr(struct Basic *bas, uint8 *x, uint8 *y)
{
	if(bas != NULL)
		ssleep(5330);

	return encodeNum_i(x, strcmp(x, y) > 0 ? -1: 0);
}

/*
	\ (���Z)
*/
int opeIdiv(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err;

	if(bas != NULL)
		ssleep(42746);

	if((err = funcRound(NULL, x)) < 0)
		return err;
	if((err = funcRound(NULL, y)) < 0)
		return err;
	if((err = opeDiv(NULL, x, y)) < 0)
		return err;
	return funcFix(NULL, x);
}

/*
	<= (�ȉ���?)
*/
int opeLe(struct Basic *bas, uint8 *x, uint8 *y)
{
	if(bas != NULL)
		ssleep(8672);

	return encodeNum_i(x, numCmp(x, y) <= 0 ? -1: 0);
}

/*
	<= (�ȉ���?) (������)
*/
int opeLeStr(struct Basic *bas, uint8 *x, uint8 *y)
{
	if(bas != NULL)
		ssleep(5398);

	return encodeNum_i(x, strcmp(x, y) <= 0 ? -1: 0);
}

/*
	< (������?)
*/
int opeLt(struct Basic *bas, uint8 *x, uint8 *y)
{
	if(bas != NULL)
		ssleep(8586);

	return encodeNum_i(x, numCmp(x, y) < 0 ? -1: 0);
}

/*
	< (������?) (������)
*/
int opeLtStr(struct Basic *bas, uint8 *x, uint8 *y)
{
	if(bas != NULL)
		ssleep(5330);

	return encodeNum_i(x, strcmp(x, y) < 0 ? -1: 0);
}

/*
	MOD
*/
int opeMod(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err;
	uint8 z[SIZEOF_NUM];

	if(bas != NULL)
		ssleep(66470);

	/* X = ROUND(X) */
	if((err = funcRound(NULL, x)) < 0)
		return err;

	/* Y = ROUND(Y) */
	if((err = funcRound(NULL, y)) < 0)
		return err;

	/* Z = (X \ Y) * Y */
	if((err = numLet(z, x)) < 0)
		return err;
	if((err = opeIdiv(NULL, z, y)) < 0)
		return err;
	if((err = opeMul(NULL, z, y)) < 0)
		return err;

	/* X = X - Z */
	return opeSub(NULL, x, z);
}

/*
	* (�ώZ)
*/
int opeMul(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err, dms;
	double a, b;

	if(bas != NULL)
		ssleep(23858);

	if((err = decodeNum_f(&a, x)) < 0)
		return err;
	if((err = decodeNum_f(&b, y)) < 0)
		return err;
	dms = (x[1] | y[1]) & 0x04;

	if((err = encodeNum_f(x, a * b)) < 0)
		return err;
	x[1] = (x[1] & ~0x04) | dms;
	return ERR_OK;
}

/*
	<> (�������Ȃ���?)
*/
int opeNe(struct Basic *bas, uint8 *x, uint8 *y)
{
	if(bas != NULL)
		ssleep(8487);

	return encodeNum_i(x, numCmp(x, y) != 0 ? -1: 0);
}

/*
	<> (�������Ȃ���?) (������)
*/
int opeNeStr(struct Basic *bas, uint8 *x, uint8 *y)
{
	if(bas != NULL)
		ssleep(5163);

	return encodeNum_i(x, strcmp(x, y) != 0 ? -1: 0);
}

/*
	- (����)
*/
int opeNeg(struct Basic *bas, uint8 *x)
{
	if(bas != NULL)
		ssleep(1629);

	return opeMul(NULL, x, NUM_MINUS1);
}

/*
	NOT
*/
int opeNot(struct Basic *bas, uint8 *x)
{
	int err, a;

	if(bas != NULL)
		ssleep(22043);

	if((err = decodeNum_i(&a, x)) < 0)
		return err;
	if(a < -32768 || a > 32767)
		return ERR_33;

	return encodeNum_i(x, a ^ 0xffffffff);
}

/*
	OR
*/
int opeOr(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err, a, b;

	if(bas != NULL)
		ssleep(27848);

	if((err = decodeNum_i(&a, x)) < 0)
		return err;
	if((err = decodeNum_i(&b, y)) < 0)
		return err;
	if(a < -32768 || a > 32767 || b < -32768 || b > 32767)
		return ERR_33;

	return encodeNum_i(x, a | b);
}

/*
	+ (����)
*/
int opePos(struct Basic *bas, uint8 *x)
{
	if(bas != NULL)
		ssleep(1569);

	return ERR_OK;
}

/*
	^ (�x�L��)
*/
int opePow(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err;
	double a, b;

	if(bas != NULL)
		ssleep(192715);

	if((err = decodeNum_f(&a, x)) < 0)
		return err;
	if((err = decodeNum_f(&b, y)) < 0)
		return err;

	return encodeNum_f(x, pow(a, b));
}

/*
	- (���Z)
*/
int opeSub(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err, dms;
	double a, b;

	if(bas != NULL)
		ssleep(7017);

	if((err = decodeNum_f(&a, x)) < 0)
		return err;
	if((err = decodeNum_f(&b, y)) < 0)
		return err;
	dms = x[1] & y[1] & 0x04;

	if((err = encodeNum_f(x, a - b)) < 0)
		return err;
	x[1] = (x[1] & ~0x04) | dms;
	return ERR_OK;
}

/*
	XOR
*/
int opeXor(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err, a, b;

	if(bas != NULL)
		ssleep(27471);

	if((err = decodeNum_i(&a, x)) < 0)
		return err;
	if((err = decodeNum_i(&b, y)) < 0)
		return err;
	if(a < -32768 || a > 32767 || b < -32768 || b > 32767)
		return ERR_33;

	return encodeNum_i(x, a ^ b);
}

/*
	ABS
*/
int funcAbs(struct Basic *bas, uint8 *x)
{
	if(bas != NULL)
		ssleep(1676);

	if(x[1] & 0x08)
		x[1] ^= 0x08;
	return ERR_OK;
}

/*
	ACS
*/
int funcAcs(struct Basic *bas, uint8 *x)
{
	int err;
	double a;

	if(bas != NULL)
		ssleep(284435);

	if((err = decodeNum_f(&a, x)) < 0)
		return err;

	return encodeNum_r(bas, x, acos(a));
}

/*
	AHC
*/
int funcAhc(struct Basic *bas, uint8 *x)
{
	int err;
	uint8 a[SIZEOF_NUM];

	if(bas != NULL)
		ssleep(10804);

	/* X < 1��? */
	if(numCmp(x, NUM_1) < 0)
		return ERR_22;

	/* A = X */
	if((err = numLet(a, x)) < 0)
		return err;

	/* X = X^2 - 1 */
	if((err = funcSqu(NULL, x)) < 0)
		return err;
	if((err = opeSub(NULL, x, NUM_1)) < 0)
		return err;

	/* X = SQR(X) + A */
	if((err = funcSqr(NULL, x)) < 0)
		return err;
	if((err = opeAdd(NULL, x, a)) < 0)
		return err;

	/* X = LOG(X) */
	return funcLog(NULL, x);
}

/*
	AHS
*/
int funcAhs(struct Basic *bas, uint8 *x)
{
	int err;
	uint8 a[SIZEOF_NUM];

	if(bas != NULL)
		ssleep(10057);

	/* A = X */
	if((err = numLet(a, x)) < 0)
		return err;

	/* X = X^2 + 1 */
	if((err = funcSqu(NULL, x)) < 0)
		return err;
	if((err = opeAdd(NULL, x, NUM_1)) < 0)
		return err;

	/* X = SQR(X) + A */
	if((err = funcSqr(NULL, x)) < 0)
		return err;
	if((err = opeAdd(NULL, x, a)) < 0)
		return err;

	/* X = LOG(X) */
	return funcLog(NULL, x);
}

/*
	AHT
*/
int funcAht(struct Basic *bas, uint8 *x)
{
	int err;
	uint8 a[SIZEOF_NUM];

	if(bas != NULL)
		ssleep(257641);

	/* X >= 1��? */
	if(numCmp(x, NUM_1) >= 0)
		return ERR_22;

	/* X <= -1��? */
	if(numCmp(x, NUM_MINUS1) <= 0)
		return ERR_22;

	/* A = 1 - X */
	if((err = numLet(a, NUM_1)) < 0)
		return err;
	if((err = opeSub(NULL, a, x)) < 0)
		return err;

	/* X = SQR((1 + X) / A) */
	if((err = opeAdd(NULL, x, NUM_1)) < 0)
		return err;
	if((err = opeDiv(NULL, x, a)) < 0)
		return err;
	if((err = funcSqr(NULL, x)) < 0)
		return err;

	/* X = LOG(X) */
	return funcLog(NULL, x);
}

/*
	ASC
*/
int funcAsc(struct Basic *bas, uint8 *x)
{
	if(bas != NULL)
		ssleep(20533);

	return encodeNum_i(x, *x);
}

/*
	ASN
*/
int funcAsn(struct Basic *bas, uint8 *x)
{
	int err;
	double a;

	if(bas != NULL)
		ssleep(270466);

	if((err = decodeNum_f(&a, x)) < 0)
		return err;

	return encodeNum_r(bas, x, asin(a));
}

/*
	ATN
*/
int funcAtn(struct Basic *bas, uint8 *x)
{
	int err;
	double a;

	if(bas != NULL)
		ssleep(159384);

	if((err = decodeNum_f(&a, x)) < 0)
		return err;

	return encodeNum_r(bas, x, atan(a));
}

/*
	CHR$
*/
int funcChrS(struct Basic *bas, uint8 *x)
{
	int err, c;

	if(bas != NULL)
		ssleep(40424);

	if((err = decodeNum_i(&c, x)) < 0)
		return err;
	if(c < 0 || c > 0xff)
		return ERR_33;

	sprintf((char *)x, "%c", c);
	return ERR_OK;
}

/*
	COS
*/
int funcCos(struct Basic *bas, uint8 *x)
{
	double r;
	int err;

	if(bas != NULL)
		ssleep(296944);

	if((err = decodeNum_r(bas, &r, x)) < 0)
		return err;

	return encodeNum_f(x, cos(r));
}

/*
	CUB
*/
int funcCub(struct Basic *bas, uint8 *x)
{
	if(bas != NULL)
		ssleep(25783);

	return opePow(NULL, x, NUM_3);
}

/*
	CUR
*/
int funcCur(struct Basic *bas, uint8 *x)
{
	int err;
	double a;

	if(bas != NULL)
		ssleep(336785);

	if((err = decodeNum_f(&a, x)) < 0)
		return err;
	return encodeNum_f(x, pow(a, 1.0 / 3.0));
}

/*
	DEG
*/
int funcDeg(struct Basic *bas, uint8 *x)
{
	if(bas != NULL)
		ssleep(51525);

	x[1] &= ~0x04;
	return ERR_OK;
}

/*
	DMS
*/
int funcDms(struct Basic *bas, uint8 *x)
{
	if(bas != NULL)
		ssleep(2564);

	x[1] |= 0x04;
	return ERR_OK;
}

/*
	DMS$
*/
int funcDmsS(struct Basic *bas, uint8 *x)
{
	int len;
	uint8 buf[256];

	if(bas != NULL)
		ssleep(35292);

	x[1] |= 0x04;

	if((len = numCorrect(x)) < 0)
		return len;
	if((len = decodeNumFormat(NULL, buf, x, TRUE)) < 0)
		return len;

	strcpy(x, buf);
	return ERR_OK;
}

/*
	EOF
*/
int funcEof(struct Basic *bas, uint8 *x)
{
	int err, fileno;

	if((err = decodeNum_i(&fileno, x)) < 0)
		return err;
	if(fileno <= 1 || fileno > sizeof(bas->fp) / sizeof(bas->fp[0]))
		return ERR_10;
	if(bas->fp[fileno] == NULL)
		return ERR_86;

	return encodeNum_i(x, (feof(bas->fp[fileno]) ? -1: 0));
}

/*
	EXP
*/
int funcExp(struct Basic *bas, uint8 *x)
{
	int err;
	double a;

	if(bas != NULL)
		ssleep(111165);

	if((err = decodeNum_f(&a, x)) < 0)
		return err;
	return encodeNum_f(x, exp(a));
}

/*
	FACT
*/
int funcFact(struct Basic *bas, uint8 *x)
{
	int err;
	uint8 count[SIZEOF_NUM];

	if(bas != NULL)
		ssleep(6950);

	if(!numIsInt(x))
		return ERR_22;

	numLet(count, x);
	numLet(x, NUM_1);

	while(numSgn(count) > 0) {
		if(bas != NULL)
			ssleep(3278);

		if((err = opeMul(NULL, x, count)) < 0)
			return err;
		if((err = opeSub(NULL, count, NUM_1)) < 0)
			return err;
	}
	return ERR_OK;
}

/*
	FIX
*/
int funcFix(struct Basic *bas, uint8 *x)
{
	int i, exp = (x[0] >> 4) * 100 + (x[0] & 0x0f) * 10 + (x[1] >> 4);

	if(bas != NULL)
		ssleep(5151);

	if(exp > 99)
		return numLet(x, NUM_0);

	for(i = exp + 1; i < 12; i++)
		if((i % 2) == 0)
			x[2 + i / 2] &= 0x0f;
		else
			x[2 + i / 2] &= 0xf0;
	return ERR_OK;
}

/*
	FRE
*/
int funcFre(struct Basic *bas, uint8 *x)
{
	int err;

	if(bas != NULL)
		ssleep(16640);

	if((err = encodeNum_i(x, *freeHigh * 0x100 + *freeLow - 1)) < 0)
		return err;
	return ERR_OK;
}

/*
	HCS
*/
int funcHcs(struct Basic *bas, uint8 *x)
{
	int err;
	uint8 a[SIZEOF_NUM];

	if(bas != NULL)
		ssleep(143954);

	/* A = EXP(-X) */
	if((err = numLet(a, x)) < 0)
		return err;
	if((err = opeNeg(NULL, a)) < 0)
		return err;
	if((err = funcExp(NULL, a)) < 0)
		return err;

	/* X = EXP(X) */
	if((err = funcExp(NULL, x)) < 0)
		return err;

	/* X = (X + A) / 2 */
	if((err = opeAdd(NULL, x, a)) < 0)
		return err;
	return opeDiv(NULL, x, NUM_2);
}

/*
	HEX$
*/
int funcHexS(struct Basic *bas, uint8 *x)
{
	int err, a;

	if(bas != NULL)
		ssleep(27026);

	if((err = decodeNum_i(&a, x)) < 0)
		return err;

	sprintf((char *)x, "%X", a);
	return ERR_OK;
}

/*
	HSN
*/
int funcHsn(struct Basic *bas, uint8 *x)
{
	int err;
	uint8 a[SIZEOF_NUM];

	if(bas != NULL)
		ssleep(144421);

	/* A = EXP(-X) */
	if((err = numLet(a, x)) < 0)
		return err;
	if((err = opeNeg(NULL, a)) < 0)
		return err;
	if((err = funcExp(NULL, a)) < 0)
		return err;

	/* X = EXP(X) */
	if((err = funcExp(NULL, x)) < 0)
		return err;

	/* X = (X - A) / 2 */
	if((err = opeSub(NULL, x, a)) < 0)
		return err;
	return opeDiv(NULL, x, NUM_2);
}

/*
	HTN
*/
int funcHtn(struct Basic *bas, uint8 *x)
{
	uint8 a[SIZEOF_NUM];
	int err;

	if(bas != NULL)
		ssleep(155842);

	if((err = numLet(a, x)) < 0)
		return err;
	if((err = funcHcs(NULL, a)) < 0)
		return err;
	if((err = funcHsn(NULL, x)) < 0)
		return err;
	return opeDiv(NULL, x, a);
}

/*
	INKEY$
*/
int funcInkeyS(struct Basic *bas, uint8 *x)
{
	const uint8 key_to_code[] = {
		0x00,
		0x00,	/* OFF */
		0x51,	/* Q */
		0x57,	/* W */
		0x45,	/* E */
		0x52,	/* R */
		0x54,	/* T */
		0x59,	/* Y */
		0x55,	/* U */
		0x41,	/* A */
		0x53,	/* S */
		0x44,	/* D */
		0x46,	/* F */
		0x47,	/* G */
		0x48,	/* H */
		0x4a,	/* J */

		0x4b,	/* K */
		0x5a,	/* Z */
		0x58,	/* X */
		0x43,	/* C */
		0x56,	/* V */
		0x42,	/* B */
		0x4e,	/* N */
		0x4d,	/* M */
		0x2c,	/* , */
		0x08,	/* BASIC */
		0x09,	/* TEXT */
		0x15,	/* CAPS */
		0x14,	/* �J�i */
		0x0a,	/* TAB */
		0x20,	/* SPACE */
		0x05,	/* �� */

		0x04,	/* �� */
		0x0f,	/* �� */
		0x0e,	/* �� */
		0x07,	/* ANS */
		0x30,	/* 0 */
		0x2e,	/* . */
		0x3d,	/* = */
		0x2b,	/* + */
		0x0d,	/* RETURN */
		0x4c,	/* L */
		0x3b,	/* ; */
		0x0c,	/* CONST */
		0x31,	/* 1 */
		0x32,	/* 2 */
		0x33,	/* 3 */
		0x2d,	/* - */

		0x19,	/* M+ */
		0x49,	/* I */
		0x4f,	/* O */
		0x0b,	/* INS */
		0x34,	/* 4 */
		0x35,	/* 5 */
		0x36,	/* 6 */
		0x2a,	/* * */
		0x18,	/* R�ECM */
		0x50,	/* P */
		0x17,	/* BS */
		0xfb,	/* �� */
		0x37,	/* 7 */
		0x38,	/* 8 */
		0x39,	/* 9 */
		0x2f,	/* / */

		0x29,	/* ) */
		0x9d,	/* nPr */
		0x9b,	/* ��DEG */
		0xfc,	/* �� */
		0x88,	/* x^2 */
		0x5e,	/* ^ */
		0x28,	/* ( */
		0x87,	/* 1/x */
		0x9e,	/* MDF */
		0x10,	/* 2ndF */
		0x95,	/* sin */
		0x96,	/* cos */
		0x91,	/* ln */
		0x92,	/* log */
		0x97,	/* tan */
		0x9c,	/* F����E */

		0x02,	/* CLS */
		0x00,	/* BREAK */
		0x00	/* DOUBLE */
	};

	if(bas != NULL)
		ssleep(32085);

	updateKey();
	x[0] = key_to_code[peekKeycode() & 0x7f];
	x[1] = 0;
	return ERR_OK;
}

/*
	INT
*/
int funcInt(struct Basic *bas, uint8 *x)
{
	int err;

	if(bas != NULL)
		ssleep(5196);

	if(numIsInt(x))
		return ERR_OK;

	if(numSgn(x) < 0)
		if((err = opeSub(NULL, x, NUM_1)) < 0)
			return err;
	return funcFix(NULL, x);
}

/*
	INP
*/
int funcInp(struct Basic *bas, uint8 *x)
{
	int err, address;
	uint8 val;

	if(bas != NULL)
		ssleep(19288);

	if((err = decodeNum_i(&address, x)) < 0)
		return err;
	if(address > 0xffff)
		return ERR_33;

	if(address < 0) {
		if((err = encodeNum_i(x, address)) < 0)
			return err;
	} else {
		z80inport(&z80, &val, address & 0xff);

		if((err = encodeNum_i(x, val)) < 0)
			return err;
	}
	return ERR_OK;
}

/*
	LEFT$
*/
int funcLeftS(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err, a;

	if(bas != NULL)
		ssleep(9660);

	if((err = decodeNum_i(&a, y)) < 0)
		return err;
	if(a < 0)
		return ERR_33;

	if(a < strlen(x))
		*(x + a) = 0;
	return ERR_OK;
}

/*
	LEN
*/
int funcLen(struct Basic *bas, uint8 *x)
{
	if(bas != NULL)
		ssleep(20175);

	return encodeNum_i(x, strlen(x));
}

/*
	LN
*/
int funcLn(struct Basic *bas, uint8 *x)
{
	int err;
	double a;

	if(bas != NULL)
		ssleep(178142);

	if((err = decodeNum_f(&a, x)) < 0)
		return err;
	if(a <= 0.0)
		return ERR_22;
	return encodeNum_f(x, log(a));
}

/*
	LOF
*/
int funcLof(struct Basic *bas, uint8 *x)
{
	return ERR_10;
}

/*
	LOG
*/
int funcLog(struct Basic *bas, uint8 *x)
{
	int err;
	double a;

	if(bas != NULL)
		ssleep(149790);

	if((err = decodeNum_f(&a, x)) < 0)
		return err;
	if(a <= 0.0)
		return ERR_22;
	return encodeNum_f(x, log(a) / log(10.0));
}

/*
	MDF
*/
int funcMdf(struct Basic *bas, uint8 *x)
{
	return ERR_10;
}

/*
	MID$
*/
int funcMidS(struct Basic *bas, uint8 *x, uint8 *y, uint8 *z)
{
	int err, a, b, len;

	if(bas != NULL)
		ssleep(18051);

	if((err = decodeNum_i(&a, y)) < 0)
		return err;
	if(--a < 0)
		return ERR_33;
	if((err = decodeNum_i(&b, z)) < 0)
		return err;
	if(b < 0)
		return ERR_33;

	len = strlen(x);
	if(a > len)
		a = len;
	if(a + b > len)
		b = len - a;

	memmove(x, x + a, b);
	*(x + b) = 0;
	return ERR_OK;
}

/*
	NCR
*/
int funcNcr(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err;
	uint8 a[SIZEOF_NUM], b[SIZEOF_NUM];

	if(bas != NULL)
		ssleep(44211);

	if(!numIsInt(x) || !numIsInt(y) || numSgn(x) < 0 || numSgn(y) < 0 || numCmp(x, y) < 0)
		return ERR_22;
	if(numSgn(y) == 0)
		return numLet(x, NUM_1);
	if(numSgn(x) == 0)
		return ERR_22;

	/* A = X */
	if((err = numLet(a, x)) < 0)
		return err;

	/* B = Y */
	if((err = numLet(b, y)) < 0)
		return err;

	for(;;) {
		/* B = B - 1 */
		if((err = opeSub(NULL, b, NUM_1)) < 0)
			return err;
		if(numSgn(b) <= 0)
			break;

		/* Y = Y * B */
		if((err = opeMul(NULL, y, b)) < 0)
			return err;

		/* A = A - 1 */
		if((err = opeSub(NULL, a, NUM_1)) < 0)
			return err;

		/* X = X * A */
		if((err = opeMul(NULL, x, a)) < 0)
			return err;
	}

	return opeDiv(NULL, x, y);
}

/*
	NPR
*/
int funcNpr(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err;
	uint8 a[SIZEOF_NUM];

	if(bas != NULL)
		ssleep(175558);

	if(!numIsInt(x) || !numIsInt(y) || numSgn(x) < 0 || numSgn(y) < 0 || numCmp(x, y) < 0)
		return ERR_22;
	if(numSgn(y) == 0)
		return numLet(x, NUM_1);
	if(numSgn(x) == 0)
		return ERR_22;

	/* A = X */
	if((err = numLet(a, x)) < 0)
		return err;

	for(;;) {
		/* Y = Y - 1 */
		if((err = opeSub(NULL, y, NUM_1)) < 0)
			return err;
		if(numSgn(y) <= 0)
			break;

		/* A = A - 1 */
		if((err = opeSub(NULL, a, NUM_1)) < 0)
			return err;

		/* X = X * A */
		if((err = opeMul(NULL, x, a)) < 0)
			return err;
	}

	return ERR_OK;
}

/*
	PEEK
*/
int funcPeek(struct Basic *bas, uint8 *x)
{
	int err, address;

	if(bas != NULL)
		ssleep(21036);

	if((err = decodeNum_i(&address, x)) < 0)
		return err;
	if(address < 0 || address > 0xffff)
		return ERR_33;

	if((err = encodeNum_i(x, memory[address])) < 0)
		return err;
	return ERR_OK;
}

/*
	PI
*/
int funcPi(struct Basic *bas, uint8 *x)
{
	if(bas != NULL)
		ssleep(1118);

	return numLet(x, NUM_PI);
}

/*
	PIOGET
*/
int funcPioget(struct Basic *bas, uint8 *x)
{
	return ERR_10;
}

/*
	POINT
*/
int funcPoint(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err, a, b, c;

	if(bas != NULL)
		ssleep(29271);

	if((err = decodeNum_i(&a, x)) < 0)
		return err;
	if((err = decodeNum_i(&b, y)) < 0)
		return err;

	if(0 <= a && a < lcdWidth && 0 <= b &&  b < lcdHeight)
		c = point(a, b) & (1 << (b % 8)) ? 1: 0;
	else
		c = -1;

	if((err = encodeNum_i(x, c)) < 0)
		return err;
	return ERR_OK;
}

/*
	POL
*/
int funcPol(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err;
	double a, b;

	if(bas != NULL)
		ssleep(411636);

	/* �� = ATN(Y / X) */
	if((err = decodeNum_f(&a, x)) < 0)
		return err;
	if((err = decodeNum_f(&b, y)) < 0)
		return err;
	if((err = encodeNum_r(bas, bas->fixed_var[0], atan2(b, a))) < 0)
		return err;

	/* r = SQR(X^2 + Y^2) */
	if((err = funcSqu(NULL, x)) < 0)
		return err;
	if((err = funcSqu(NULL, y)) < 0)
		return err;
	if((err = opeAdd(NULL, x, y)) < 0)
		return err;
	if((err = funcSqr(NULL, x)) < 0)
		return err;
	return numLet(bas->fixed_var[1], x);
}

/*
	RCP
*/
int funcRcp(struct Basic *bas, uint8 *x)
{
	int err;
	uint8 a[SIZEOF_NUM];

	if(bas != NULL)
		ssleep(28386);

	/* A = X */
	if((err = numLet(a, x)) < 0)
		return err;

	/* X = 1 */
	if((err = numLet(x, NUM_1)) < 0)
		return err;

	/* X = X / A */
	return opeDiv(NULL, x, a);
}

/*
	REC
*/
int funcRec(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err;

	if(bas != NULL)
		ssleep(625379);

	/* y = r * SIN(��) */
	if((err = numLet(bas->fixed_var[0], y)) < 0)
		return err;
	if((err = funcSin(NULL, bas->fixed_var[0])) < 0)
		return err;
	if((err = opeMul(NULL, bas->fixed_var[0], x)) < 0)
		return err;

	/* x = r * cos(��) */
	if((err = numLet(bas->fixed_var[1], y)) < 0)
		return err;
	if((err = funcCos(NULL, bas->fixed_var[1])) < 0)
		return err;
	return opeMul(NULL, x, bas->fixed_var[1]);
}

/*
	RIGHT$
*/
int funcRightS(struct Basic *bas, uint8 *x, uint8 *y)
{
	int err, a, len;

	if(bas != NULL)
		ssleep(10262);

	if((err = decodeNum_i(&a, y)) < 0)
		return err;
	if(a < 0)
		return ERR_33;

	len = strlen(x);
	if(a > len)
		a = len;

	memmove(x, x + len - a, a + 1);
	return ERR_OK;
}

/*
	RND
*/
int funcRnd(struct Basic *bas, uint8 *x)
{
	int err, max;

	if(bas != NULL)
		ssleep(64431);

	if(numSgn(x) <= 0)
		srand(bas->seed);

	if(numCmp(x, NUM_2) < 0)
		return encodeNum_f(x, (double )rand() / (RAND_MAX + 1));
	else if(numIsInt(x)) {
		if((err = decodeNum_i(&max, x)) < 0)
			return err;
		return encodeNum_i(x, 1 + rand() % max);
	} else {
		if((err = decodeNum_i(&max, x)) < 0)
			return err;
		return encodeNum_i(x, 1 + rand() % (max + 1));
	}
}

/*
	ROUND (���z�֐�)
*/
int funcRound(struct Basic *bas, uint8 *x)
{
	int err;

	if(numSgn(x) >= 0)
		err = opeAdd(NULL, x, NUM_0_5);
	else
		err = opeSub(NULL, x, NUM_0_5);
	if(err < 0)
		return err;
	return funcFix(NULL, x);
}

/*
	SGN
*/
int funcSgn(struct Basic *bas, uint8 *x)
{
	if(bas != NULL)
		ssleep(2119);

	return encodeNum_i(x, numSgn(x));
}

/*
	SIN
*/
int funcSin(struct Basic *bas, uint8 *x)
{
	double r;
	int err;

	if(bas != NULL)
		ssleep(317274);

	if((err = decodeNum_r(bas, &r, x)) < 0)
		return err;
	return encodeNum_f(x, sin(r));
}

/*
	SQR
*/
int funcSqr(struct Basic *bas, uint8 *x)
{
	double a;
	int err;

	if(bas != NULL)
		ssleep(51503);

	if((err = decodeNum_f(&a, x)) < 0)
		return err;
	return encodeNum_f(x, sqrt(a));
}

/*
	SQU
*/
int funcSqu(struct Basic *bas, uint8 *x)
{
	if(bas != NULL)
		ssleep(12628);

	return opePow(NULL, x, NUM_2);
}

/*
	STR$
*/
int funcStrS(struct Basic *bas, uint8 *x)
{
	int len;
	uint8 buf[256];

	if(bas != NULL)
		ssleep(17251);

	if((len = numCorrect(x)) < 0)
		return len;
	if((len = decodeNum(buf, x)) < 0)
		return len;
	if(len > 0 && buf[len - 1] == '.') {
		len--;
		buf[len] = 0;
	}

	strcpy(x, buf);
	return ERR_OK;
}

/*
	TAN
*/
int funcTan(struct Basic *bas, uint8 *x)
{
	int err;
	double a;

	if(bas != NULL)
		ssleep(178251);

	if((err = decodeNum_r(bas, &a, x)) < 0)
		return err;
	return encodeNum_f(x, tan(a));
}

/*
	TEN
*/
int funcTen(struct Basic *bas, uint8 *x)
{
	int err;
	uint8 a[SIZEOF_NUM];

	if(bas != NULL)
		ssleep(41220);

	if((err = numLet(a, x)) < 0)
		return err;
	if((err = numLet(x, NUM_10)) < 0)
		return err;
	return opePow(NULL, x, a);
}

/*
	VAL
*/
int funcVal(struct Basic *bas, uint8 *x)
{
	uint8 *str;

	if(bas != NULL)
		ssleep(8599);

	for(str = x; isspace(*str); str++)
		;

	if(encodeNumDms(x, str, FALSE) < 0)
		memcpy(x, NUM_0, SIZEOF_NUM);
	return ERR_OK;
}

/*
	VDEG
*/
int funcVdeg(struct Basic *bas, uint8 *x)
{
	int len;
	uint8 tmp[SIZEOF_NUM];

	if(bas != NULL)
		ssleep(56998);

	if((len = encodeNumDms(tmp, x, TRUE)) < 0)
		return len;
	if(x[len] != 0)
		return ERR_22;

	tmp[1] &= ~0x04;
	return numLet(x, tmp);
}

/* �֐��E���Z�q�\ */
const struct Operator opeTable[] = {
	{ NULL },	/* 00 */
	{ NULL },	/* 01 */
	{ NULL },	/* 02 */
	{ NULL },	/* 03 */
	{ NULL },	/* 04 */
	{ NULL },	/* 05 */
	{ NULL },	/* 06 */
	{ NULL },	/* 07 */
	{ NULL },	/* 08 */
	{ NULL },	/* 09 */
	{ NULL },	/* 0a */
	{ NULL },	/* 0b */
	{ NULL },	/* 0c */
	{ NULL },	/* 0d */
	{ NULL },	/* 0e */
	{ NULL },	/* 0f MON */
	{ NULL },	/* 10 RUN */
	{ NULL },	/* 11 NEW */
	{ NULL },	/* 12 CONT */
	{ NULL },	/* 13 PASS */
	{ NULL },	/* 14 LIST */
	{ NULL },	/* 15 LLIST */
	{ NULL },	/* 16 BLOAD/CLOAD */
	{ NULL },	/* 17 RENUM */
	{ NULL },	/* 18 LOAD */
	{ NULL },	/* 19 */
	{ NULL },	/* 1a AUTO */
	{ NULL },	/* 1b DELETE */
	{ NULL },	/* 1c FILES */
	{ NULL },	/* 1d */
	{ NULL },	/* 1e */
	{ NULL },	/* 1f LCOPY */
	{ NULL },	/* 20 BSAVE/CSAVE */
	{ NULL },	/* 21 OPEN */
	{ NULL },	/* 22 CLOSE */
	{ NULL },	/* 23 SAVE */
	{ NULL },	/* 24 */
	{ NULL },	/* 25 RANDOMIZE */
	{ NULL },	/* 26 DEGREE */
	{ NULL },	/* 27 RADIAN */
	{ NULL },	/* 28 GRAD */
	{ NULL },	/* 29 BEEP */
	{ NULL },	/* 2a WAIT */
	{ NULL },	/* 2b GOTO */
	{ NULL },	/* 2c TRON */
	{ NULL },	/* 2d TROFF */
	{ NULL },	/* 2e CLEAR */
	{ NULL },	/* 2f USING */
	{ NULL },	/* 30 DIM */
	{ NULL },	/* 31 CALL */
	{ NULL },	/* 32 POKE */
	{ NULL },	/* 33 GPRINT */
	{ NULL },	/* 34 PSET */
	{ NULL },	/* 35 PRESET */
	{ NULL },	/* 36 */
	{ NULL },	/* 37 */
	{ NULL },	/* 38 */
	{ NULL },	/* 39 */
	{ NULL },	/* 3a ERASE */
	{ NULL },	/* 3b LFILES */
	{ NULL },	/* 3c KILL */
	{ NULL },	/* 3d */
	{ NULL },	/* 3e */
	{ NULL },	/* 3f */
	{ NULL },	/* 40 */
	{ NULL },	/* 41 */
	{ NULL },	/* 42 */
	{ NULL },	/* 43 */
	{ NULL },	/* 44 */
	{ NULL },	/* 45 OUT */
	{ NULL },	/* 46 */
	{ NULL },	/* 47 */
	{ NULL },	/* 48 PIOSET */
	{ NULL },	/* 49 PIOPUT */
	{ NULL },	/* 4a SPOUT */
	{ NULL },	/* 4b SPINP */
	{ NULL },	/* 4c HDCOPY */
	{ NULL },	/* 4d ENDIF */
	{ NULL },	/* 4e REPEAT */
	{ NULL },	/* 4f UNTIL */
	{ NULL },	/* 50 CLS */
	{ NULL },	/* 51 LOCATE */
	{ NULL },	/* 52 TO */
	{ NULL },	/* 53 STEP */
	{ NULL },	/* 54 THEN */
	{ NULL },	/* 55 ON */
	{ NULL },	/* 56 IF */
	{ NULL },	/* 57 FOR */
	{ NULL },	/* 58 LET */
	{ NULL },	/* 59 REM */
	{ NULL },	/* 5a END */
	{ NULL },	/* 5b NEXT */
	{ NULL },	/* 5c STOP */
	{ NULL },	/* 5d READ */
	{ NULL },	/* 5e DATA */
	{ NULL },	/* 5f */
	{ NULL },	/* 60 PRINT */
	{ NULL },	/* 61 INPUT */
	{ NULL },	/* 62 GOSUB */
	{ NULL },	/* 63 LNINPUT */
	{ NULL },	/* 64 LPRINT */
	{ NULL },	/* 65 RETURN */
	{ NULL },	/* 66 RESTORE */
	{ NULL },	/* 67 */
	{ NULL },	/* 68 GCURSOR */
	{ NULL },	/* 69 LINE */
	{ NULL },	/* 6a */
	{ NULL },	/* 6b */
	{ NULL },	/* 6c */
	{ NULL },	/* 6d */
	{ NULL },	/* 6e */
	{ NULL },	/* 6f CIRCLE */
	{ NULL },	/* 70 PAINT */
	{ NULL },	/* 71 OUTPUT */
	{ NULL },	/* 72 APPEND */
	{ NULL },	/* 73 AS */
	{ NULL },	/* 74 */
	{ NULL },	/* 75 */
	{ NULL },	/* 76 ELSE */
	{ NULL },	/* 77 */
	{ NULL },	/* 78 */
	{ NULL },	/* 79 */
	{ NULL },	/* 7a WHILE */
	{ NULL },	/* 7b WEND */
	{ NULL },	/* 7c SWITCH */
	{ NULL },	/* 7d CASE */
	{ NULL },	/* 7e DEFAULT */
	{ NULL },	/* 7f ENDSWITCH */
	{ funcMdf, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 80 MDF */
	{ funcRec, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, NULL },	/* 81 REC */
	{ funcPol, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, NULL },	/* 82 POL */
	{ NULL },	/* 83 */
	{ NULL },	/* 84 */
	{ NULL },	/* 85 */
	{ funcTen, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 86 TEN */
	{ funcRcp, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 87 RCP */
	{ funcSqu, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 88 SQU */
	{ funcCur, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 89 CUR */
	{ funcHsn, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 8a HSN */
	{ funcHcs, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 8b HCS */
	{ funcHtn, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 8c HTN */
	{ funcAhs, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 8d AHS */
	{ funcAhc, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 8e AHC */
	{ funcAht, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 8f AHT */
	{ funcFact, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 90 FACT */
	{ funcLn, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 91 LN */
	{ funcLog, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 92 LOG */
	{ funcExp, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 93 EXP */
	{ funcSqr, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 94 SQR */
	{ funcSin, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 95 SIN */
	{ funcCos, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 96 COS */
	{ funcTan, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 97 TAN */
	{ funcInt, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 98 INT */
	{ funcAbs, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 99 ABS */
	{ funcSgn, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 9a SGN */
	{ funcDeg, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 9b DEG */
	{ funcDms, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 9c DMS */
	{ funcAsn, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 9d ASN */
	{ funcAcs, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 9e ACS */
	{ funcAtn, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 9f ATN */
	{ funcRnd, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* a0 RND */
	{ opeAnd, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, NULL },	/* a1 AND */
	{ opeOr, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, NULL },	/* a2 OR */
	{ opeNot, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* a3 NOT */
	{ funcPeek, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* a4 PEEK */
	{ opeXor, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, NULL },	/* a5 XOR */
	{ funcInp, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* a6 INP */
	{ NULL },	/* a7 */
	{ funcPioget, 0, { 0 }, TYPE_NUM, NULL },	/* a8 PIOGET */
	{ NULL },	/* a9 */
	{ NULL },	/* aa */
	{ NULL },	/* ab */
	{ NULL },	/* ac */
	{ funcPoint, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, NULL },	/* ad POINT */
	{ funcPi, 0, { 0 }, TYPE_NUM, NULL },	/* ae PI */
	{ funcFre },	/* af FRE */
	{ funcEof, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* b0 EOF */
	{ NULL },	/* b1 */
	{ funcLof, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* b2 LOF */
	{ NULL },	/* b3 */
	{ NULL },	/* b4 */
	{ NULL },	/* b5 */
	{ funcNcr, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, NULL },	/* b6 NCR */
	{ funcNpr, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, NULL },	/* b7 NPR */
	{ NULL },	/* b8 */
	{ NULL },	/* b9 */
	{ NULL },	/* ba */
	{ NULL },	/* bb */
	{ NULL },	/* bc */
	{ NULL },	/* bd */
	{ NULL },	/* be */
	{ funcCub, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* bf CUB */
	{ NULL },	/* c0 */
	{ NULL },	/* c1 */
	{ NULL },	/* c2 */
	{ NULL },	/* c3 */
	{ NULL },	/* c4 */
	{ NULL },	/* c5 */
	{ opeMod, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, NULL },	/* c6 MOD */
	{ funcFix, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* c7 FIX */
	{ NULL },	/* c8 */
	{ NULL },	/* c9 */
	{ NULL },	/* ca */
	{ NULL },	/* cb */
	{ NULL },	/* cc */
	{ NULL },	/* cd */
	{ NULL },	/* ce */
	{ NULL },	/* cf */
	{ funcAsc, 1, { TYPE_STR }, TYPE_NUM, NULL },	/* d0 ASC */
	{ funcVal, 1, { TYPE_STR }, TYPE_NUM, NULL },	/* d1 VAL */
	{ funcLen, 1, { TYPE_STR }, TYPE_NUM, NULL },	/* d2 LEN */
	{ funcVdeg, 1, { TYPE_STR }, TYPE_NUM, NULL },	/* d3 VDEG */
	{ NULL },	/* d4 */
	{ NULL },	/* d5 */
	{ NULL },	/* d6 */
	{ NULL },	/* d7 */
	{ NULL },	/* d8 */
	{ NULL },	/* d9 */
	{ NULL },	/* da */
	{ NULL },	/* db */
	{ NULL },	/* dc */
	{ NULL },	/* dd */
	{ NULL },	/* de */
	{ NULL },	/* df */
	{ NULL },	/* e0 */
	{ NULL },	/* e1 */
	{ NULL },	/* e2 */
	{ NULL },	/* e3 */
	{ NULL },	/* e4 */
	{ NULL },	/* e5 */
	{ NULL },	/* e6 */
	{ NULL },	/* e7 */
	{ NULL },	/* e8 */
	{ funcInkeyS, 0, { 0 }, TYPE_STR, NULL },	/* e9 INKEY$ */
	{ funcMidS, 3, { TYPE_STR, TYPE_NUM, TYPE_NUM }, TYPE_STR, NULL },	/* ea MID$ */
	{ funcLeftS, 2, { TYPE_STR, TYPE_NUM }, TYPE_STR, NULL },	/* eb LEFT$ */
	{ funcRightS, 2, { TYPE_STR, TYPE_NUM }, TYPE_STR, NULL },	/* ec RIGHT$ */
	{ NULL },	/* ed */
	{ NULL },	/* ee */
	{ NULL },	/* ef */
	{ funcChrS, 1, { TYPE_NUM }, TYPE_STR, NULL },	/* f0 CHR$ */
	{ funcStrS, 1, { TYPE_NUM }, TYPE_STR, NULL },	/* f1 STR$ */
	{ funcHexS, 1, { TYPE_NUM }, TYPE_STR, NULL },	/* f2 HEX$ */
	{ funcDmsS, 1, { TYPE_NUM }, TYPE_STR, NULL },	/* f3 DMS$ */
	{ NULL },	/* f4 */
	{ NULL },	/* f5 */
	{ NULL },	/* f6 */
	{ NULL },	/* f7 */
	{ NULL },	/* f8 */
	{ NULL },	/* f9 */
	{ NULL },	/* fa */
	{ NULL },	/* fb */
	{ NULL },	/* fc */
	{ NULL },	/* fd */
	{ NULL },	/* fe */
	{ NULL },	/* ff */

	{ opeAdd, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, &opeTable[0x10e] },	/* 100 + */
	{ opeSub, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, NULL },	/* 101 - */
	{ opeMul, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, NULL },	/* 102 * */
	{ opeDiv, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, NULL },	/* 103 / */
	{ opeIdiv, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, NULL },	/* 104 \ */
	{ opePow, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, NULL },	/* 105 ^ */
	{ opeEq, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, &opeTable[0x10f] },	/* 106 = */
	{ opeNe, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, &opeTable[0x110] },	/* 107 <> */
	{ opeGt, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, &opeTable[0x111] },	/* 108 > */
	{ opeGe, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, &opeTable[0x112] },	/* 109 >= */
	{ opeLt, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, &opeTable[0x113] },	/* 10a < */
	{ opeLe, 2, { TYPE_NUM, TYPE_NUM }, TYPE_NUM, &opeTable[0x114] },	/* 10b <= */
	{ opePos, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 10c + */
	{ opeNeg, 1, { TYPE_NUM }, TYPE_NUM, NULL },	/* 10d - */

	{ opeCat, 2, { TYPE_STR, TYPE_STR }, TYPE_STR, NULL },	/* 0x10e + */
	{ opeEqStr, 2, { TYPE_STR, TYPE_STR }, TYPE_NUM, NULL },	/* 0x10f = */
	{ opeNeStr, 2, { TYPE_STR, TYPE_STR }, TYPE_NUM, NULL },	/* 0x110 <> */
	{ opeGtStr, 2, { TYPE_STR, TYPE_STR }, TYPE_NUM, NULL },	/* 0x111 > */
	{ opeGeStr, 2, { TYPE_STR, TYPE_STR }, TYPE_NUM, NULL },	/* 0x112 >= */
	{ opeLtStr, 2, { TYPE_STR, TYPE_STR }, TYPE_NUM, NULL },	/* 0x113 < */
	{ opeLeStr, 2, { TYPE_STR, TYPE_STR }, TYPE_NUM, NULL }	/* 0x114 <= */
};

/*
	�X�^�b�N�ɐ��l��ς�
*/
int pushNum(const uint8 *num)
{
	if(typeSp >= &typeStack[sizeof(typeStack) / sizeof(typeStack[0]) - 1])
		return ERR_54;
	typeSp++;
	valueSp++;

	*typeSp = TYPE_NUM;
	memcpy(*valueSp, num, 8);
	return ERR_OK;
}

/*
	�X�^�b�N�ɐ��l��ς�(���������_)
*/
int pushNum_f(double f)
{
	int err;
	uint8 num[SIZEOF_NUM];

	if((err = encodeNum_f(num, f)) < 0)
		return err;
	return pushNum(num);
}

/*
	�X�^�b�N�ɐ��l��ς�(����)
*/
int pushNum_i(int i)
{
	int err;
	uint8 num[SIZEOF_NUM];

	if((err = encodeNum_i(num, i)) < 0)
		return err;
	return pushNum(num);
}

/*
	�X�^�b�N�ɐ��l��ς�
*/
int pushNum_s(char *str)
{
	int err;
	uint8 num[SIZEOF_NUM];

	if((err = encodeNumDms(num, (uint8 *)str, TRUE)) < 0)
		return err;
	return pushNum(num);
}

/*
	�X�^�b�N�̐擪�̐��l�𓾂�
*/
int peekNum(uint8 **num)
{
	if(typeSp < typeStack)
		return ERR_10;
	if(*typeSp != TYPE_NUM)
		return ERR_90;
	if(num != NULL)
		*num = *valueSp;
	return ERR_OK;
}

/*
	�X�^�b�N�̐擪�̐��l�𕂓������_�Ƃ��ē���
*/
/*
int peekNum_f(double *d)
{
	int err;
	uint8 *num;

	if((err = peekNum(&num)) < 0)
		return err;
	return decodeNum_f(d, num);
}
*/

/*
	�X�^�b�N�̐擪�̐��l�𐮐��Ƃ��ē���
*/
int peekNum_i(int *i)
{
	int err;
	uint8 *num;

	if((err = peekNum(&num)) < 0)
		return err;
	return decodeNum_i(i, num);
}

/*
	�X�^�b�N���琔�l���~�낷
*/
int popNum(uint8 **num)
{
	int err;

	if((err = peekNum(num)) < 0)
		return err;
	typeSp--;
	valueSp--;
	return ERR_OK;
}

/*
	�X�^�b�N���琔�l�𐮐��Ƃ��č~�낷
*/
int popNum_i(int *i)
{
	int err;
	uint8 *num;

	if((err = popNum(&num)) < 0)
		return err;
	return decodeNum_i(i, num);
}

/*
	�X�^�b�N���琔�l�𕂓������_�Ƃ��č~�낷
*/
int popNum_f(double *f)
{
	int err;
	uint8 *num;

	if((err = popNum(&num)) < 0)
		return err;
	return decodeNum_f(f, num);
}

/*
	�X�^�b�N�ɕ������ς�
*/
int pushStr(const uint8 *str, int size)
{
	if(typeSp >= &typeStack[sizeof(typeStack) / sizeof(typeStack[0]) - 1])
		return ERR_54;
	typeSp++;
	valueSp++;

	*typeSp = TYPE_STR;
	memcpy(*valueSp, str, size);
	*(*valueSp + size) = 0;
	return ERR_OK;
}

/*
	�X�^�b�N�̐擪�̕�����𓾂�
*/
int peekStr(uint8 **str)
{
	if(typeSp < typeStack)
		return ERR_10;
	if(*typeSp != TYPE_STR)
		return ERR_90;
	if(str != NULL)
		*str = *valueSp;
	return ERR_OK;
}

/*
	�X�^�b�N���當������~�낷
*/
int popStr(uint8 **str)
{
	int err;

	if((err = peekStr(str)) < 0)
		return err;
	typeSp--;
	valueSp--;
	return ERR_OK;
}

/*
	�󔒂�?
*/
int isBlank(const uint8 **p)
{
	return **p == ' ' || **p == '\t';
}

/*
	�󔒂��΂�
*/
uint8 *skipBlank(const uint8 **p)
{
	while(isBlank(p))
		(*p)++;
	return (uint8 *)*p;
}

/*
	�L�������ׂ�
*/
int peekSymbol(const uint8 **p, const uint8 *symbol)
{
	return memicmp(*p, symbol, strlen(symbol)) == 0;
}
#define peekSymbol(p, symbol)	peekSymbol(p, (const uint8 *)(symbol))

/*
	�L���𓾂�
*/
int fetchSymbol(const uint8 **p, const uint8 *symbol)
{
	if(peekSymbol(p, symbol)) {
		(*p) += strlen(symbol);
		skipBlank(p);
		return TRUE;
	} else
		return FALSE;
}
#define fetchSymbol(p, symbol)	fetchSymbol(p, (const uint8 *)(symbol))

/*
	���J�b�R�𓾂�
*/
int fetchLParen(const uint8 **p)
{
	return fetchSymbol(p, "(");
}

/*
	�E�J�b�R�𓾂�
*/
int fetchRParen(const uint8 **p)
{
	return fetchSymbol(p, ")");
}

/*
	���_�u���N�I�[�e�[�V�����𓾂�
*/
int fetchLDquote(const uint8 **p)
{
	if(**p != '\"')
		return FALSE;

	(*p)++;
	return TRUE;
}

/*
	�E�_�u���N�I�[�e�[�V�����𓾂�
*/
int fetchRDquote(const uint8 **p)
{
	return fetchSymbol(p, "\"");
}

/*
	�R���}�𓾂�
*/
int fetchComma(const uint8 **p)
{
	return fetchSymbol(p, ",");
}

/*
	�ϐ��������ʁE�^�𓾂�
*/
int getVarType(const uint8 *name, int *kind, int *type)
{
	int len = 0;
	const uint8 *p = name;

	if(!isalpha(*p))
		return ERR_OK;

	while(isalnum(*p))
		p++, len++;
	if(*p == '$') {
		*type = TYPE_STR;
		p++;
	} else
		*type = TYPE_NUM;
	skipBlank(&p);

	if(*p == '(')
		*kind = KIND_ARRAY;
	else if(len > 1)
		*kind = KIND_SIMPLE;
	else
		*kind = KIND_FIXED;

	skipBlank(&p);
	return (int )(p - name);
}

/*
	�o�^���ꂽ�ϐ�����������
*/
int findVar(struct Basic *bas, uint8 **var, uint8 *ret_name1, const uint8 *name, int kind, int type)
{
	uint8 name1, **v;

	/* �Œ�ϐ��Ȃ�΂����߂� */
	if(kind == KIND_FIXED) {
		*var = bas->fixed_var['Z' - name[0]];
		return ERR_OK;
	}

	/* �ϐ��̓�����2�����ڂ����߂� */
	if(kind == KIND_SIMPLE) {
		if(type == TYPE_NUM)
			name1 = name[1];
		else
			name1 = name[1] + 0x40;
	} else {
		if(type == TYPE_NUM)
			name1 = (isalnum(name[1]) ? name[1]: 0) + 0x80;
		else
			name1 = (isalnum(name[1]) ? name[1]: 0xe0) + 0xc0;
	}
	if(ret_name1 != NULL)
		*ret_name1 = name1;
	/*printf("DEBUG: namer=%02x%02x, type=%d\n", name[0], name1, type);*/

	/* �ϐ����������� */
	for(v = bas->vars; *v != NULL; v++)
		if((*v)[0] == name[0] && (*v)[1] == name1) {
			*var = *v;
			return ERR_OK;
		}

	/* �ϐ����Ȃ����� */
	*var = NULL;
	return (kind == KIND_ARRAY ? ERR_31: 0);
}

/*
	�ϐ���o�^����
*/
int allocVar(struct Basic *bas, uint8 **var, const uint8 *name, int kind, int type, int array_size0, int array_size1, int len)
{
	int n, var_size;
	uint8 **v, *p, name1;

	/* �m�ۍς݂�? */
	findVar(bas, &p, &name1, name, kind, type);
	if(p != NULL)
		return ERR_OK;

	/* �ϐ��\�̗̈���m�ۂ��� */
	for(v = bas->vars, n = 0; *v != NULL; v++, n++)
		;
	bas->vars = realloc(bas->vars, (n + 2) * sizeof(*bas->vars));
	v = &bas->vars[n];

	/* �ϐ��̗̈���m�ۂ��� */
	if(type == TYPE_NUM)
		len = 8;
	else if(kind == KIND_SIMPLE)
		len = 16;
	var_size = (array_size0 + 1) * (array_size1 + 1) * len;

	*v++ = *var = p = malloc(7 + var_size);
	*v = NULL;

	/* �ϐ��̏����������� */
	*p++ = name[0];
	*p++ = name1;
	*p++ = (var_size + 3) >> 8;
	*p++ = (var_size + 3) & 0xff;
	*p++ = array_size1;
	*p++ = array_size0;
	*p++ = len;
	memset(p, 0, var_size);
	return ERR_OK;
}

/*
	�ϐ����������
*/
int freeVar(struct Basic *bas, const uint8 *name, int kind, int type)
{
	int err;
	uint8 *var, **v;

	if((err = findVar(bas, &var, NULL, name, kind, type)) < 0)
		return err;
	if(kind == KIND_FIXED)
		return ERR_OK;

	for(v = bas->vars; *v != var && *v != NULL; v++)
		;
	if(*v != var)
		return ERR_OK;

	free(*v);

	for(; *v != NULL; v++)
		*v = *(v + 1);

	bas->vars = realloc(bas->vars, (int )(v - bas->vars) * sizeof(*bas->vars));
	return ERR_OK;
}

/*
	�S�Ă̕ϐ����������
*/
int freeAllVars(struct Basic *bas)
{
	uint8 **v;

	for(v = bas->vars; *v != NULL; v++)
		free(*v);

	bas->vars = realloc(bas->vars, 1 * sizeof(*bas->vars));
	*bas->vars = NULL;
	return ERR_OK;
}

/*
	�S�Ă̕ϐ�������������
*/
int clearAllVars(struct Basic *bas)
{
	memset(bas->fixed_var, 0, sizeof(*bas->fixed_var) * 26);
	return freeAllVars(bas);
}

/*
	�P���ϐ��̒l�𓾂�
*/
int getSimpleVal(uint8 **val, int *type, int *size, uint8 *var)
{
	/* �^�𓾂� */
	if(var[1] < 0x70)
		*type = TYPE_NUM;
	else
		*type = TYPE_STR;
	/*printf("DEBUG: var=%02x%02x, type=%d\n", var[0], var[1], *type);*/

	/* �����𓾂� */
	*size = var[6];

	/* �l�𓾂� */
	*val = &var[7];
	return ERR_OK;
}

/*
	�z��ϐ��̒l�𓾂�
*/
int getArrayVal(uint8 **val, int *type, int *size, uint8 *var, int index0, int index1)
{
	int array_size0, array_size1;

	/* �^�𓾂� */
	if(0x20 <= var[1] && var[1] < 0xf0 && var[1] != 0xa0)
		*type = TYPE_NUM;
	else
		*type = TYPE_STR;
	/*printf("DEBUG: var=%02x%02x, type=%d\n", var[0], var[1], *type);*/

	/* 2�����ڂ̗v�f���𓾂� */
	array_size1 = var[4];

	if(index1 < 0)
		return ERR_10;
	if(index1 > array_size1)
		return ERR_31;

	/* 1�����ڂ̗v�f���𓾂� */
	array_size0 = var[5];

	if(index0 < 0)
		return ERR_10;
	if(index0 > array_size0)
		return ERR_31;

	/* �����𓾂� */
	*size = var[6];

	/* �l�𓾂� */
	*val = &var[7 + (index1 * (array_size0 + 1) + index0) * *size];
	return ERR_OK;
}

/*
	�ϐ��ɒl��������
*/
int setVarVal(uint8 *var, int kind, int type, int size, const uint8 *val)
{
	if(type == TYPE_NUM)
		memcpy(var, val, SIZEOF_NUM);
	else if(kind == KIND_FIXED) {
		*var = 0xf5;
		memcpy(var + 1, val, SIZEOF_NUM - 1);
	} else
		strncpy((char *)var, (const char *)val, size);
	return ERR_OK;
}

/*
	���e����������𓾂� (_pushParam�̉�����)
*/
static int fetchLitStr(struct Element *e, const uint8 **p)
{
	if(**p == '\"') {
		(*p)++;

		e->priority = PRIORITY_VAL;
		e->ele_type = ELE_TYPE_STR;
		e->x.str = *p;

		while(**p != '\"' && **p != '\r')
			(*p)++;
		if(**p == '\"')
			(*p)++;

		skipBlank(p);
		return 1;
	} else
		return 0;
}

/*
	���e�������l�𓾂� (_pushParam�̉�����)
*/
static int fetchLitNum(struct Element *e, const uint8 **p, int dms)
{
	int len;

	if((len = encodeNumDms(e->x.num, *p, dms)) > 0) {
		*p += len;

		e->priority = PRIORITY_VAL;
		e->ele_type = ELE_TYPE_NUM;

		skipBlank(p);
		return 1;
	} else
		return (len == ERR_20 ? ERR_20: 0);
}

/*
	�֐��𓾂� (_pushParam�̉�����)
*/
static int fetchFunc(struct Element *e, const uint8 **p)
{
	if(**p == CODE_RESERVED) {
		if(*(*p + 1) < 0x80)
			return 0;
		if(*(*p + 1) == CODE_AND || *(*p + 1) == CODE_OR || *(*p + 1) == CODE_XOR || *(*p + 1) == CODE_MOD)
			return 0;
		(*p)++;

		if(opeTable[**p].func == NULL)
			return ERR_10;
		e->x.ope = **p;
	} else if(**p == '+')
		e->x.ope = CODE_POS;
	else if(**p == '-')
		e->x.ope = CODE_NEG;
	else
		return 0;
	(*p)++;
	skipBlank(p);

	e->ele_type = ELE_TYPE_OPE;
	e->priority = PRIORITY_FUNC;
	return opeTable[e->x.ope].params + 1;
}

/*
	�ϐ��𓾂� (_pushParam�̉�����)
*/
static int fetchVar(struct Element *e, struct Basic *bas, const uint8 **p)
{
	int len, err, kind, type;

	if((len = getVarType(*p, &kind, &type)) <= 0)
		return 0;
	if((err = findVar(bas, &e->x.var, NULL, *p, kind, type)) < 0)
		return err;
	*p += len;

	if(kind == KIND_FIXED) {
		if(type == TYPE_STR) {
			if(memcmp(e->x.var, "\0\0\0\0\0\0\0\0", 8) == 0) { /* ����������? */
				e->ele_type = ELE_TYPE_STR;
				e->priority = PRIORITY_VAL;
				e->x.str = (const uint8 *)"";
				return 1;
			}

			if(e->x.var[0] != 0xf5) /* �ϐ��ƒl�̌^���قȂ邩? */
				return ERR_91;
		} else {
			if(e->x.var[0] == 0xf5) /* �ϐ��ƒl�̌^���قȂ邩? */
				return ERR_91;
		}

		e->ele_type = ELE_TYPE_FIXED;
		e->priority = PRIORITY_VAL;
		return 1;
	} else if(kind == KIND_SIMPLE) {
		if(e->x.var == NULL) {
			if(type == TYPE_NUM) {
				e->ele_type = TYPE_NUM;
				e->priority = PRIORITY_VAL;
				memcpy(e->x.num, NUM_0, sizeof(e->x.num));
				return 1;
			} else {
				e->ele_type = TYPE_STR;
				e->priority = PRIORITY_VAL;
				e->x.str = (const uint8 *)"";
				return 1;
			}
		}

		e->ele_type = ELE_TYPE_SIMPLE;
		e->priority = PRIORITY_VAL;
		return 1;
	} else {
		e->ele_type = ELE_TYPE_ARRAY;
		e->priority = PRIORITY_ARRAY;
		return 3;
	}
}

/*
	���Z�q�𓾂� (_pushParam�̉�����)
*/
static int fetchOpe(struct Element *e, const uint8 **p)
{
	if(**p == CODE_RESERVED) {
		if(*(*p + 1) == CODE_OR) {
			e->x.ope = CODE_OR;
			e->priority = PRIORITY_OPE + 0;
		} else if(*(*p + 1) == CODE_AND) {
			e->x.ope = CODE_AND;
			e->priority = PRIORITY_OPE + 0;
		} else if(*(*p + 1) == CODE_XOR) {
			e->x.ope = CODE_XOR;
			e->priority = PRIORITY_OPE + 0;
		} else if(*(*p + 1) == CODE_MOD) {
			e->x.ope = CODE_MOD;
			e->priority = PRIORITY_OPE + 4;
		} else
			return 0;
		(*p)++;
	} else if(**p == '<') {
		if(*(*p + 1) == '=') {
			e->x.ope = CODE_LE;
			e->priority = PRIORITY_OPE + 2;
			(*p)++;
		} else if(*(*p + 1) == '>') {
			e->x.ope = CODE_NE;
			e->priority = PRIORITY_OPE + 2;
			(*p)++;
		} else {
			e->x.ope = CODE_LT;
			e->priority = PRIORITY_OPE + 2;
		}
	} else if(**p == '=') {
		if(*(*p + 1) == '<') {
			e->x.ope = CODE_LE;
			e->priority = PRIORITY_OPE + 2;
			(*p)++;
		} else if(*(*p + 1) == '>') {
			e->x.ope = CODE_GE;
			e->priority = PRIORITY_OPE + 2;
			(*p)++;
		} else {
			e->x.ope = CODE_EQ;
			e->priority = PRIORITY_OPE + 2;
		}
	} else if(**p == '>') {
		if(*(*p + 1) == '=') {
			e->x.ope = CODE_GE;
			e->priority = PRIORITY_OPE + 2;
			(*p)++;
		} else if(*(*p + 1) == '<') {
			e->x.ope = CODE_NE;
			e->priority = PRIORITY_OPE + 2;
			(*p)++;
		} else {
			e->x.ope = CODE_GT;
			e->priority = PRIORITY_OPE + 2;
		}
	} else if(**p == '+') {
		e->x.ope = CODE_ADD;
		e->priority = PRIORITY_OPE + 3;
	} else if(**p == '-') {
		e->x.ope = CODE_SUB;
		e->priority = PRIORITY_OPE + 3;
	} else if(**p == '*') {
		e->x.ope = CODE_MUL;
		e->priority = PRIORITY_OPE + 4;
	} else if(**p == '/') {
		e->x.ope = CODE_DIV;
		e->priority = PRIORITY_OPE + 4;
	} else if(**p == '\\') {
		e->x.ope = CODE_IDIV;
		e->priority = PRIORITY_OPE + 4;
	} else if(**p == '^') {
		e->x.ope = CODE_POW;
		e->priority = PRIORITY_OPE + 5;
	} else
		return 0;
	(*p)++;
	skipBlank(p);

	e->ele_type = ELE_TYPE_OPE;
	return opeTable[e->x.ope].params + 1;
}

/*
	�v�f���X�^�b�N�ɐς� (_pushParam�̉�����)
*/
static int pushEle(const struct Element *e)
{
	if(top >= &stack[sizeof(stack) / sizeof(stack[0]) - 1])
		return ERR_54;
	top++;

	*top = *e;
	return 0;
}

/*
	���e����������̒����𓾂� (popEle�̉�����)
*/
static int getStrLen(const uint8 *str)
{
	const uint8 *p;

	for(p = str; *p != 0 && *p != '\"' && *p != '\r'; p++)
		;
	return (int )(p - str);
}

/*
	���Z�q�E�֐������s���� (popEle�̉�����)
*/
static int exeOpe(struct Basic *bas, int ope)
{
	const struct Operator *o;
	int err, depth;

	if(&opeTable[ope] == NULL)
		return ERR_10;

	depth = (int )(typeSp - typeStack);

	for(o = &opeTable[ope]; o != NULL; o = o->next)
		if(o->params == 0 || (o->params - 1 <= depth && memcmp(o->type, typeSp - (o->params - 1), o->params * sizeof(o->type[0])) == 0))
			break;
	if(o == NULL)
		return ERR_90;

	valueSp -= (o->params - 1);
	typeSp -= (o->params - 1);

	if(o->params == 0)
		err = o->func(bas, valueSp[0]);
	else if(o->params == 1)
		err = o->func(bas, valueSp[0]);
	else if(o->params == 2)
		err = o->func(bas, valueSp[0], valueSp[1]);
	else if(o->params == 3)
		err = o->func(bas, valueSp[0], valueSp[1], valueSp[2]);
	else
		err = -10;
	if(err < 0)
		return err;

	*typeSp = o->ret;
	return 0;
}

/*
	�v�f���X�^�b�N����~�낷 (_pushParam�̉�����)
*/
static int popEle(struct Basic *bas)
{
	int err, type, size, index0, index1;
	uint8 *val;

	if(top < stack)
		return ERR_10;

	switch(top->ele_type) {
	case ELE_TYPE_NUM: /* ���e�������l */
		pushNum(top->x.num);
		break;
	case ELE_TYPE_STR: /* ���e���������� */
		pushStr(top->x.str, getStrLen(top->x.str));
		break;
	case ELE_TYPE_FIXED: /* �Œ�ϐ� */
		if(top->x.var[0] != 0xf5)
			pushNum(top->x.var);
		else
			pushStr(top->x.var + 1, 7);
		break;
	case ELE_TYPE_SIMPLE: /* �P���ϐ� */
		if((err = getSimpleVal(&val, &type, &size, top->x.var)) < 0)
			return err;
		if(type == TYPE_NUM)
			pushNum(val);
		else
			pushStr(val, size);
		break;
	case ELE_TYPE_ARRAY: /* �z��ϐ� */
		if((err = popNum_i(&index1)) < 0)
			return err;
		if((err = popNum_i(&index0)) < 0)
			return err;
		if((err = getArrayVal(&val, &type, &size, top->x.var, index0, index1)) < 0)
			return err;
		if(type == TYPE_NUM)
			pushNum(val);
		else
			pushStr(val, size);
		break;
	case ELE_TYPE_OPE: /* ���Z�q�E�֐� */
		if((err = exeOpe(bas, top->x.ope)) < 0)
			return err;
		break;
	default:
		return ERR_10;
	}

	top--;
	return 0;
}

/*
	�}�j���A�����[�h��?
*/
static int isManual(struct Basic *bas, const uint8 *p)
{
	return (p < bas->prog || p >= bas->prog + bas->prog_size);
}

/*
	�l���X�^�b�N�ɐς� (pushParam�̉�����)
*/
static int _pushParam(struct Basic *bas, struct Element *bottom, const uint8 **p)
{
	struct Element e;
	int result, man = isManual(bas, *p);

	for(;;) {
		if((result = fetchLitStr(&e, p)) || (result = fetchLitNum(&e, p, man)) || (result = fetchFunc(&e, p)) || (result = fetchVar(&e, bas, p))) {
			int params;

			if(result < 0)
				return result;
			params = result - 1;

			if((result = pushEle(&e)) < 0)
				return result;

			if(params > 0) {
				if(!fetchLParen(p)) {
					if(params != 1)
						return ERR_10;
					continue;
				}
				for(;;) {
					if((result = _pushParam(bas, top, p)) < 0)
						return result;
					if(--params == 0)
						break;
					if(!fetchComma(p)) {
						if(e.ele_type != ELE_TYPE_ARRAY)
							return ERR_10;
						while(params-- > 0)
							pushNum(NUM_0);
						break;
					}
				}
				if(!fetchRParen(p))
					return ERR_10;
			}
		} else if(fetchLParen(p)) {
			if((result = _pushParam(bas, top, p)) < 0)
				return result;
			if(!fetchRParen(p))
				return ERR_10;
		} else
			return ERR_10;

		if((result = fetchOpe(&e, p))) {
			if(result < 0)
				return result;

			while(top > bottom && top->priority >= e.priority)
				if((result = popEle(bas)) < 0)
					return result;

			if((result = pushEle(&e)) < 0)
				return result;
		} else {
			while(top > bottom)
				if((result = popEle(bas)) < 0)
					return result;
			return 0;
		}
	}
}

/*
	�l���X�^�b�N�ɐς�
*/
int pushParam(struct Basic *bas, const uint8 **p)
{
	top = stack;
	return _pushParam(bas, top, p);
}

/*
	�l���X�^�b�N�ɐς�
*/
int pushParamOrEmpty(struct Basic *bas, const uint8 **p)
{
	int err;
	const uint8 *q = *p;

	if((err = pushParam(bas, p)) < 0)
		if(err == ERR_10 && *p == q)
			return 1;
	return err;
}

/*
	�l�𓾂�
*/
int fetchParam(struct Basic *bas, uint8 **param, int *type, const uint8 **p)
{
	int err;

	if((err = pushParam(bas, p)) < 0)
		return err;

	if(peekNum(NULL) >= 0) {
		if((err = popNum(param)) < 0)
			return err;
		*type = TYPE_NUM;
		return 0;
	} else if(peekStr(NULL) >= 0) {
		if((err = popStr(param)) < 0)
			return err;
		*type = TYPE_STR;
		return 0;
	} else
		return ERR_10;
}

/*
	�l(���l)�𓾂�
*/
int fetchNum(struct Basic *bas, uint8 **num, const uint8 **p)
{
	int err;

	if((err = pushParam(bas, p)) < 0)
		return err;
	if((err = popNum(num)) < 0)
		return err;
	return err;
}

/*
	�l(���l)�𓾂�
*/
int fetchNumOrEmpty(struct Basic *bas, uint8 **num, const uint8 **p)
{
	int err;

	if((err = pushParamOrEmpty(bas, p)) != 0)
		return err;
	if((err = popNum(num)) < 0)
		return err;
	return err;
}

/*
	�l(���l)�𐮐��Ƃ��ē���
*/
int fetchNum_i(struct Basic *bas, int *i, const uint8 **p)
{
	int err;

	if((err = pushParam(bas, p)) < 0)
		return err;
	return popNum_i(i);
}

/*
	�l(���l)�𐮐��Ƃ��ē���
*/
int fetchNumOrEmpty_i(struct Basic *bas, int *i, const uint8 **p)
{
	int err;

	if((err = pushParamOrEmpty(bas, p)) != 0)
		return err;
	return popNum_i(i);
}

/*
	�l(���l)�𕂓������_�Ƃ��ē���
*/
int fetchNum_f(struct Basic *bas, double *f, const uint8 **p)
{
	int err;

	if((err = pushParam(bas, p)) < 0)
		return err;
	return popNum_f(f);
}

/*
	�l(���l)�𕂓������_�Ƃ��ē���
*/
int fetchNumOrEmpty_f(struct Basic *bas, double *f, const uint8 **p)
{
	int err;

	if((err = pushParamOrEmpty(bas, p)) != 0)
		return err;
	return popNum_f(f);
}

/*
	�l(������)�𓾂�
*/
int fetchStr(struct Basic *bas, uint8 **str, const uint8 **p)
{
	int err;

	if((err = pushParam(bas, p)) < 0)
		return err;
	return popStr(str);
}

/*
	�l(������)�𓾂�
*/
int fetchStrOrEmpty(struct Basic *bas, uint8 **str, const uint8 **p)
{
	int err;

	if((err = pushParamOrEmpty(bas, p)) != 0)
		return err;
	return popStr(str);
}

/*
	�ϐ��𓾂�
*/
int fetchVarVal(struct Basic *bas, uint8 **val, int *kind, int *type, int *size, const uint8 **p)
{
	int err, len, index0, index1;
	uint8 *var, name1;

	/* �ϐ��̃A�h���X�𓾂� */
	if((len = getVarType(*p, kind, type)) <= 0)
		return ERR_10;
	if((err = findVar(bas, &var, &name1, *p, *kind, *type)) < 0)
		return err;

	/* �l�̃A�h���X�𓾂� */
	if(*kind == KIND_FIXED) {
		*val = var;
		*size = 8;

		*p += len;
		return 0;
	} else if(*kind == KIND_SIMPLE) {
		if(var == NULL)
			if((err = allocVar(bas, &var, *p, *kind, *type, 0, 0, 0)) < 0)
				return err;
		if((err = getSimpleVal(val, type, size, var)) < 0)
			return err;

		*p += len;
		return 0;
	} else {
		int count = 2;

		*p += len;

		if(!fetchLParen(p))
			return ERR_20;
		for(;;) {
			if((err = pushParam(bas, p)) < 0)
				return err;
			if(--count == 0)
				break;
			if(!fetchComma(p)) {
				while(count-- > 0)
					pushNum(NUM_0);
				break;
			}
		}
		if(!fetchRParen(p))
			return ERR_20;

		if((err = popNum_i(&index1)) < 0)
			return err;
		if((err = popNum_i(&index0)) < 0)
			return err;
		if((err = getArrayVal(val, type, size, var, index0, index1)) < 0)
			return err;
		return 0;
	}
}

/*
	�\��ꂩ�`�F�b�N����
*/
int isKeyword(const uint8 **p, uint8 code)
{
	if(**p != CODE_RESERVED)
		return FALSE;
	if(*(*p + 1) != code)
		return FALSE;
	return TRUE;
}

/*
	�\���𓾂�
*/
int fetchKeyword(const uint8 **p, uint8 code)
{
	if(!isKeyword(p, code))
		return FALSE;

	*p += 2;
	skipBlank(p);
	return TRUE;
}

/*
	�\���𓾂�
*/
int fetchAnyKeyword(const uint8 **p)
{
	if(**p != CODE_RESERVED)
		return FALSE;

	*p += 2;
	skipBlank(p);
	return TRUE;
}

/*
	�s�̏I�[��?
*/
int isLineTerm(const uint8 **p)
{
	return p == NULL || **p == '\r' || **p == '\'';
}

/*
	�X�e�[�g�����g�̏I�[��?
*/
int isStaTerm(const uint8 **p)
{
	return p == NULL || **p == ':';
}

/*
	�I�[��?
*/
int isTerm(const uint8 **p)
{
	return isLineTerm(p) || isStaTerm(p) || isKeyword(p, CODE_THEN) || isKeyword(p, CODE_ELSE) || isKeyword(p, CODE_TO);
}

/*
	�s�ԍ���?
*/
int isLineNo(const uint8 **p)
{
	return isdigit(**p);
}

/*
	�s�ԍ��𓾂�
*/
int fetchLineNoOnly(const uint8 **p)
{
	int i, line_no = 0;

	if(!isdigit(**p))
		return ERR_12;

	line_no = 0;

	for(i = 0; i < 5 && isLineNo(p); i++)
		line_no = line_no * 10 + (*(*p)++ - '0');
	if(line_no <= 0 || line_no > 0xff00)
		return ERR_41;

	return line_no;
}

/*
	�s�ԍ��𓾂�
*/
int fetchLineNo(const uint8 **p)
{
	int line_no;

	if((line_no = fetchLineNoOnly(p)) < 0)
		return line_no;

	skipBlank(p);
	return line_no;
}

/*
	���x����?
*/
int isLabel(const uint8 **p)
{
	return (**p == '*' || **p == '\"') && isalpha(*(*p + 1));
}

/*
	���x���𓾂�
*/
int fetchLabel(const uint8 **p)
{
	if(fetchSymbol(p, "*")) {
		do {
		} while(isalnum(*++(*p)));
	} else if(fetchSymbol(p, "\"")) {
		do {
		} while(isalnum(*++(*p)));

		fetchSymbol(p, "\"");
	} else
		return FALSE;

	skipBlank(p);
	return TRUE;
}

/*
	�\���ƕ�������r���� (getKeywordFromText, getKeywordFromCode�̉�����)
*/
static int cmpKeyword(const uint8 *keyword, const uint8 *str)
{
	const uint8 *p, *q;

	if(*str == '.')
		return -1;

	for(p = keyword, q = str; *p == toupper(*q) && *p != 0 && *q != '.'; p++, q++)
		;

	if(*p == 0)
		return (int )(q - str);
	else if(*q == '.')
		return (int )(q - str) + 1;
	else
		return -1;
}

/*
	�s�ԍ��𕜍�������
*/
int decodeLineNo(int *line_no, int *len, const uint8 *src)
{
	const uint8 *p = src;

	if(src == NULL)
		return 0;
	if(*p == 0xff)
		return 0;

	*line_no = *p++ * 0x100;
	*line_no += *p++;
	*len = *p;
	return 3;
}

/*
	�R�[�h����\������������
*/
int getKeywordFromCode(uint8 *name, int code)
{
	const struct KeywordTable *k;

	for(k = keywordTable; k->name != NULL; k++)
		if(k->code == code) {
			strcpy((char *)name, (const char *)k->name);
			return strlen((char *)name);
		}

	sprintf((char *)name, "RESERVED %02X", code);
	return 11;
}

/*
	���ԃR�[�h���e�L�X�g�ɕϊ�����
*/
int decodeProg(uint8 *dst, const uint8 *src)
{
	const uint8 *p = src;
	uint8 *q = dst;

	if(*p == 0xff)
		return 0;

	while(*p != '\r' && *p != 0) {
		if(*p == CODE_RESERVED) {
			p++;
			q += getKeywordFromCode(q, *p++);
			*q++ = ' ';
		} else
			*q++ = *p++;
	}
	/**q++ = *p;*/
	*q++ = 0;

	return (int )(p - src) + 1;
}

/*
	�s�ԍ��ƒ��ԃR�[�h���e�L�X�g�ɕϊ�����
*/
int decodeLineNoProg(uint8 *dst, const uint8 *src, const uint8 *sep)
{
	int line_no = 0, len;
	const uint8 *p = src;

	if(p == NULL || IS_LAST(p)) {
		strcpy(dst, "");
		return 0;
	}

	p += decodeLineNo(&line_no, &len, p);
	sprintf((char *)dst, "%d%s", line_no, sep);
	p += decodeProg(dst + strlen(dst), p);

	return (int )(p - src);
}
#define decodeLineNoProg(dst, src, sep)	decodeLineNoProg((uint8 *)(dst), (const uint8 *)(src), (const uint8 *)(sep))

/*
	�w��̍s�Ɉړ�����
*/
int jumpToLineNo(const uint8 **p, const uint8 *top, int line_no)
{
	for(*p = top; !IS_LAST(*p) && LINE_NO(*p) < line_no; *p += LINE_SIZE(*p))
		;

	return !IS_LAST(*p) && LINE_NO(*p) == line_no;
}

/*
	�ŏI�s�Ɉړ�����
*/
int jumpToLast(const uint8 **p, const uint8 *top)
{
	int line_no = 0;
	const uint8 *q;

	for(q = top; !IS_LAST(q); q += LINE_SIZE(q))
		line_no = LINE_NO(q);

	if(p != NULL)
		*p = q;
	return line_no;
}

/*
	���x�����r���� (jumpToLabel�̉�����)
*/
static int cmpLabel(const uint8 *label, const uint8 *p)
{
	skipBlank(&p);

	if(*p != '*')
		return FALSE;
	p++, label++;

	if(*p != *label || !isalpha(*p) || !isalpha(*label))
		return FALSE;

	while(*p == *label && isalnum(*p) && isalnum(*label))
		p++, label++;

	return !isalnum(*p) && !isalnum(*label);
}

/*
	�w��̃��x���Ɉړ�����
*/
int jumpToLabel(const uint8 **p, const uint8 *top, const uint8 *label)
{
	if(*label == '*' || *label == '\"') {
		int len, line_no, size;

		*p = top;

		while((len = decodeLineNo(&line_no, &size, *p)) > 0) {
			if(cmpLabel(label, *p + len))
				return TRUE;

			*p += size + len;
		}
	}

	*p = NULL;
	return FALSE;
}

/*
	�s�܂��̓��x������������
*/
int findLine(const uint8 **p, const uint8 **found, const uint8 *top)
{
	int line_no;

	if(isLineNo(p)) {
		if((line_no = fetchLineNo(p)) < 0)
			return ERR_10;
		if(!jumpToLineNo(found, top, line_no))
			return ERR_40;
	} else if(isLabel(p)) {
		const uint8 *label = *p;

		if(!fetchLabel(p))
			return ERR_10;
		if(!jumpToLabel(found, top, label))
			return ERR_40;
	} else
		return ERR_10;

	skipBlank(p);
	return 0;
}

/*
	���̃R�[�h�Ɉړ�����
*/
int goNext(const uint8 **p)
{
	if(**p == CODE_RESERVED)
		*p += 2;
	else if(**p == '\r') {
		(*p)++;

		if(**p == 0xff) {
			*p = NULL;
			return FALSE;
		}

		*p += 3;
	} else
		(*p)++;
	return TRUE;
}

/*
	���̍s�Ɉړ�����
*/
int goNextLine(const uint8 **p)
{
	if(*p == NULL || **p == 0xff) {
		*p = NULL;
		return FALSE;
	}

	while(**p != '\r')
		goNext(p);
	(*p)++;

	if(**p != 0xff)
		return TRUE;
	else {
		*p = NULL;
		return FALSE;
	}
}

/*
	���̍s�̍ŏ��̃R�[�h�Ɉړ�����
*/
int goNextLineCode(const uint8 **p)
{
	int line_no, size;

	if(!goNextLine(p))
		return FALSE;

	*p += decodeLineNo(&line_no, &size, *p);
	skipBlank(p);
	fetchLabel(p);
	return TRUE;
}

/*
	DATA�̓ǂݍ��݈ʒu��ݒ肷��
*/
static const uint8 *restoreData(const uint8 *p)
{
	int len, line_no, size;

	if((len = decodeLineNo(&line_no, &size, p)) <= 0)
		return NULL;
	p += len;

	for(;;) {
		if(p == NULL)
			return NULL;
		else if(fetchKeyword(&p, CODE_DATA))
			return p;

		goNext(&p);
	}
}

/*
	�t���[����X�^�b�N�̐擪��i�߂�
*/
static int pushFlow(struct Basic *bas, void **top, int kind)
{
	if(++bas->top >= &bas->stack[sizeof(bas->stack) / sizeof(bas->stack[0])])
		return ERR_54;

	/*
	printf("%*cPUSH LINE=%d KIND=%d DEPTH=%d\n", (int )(bas->top - bas->stack), ' ', bas->line_no, kind, (int )(bas->top - bas->stack));
	*/

	bas->top->kind = kind;
	*top = bas->top;
	return ERR_OK;
}

/*
	�t���[����X�^�b�N�̐擪�𓾂�
*/
static int peekFlow(struct Basic *bas, void **top, int kind)
{
	if(bas->top < bas->stack || bas->top->kind != kind) {
		if(kind == CODE_GOSUB)
			return ERR_51;
		else if(kind == CODE_FOR)
			return ERR_52;
		else if(kind == CODE_REPEAT)
			return ERR_62;
		else if(kind == CODE_WHILE)
			return ERR_64;
		else if(kind == CODE_SWITCH)
			return ERR_69;
		else
			return ERR_10;
	}

	if(top != NULL)
		*top = bas->top;
	return ERR_OK;
}

/*
	�t���[����X�^�b�N�̐擪��߂�
*/
static int popFlow(struct Basic *bas, void **top, int kind)
{
	int err;

	/*
	printf("%*cPOP LINE=%d KIND=%d DEPTH=%d\n", (int )(bas->top - bas->stack), ' ', bas->line_no, kind, (int )(bas->top - bas->stack));
	*/

	while((err = peekFlow(bas, top, kind)) < 0) {
		if(kind != CODE_GOSUB)
			return err;
		if(bas->top <= bas->stack)
			return err;
		bas->top--;
	}
	bas->top--;
	return ERR_OK;
}

static int staAuto(struct Basic *, const uint8 **);
static int staBeep(struct Basic *, const uint8 **);
static int staBload(struct Basic *, const uint8 **);
static int staBsave(struct Basic *, const uint8 **);
static int staCall(struct Basic *, const uint8 **);
static int staCase(struct Basic *, const uint8 **);
static int staCircle(struct Basic *, const uint8 **);
static int staClear(struct Basic *, const uint8 **);
static int staClose(struct Basic *, const uint8 **);
static int staCls(struct Basic *, const uint8 **);
static int staCont(struct Basic *, const uint8 **);
static int staData(struct Basic *, const uint8 **);
static int staDefault(struct Basic *, const uint8 **);
static int staDegree(struct Basic *, const uint8 **);
static int staDelete(struct Basic *, const uint8 **);
static int staDim(struct Basic *, const uint8 **);
static int staElse(struct Basic *, const uint8 **);
static int staEnd(struct Basic *, const uint8 **);
static int staEndif(struct Basic *, const uint8 **);
static int staEndswitch(struct Basic *, const uint8 **);
static int staErase(struct Basic *, const uint8 **);
static int staFiles(struct Basic *, const uint8 **);
static int staFor(struct Basic *, const uint8 **);
static int staGcursor(struct Basic *, const uint8 **);
static int staGosub(struct Basic *, const uint8 **);
static int staGoto(struct Basic *, const uint8 **);
static int staGprint(struct Basic *, const uint8 **);
static int staGrad(struct Basic *, const uint8 **);
static int staHdcopy(struct Basic *, const uint8 **);
static int staIf(struct Basic *, const uint8 **);
static int staInput(struct Basic *, const uint8 **);
static int staKill(struct Basic *, const uint8 **);
static int staLcopy(struct Basic *, const uint8 **);
static int staLet(struct Basic *, const uint8 **);
static int staLfiles(struct Basic *, const uint8 **);
static int staLine(struct Basic *, const uint8 **);
static int staList(struct Basic *, const uint8 **);
static int staLlist(struct Basic *, const uint8 **);
static int staLninput(struct Basic *, const uint8 **);
static int staLoad(struct Basic *, const uint8 **);
static int staLocate(struct Basic *, const uint8 **);
static int staLprint(struct Basic *, const uint8 **);
static int staNew(struct Basic *, const uint8 **);
static int staNext(struct Basic *, const uint8 **);
static int staOn(struct Basic *, const uint8 **);
static int staOpen(struct Basic *, const uint8 **);
static int staOut(struct Basic *, const uint8 **);
static int staPaint(struct Basic *, const uint8 **);
static int staPass(struct Basic *, const uint8 **);
static int staPioput(struct Basic *, const uint8 **);
static int staPioset(struct Basic *, const uint8 **);
static int staPoke(struct Basic *, const uint8 **);
static int staPreset(struct Basic *, const uint8 **);
static int staPrint(struct Basic *, const uint8 **);
static int staPset(struct Basic *, const uint8 **);
static int staRadian(struct Basic *, const uint8 **);
static int staRandomize(struct Basic *, const uint8 **);
static int staRead(struct Basic *, const uint8 **);
static int staRem(struct Basic *, const uint8 **);
static int staRenum(struct Basic *, const uint8 **);
static int staRepeat(struct Basic *, const uint8 **);
static int staRestore(struct Basic *, const uint8 **);
static int staReturn(struct Basic *, const uint8 **);
static int staRun(struct Basic *, const uint8 **);
static int staSave(struct Basic *, const uint8 **);
static int staSpinp(struct Basic *, const uint8 **);
static int staSpout(struct Basic *, const uint8 **);
static int staStop(struct Basic *, const uint8 **);
static int staSwitch(struct Basic *, const uint8 **);
static int staTroff(struct Basic *, const uint8 **);
static int staTron(struct Basic *, const uint8 **);
static int staUntil(struct Basic *, const uint8 **);
static int staWait(struct Basic *, const uint8 **);
static int staWend(struct Basic *, const uint8 **);
static int staWhile(struct Basic *, const uint8 **);

/*
	AUTO
*/
static int staAuto(struct Basic *bas, const uint8 **p)
{
	int start = -1, step = -1;

	/* �J�n�ԍ��𓾂� */
	if(isLineNo(p))
		if((start = fetchLineNo(p)) < 0)
			return ERR_10;

	/* �����𓾂� */
	if(fetchComma(p))
		if((step = fetchLineNo(p)) < 0)
			return ERR_10;
	if(!isTerm(p))
		return ERR_10;

	if(start < 0)
		bas->auto_line_no = 10;
	else
		bas->auto_line_no = start;
	if(step < 0)
		bas->auto_step = 10;
	else
		bas->auto_step = step;
	return ERR_AUTO;
}

/*
	�u�U�[�����o�͂��� (staBeep�̉�����)
*/
static void buzz(int hz)
{
	int pos, len;

	if(buzzer == BUZZER_NONE)
		return;

	if(hz <= 0)
		memset(soundWriteBuffer, 0, soundBufferSize);
	else {
		len = FREQ_SOUND / hz;

		for(pos = 0; pos < soundBufferSize - len; pos += len) {
			memset(soundWriteBuffer + pos, 0x5f, len);
			memset(soundWriteBuffer + pos + len / 2, 0, soundBufferSize - pos - len / 2);
		}
	}

	flipSoundBuffer();
}

/*
	BEEP
*/
static int staBeep(struct Basic *bas, const uint8 **p)
{
	int err, count = 1, freq = 7, len = 2000, hz, i;

	if((err = fetchNum_i(bas, &count, p)) < 0)
		return err;
	if(count < 0)
		return ERR_33;

	if(fetchComma(p)) {
		if((err = fetchNumOrEmpty_i(bas, &freq, p)) < 0)
			return err;
		if(freq < 0)
			return ERR_33;

		if(fetchComma(p)) {
			if((err = fetchNum_i(bas, &len, p)) < 0)
				return err;
			if(len < 0)
				return ERR_33;
		}
	}

	if(!isTerm(p))
		return ERR_10;

	hz = 1300000 / (166 + 22 * freq);

	while(count-- > 0)
		for(i = 0; i < len; i++) {
			buzz(hz);
			ssleep(freqCPU / hz);
		}

	buzz(0);
	return ERR_OK_NEXT;
}

/*
	BLOAD/CLOAD
*/
static int staBload(struct Basic *bas, const uint8 **p)
{
	int err, mode, address = 0, begin;
	uint8 *filename;

	if(fetchSymbol(p, "M")) {
		mode = 1;

		if(fetchNum_i(bas, &address, p) < 0)
			address = -1;
	} else if(fetchSymbol(p, "?"))
		mode = 2;
	else
		mode = 0;

	if(isTerm(p))
		filename = (uint8 *)pathSioIn;
	else {
		if(mode > 0)
			if(!fetchComma(p))
				return ERR_10;
		if((err = fetchStr(bas, &filename, p)) < 0)
			return err;
		if(!isTerm(p))
			return ERR_10;
	}

	switch(mode) {
	case 0: /* BASIC */
		if(!inportBas((char *)filename))
			return ERR_80;
		if(!loadBas(bas))
			return ERR_80;
		break;
	case 1: /* �}�V���� */
		if(address < 0) {
			if(readHex((char *)filename, memory, &begin, 0x8000, FALSE) == 0)
				return ERR_80;
		} else {
			if(readHex((char *)filename, &memory[address], &begin, 0x8000 - address, FALSE) == 0)
				return ERR_80;
		}
		storeRAM(pathRAM);
		break;
	case 2: /*��r */
		if(!cmpFile((char *)filename, pathBasic))
			return ERR_82;
		break;
	}
	return ERR_OK_NEXT;
}

/*
	BSAVE/CSAVE
*/
static int staBsave(struct Basic *bas, const uint8 **p)
{
	int err, mode, start = 0, end = 0;
	uint8 *filename;

	if(fetchSymbol(p, "M")) {
		mode = 1;

		if((err = fetchNum_i(bas, &start, p)) < 0)
			return err;
		if(start < 0 || start > 0xffff)
			return ERR_33;
		if(!fetchComma(p))
			return ERR_10;
		if((err = fetchNum_i(bas, &end, p)) < 0)
			return err;
		if(end < 0 || end > 0xffff)
			return ERR_33;
		if(start > end)
			return ERR_15;
	} else
		mode = 0;

	if(isTerm(p))
		filename = (uint8 *)pathSioOut;
	else {
		if(mode > 0)
			if(!fetchComma(p))
				return ERR_10;
		if((err = fetchStr(bas, &filename, p)) < 0)
			return err;
		if(!isTerm(p))
			return ERR_10;
	}

	switch(mode) {
	case 0: /* BASIC */
		if(!exportBas((char *)filename))
			return ERR_80;
		break;
	case 1: /* �}�V���� */
		if(writeHex((char *)filename, memory, start, end - start + 1) <= 0)
			return ERR_80;
		break;
	}
	return ERR_OK_NEXT;
}

/*
	CALL
*/
static int staCall(struct Basic *bas, const uint8 **p)
{
	int err, address;

	if((err = fetchNum_i(bas, &address, p)) < 0)
		return err;
	if(address < 0 || address > 0xffff)
		return ERR_33;

	ssleep(46013);

	z80.i.states = 0;
	z80.r16.af = 0x0044;
	z80.r16.bc = 0x0000;
	z80.r16.de = 0x0000;
	z80.r16.hl = 0x0100;
	z80.r16.ix = 0x7c05;
	z80.r16.iy = 0x7c03;
	z80.r16.sp = 0x7ff6;
	z80.r16.pc = address;
	if(address < 0x8000)
		exec(TRUE);
	else
		z80subroutine(&z80, address);
	return ERR_OK_NEXT;
}

/*
	CASE(�u���b�N�\��)
*/
static int staCase(struct Basic *bas, const uint8 **p)
{
	struct SwitchCase *switch_case;
	int err;

	if((err = popFlow(bas, (void **)&switch_case, CODE_SWITCH)) < 0)
		return err;

	for(;;) {
		if(fetchKeyword(p, CODE_ENDSWITCH))
			return ERR_OK_NEXT;
		if(!goNextLineCode(p))
			return ERR_OK_JUMP;
	}
}

/* �h��Ԃ��̂��߂̈ꎞ�̈� */
static uint8 dot[6][144];

/*
	�ꎞ�̈���������� (������)
*/
static void cleardot(void)
{
	memset(dot, 0, sizeof(dot));
}

/*
	�ꎞ�̈�ɓ_��`�� (paint�̉�����)
*/
static void putdot(uint16 x, uint16 y, uint8 mode)
{
	if(x < 0 || x >= lcdWidth || y < 0 || y >= lcdHeight)
		return;

	dot[y / 8][x] |= (1 << (y % 8));
}

/*
	�ꎞ�̈�ɓ_�����邪���ׂ� (paint�̉�����)
*/
static int getdot(uint16 x, uint16 y)
{
	if(x < 0 || x >= lcdWidth || y < 0 || y >= lcdHeight)
		return 0;

	return dot[y / 8][x] & (1 << (y % 8));
}

/*
	VRAM�ɓ_�����邪���ׂ� (paint�̉�����)
*/
static int getdot_vram(int16 x, int16 y)
{
	return point(x, y) & (1 << (y % 8));
}

/*
	1�s�h��Ԃ� (paint�̉�����)
*/
static int paint_line(int16 x, int16 y)
{
	int i, n = 0;

	if(getdot(x, y) || getdot_vram(x, y))
		return FALSE;

	for(i = x; i < lcdWidth && !getdot_vram(i, y); i++, n++)
		putdot(i, y, 1);
	for(i = x - 1; i >= 0 && !getdot_vram(i, y); i--, n++)
		putdot(i, y, 1);
	return TRUE;
}

/*
	�ꎞ�̈悩��VRAM�ɓ]������ (staCircle, staPaint�̉�����)
*/
static int draw(uint8 pat, uint8 mode)
{
	int i, j, n = 0;

	for(j = 0; j < lcdHeight; j++)
		for(i = 0; i < lcdWidth; i++)
			if(getdot(i, j)) {
				n++;

				if(
				(pat == 1 && j % 2 == 0) ||
				(pat == 2 && i % 2 == 0) ||
				(pat == 3 && (i + j) % 4 == 0) ||
				(pat == 4 && (i - j) % 4 == 0) ||
				(pat == 5 && (i + j) % 2 == 0) ||
				(pat == 6)
				)
					pset(i, j, mode);
			}

	return n;
}

#define SQU(x)	((double )(x) * (double )(x))

/*
	CIRCLE
*/
static int staCircle(struct Basic *bas, const uint8 **p)
{
	double a1 = .0, a2 = 360.0, ratio = 1.0, a, y;
	int err, x0, y0, r, l1 = FALSE, l2 = FALSE, pat = 0, n = 0, x, z, d, outline;
	uint8 mode = 1;

	if(!fetchSymbol(p, "("))
		return ERR_10;
	if((err = fetchNum_i(bas, &x0, p)) < 0)
		return err;
	if(x0 < -32768 || x0 > 32767)
		return ERR_33;
	if(!fetchSymbol(p, ","))
		return ERR_10;
	if((err = fetchNum_i(bas, &y0, p)) < 0)
		return err;
	if(y0 < -32768 || y0 > 32767)
		return ERR_33;
	if(!fetchSymbol(p, ")"))
		return ERR_10;
	if(!fetchSymbol(p, ","))
		return ERR_10;
	if((err = fetchNum_i(bas, &r, p)) < 0)
		return err;
	if(r < 1 || r > 32727)
		return ERR_33;

	if(fetchSymbol(p, ",")) {
		if((err = fetchNumOrEmpty_f(bas, &a1, p)) < 0)
			return err;
		if(a1 < .0) {
			l1 = TRUE;
			a1 = -a1;
		}
		if(a1 > 360.0)
			a1 -= ((int )a1 / 360) * 360.0;

		if(fetchSymbol(p, ",")) {
			if((err = fetchNumOrEmpty_f(bas, &a2, p)) < 0)
				return err;
			if(a2 < .0) {
				l2 = TRUE;
				a2 = -a2;
			}
			if(a2 > 360.0)
				a2 -= ((int )a2 / 360) * 360.0;

			if(fetchSymbol(p, ",")) {
				if((err = fetchNumOrEmpty_f(bas, &ratio, p)) < 0)
					return err;
				if(ratio <= 0.0)
					return ERR_33;

				if(fetchSymbol(p, ",")) {
					if(fetchSymbol(p, "R"))
						mode = 0;
					else if(fetchSymbol(p, "S"))
						mode = 1;
					else if(fetchSymbol(p, "X"))
						mode = 2;

					if(fetchSymbol(p, ",")) {
						if((err = fetchNum_i(bas, &pat, p)) < 0)
							return err;
						if(pat < 0 || pat > 8)
							return ERR_33;
					}
				}
			}
		}
	}

	if(!isTerm(p))
		return ERR_10;

	cleardot();

	for(z = 0; z <= lcdHeight * ratio; z++)
		for(x = lcdWidth - 1; x >= 0; x--) {
			y = (double )z / ratio;
			a = (atan2(y, x) * 180.0 + M_PI / 2.0) / M_PI;
			d = r * r + r + 1;

			if(SQU(x) + SQU(y) <= d) {
				outline = SQU(x + 1) + SQU(y) > d || SQU(x) + SQU(y + 1) > d;

				if(a1 <= a && a <= a2) {
					if(outline)
						pset(x0 + x, y0 - z, mode);
					else
						putdot(x0 + x, y0 - z, 1);
				}
				if(a1 <= 180 - a && 180 - a <= a2) {
					if(outline)
						pset(x0 - x, y0 - z, mode);
					else
						putdot(x0 - x, y0 - z, 1);
				}
				if(a1 <= 180 + a && 180 + a <= a2) {
					if(outline)
						pset(x0 - x, y0 + z, mode);
					else
						putdot(x0 - x, y0 + z, 1);
				}
				if(a1 <= 360 - a && 360 - a <= a2) {
					if(outline)
						pset(x0 + x, y0 + z, mode);
					else
						putdot(x0 + x, y0 + z, 1);
				}
			}
		}

	n = draw(pat, mode);
	if(l1)
		line(x0, y0, x0 + r * cos(a1 * M_PI / 180.0), y0 + r * sin(a1 * M_PI / 180.0) * ratio, mode, 0xffff);
	if(l2)
		line(x0, y0, x0 + r * cos(a2 * M_PI / 180.0), y0 + r * sin(a2 * M_PI / 180.0) * ratio, mode, 0xffff);

	ssleep(3146886);
	ssleep(r * 207999);
	ssleep(n * 3874);
	return ERR_OK_NEXT;
}

/*
	CLEAR
*/
static int staClear(struct Basic *bas, const uint8 **p)
{
	if(!isTerm(p))
		return ERR_10;

	ssleep(10000); /* ??? */

	clearAllVars(bas);
	return ERR_OK_NEXT;
}

/*
	CLOSE
*/
static int staClose(struct Basic *bas, const uint8 **p)
{
	int err, fileno;

	do {
		if(!fetchSymbol(p, "#"))
			return ERR_10;
		if((err = fetchNum_i(bas, &fileno, p)) < 0)
			return err;
		if(fileno <= 0 || fileno > sizeof(bas->fp) / sizeof(bas->fp[0]))
			return ERR_10;
		if(bas->fp[fileno] == NULL)
			return ERR_10;

		fclose(bas->fp[fileno]);
		bas->fp[fileno] = NULL;
	} while(fetchSymbol(p, ","));

	if(!isTerm(p))
		return ERR_10;

	return ERR_OK_NEXT;
}

/*
	CLS
*/
static int staCls(struct Basic *bas, const uint8 **p)
{
	if(!isTerm(p))
		return ERR_10;

	ssleep(191112);

	gcls();
	return ERR_OK_NEXT;
}

/*
	CONT
*/
static int staCont(struct Basic *bas, const uint8 **p)
{
	if(!isTerm(p))
		return ERR_10;
	if(bas->p == NULL)
		return ERR_13;

	ssleep(1000000);

	/*bas->cont = TRUE;*/
	return ERR_OK_CONT;
}

/*
	DATA
*/
static int staData(struct Basic *bas, const uint8 **p)
{
	while(!isTerm(p))
		goNext(p);

	ssleep(1125);

	return ERR_OK_NEXT;
}

/*
	DEFAULT(�u���b�N�\��)
*/
static int staDefault(struct Basic *bas, const uint8 **p)
{
	return staCase(bas, p);
}

/*
	DEGREE
*/
static int staDegree(struct Basic *bas, const uint8 **p)
{
	if(!isTerm(p))
		return ERR_10;

	ssleep(4999);

	*angle = ANGLE_DEGREE;
	return ERR_OK_NEXT;
}

/*
	DELETE
*/
static int staDelete(struct Basic *bas, const uint8 **p)
{
	int start, end, line_no;
	uint8 buf[256];

	/* �J�n�s�ԍ��𓾂� */
	if(isLineNo(p)) {
		if((start = fetchLineNo(p)) < 0 && start != -12)
			return start;
	} else
		start = 1;

	/* �I���s�ԍ��𓾂� */
	if(fetchSymbol(p, "-")) {
		if(isLineNo(p)) {
			if((end = fetchLineNo(p)) < 0)
				return end;
		} else
			end = 0xff00;
	} else
		end = start;

	if(!isTerm(p))
		return ERR_10;
	if(start > end)
		return ERR_44;

	for(line_no = start; line_no <= end; line_no++) {
		sprintf((char *)buf, "%d", line_no);
		insertProg(bas, buf, NULL, NULL);
	}
	saveBas(bas);
	return ERR_OK;
}

/*
	DIM
*/
static int staDim(struct Basic *bas, const uint8 **p)
{
	int err, len, kind, type, dims, max0, max1, size;
	uint8 *var;
	const uint8 *name;

	ssleep(631); /* ??? */

	do {
		ssleep(7349); /* ??? */

		/* �ϐ����𓾂� */
		name = *p;
		if((len = getVarType(*p, &kind, &type)) == 0)
			return ERR_10;
		*p += len;

		/* �m�ۍς݂�? */
		if(findVar(bas, &var, NULL, name, KIND_ARRAY, type) == 0)
			return ERR_30;

		/* �v�f���𓾂� */
		if(!fetchLParen(p))
			return ERR_10;

		dims = max0 = max1 = size = 0;
		do {
			if(dims == 0) {
				if((err = fetchNum_i(bas, &max0, p)) < 0)
					return err;
				if(max0 < 0 || max0 > 255)
					return ERR_33;
			} else if(dims == 1) {
				if((err = fetchNum_i(bas, &max1, p)) < 0)
					return err;
				if(max1 < 0 || max0 > 255)
					return ERR_33;
			} else
				return ERR_10;
			dims++;
		} while(fetchComma(p));

		if(!fetchRParen(p))
			return ERR_10;

		/* ������̒����𓾂� */
		if(fetchSymbol(p, "*")) {
			if(type != TYPE_STR)
				return ERR_10;
			if((err = fetchNum_i(bas, &size, p)) < 0)
				return err;
			if(size < 0 || size > 255)
				return ERR_33;
		} else
			size = 16;

		/* �z��ϐ����m�ۂ��� */
		if((err = allocVar(bas, &var, name, KIND_ARRAY, type, max0, max1, size)) < 0)
			return err;
	} while(fetchComma(p));

	if(!isTerm(p))
		return ERR_10;
	return ERR_OK_NEXT;
}

/*
	ELSE
*/
static int staElse(struct Basic *bas, const uint8 **p)
{
	goNextLine(p);
	return ERR_OK_JUMP;
}

/*
	ELSE(�u���b�N�\��)
*/
static int staBlockElse(struct Basic *bas, const uint8 **p)
{
	int depth = 0;

	if(!isTerm(p))
		return ERR_10;

	for(;;) {
		if(!goNextLineCode(p))
			return ERR_61;

		if(fetchKeyword(p, CODE_IF)) {
			for(;;) {
				if(isLineTerm(p))
					break;
				if(fetchKeyword(p, CODE_THEN)) {
					if(isLineTerm(p))
						depth++;
					break;
				}

				goNext(p);
			}
		} else if(fetchKeyword(p, CODE_ENDIF)) {
			if(depth == 0)
				return ERR_OK_NEXT;
			depth--;
		}
	}
}

/*
	END
*/
static int staEnd(struct Basic *bas, const uint8 **p)
{
	if(!isTerm(p))
		return ERR_10;

	/**p = NULL;*/
	return ERR_END;
}

/*
	ENDIF(�u���b�N�\��)
*/
static int staEndif(struct Basic *bas, const uint8 **p)
{
	if(!isTerm(p))
		return ERR_10;
	return ERR_OK_NEXT;
}

/*
	ENDSWITCH(�u���b�N�\��)
*/
static int staEndswitch(struct Basic *bas, const uint8 **p)
{
	struct SwitchCase *switch_case;
	int err;

	if(!isTerm(p))
		return ERR_10;

	if((err = popFlow(bas, (void **)&switch_case, CODE_SWITCH)) < 0)
		return err;
	return ERR_OK_NEXT;
}

/*
	ERASE
*/
static int staErase(struct Basic *bas, const uint8 **p)
{
	int len, kind, type;
	const uint8 *name;

	ssleep(631); /* ??? */

	do {
		ssleep(7349); /* ??? */

		name = *p;
		if((len = getVarType(*p, &kind, &type)) > 0)
			freeVar(bas, name, KIND_ARRAY, type);

		*p += len;
	} while(fetchComma(p));

	if(!isTerm(p))
		return ERR_10;
	return ERR_OK_NEXT;
}

/*
	FILES
*/
static int staFiles(struct Basic *bas, const uint8 **p)
{
	char path[MAX_PATH] = "";
	uint8 ch;

	if(!isTerm(p))
		return ERR_10;

	pushVram();
	ch = selectFile(path);
	popVram();

	if(ch == '\r' && strcmp(path, "") != 0) {
		if(!inportBas(path))
			return ERR_94;
		if(!loadBas(bas))
			return ERR_94;
	}
	return staEnd(bas, p);
}

/*
	FOR
*/
static int staFor(struct Basic *bas, const uint8 **p)
{
	struct ForLoop *for_loop;
	int err, kind, type, size;
	uint8 *val, *num;

	/* �t���[����X�^�b�N�̐擪��i�߂� */
	if((err = pushFlow(bas, (void **)&for_loop, CODE_FOR)) < 0)
		return err;

	/* �ϐ����X�^�b�N�ɐς� */
	for_loop->var = *p;

	if((err = fetchVarVal(bas, &val, &kind, &type, &size, p)) < 0)
		return err;
	if(!fetchSymbol(p, "="))
		return ERR_10;
	if(type != TYPE_NUM)
		return ERR_10;
	if((err = fetchNum(bas, &num, p)) < 0)
		return err;
	if((err = setVarVal(val, kind, type, size, num)) < 0)
		return err;

	/* TO�̒l���X�^�b�N�ɐς� */
	if(!fetchKeyword(p, CODE_TO))
		return ERR_10;
	if((err = fetchNum(bas, &num, p)) < 0)
		return err;
	numLet(for_loop->to, num);

	/* STEP�̒l���X�^�b�N�ɐς� */
	if(!fetchKeyword(p, CODE_STEP))
		numLet(for_loop->step, NUM_1);
	else {
		if((err = fetchNum(bas, &num, p)) < 0)
			return err;
		numLet(for_loop->step, num);
	}
	if(!isTerm(p))
		return ERR_10;

	/* �߂�ʒu���X�^�b�N�ɐς� */
	for_loop->line_no = bas->line_no;
	for_loop->ret = *p;
	return ERR_OK_NEXT;
}

/*
	GCURSOR
*/
static int staGcursor(struct Basic *bas, const uint8 **p)
{
	int err, x, y;

	if(!fetchSymbol(p, "("))
		return ERR_10;
	if((err = fetchNum_i(bas, &x, p)) < 0)
		return err;
	if(!fetchSymbol(p, ","))
		return ERR_10;
	if((err = fetchNum_i(bas, &y, p)) < 0)
		return err;
	if(!fetchSymbol(p, ")"))
		return ERR_10;
	if(!isTerm(p))
		return ERR_10;

	if(x < -32768 || x > 65535 || y < -32768 || y > 65535)
		return ERR_33;

	ssleep(10856);

	z80write16(&z80, 0x79db, x);
	z80write16(&z80, 0x79dd, y);
	return ERR_OK_NEXT;
}

/*
	GOSUB
*/
static int staGosub(struct Basic *bas, const uint8 **p)
{
	struct GosubReturn *gosub_return;
	int err;
	const uint8 *p_next;

	if((err = findLine(p, &p_next, bas->prog)) < 0)
		return err;
	if(!isTerm(p))
		return ERR_10;

	ssleep(3409); /* ??? */

	if((err = pushFlow(bas, (void **)&gosub_return, CODE_GOSUB)) < 0)
		return err;
	gosub_return->line_no = bas->line_no;
	gosub_return->ret = *p;

	*p = p_next;
	return ERR_OK_JUMP;
}

/*
	GOTO
*/
static int staGoto(struct Basic *bas, const uint8 **p)
{
	int err;
	const uint8 *p_next;

	if((err = findLine(p, &p_next, bas->prog)) < 0)
		return err;
	if(!isTerm(p))
		return ERR_10;

	ssleep(3218);

	*p = p_next;
	return ERR_OK_JUMP;
}

/*
*/
static void pause(struct Basic *bas)
{
	if(*pauseWhenPrint & 0x04)
		while(getKeycode() != GKEY_RETURN)
			;
	else {
		int i, wait = *waitTimeH * 0x100 + *waitTimeL;

		for(i = 0; i < wait; i++)
			ssleep(freqCPU / 64);
	}
}

/*
	�p�^�[����`�� (staGprint�̉�����)
*/
static void gprint1(struct Basic *bas, uint16 x, uint16 y, uint8 pat)
{
	pset(x, y - 7, pat & 0x01 ? 1: 0);
	pset(x, y - 6, pat & 0x02 ? 1: 0);
	pset(x, y - 5, pat & 0x04 ? 1: 0);
	pset(x, y - 4, pat & 0x08 ? 1: 0);
	pset(x, y - 3, pat & 0x10 ? 1: 0);
	pset(x, y - 2, pat & 0x20 ? 1: 0);
	pset(x, y - 1, pat & 0x40 ? 1: 0);
	pset(x, y - 0, pat & 0x80 ? 1: 0);
}

/*
	GPRINT
*/
static int staGprint(struct Basic *bas, const uint8 **p)
{
	int err, type, pat;
	uint16 x = z80read16(&z80, 0x79db), y = z80read16(&z80, 0x79dd);
	uint8 *num_or_str;

	ssleep(4409);

	while(!isTerm(p)) {
		if((err = fetchParam(bas, &num_or_str, &type, p)) < 0)
			return err;

		if(type == TYPE_NUM) {
			if((err = decodeNum_i(&pat, num_or_str)) < 0)
				return err;
			if(pat < 0 || pat > 0xff)
				return ERR_33;

			ssleep(1043);
			gprint1(bas, x++, y, pat);
		} else if(type == TYPE_STR) {
			uint8 *q;

			for(q = num_or_str; *q != 0 && *(q + 1) != 0; q += 2) {
				if(!isxdigit(*q) || !isxdigit(*(q + 1)))
					return ERR_10;
				sscanf((const char *)q, "%2X", &pat);

				ssleep(1043);
				gprint1(bas, x++, y, pat);
			}
		} else
			return ERR_10;

		if(isTerm(p)) {
			z80write16(&z80, 0x79db, 0);
			break;
		} if(fetchSymbol(p, ";"))
			z80write16(&z80, 0x79db, x);
		else
			return ERR_10;
	}

	pause(bas);
	return ERR_OK_NEXT;
}

/*
	GRAD
*/
static int staGrad(struct Basic *bas, const uint8 **p)
{
	if(!isTerm(p))
		return ERR_10;

	ssleep(5011);

	*angle = ANGLE_GRAD;
	return ERR_OK_NEXT;
}

/*
	HDCOPY
*/
static int staHdcopy(struct Basic *bas, const uint8 **p)
{
	if(!isTerm(p))
		return ERR_10;

	ssleep(4467);

	return ERR_OK_NEXT;
}

/*
	�����s��ELSE�܂ňړ����� (staIf, staBlockIf�̉�����)
*/
static int goElse(struct Basic *bas, const uint8 **p)
{
	int depth = 1;

	for(;;) {
		if(isLineTerm(p))
			return ERR_OK_NEXT;
		else if(fetchKeyword(p, CODE_ELSE)) {
			depth--;

			if(depth > 0)
				;
			else if(isLineNo(p) || isLabel(p)) {
				return staGoto(bas, p);
			} else
				return ERR_OK_NEXT;
		} else if(fetchKeyword(p, CODE_IF))
			depth++;
		else
			goNext(p);
	}
}

/*
	IF
*/
static int staIf(struct Basic *bas, const uint8 **p)
{
	int err, type, zero;
	uint8 *bool;

	if((err = fetchParam(bas, &bool, &type, p)) < 0)
		return err;
	if(type == TYPE_NUM)
		zero = numIsZero(bool);
	else if(type == TYPE_STR)
		zero = strlen(bool) == 0;
	else
		return ERR_10;

	if(!zero) {
		ssleep(8565);

		if(!fetchKeyword(p, CODE_THEN))
			return ERR_OK_NEXT;
		if(isLineNo(p) || isLabel(p))
			return staGoto(bas, p);
		return ERR_OK_NEXT;
	} else {
		ssleep(6710);

		return goElse(bas, p);
	}
}

/*
	IF(�u���b�N�\��)
*/
static int staBlockIf(struct Basic *bas, const uint8 **p)
{
	int err, type, zero;
	uint8 *bool;

	if((err = fetchParam(bas, &bool, &type, p)) < 0)
		return err;
	if(type == TYPE_NUM)
		zero = numIsZero(bool);
	else if(type == TYPE_STR)
		zero = strlen(bool) == 0;
	else
		return ERR_10;

	if(!zero) { /* �^ */
		ssleep(13163);

		if(!fetchKeyword(p, CODE_THEN)) /* THEN���Ȃ� */
			return ERR_OK_NEXT; /* ���̃X�e�[�g�����g�����s */
		if(isLineNo(p) || isLabel(p)) /* �s�ԍ��܂��̓��x�� */
			return staGoto(bas, p); /* �����ֈړ����� */
		return ERR_OK_NEXT;
	} else { /* �U */
		int depth = 0;

		ssleep(12814);

		if(!fetchKeyword(p, CODE_THEN)) /* THEN���Ȃ� */
			return goElse(bas, p); /* �����s��ELSE�ɃW�����v����(�ʏ��IF) */
		if(!isLineTerm(p)) /* �s���łȂ� */
			return goElse(bas, p); /* �����s��ELSE�ɃW�����v����(�ʏ��IF) */

		/* ELSE�܂���ENDIF�̍s�܂ňړ����� */
		for(;;) {
			if(!goNextLineCode(p))
				return ERR_OK_JUMP;

			if(fetchKeyword(p, CODE_IF))
				for(;;) {
					if(isLineTerm(p))
						break;
					if(fetchKeyword(p, CODE_THEN)) {
						if(isLineTerm(p))
							depth++;
						break;
					}

					goNext(p);
				}
			else if(fetchKeyword(p, CODE_ELSE)) {
				if(depth == 0)
					return ERR_OK_NEXT;
			} else if(fetchKeyword(p, CODE_ENDIF)) {
				if(depth == 0)
					return ERR_OK_NEXT;
				depth--;
			}
		}
	}
}

/*
*/
static int fetchArrayWild(struct Basic *bas, uint8 **var, int *type, const uint8 **p)
{
	int err, len, kind;
	const uint8 *q;

	if((len = getVarType(*p, &kind, type)) <= 0)
		return FALSE;
	if(kind != KIND_ARRAY)
		return FALSE;

	q = *p + len;

	if(!fetchSymbol(&q, "("))
		return FALSE;
	if(!fetchSymbol(&q, "*"))
		return FALSE;
	if(!fetchSymbol(&q, ")"))
		return FALSE;

	if((err = findVar(bas, var, NULL, *p, kind, *type)) < 0)
		return err;

	*p = q;
	return TRUE;
}

/*
	�t�@�C������l��ǂݍ���
*/
static int fetchValFromFile(uint8 *val, int type, FILE *fp)
{
	int ch, in_quote = FALSE;
	char buf[256 + 1], *p = buf;

	/* �ŏ��̋󔒂�ǂݔ�΂� */
	for(;;) {
		if((ch = getc(fp)) < 0)
			break;
		else if(ch != ' ' && ch != '\t') {
			ungetc(ch, fp);
			break;
		}
	}
	if(ch < 0)
		return ERR_85;

	/* �l�𓾂� */
	for(;;) {
		if((ch = getc(fp)) < 0)
			break;
		else if(ch == '\n' || ch == '\r')
			break;
		else if(ch == ' ' || ch == '\t' || ch == ',') {
			if(!in_quote)
				break;
		} else if(ch == '\"') {
			if(p == buf) {
				in_quote = TRUE;
				continue;
			} else if(in_quote) {
				/* ,�̌�̋󔒂�ǂݔ�΂� */
				for(;;) {
					if((ch = getc(fp)) < 0)
						break;
					else if(ch == ',' || ch == '\n' || ch == '\r')
						break;
					else if(ch != ' ' && ch != '\t') {
						ungetc(ch, fp);
						break;
					}
				}
				break;
			}
		}

		if(p >= &buf[256]) {
			ungetc(ch, fp);
			break;
		}
		*p++ = ch;
	}
	*p = 0;

	/* ��̋󔒂�ǂݔ�΂� */
	if(ch >= 0)
		for(;;) {
			if((ch = getc(fp)) < 0)
				break;
			else if(ch != ' ' && ch != '\t') {
				ungetc(ch, fp);
				break;
			}
		}

	/* �ϊ����� */
	if(type == TYPE_NUM) {
		if(encodeNum(val, (uint8 *)buf) < 0)
			numLet(val, NUM_0);
	} else if(type == TYPE_STR)
		strcpy(val, buf);
	else
		return ERR_10;

	return ERR_OK;
}

/*
	INPUT #
*/
static int staInputFile(struct Basic *bas, const uint8 **p)
{
	int err, fileno, type, size;
	uint8 *array, *val, num_or_str[256 + 1];

	if(!fetchSymbol(p, "#"))
		return ERR_10;
	if((err = fetchNum_i(bas, &fileno, p)) < 0)
		return err;
	if(fileno <= 1 || fileno > sizeof(bas->fp) / sizeof(bas->fp[0]))
		return ERR_10;
	if(bas->fp[fileno] == NULL)
		return ERR_85;

	for(;;) {
		if(isTerm(p))
			break;
		if(!fetchSymbol(p, ","))
			return ERR_10;

		if(fetchArrayWild(bas, &array, &type, p)) {
			int i, j;

			for(i = 0;; i++) {
				if(getArrayVal(&val, &type, &size, array, i, 0) < 0)
					break;
				if((err = fetchValFromFile(num_or_str, type, bas->fp[fileno])) < 0)
					return err;
				if((err = setVarVal(val, KIND_ARRAY, type, size, num_or_str)) < 0)
					return err;

				for(j = 1;; j++) {
					if(getArrayVal(&val, &type, &size, array, i, j) < 0)
						break;
					if((err = fetchValFromFile(num_or_str, type, bas->fp[fileno])) < 0)
						return err;
					if((err = setVarVal(val, KIND_ARRAY, type, size, num_or_str)) < 0)
						return err;
				}
			}

			if(fetchSymbol(p, ","))
				;
			else if(fetchSymbol(p, ";"))
				;
		} else {
			int kind;

			if((err = fetchVarVal(bas, &val, &kind, &type, &size, p)) < 0)
				return err;
			if((err = fetchValFromFile(num_or_str, type, bas->fp[fileno])) < 0)
				return err;
			if((err = setVarVal(val, kind, type, size, num_or_str)) < 0)
				return err;
		}
	}

	return ERR_OK_NEXT;
}

static int encodeProg(uint8 *, const uint8 *, int);

/*
	INPUT
*/
static int staInput(struct Basic *bas, const uint8 **p)
{
	int err, kind, type, size;
	uint8 k, buf[256], prog[256], *q, *val, *prompt, *num_or_str;

	if(peekSymbol(p, "#"))
		return staInputFile(bas, p);

	do {
		/* �v�����v�g��\������ */
		if(**p == '"') {
			if((err = fetchStr(bas, &prompt, p)) < 0)
				return err;

			if(!fetchComma(p)) {
				if(!fetchSymbol(p, ";"))
					return ERR_10;
				gprintf("%s", prompt);
				prompt = (uint8 *)"";
			}
		} else
			prompt = (uint8 *)"?";

		/* �ϐ��𓾂� */
		if((err = fetchVarVal(bas, &val, &kind, &type, &size, p)) < 0)
			return err;

		for(;;) {
			/* ���[�U�̓��͂𓾂� */
			waitRelease();

			for(;;) {
				if((k = ggetline(buf, prompt, GETLINE_RUN)) == 0x0d)
					break;
				else if(k == 0x05)
					return ERR_BREAK;
				else if(setMode(k))
					return ERR_END;
			}

			for(q = buf; *q != 0 && *q != '\r' && *q != '\n'; q++)
				;
			*q = 0;

			gprintf("%s\r", buf);

			/* �ϐ��ɑ������ */
			err = 0;

			if(type == TYPE_STR) {
				if(strcmp(buf, "") != 0)
					setVarVal(val, kind, type, size, (const uint8 *)buf);
			} else if(type == TYPE_NUM) {
				encodeProg(prog, buf, MODE_RUN);
				q = prog;
				skipBlank(&q);

				if((err = fetchParam(bas, &num_or_str, &type, &q)) < 0)
					;
				else if(type != TYPE_NUM)
					err = ERR_90;
 				else if((err = setVarVal(val, kind, type, size, num_or_str)) < 0)
 					;
 				else if(*q != 0 && *q != '\r' && *q != '\n')
 					err = ERR_90;
			}

			/* �G���[��? */
			if(err < 0) {
				glocate(0, *curRow);
				gprintf("ERROR %d\r", -err);
			} else
				break;
		}
	} while(fetchComma(p));

	if(!isTerm(p))
		return ERR_10;
	return ERR_OK_NEXT;
}

/*
	KILL
*/
static int staKill(struct Basic *bas, const uint8 **p)
{
	int err;
	uint8 *filename;

	if((err = fetchStr(bas, &filename, p)) < 0)
		return err;
	if(!isTerm(p))
		return ERR_10;

	if(unlink((char *)filename) < 0)
		return ERR_94;
	return ERR_OK_NEXT;
}

/*
	LCOPY
*/
static int staLcopy(struct Basic *bas, const uint8 **p)
{
	int start, end, to;

	if((start = fetchLineNo(p)) < 0)
		return start;
	if((end = fetchLineNo(p)) < 0)
		return end;
	if((to = fetchLineNo(p)) < 0)
		return to;
	if(!isTerm(p))
		return ERR_10;

	return ERR_OK_NEXT;
}

/*
	LET
*/
static int staLet(struct Basic *bas, const uint8 **p)
{
	int err, kind, type, size;
	uint8 *val, *num_or_str;

	do {
		if((err = fetchVarVal(bas, &val, &kind, &type, &size, p)) < 0)
			return err;
		if(!fetchSymbol(p, "="))
			return ERR_10;
		if(type == TYPE_NUM) {
			ssleep(7943);

			if((err = fetchNum(bas, &num_or_str, p)) < 0)
				return err;
			if((err = numCorrect(num_or_str)) < 0)
				return err;
		} else {
			ssleep(6826);

			if((err = fetchStr(bas, &num_or_str, p)) < 0)
				return err;
		}

		if((err = setVarVal(val, kind, type, size, num_or_str)) < 0)
			return err;
	} while(fetchSymbol(p, ","));

	if(!isTerm(p))
		return ERR_10;

	return ERR_OK;
}

/*
	LFILES
*/
static int staLfiles(struct Basic *bas, const uint8 **p)
{
	int err;
	uint8 *str;

	if((err = fetchStr(bas, &str, p)) < 0)
		return err;
	if(!isTerm(p))
		return ERR_10;

	return ERR_OK_NEXT;
}

#define MIN(x, y)	((x) < (y) ? (x): (y))
#define MAX(x, y)	((x) > (y) ? (x): (y))

/*
	LINE
*/
static int staLine(struct Basic *bas, const uint8 **p)
{
	int err, x1, y1, x2, y2, pat = 0xffff, draw = 0, w, h;
	uint8 mode = 1;

	if(fetchSymbol(p, "(")) {
		if((err = fetchNum_i(bas, &x1, p)) < 0)
			return err;
		if(!fetchSymbol(p, ","))
			return ERR_10;
		if((err = fetchNum_i(bas, &y1, p)) < 0)
			return err;
		if(!fetchSymbol(p, ")"))
			return ERR_10;
	} else {
		x1 = z80read16(&z80, 0x7967);
		y1 = z80read16(&z80, 0x7969);
	}

	if(!fetchSymbol(p, "-"))
		return ERR_10;

	if(!fetchSymbol(p, "("))
		return ERR_10;
	if((err = fetchNum_i(bas, &x2, p)) < 0)
		return err;
	if(!fetchSymbol(p, ","))
		return ERR_10;
	if((err = fetchNum_i(bas, &y2, p)) < 0)
		return err;
	if(!fetchSymbol(p, ")"))
		return ERR_10;

	if(fetchSymbol(p, ",")) {
		if(fetchSymbol(p, "X"))
			mode = 2;
		else if(fetchSymbol(p, "S"))
			mode = 1;
		else if(fetchSymbol(p, "R"))
			mode = 0;
		else
			goto line_pat;
	}

	if(fetchSymbol(p, ",")) {
line_pat:;
		if(peekSymbol(p, "BF") || peekSymbol(p, "B") || fetchNum_i(bas, &pat, p) < 0)
			goto line_box;
	}

	if(fetchSymbol(p, ",")) {
line_box:;
		if(fetchSymbol(p, "BF"))
			draw = 2;
		else if(fetchSymbol(p, "B"))
			draw = 1;
	}

	if(!isTerm(p))
		return ERR_10;

	z80write16(&z80, 0x7967, x2);
	z80write16(&z80, 0x7969, y2);

	w = MIN(abs(x2 - x1), lcdWidth);
	h = MIN(abs(y2 - y1), lcdHeight);

	ssleep(21170);

	if(draw == 0) {
		ssleep(3249 * MAX(w, h));
		line(x1, y1, x2, y2, mode, pat);
	} else if(draw == 1) {
		ssleep(3166 * 2 * (w + h));
		box(x1, y1, x2, y2, mode, pat);
	} else if(draw == 2) {
		ssleep(3166 * w * h);
		boxfill(x1, y1, x2, y2, mode, pat);
	}
	return ERR_OK_NEXT;
}

/*
	LIST
*/
static int staList(struct Basic *bas, const uint8 **p)
{
	int err, size;
	const uint8 *prog;

	if(isTerm(p))
		prog = bas->prog;
	else {
		if((err = findLine(p, &prog, bas->prog)) < 0)
			return err;
	}
	if(prog == NULL)
		return ERR_40;

	bas->p = prog;
	decodeLineNo(&bas->line_no, &size, bas->p);
	return ERR_LIST;
}

/*
	LLIST
*/
static int staLlist(struct Basic *bas, const uint8 **p)
{
	int start, end;

	if(isTerm(p)) {

	} else if(isLabel(p)) {
		const uint8 *prog;

		jumpToLabel(&prog, bas->prog, *p);
	} else {
		if((start = fetchLineNo(p)) < 0 && start != ERR_12)
			return start;

		if(fetchSymbol(p, "-"))
			if((end = fetchLineNo(p)) < 0 && end != ERR_12)
				return end;
	}
	if(!isTerm(p))
		return ERR_10;

	return ERR_OK_NEXT;
}

/*
	LNINPUT
*/
static int staLninput(struct Basic *bas, const uint8 **p)
{
	return ERR_10;
}

/*
	LOAD
*/
static int staLoad(struct Basic *bas, const uint8 **p)
{
	int err;
	uint8 *filename;

	if((err = fetchStr(bas, &filename, p)) < 0)
		return err;
	if(!isTerm(p))
		return ERR_10;

	if(!inportBas((char *)filename))
		return ERR_94;
	if(!loadBas(bas))
		return ERR_94;
	return staEnd(bas, p);
}

/*
	LOCATE
*/
static int staLocate(struct Basic *bas, const uint8 **p)
{
	int err, col = z80read8(&z80, 0x7922), row = z80read8(&z80, 0x7923);

	if((err = fetchNumOrEmpty_i(bas, &col, p)) < 0)
		return err;
	if(fetchComma(p))
		if((err = fetchNum_i(bas, &row, p)) < 0)
			return err;
	if(!isTerm(p))
		return ERR_10;

	ssleep(11661);

	if(col < 0 || col >= lcdCols)
		return ERR_33;
	z80write8(&z80, 0x7920, col);
	if(row < 0 || row >= lcdRows)
		return ERR_33;
	z80write8(&z80, 0x7921, row);

	*noWrap = 0x01;
	return ERR_OK_NEXT;
}

/*
	LPRINT
*/
static int staLprint(struct Basic *bas, const uint8 **p)
{
	return ERR_10;
}

/*
	MON
*/
static int staMon(struct Basic *bas, const uint8 **p)
{
	if(!isTerm(p))
		return ERR_10;

	*mode = MODE_MON;
	return staEnd(bas, p);
}

/*
	���s��������������
*/
static void clearCont(struct Basic *bas)
{
	bas->p = NULL;
	bas->line_no = 0;
}

/*
	NEW
*/
static int staNew(struct Basic *bas, const uint8 **p)
{
	if(!isTerm(p))
		return ERR_10;

	if(removeFile(pathBasic) < 0)
		return ERR_93;
	clearCont(bas);
	clearBas(bas);
	return staEnd(bas, p);
}

/*
	NEXT
*/
static int staNext(struct Basic *bas, const uint8 **p)
{
	struct ForLoop *for_loop;
	int err, kind, type, size, dir;
	uint8 *val, *val_next, result[SIZEOF_NUM];
	const uint8 *var;

	ssleep(8500);

	/* NEXT�̌��̕ϐ��𓾂� */
	if(isTerm(p))
		val_next = NULL;
	else {
		if((err = fetchVarVal(bas, &val_next, &kind, &type, &size, p)) < 0)
			return err;
		if(type != TYPE_NUM)
			return ERR_10;
		if(!isTerm(p))
			return ERR_10;
	}

	/* ���[�v�ϐ��𓾂� */
	for(;;) {
		if((err = peekFlow(bas, (void **)&for_loop, CODE_FOR)) < 0)
			return err;

		var = for_loop->var;

		if((err = fetchVarVal(bas, &val, &kind, &type, &size, &var)) < 0)
			return err;
		if(val_next == NULL || val_next == val)
			break;

		if((err = popFlow(bas, NULL, CODE_FOR)) < 0)
			return err;
	}

	/* STEP�̕����𓾂� */
	dir = numSgn(for_loop->step);

	/* STEP��̕ϐ��̒l�����߂� */
	if((err = opeAdd(NULL, val, for_loop->step)) < 0)
		return err;

	/* ���[�v���邩? */
	numLet(result, for_loop->to);
	if(dir > 0)
		err = opeGe(NULL, result, val);
	else if(dir < 0)
		err = opeLe(NULL, result, val);
	else
		err = opeEq(NULL, result, val);
	if(err < 0)
		return err;

	if(!numIsZero(result)) {
		*p = for_loop->ret;
		bas->line_no = for_loop->line_no;
	} else
		if((err = popFlow(bas, NULL, CODE_FOR)) < 0)
			return err;
	return ERR_OK_NEXT;
}

/*
	ON..GOTO, ON..GOSUB
*/
static int staOn(struct Basic *bas, const uint8 **p)
{
	int err, index;
	uint8 code;
	const uint8 *label_or_no = NULL;

	if((err = fetchNum_i(bas, &index, p)) < 0)
		return err;

	if(fetchKeyword(p, CODE_GOTO)) {
		ssleep(8852);

		code = CODE_GOTO;
	} else if(fetchKeyword(p, CODE_GOSUB)) {
		ssleep(9198);

		code = CODE_GOSUB;
	} else
		return ERR_10;

	for(;;) {
		if(--index == 0)
			label_or_no = *p;

		if(fetchLineNo(p) < 0)
			fetchLabel(p);

		while(!peekSymbol(p, ",") && !isTerm(p))
			(*p)++;
		if(!fetchComma(p))
			break;
	}
	if(!isTerm(p))
		return ERR_10;

	if(label_or_no == NULL)
		return ERR_OK_NEXT;
	if(code == CODE_GOSUB) {
		struct GosubReturn *gosub_return;

		if((err = pushFlow(bas, (void **)&gosub_return, CODE_GOSUB)) < 0)
			return err;
		gosub_return->line_no = bas->line_no;
		gosub_return->ret = *p;
	}
	if((err = findLine(&label_or_no, p, bas->prog)) < 0)
		return err;
	return ERR_OK_JUMP;
}

/*
	OPEN
*/
static int staOpen(struct Basic *bas, const uint8 **p)
{
	int err, fileno = 1, mode = 0;
	uint8 *filename;
	char path[MAX_PATH];

	if((err = fetchStr(bas, &filename, p)) < 0)
		return err;

	if(memcmp(filename, "E:", 2) == 0)
		strcpy(path, filename + 2);
	else
		return ERR_95;

	if(fetchKeyword(p, CODE_FOR)) {
		if(fetchKeyword(p, CODE_INPUT))
			mode = CODE_INPUT;
		else if(fetchKeyword(p, CODE_OUTPUT))
			mode = CODE_OUTPUT;
		else if(fetchKeyword(p, CODE_APPEND))
			mode = CODE_APPEND;
		else
			return ERR_10;

		if(!fetchKeyword(p, CODE_AS))
			return ERR_10;
		if(!fetchSymbol(p, "#"))
			return ERR_10;
		if((err = fetchNum_i(bas, &fileno, p)) < 0)
			return err;
		if(fileno <= 1 || fileno > sizeof(bas->fp) / sizeof(bas->fp[0]))
			return ERR_10;
		if(bas->fp[fileno] != NULL)
			return ERR_86;
	}
	if(!isTerm(p))
		return ERR_10;

	if(mode == CODE_INPUT)
		bas->fp[fileno] = fopen(path, "r");
	else if(mode == CODE_OUTPUT)
		bas->fp[fileno] = fopen(path, "w");
	else if(mode == CODE_APPEND)
		bas->fp[fileno] = fopen(path, "a");
	else
		bas->fp[fileno] = NULL;
	if(bas->fp[fileno] == NULL)
		return ERR_94;

	return ERR_OK_NEXT;
}

/*
	OUT
*/
static int staOut(struct Basic *bas, const uint8 **p)
{
	int err, port = 0x18, val;

	if((err = fetchNum_i(bas, &val, p)) < 0)
		return err;
	if(fetchComma(p)) {
		port = val;
		if((err = fetchNum_i(bas, &val, p)) < 0)
			return err;
	}
	if(!isTerm(p))
		return ERR_10;

	ssleep(5846);

	z80outport(&z80, port & 0xff, val & 0xff);
	return ERR_OK_NEXT;
}

/*
	�h��Ԃ� (staCircle, staPaint�̉�����)
*/
static int paint(int16 x, int16 y, uint8 pat)
{
	int i, j, redo;

	cleardot();
	paint_line(x, y);

	do {
		redo = FALSE;

		for(j = 1; j < lcdHeight; j++)
			for(i = 0; i < lcdWidth; i++)
				if(getdot(i, j - 1))
					redo |= paint_line(i, j);
		for(j = lcdHeight - 2; j >= 0; j--)
			for(i = 0; i < lcdWidth; i++)
				if(getdot(i, j + 1))
					redo |= paint_line(i, j);
	} while(redo);

	return draw(pat, 1);
}

/*
	PAINT
*/
static int staPaint(struct Basic *bas, const uint8 **p)
{
	int err, x, y, pat, n;

	if(!fetchSymbol(p, "("))
		return ERR_10;
	if((err = fetchNum_i(bas, &x, p)) < 0)
		return err;
	if(!fetchSymbol(p, ","))
		return ERR_10;
	if((err = fetchNum_i(bas, &y, p)) < 0)
		return err;
	if(!fetchSymbol(p, ")"))
		return ERR_10;
	if(!fetchSymbol(p, ","))
		return ERR_10;
	if((err = fetchNum_i(bas, &pat, p)) < 0)
		return err;
	if(pat < 1 || pat > 6)
		return ERR_33;
	if(!isTerm(p))
		return ERR_10;

	n = paint(x, y, pat);

	ssleep(3442664 + n * 3874);
	return ERR_OK_NEXT;
}

/*
	PASS
*/
static int staPass(struct Basic *bas, const uint8 **p)
{
	int err;
	uint8 *pass;

	if((err = fetchStr(bas, &pass, p)) < 0)
		return err;
	if(!isTerm(p))
		return ERR_10;

	return ERR_OK_NEXT;
}

/*
	PIOPUT
*/
static int staPioput(struct Basic *bas, const uint8 **p)
{
	int err, x;

	if((err = fetchNum_i(bas, &x, p)) < 0)
		return err;
	if(!isTerm(p))
		return ERR_10;

	return ERR_OK_NEXT;
}

/*
	PIOSET
*/
static int staPioset(struct Basic *bas, const uint8 **p)
{
	int err, x;

	if((err = fetchNum_i(bas, &x, p)) < 0)
		return err;
	if(!isTerm(p))
		return ERR_10;

	ssleep(7289);

	return ERR_OK_NEXT;
}

/*
	POKE
*/
static int staPoke(struct Basic *bas, const uint8 **p)
{
	int err, address, val;

	if((err = fetchNum_i(bas, &address, p)) < 0)
		return err;

	if(!fetchSymbol(p, ","))
		return ERR_10;
	do {
		if((err = fetchNum_i(bas, &val, p)) < 0)
			return err;
		if(address < 0 || address > 0xffff)
			return ERR_33;
		if(val < 0 || val > 0xff)
			return ERR_33;

		z80write8(&z80, address++, val);
	} while(fetchSymbol(p, ","));

	if(!isTerm(p))
		return ERR_10;

	ssleep(52418);
	return ERR_OK_NEXT;
}

/*
	PRESET
*/
static int staPreset(struct Basic *bas, const uint8 **p)
{
	int err, x, y;

	if(!fetchSymbol(p, "("))
		return ERR_10;

	if((err = fetchNum_i(bas, &x, p)) < 0)
		return err;

	if(!fetchSymbol(p, ","))
		return ERR_10;

	if((err = fetchNum_i(bas, &y, p)) < 0)
		return err;

	if(!fetchSymbol(p, ")"))
		return ERR_10;
	if(!isTerm(p))
		return ERR_10;

	ssleep(12174);

	pset(x, y, 0);
	return ERR_OK_NEXT;
}

/*
*/
static int formatVal(char *buf, const uint8 *val, int type)
{
	int err;

	if(type == TYPE_NUM) {
		if(numSgn(val) >= 0) {
			buf[0] = ' ';
			err = decodeNum((uint8 *)buf + 1, val);
		} else
			err = decodeNum((uint8 *)buf, val);
		if(err < 0)
			return err;

		strcat(buf, " ");
		return ERR_OK;
	} else if(type == TYPE_STR) {
		strcpy(buf, val);
		return ERR_OK;
	} else
		return ERR_10;
}

/*
	PRINT #
*/
static int staPrintFile(struct Basic *bas, const uint8 **p)
{
	int err, fileno, type, size;
	char buf[260 + 1];
	uint8 *num_or_str, *array;

	if(!fetchSymbol(p, "#"))
		return ERR_10;
	if((err = fetchNum_i(bas, &fileno, p)) < 0)
		return err;
	if(fileno <= 1 || fileno > sizeof(bas->fp) / sizeof(bas->fp[0]))
		return ERR_10;
	if(bas->fp[fileno] == NULL)
		return ERR_85;

	if(!fetchSymbol(p, ","))
		return ERR_10;

	do {
		if(fetchArrayWild(bas, &array, &type, p)) {
			int i, j;

			for(i = 0;; i++) {
				if(getArrayVal(&num_or_str, &type, &size, array, i, 0) < 0)
					break;
				if((err = formatVal(buf, num_or_str, type)) < 0)
					return err;
				fprintf(bas->fp[fileno], "%s\n", buf);

				for(j = 1;; j++) {
					if(getArrayVal(&num_or_str, &type, &size, array, i, j) < 0)
						break;
					if((err = formatVal(buf, num_or_str, type)) < 0)
						return err;
					fprintf(bas->fp[fileno], "%s\n", buf);
				}
			}

			if(fetchSymbol(p, ","))
				;
			else if(fetchSymbol(p, ";"))
				;
		} else {
			if((err = fetchParam(bas, &num_or_str, &type, p)) < 0)
				return err;
			if((err = formatVal(buf, num_or_str, type)) < 0)
				return err;

			if(fetchSymbol(p, ",")) {
				int blank = ((strlen(buf) - 1) / 20 + 1) * 20;

				if(type == TYPE_NUM)
					fprintf(bas->fp[fileno], "%*s", blank, buf);
				else
					fprintf(bas->fp[fileno], "%*s", blank, buf);
			} else {
				if(type == TYPE_NUM)
					fprintf(bas->fp[fileno], "%s", buf);
				else
					fprintf(bas->fp[fileno], "%s ", buf);
				if(!fetchSymbol(p, ";"))
					fprintf(bas->fp[fileno], "\n");
			}
		}
	} while(!isTerm(p));

	return ERR_OK_NEXT;
}

/*
	���������������� (������)
*/
static void initFormat(struct Basic *bas)
{
	bas->format_comma = bas->format_period = bas->format_exp = bas->format_ints = bas->format_decs = bas->format_strs = 0;
}

/*
	������ݒ肷�� (PRINT, USING�̉�����)
*/
static int _staUsing(struct Basic *bas, const uint8 **p)
{
	int err;
	uint8 *format, *f;

	initFormat(bas);

	if(p == NULL)
		return ERR_OK_NEXT;

	if(**p != '"')
		return ERR_10;
	if((err = fetchStr(bas, &format, p)) < 0)
		return err;

	/* ������̏����𓾂� */
	for(f = format; *f == '&'; f++)
		bas->format_strs++;

	/* ���l�̏����𓾂� */
	for(;; f++)
		if(*f == '#')
			bas->format_ints++;
		else if(*f == ',')
			bas->format_comma = TRUE;
		else
			break;
	if(*f == '.') {
		bas->format_period = TRUE;

		for(f = f + 1; *f == '#'; f++)
			bas->format_decs++;
	}
	if(*f == '^') {
		f++;
		bas->format_exp = TRUE;
	}

	/* ������̏����𓾂� */
	if(*f == '&')
		for(f = f + 1; *f == '&'; f++)
			bas->format_strs++;

	if(*f != 0 || (bas->format_comma && bas->format_exp)) {
		initFormat(bas);
		return ERR_71;
	}
	return ERR_OK_NEXT;
}

/*
	PRINT
*/
static int staPrint(struct Basic *bas, const uint8 **p)
{
	int err, type, i, len, cat;
	uint8 *val, *c, buf[32];

	if(peekSymbol(p, "#"))
		return staPrintFile(bas, p);

	ssleep(9506);

	if(*noWrap == 0x00) {
		gputchr(0);
		cat = FALSE;
	} else
		cat = TRUE;

	if(isTerm(p)) {
		*noWrap = 0x00;
		pause(bas);
		return ERR_OK_NEXT;
	}

	for(i = 0; ; i++) {
		/* USING */
		if(fetchKeyword(p, CODE_USING)) {
			if((err = _staUsing(bas, p)) < 0)
				if(err != ERR_10)
					return err;
			if(!fetchSymbol(p, ";"))
				return ERR_10;
		}

		/* �l�𓾂� */
		if((err = fetchParam(bas, &val, &type, p)) < 0)
			return err;
		if(type == TYPE_STR)
			len = (bas->format_strs > 0 ? bas->format_strs: strlen(val));
		else if(type == TYPE_NUM) {
			if((err = numCorrect9(val)) < 0)
				return err;
			if((err = decodeNumFormat(bas, buf, val, TRUE)) < 0)
				return err;
			val = buf;
			len = strlen(buf);
		} else
			return ERR_10;

		/* �l��\������ */
		if(type == TYPE_NUM) {
			if(!cat && i == 0 && isTerm(p))
				while(*curCol != lcdCols - len)
					moveCursor(0x1c);
			else if(cat || peekSymbol(p, ";"))
				;
			else if(*curCol == 0)
				while(*curCol != lcdCols / 2 - len)
					moveCursor(0x1c);
			else if(*curCol <= lcdCols / 2)
				while(*curCol != lcdCols - len)
					moveCursor(0x1c);
			else {
				moveCursor('\r');
				while(*curCol != lcdCols / 2 - len)
					moveCursor(0x1c);
			}
		}
		for(c = val; *c != 0 && (int )(c - val) < len; c++) {
			ssleep(1661);

			if(*c >= 0x20)
				if(gputchr(*c))
					ssleep(143306);
		}
		if(type == TYPE_STR) {
			if(!cat && i == 0 && isTerm(p))
				;
			else if(peekSymbol(p, ";") || cat)
				;
			else if(*curCol <= lcdCols / 2)
				while(*curCol != lcdCols / 2)
					moveCursor(0x1c);
			else
				moveCursor('\r');
		}

		/* ���̒l�ֈڂ� */
		if(isTerm(p)) {
			*noWrap = 0x00;
			break;
		} else if(fetchSymbol(p, ",")) {
			*noWrap = 0x01;
			cat = FALSE;
		} else if(fetchSymbol(p, ";")) {
			*noWrap = 0x01;
			if(isTerm(p))
				break;
			cat = TRUE;
		} else
			return ERR_10;
	}

	pause(bas);
	return ERR_OK_NEXT;
}

/*
	DATA���X�^�b�N�ɐς� (staRead�̉�����)
*/
static int pushData(struct Basic *bas, const uint8 **p, int type)
{
	int err;

	if(*p == NULL)
		return ERR_53;

	if(type == TYPE_NUM) {
		int len;
		uint8 num[SIZEOF_NUM];

		if(peekSymbol(p, ",") || isTerm(p))
			numLet(num, NUM_0);
		else {
			if((len = encodeNum(num, *p)) < 0)
				return len;
			*p += len;
		}

		if((err = pushNum(num)) < 0)
			return err;
	} else {
		int size;
		const uint8 *str;

		if(fetchLDquote(p)) {
			str = *p;
			size = getStrLen(*p);

			*p += size;
			fetchRDquote(p);
		} else {
			str = *p;

			for(size = 0; !peekSymbol(p, ",") && !isTerm(p); size++, (*p)++)
				;
		}

		if((err = pushStr(str, size)) < 0)
			return err;
	}
	skipBlank(p);

	if(fetchComma(p))
		return ERR_OK_NEXT;

	for(;;) {
		if(*p == NULL)
			return ERR_OK_NEXT;
		else if(fetchKeyword(p, CODE_DATA))
			return ERR_OK_NEXT;

		goNext(p);
	}
}

/*
	PSET
*/
static int staPset(struct Basic *bas, const uint8 **p)
{
	int err, x, y;
	uint8 mode = 1;

	if(!fetchSymbol(p, "("))
		return ERR_10;

	if((err = fetchNum_i(bas, &x, p)) < 0)
		return err;

	if(!fetchSymbol(p, ","))
		return ERR_10;

	if((err = fetchNum_i(bas, &y, p)) < 0)
		return err;

	if(!fetchSymbol(p, ")"))
		return ERR_10;

	if(fetchSymbol(p, ",")) {
		if(fetchSymbol(p, "X"))
			mode = 2;
		else
			return ERR_10;
	}
	if(!isTerm(p))
		return ERR_10;

	ssleep(12320);

	pset(x, y, mode);
	return ERR_OK_NEXT;
}

/*
	RADIAN
*/
static int staRadian(struct Basic *bas, const uint8 **p)
{
	if(!isTerm(p))
		return ERR_10;

	ssleep(5008);

	*angle = ANGLE_RADIAN;
	return ERR_OK_NEXT;
}

/*
	RANDOMIZE
*/
static int staRandomize(struct Basic *bas, const uint8 **p)
{
	if(!isTerm(p))
		return ERR_10;

	ssleep(61353);

	bas->seed = time(NULL) & 0xffffffff;
	srand(bas->seed);

	return ERR_OK_NEXT;
}

/*
	READ
*/
static int staRead(struct Basic *bas, const uint8 **p)
{
	int err, kind, type, size;
	uint8 *val;

	ssleep(1358);

	for(;;) {
		ssleep(1360);

		if((err = fetchVarVal(bas, &val, &kind, &type, &size, p)) < 0)
			return err;

		if((err = pushData(bas, &bas->d, type)) < 0)
			return err;
		if(type == TYPE_NUM) {
			uint8 *num;

			if((err = popNum(&num)) < 0)
				return err;
			if((err = setVarVal(val, kind, type, size, num)) < 0)
				return err;
		} else {
			uint8 *str;

			if((err = popStr(&str)) < 0)
				return err;
			if((err = setVarVal(val, kind, type, size, str)) < 0)
				return err;
		}

		if(isTerm(p))
			break;
		if(!fetchComma(p))
			return ERR_10;
	}

	return ERR_OK_NEXT;
}

/*
	REM
*/
static int staRem(struct Basic *bas, const uint8 **p)
{
	ssleep(2337);

	goNextLine(p);
	return ERR_OK_JUMP;
}

/*
	�폜����
*/
static int _delete(uint8 *area, int area_size, uint8 *p, int len)
{
	memmove(p, p + len, (int )((area + area_size) - (p + len)));
	return ERR_OK;
}

/*
	�s�̌���폜����
*/
static int deleteWord(uint8 *area, int area_size, uint8 *line, uint8 *p, int len)
{
	_delete(area, area_size, p, len);
	line[2] -= len;
	return ERR_OK;
}

/*
	�}������
*/
static int _insert(uint8 *area, int area_size, uint8 *p, int len)
{
	uint8 *last;

	/* �I�[�o�[���邩? */
	for(last = area; !IS_LAST(last); last += LINE_SIZE(last))
		;
	last++;

	if((int )(last - area) + len >= area_size)
		return ERR_60;

	/* �}������ */
	memmove(p + len, p, (int )((area + area_size) - (p + len)));
	return ERR_OK;
}

/*
	�s�̌��}������
*/
static int insertWord(uint8 *area, int area_size, uint8 *line, uint8 *p, const uint8 *word, int len)
{
	int err;

	if(LINE_SIZE(p) + len > 0xff)
		return ERR_60; /* ??? */
	if((err = _insert(area, area_size, p, len)) < 0)
		return err;

	line[2] += len;
	memmove(p, word, len);
	return ERR_OK;
}

/*
	�s�ԍ���t������ (staRenum�̉�����)
*/
static void _renum(struct Basic *bas, int line_no_old, int line_no_new)
{
	uint8 *line, *p, *q, *r;
	int len, line_no;
	uint8 buf[8];

	for(line = bas->prog; !IS_LAST(line); line += LINE_SIZE(line)) {
		for(p = line + 3; *p != '\r'; goNext((const uint8 **)&p)) {
			q = p;

			/* �s�ԍ��܂��̓��x�����p�����[�^�̃X�e�[�g�����g�łȂ���Ώ������Ȃ� */
			if(*q != CODE_RESERVED)
				continue;
			q++;

			if(
			*q != CODE_RUN &&
			*q != CODE_CONT &&
			*q != CODE_LIST &&
			*q != CODE_LLIST &&
			*q != CODE_RENUM &&
			*q != CODE_AUTO &&
			*q != CODE_DELETE &&
			*q != CODE_GOTO &&
			*q != CODE_THEN &&
			*q != CODE_GOSUB &&
			*q != CODE_RETURN &&
			*q != CODE_RESTORE &&
			*q != CODE_ELSE
			)
				continue;
			q++;

			skipBlank((const uint8 **)&q);

			do {

				if(isLabel((const uint8 **)&q)) {
					/* ���x����ǂݔ�΂� */
					fetchLabel((const uint8 **)&q);
				} else if(isLineNo((const uint8 **)&q)) {
					/* �s�ԍ��𓾂� */
					r = q;
					line_no = fetchLineNoOnly((const uint8 **)&r);

					/* �ϊ��O�̍s�ԍ���? */
					if(line_no == line_no_old) {
						/* �s�ԍ����폜���� */
						len = (int )(r - q);
						deleteWord(bas->prog, bas->prog_size, line, q, len);
	
						/* �s�ԍ���}������ */
						sprintf((char *)buf, "%d", line_no_new);
						insertWord(bas->prog, bas->prog_size, line, q, buf, strlen((char *)buf));
					}

					/* �s�ԍ���ǂݔ�΂� */
					fetchLineNo((const uint8 **)&q);
				}
			} while(fetchComma((const uint8 **)&q));
		}
	}
}

/*
	RENUM
*/
static int staRenum(struct Basic *bas, const uint8 **p)
{
	int line_no_new = 10, line_no_start = 0, step = 10, line_no, line_no_old;
	uint8 *line;

	if(isLineNo(p)) {
		if((line_no_new = fetchLineNo(p)) < 0)
			return line_no_new;

		if(fetchSymbol(p, ",")) {
			if((line_no_start = fetchLineNo(p)) < 0)
				return line_no_start;

			if(fetchSymbol(p, ",")) {
				if((step = fetchLineNo(p)) < 0)
					return step;
			}
		}
	}
	if(!isTerm(p))
		return ERR_10;

	/* �J�n�ԍ��ƐV�ԍ����`�F�b�N���� */
	for(line = bas->prog; LINE_NO(line) <= line_no_start && !IS_LAST(line); line += LINE_SIZE(line))
		if(LINE_NO(line) > line_no_new)
			return ERR_43;

	/* �J�n�ԍ��܂ňړ����� */
	for(line = bas->prog; LINE_NO(line) < line_no_start && !IS_LAST(line); line += LINE_SIZE(line))
		;
	if(IS_LAST(line))
		return ERR_OK;

	/* �s�ԍ���t������ */
	for(line_no = line_no_new; !IS_LAST(line); line_no += step, line += LINE_SIZE(line)) {
		line_no_old = line[0] * 0x100 + line[1];

		line[0] = (line_no >> 8) & 0xff;
		line[1] = line_no & 0xff;
		_renum(bas, line_no_old, line_no);
	}

	saveBas(bas);
	return ERR_OK;
}

/*
	REPEAT
*/
static int staRepeat(struct Basic *bas, const uint8 **p)
{
	struct RepeatLoop *repeat_loop;
	int err;

	if(!isTerm(p))
		return ERR_10;

	if((err = pushFlow(bas, (void **)&repeat_loop, CODE_REPEAT)) < 0)
		return err;

	ssleep(2337);

	repeat_loop->line_no = bas->line_no;
	repeat_loop->ret = *p;
	return ERR_OK_NEXT;
}

/*
	RESTORE
*/
static int staRestore(struct Basic *bas, const uint8 **p)
{
	int err;
	const uint8 *start = bas->prog;

	if(isLineNo(p) || isLabel(p))
		if((err = findLine(p, &start, bas->prog)) < 0)
			return err;
	if(!isTerm(p))
		return ERR_10;

	ssleep(2562);

	bas->d = restoreData(start);
	return ERR_OK_NEXT;
}

/*
	RETURN
*/
static int staReturn(struct Basic *bas, const uint8 **p)
{
	struct GosubReturn *gosub_return;
	int err;

	ssleep(3409);

	if((err = popFlow(bas, (void **)&gosub_return, CODE_GOSUB)) < 0)
		return err;

	if(isTerm(p)) {
		*p = gosub_return->ret;
		bas->line_no = gosub_return->line_no;
		return ERR_OK_NEXT;
	} else
		return staGoto(bas, p);
}

/*
	RUN
*/
static int staRun(struct Basic *bas, const uint8 **p)
{
	int err;
	const uint8 *start = bas->prog;

	ssleep(1000000);

	if(!loadBas(bas))
		return ERR_94;

	if(isLineNo(p) || isLabel(p))
		if((err = findLine(p, &start, bas->prog)) < 0)
			return err;
	if(!isTerm(p))
		return ERR_10;

	*p = start;
	bas->d = restoreData(start);
	bas->top = &bas->stack[-1];

	freeAllVars(bas);
	_staUsing(bas, NULL);

	*pauseWhenPrint = 0x02;
	*waitTimeL = *waitTimeH = 0;

	srand(bas->seed);

	z80write16(&z80, 0x79db, 7);
	z80write16(&z80, 0x79dd, 0);
	return ERR_OK_JUMP;
}

/*
	SAVE
*/
static int staSave(struct Basic *bas, const uint8 **p)
{
	int err;
	uint8 *filename;

	if((err = fetchStr(bas, &filename, p)) < 0)
		return err;
	if(!isTerm(p))
		return ERR_10;

	if(!exportBas((char *)filename))
		return ERR_94;
	return ERR_OK;
}

/*
	SPINP
*/
static int staSpinp(struct Basic *bas, const uint8 **p)
{
	return ERR_10;
}

/*
	SPOUT
*/
static int staSpout(struct Basic *bas, const uint8 **p)
{
	return ERR_10;
}

/*
	STOP
*/
static int staStop(struct Basic *bas, const uint8 **p)
{
	if(!isTerm(p))
		return ERR_10;

	return ERR_BREAK;
}

/*
	SWITCH(�u���b�N�\��)
*/
static int staSwitch(struct Basic *bas, const uint8 **p)
{
	struct SwitchCase *switch_case;
	int err, type;
	uint8 *dummy, *bool;
	const uint8 *var, *tmp;

	if((err = peekFlow(bas, (void **)&switch_case, CODE_SWITCH)) != ERR_69)
		return ERR_69;

	var = *p;
	if((err = fetchParam(bas, &dummy, &type, p)) < 0)
		return err;
	if(!isTerm(p))
		return ERR_10;

	ssleep(7911);

	for(;;) {
		ssleep(12969);

		if(!goNextLineCode(p))
			return ERR_68;

		if(fetchKeyword(p, CODE_SWITCH))
			return ERR_50;
		else if(fetchKeyword(p, CODE_CASE)) {
			tmp = var;
			if((err = pushParam(bas, &tmp)) < 0)
				return err;
			if((err = pushParam(bas, p)) < 0)
				return err;
			if((err = exeOpe(bas, CODE_EQ)) < 0)
				return err;
			if((err = popNum(&bool)) < 0)
				return err;
			if(!isTerm(p))
				return ERR_10;

			if(!numIsZero(bool)) {
				if((err = pushFlow(bas, (void **)&switch_case, CODE_SWITCH)) < 0)
					return err;
				return ERR_OK_NEXT;
			}
		} else if(fetchKeyword(p, CODE_DEFAULT)) {
			if((err = pushFlow(bas, (void **)&switch_case, CODE_SWITCH)) < 0)
				return err;
			return ERR_OK_NEXT;
		} else if(fetchKeyword(p, CODE_ENDSWITCH))
			return ERR_OK_NEXT;
	}
}

/*
	TROFF
*/
static int staTroff(struct Basic *bas, const uint8 **p)
{
	return ERR_OK_NEXT;
}

/*
	TRON
*/
static int staTron(struct Basic *bas, const uint8 **p)
{
	return ERR_OK_NEXT;
}

/*
	UNTIL
*/
static int staUntil(struct Basic *bas, const uint8 **p)
{
	struct RepeatLoop *repeat_loop;
	int err;
	uint8 *bool;

	if((err = fetchNum(bas, &bool, p)) < 0)
		return err;
	if(!isTerm(p))
		return ERR_10;

	ssleep(6362); /* ??? */

	if((err = popFlow(bas, (void **)&repeat_loop, CODE_REPEAT)) < 0)
		return err;

	if(!numIsZero(bool))
		return ERR_OK_NEXT;
	*p = repeat_loop->ret;
	bas->line_no = repeat_loop->line_no;
	return staRepeat(bas, p);
}

/*
	USING
*/
static int staUsing(struct Basic *bas, const uint8 **p)
{
	int err;

	ssleep(7286);

	if((err = _staUsing(bas, p)) < 0)
		if(err != -10)
			return err;

	if(!isTerm(p))
		return ERR_10;
	return ERR_OK_NEXT;
}

/*
	WAIT
*/
static int staWait(struct Basic *bas, const uint8 **p)
{
	int err, pause = FALSE, w = 0;

	ssleep(6194);

	if((err = fetchNumOrEmpty_i(bas, &w, p)) < 0)
		return err;
	else if(err > 0)
		pause = TRUE;
	if(w < 0 || w > 0xffff)
		return ERR_33;
	if(!isTerm(p))
		return ERR_10;

	*pauseWhenPrint = (pause ? 0x06: 0x02);
	*waitTimeL = w & 0xff;
	*waitTimeH = w >> 8;
	return ERR_OK_NEXT;
}

/*
	WEND
*/
static int staWend(struct Basic *bas, const uint8 **p)
{
	struct WhileLoop *while_loop;
	int err;

	if(!isTerm(p))
		return ERR_10;

	ssleep(6362); /* ??? */

	if((err = popFlow(bas, (void **)&while_loop, CODE_WHILE)) < 0)
		return err;

	*p = while_loop->ret;
	bas->line_no = while_loop->line_no;
	return staWhile(bas, p);
}

/*
	WHILE
*/
static int staWhile(struct Basic *bas, const uint8 **p)
{
	struct WhileLoop *while_loop;
	int err, depth = 0;
	uint8 *bool;

	if((err = pushFlow(bas, (void **)&while_loop, CODE_WHILE)) < 0)
		return err;
	while_loop->line_no = bas->line_no;
	while_loop->ret = *p;

	if((err = fetchNum(bas, &bool, p)) < 0)
		return err;
	if(!isTerm(p))
		return ERR_10;

	ssleep(6748);

	if(!numIsZero(bool))
		return ERR_OK_NEXT;
	else {
		if((err = popFlow(bas, (void **)&while_loop, CODE_WHILE)) < 0)
			return err;

		for(;;) {
			if(*p == NULL)
				return ERR_63;
			else if(fetchKeyword(p, CODE_WEND)) {
				if(depth > 0)
					depth--;
				else
					return ERR_OK_NEXT;
			} else if(fetchKeyword(p, CODE_WHILE))
				depth++;

			goNext(p);
		}
	}
}

/* �X�e�[�g�����g�\ */
struct Statement staTable[] = {
	{ NULL },	/* 00 */
	{ NULL },	/* 01 */
	{ NULL },	/* 02 */
	{ NULL },	/* 03 */
	{ NULL },	/* 04 */
	{ NULL },	/* 05 */
	{ NULL },	/* 06 */
	{ NULL },	/* 07 */
	{ NULL },	/* 08 */
	{ NULL },	/* 09 */
	{ NULL },	/* 0a */
	{ NULL },	/* 0b */
	{ NULL },	/* 0c */
	{ NULL },	/* 0d */
	{ NULL },	/* 0e */
	{ staMon, MODE_PRO | MODE_MAN },	/* 0f MON */
	{ staRun, MODE_MAN },	/* 10 RUN */
	{ staNew, MODE_PRO | MODE_MAN },	/* 11 NEW */
	{ staCont, MODE_MAN },	/* 12 CONT */
	{ staPass, MODE_PRO | MODE_MAN },	/* 13 PASS */
	{ staList, MODE_PRO },	/* 14 LIST */
	{ staLlist, MODE_PRO },	/* 15 LLIST */
	{ staBload, MODE_PRO | MODE_MAN },	/* 16 BLOAD */
	{ staRenum, MODE_PRO },	/* 17 RENUM */
	{ staLoad, MODE_PRO | MODE_MAN },	/* 18 LOAD */
	{ NULL },	/* 19 */
	{ staAuto, MODE_PRO },	/* 1a AUTO */
	{ staDelete, MODE_PRO },	/* 1b DELETE */
	{ staFiles, MODE_PRO | MODE_MAN },	/* 1c FILES */
	{ NULL },	/* 1d */
	{ NULL },	/* 1e */
	{ staLcopy, MODE_PRO },	/* 1f LCOPY */
	{ staBsave, MODE_PRO | MODE_MAN },	/* 20 BSAVE */
	{ staOpen, MODE_PRO | MODE_MAN | MODE_RUN },	/* 21 OPEN */
	{ staClose, MODE_PRO | MODE_MAN | MODE_RUN },	/* 22 CLOSE */
	{ staSave, MODE_PRO | MODE_MAN },	/* 23 SAVE */
	{ NULL },	/* 24 */
	{ staRandomize, MODE_PRO | MODE_MAN | MODE_RUN },	/* 25 RANDOMIZE */
	{ staDegree, MODE_PRO | MODE_MAN | MODE_RUN },	/* 26 DEGREE */
	{ staRadian, MODE_PRO | MODE_MAN | MODE_RUN },	/* 27 RADIAN */
	{ staGrad, MODE_PRO | MODE_MAN | MODE_RUN },	/* 28 GRAD */
	{ staBeep, MODE_PRO | MODE_MAN | MODE_RUN },	/* 29 BEEP */
	{ staWait, MODE_PRO | MODE_MAN | MODE_RUN },	/* 2a WAIT */
	{ staGoto, MODE_MAN | MODE_RUN },	/* 2b GOTO */
	{ staTron, MODE_PRO | MODE_MAN | MODE_RUN },	/* 2c TRON */
	{ staTroff, MODE_PRO | MODE_MAN | MODE_RUN },	/* 2d TROFF */
	{ staClear, MODE_PRO | MODE_MAN | MODE_RUN },	/* 2e CLEAR */
	{ staUsing, MODE_PRO | MODE_MAN | MODE_RUN },	/* 2f USING */
	{ staDim, MODE_PRO | MODE_MAN | MODE_RUN },	/* 30 DIM */
	{ staCall, MODE_PRO | MODE_MAN | MODE_RUN },	/* 31 CALL */
	{ staPoke, MODE_PRO | MODE_MAN | MODE_RUN },	/* 32 POKE */
	{ staGprint, MODE_PRO | MODE_MAN | MODE_RUN },	/* 33 GPRINT */
	{ staPset, MODE_PRO | MODE_MAN | MODE_RUN },	/* 34 PSET */
	{ staPreset, MODE_PRO | MODE_MAN | MODE_RUN },	/* 35 PRESET */
	{ NULL },	/* 36 */
	{ NULL },	/* 37 */
	{ NULL },	/* 38 */
	{ NULL },	/* 39 */
	{ staErase, MODE_PRO | MODE_MAN | MODE_RUN },	/* 3a ERASE */
	{ staLfiles, MODE_PRO | MODE_MAN },	/* 3b LFILES */
	{ staKill, MODE_PRO | MODE_MAN },	/* 3c KILL */
	{ NULL },	/* 3d */
	{ NULL },	/* 3e */
	{ NULL },	/* 3f */
	{ NULL },	/* 40 */
	{ NULL },	/* 41 */
	{ NULL },	/* 42 */
	{ NULL },	/* 43 */
	{ NULL },	/* 44 */
	{ staOut, MODE_PRO | MODE_MAN | MODE_RUN },	/* 45 OUT */
	{ NULL },	/* 46 */
	{ NULL },	/* 47 */
	{ staPioset, MODE_PRO | MODE_MAN | MODE_RUN },	/* 48 PIOSET */
	{ staPioput, MODE_PRO | MODE_MAN | MODE_RUN },	/* 49 PIOPUT */
	{ staSpout, MODE_PRO | MODE_MAN | MODE_RUN },	/* 4a SPOUT */
	{ staSpinp, MODE_PRO | MODE_MAN | MODE_RUN },	/* 4b SPINP */
	{ staHdcopy, MODE_PRO | MODE_MAN | MODE_RUN },	/* 4c HDCOPY */
	{ NULL },	/* 4d ENDIF */
	{ staRepeat, MODE_RUN },	/* 4e REPEAT */
	{ staUntil, MODE_RUN },	/* 4f UNTIL */
	{ staCls, MODE_RUN },	/* 50 CLS */
	{ staLocate, MODE_RUN },	/* 51 LOCATE */
	{ NULL },	/* 52 TO */
	{ NULL },	/* 53 STEP */
	{ NULL },	/* 54 THEN */
	{ staOn, MODE_RUN },	/* 55 ON */
	{ staIf, MODE_RUN },	/* 56 IF */
	{ staFor, MODE_RUN },	/* 57 FOR */
	{ staLet, MODE_MAN | MODE_RUN },	/* 58 LET */
	{ staRem, MODE_RUN },	/* 59 REM */
	{ staEnd, MODE_RUN },	/* 5a END */
	{ staNext, MODE_RUN },	/* 5b NEXT */
	{ staStop, MODE_RUN },	/* 5c STOP */
	{ staRead, MODE_RUN },	/* 5d READ */
	{ staData, MODE_RUN },	/* 5e DATA */
	{ NULL },	/* 5f */
	{ staPrint, MODE_PRO | MODE_MAN | MODE_RUN },	/* 60 PRINT */
	{ staInput, MODE_RUN },	/* 61 INPUT */
	{ staGosub, MODE_RUN },	/* 62 GOSUB */
	{ staLninput, MODE_RUN },	/* 63 LNINPUT */
	{ staLprint, MODE_PRO | MODE_MAN | MODE_RUN },	/* 64 LPRINT */
	{ staReturn, MODE_RUN },	/* 65 RETURN */
	{ staRestore, MODE_RUN },	/* 66 RESTORE */
	{ NULL },	/* 67 */
	{ staGcursor, MODE_PRO | MODE_MAN | MODE_RUN },	/* 68 GCURSOR */
	{ staLine, MODE_PRO | MODE_MAN | MODE_RUN },	/* 69 LINE */
	{ NULL },	/* 6a */
	{ NULL },	/* 6b */
	{ NULL },	/* 6c */
	{ NULL },	/* 6d */
	{ NULL },	/* 6e */
	{ staCircle, MODE_PRO | MODE_MAN | MODE_RUN },	/* 6f CIRCLE */
	{ staPaint, MODE_PRO | MODE_MAN | MODE_RUN },	/* 70 PAINT */
	{ NULL },	/* 71 OUTPUT */
	{ NULL },	/* 72 APPEND */
	{ NULL },	/* 73 AS */
	{ NULL },	/* 74 */
	{ NULL },	/* 75 */
	{ staElse, MODE_RUN },	/* 76 ELSE */
	{ NULL },	/* 77 */
	{ NULL },	/* 78 */
	{ NULL },	/* 79 */
	{ staWhile, MODE_RUN },	/* 7a WHILE */
	{ staWend, MODE_RUN },	/* 7b WEND */
	{ NULL },	/* 7c SWITCH */
	{ NULL },	/* 7d CASE */
	{ NULL },	/* 7e DEFAULT */
	{ NULL }	/* 7f ENDSWITCH */
};

/*
	1�X�e�[�g�����g���s����
*/
int runSta1(struct Basic *bas, const uint8 **p)
{
	int err;
	const uint8 *exe;

	/* �I����? */
	if(*p == NULL)
		return ERR_OK_JUMP;

	/* BUSY��\������ */
	putstatus(STATUS_BUSY, TRUE);

	/* ��؂�Ƌ󔒂�ǂݔ�΂� */
	while(isStaTerm(p) || isBlank(p))
		(*p)++;

	/* BREAK��������Ă��邩? */
	updateKey();
	if(peekKeycode() == GKEY_BREAK)
		return ERR_BREAK;

	/* �s����? */
	if(**p == '\r' || **p == '\'') {
		goNextLine(p);
		return ERR_OK_JUMP;
	}

	/* �X�e�[�g�����g�����s���� */
	exe = *p;

	if(*exe == CODE_RESERVED) {
		struct Statement *sta = &staTable[*(exe + 1)];

		if(*(exe + 1) >= 0x80)
			return ERR_10;
		if(sta->sta == NULL)
			return ERR_10;

		if(sta->mode & MODE_RUN & *mode)
			; /* RUN���[�h */
		else if((sta->mode & MODE_MAN) && (*mode & MODE_RUN) && isManual(bas, exe))
			; /* MANUAL���[�h */
		else if(sta->mode & MODE_PRO & *mode)
			; /* PRO���[�h */
		else
			return ERR_12;

		fetchAnyKeyword(&exe);
		err = sta->sta(bas, &exe);
		if(IS_ERROR(err))
			return err;
	} else if(isalpha(*exe)) {
		err = staLet(bas, &exe);
		if(IS_ERROR(err))
			return err;
	} else
		return ERR_10;

	*p = exe;

	/* BREAK��������Ă��邩? */
	if(peekKeycode() == GKEY_BREAK)
		return ERR_BREAK;
	return err;
}

/*
	�����̃X�e�[�g�����g�����s���� (runProg, runLine�̉�����)
*/
static int runSta(struct Basic *bas, const uint8 **p)
{
	int err;

	while((err = runSta1(bas, p)) == ERR_OK_NEXT)
		updateLCD();
	if(*p == NULL) /* �I�� */
		return ERR_END;
	if(err == ERR_BREAK) { /* ���f */
		gprintf("\rBREAK");
		return err;
	} else if(IS_ERROR(err)) { /* �G���[ */
		gprintf("\rERROR %d", -err);
		return err;
	} else
		return err;
}

/*
	1�s���s���� (runProg�̉�����)
*/
static int runLine(struct Basic *bas, const uint8 **p)
{
	int err;

	/* �u���b�N���䖽�߂����s���� */
	skipBlank(p);
	if(fetchKeyword(p, CODE_IF))
		err = staBlockIf(bas, p);
	else if(fetchKeyword(p, CODE_ELSE))
		err = staBlockElse(bas, p);
	else if(fetchKeyword(p, CODE_ENDIF))
		err = staEndif(bas, p);
	else if(fetchKeyword(p, CODE_SWITCH))
		err = staSwitch(bas, p);
	else if(fetchKeyword(p, CODE_CASE))
		err = staCase(bas, p);
	else if(fetchKeyword(p, CODE_DEFAULT))
		err = staDefault(bas, p);
	else if(fetchKeyword(p, CODE_ENDSWITCH))
		err = staEndswitch(bas, p);
	else
		err = ERR_OK_NEXT;
	if(err < 0) {
		gprintf("\rERROR %d", -err);
		return err;
	} else if(err == ERR_OK_JUMP)
		return err;

	/* �X�e�[�g�����g�����s���� */
	return runSta(bas, p);
}

/*
	�����񂩂�\������������ (encodeProg�̉�����)
*/
static int getKeywordFromName(int *code, const uint8 *name)
{
	const struct KeywordTable *k;
	int len;
	const uint8 *p;

	/* �ꗗ����\������������ */
	for(k = keywordTable; k->name != NULL; k++)
 		if((len = cmpKeyword(k->name, name)) > 0 && k->level <= machineSub) {
				*code = k->code;
				return len;
			}

	/* RESERVED��? */
	if(strnicmp((const char *)name, "RESERVED", 8) != 0)
		return ERR_OK;
	for(p = name + 8; *p == ' '; p++)
		;

	/* 16�i��2���̒l�𓾂� */
	if(!isxdigit(p[0]) || !isxdigit(p[1]))
		return ERR_OK;
	sscanf((const char *)p, "%02x", code);
	p += 2;

	return (int )(p - name);
}

/*
	���e����������𒆊ԃR�[�h�ɕϊ����� (encodeProg�̉�����)
*/
static void encodeStr(uint8 **dst, const uint8 **src)
{
	if(**src != '"')
		return;

	do {
		*(*dst)++ = *(*src)++;
	} while(**src != '"' && **src != '\r' && **src != '\n' && **src != 0);

	if(**src == '"')
		*(*dst)++ = *(*src)++;
}

/*
	DATA�𒆊ԃR�[�h�ɕϊ����� (encodeProg�̉�����)
*/
static void encodeData(uint8 **dst, const uint8 **src)
{
	int in_str = FALSE;

	while((**src != ':' || (**src == ':' && in_str)) && **src != '\r' && **src != '\n' && **src != 0) {
		if(**src == '"')
			in_str = !in_str;
		*(*dst)++ = *(*src)++;
	}
}

/*
	���x���𒆊ԃR�[�h�ɕϊ����� (encodeProg�̉�����)
*/
static void encodeLabel(uint8 **dst, const uint8 **src)
{
	while(**src == ' ' || **src == '\t')
		*(*dst)++ = *(*src)++;

	if(**src != '*')
		return;
	*(*dst)++ = *(*src)++;

	if(!isalpha(**src))
		return;
	*(*dst)++ = toupper(*(*src)++);

	while(isalnum(**src))
		*(*dst)++ = toupper(*(*src)++);
}

/*
	�R�����g�𒆊ԃR�[�h�ɕϊ����� (encodeProg�̉�����)
*/
static void encodeRem(uint8 **dst, const uint8 **src)
{
	while(**src != '\r' && **src != '\n' && **src != 0)
		*(*dst)++ = *(*src)++;
}

/*
	16�i���𒆊ԃR�[�h�ɕϊ����� (encodeProg�̉�����)
*/
static void encodeHex(uint8 **dst, const uint8 **src)
{
	if(**src == '&') {
		*(*dst)++ = *(*src)++;

		if(**src == 'H' || **src == 'h') {
			*(*dst)++ = toupper(*(*src)++);

			while(isxdigit(**src))
				*(*dst)++ = toupper(*(*src)++);
		}
	}
}

/*
	10�i���𒆊ԃR�[�h�ɕϊ����� (encodeProg�̉�����)
*/
static void encodeDec(uint8 **dst, const uint8 **src)
{
	const uint8 *p;

	/* ������ */
	while(isdigit(**src) || **src == '.')
		*(*dst)++ = *(*src)++;

	/* �w���������邩? */
	for(p = *src; isspace(*p); p++)
		;
	if(toupper(*p) != 'E')
		return;
	for(; *p == ' ' || *p == '\t'; p++)
		;
	if(!isdigit(*p) && *p != '+' && *p != '-')
		return;

	/* �w���� */
	while(isspace(**src))
		(*src)++;
	while(toupper(**src) == 'E')
		*(*dst)++ = toupper(*(*src)++);
	while(isspace(**src))
		(*src)++;
	while(isdigit(**src) || **src == '+' || **src == '-')
		*(*dst)++ = *(*src)++;
}

/*
	�e�L�X�g���璆�ԃR�[�h�ɕϊ����� (runProg, insertProg�̉�����)
*/
static int encodeProg(uint8 *dst, const uint8 *src, int mode)
{
	int len, code;
	const uint8 *p = src;
	uint8 *q = dst;

	encodeLabel(&q, &p);

	for(;;) {
		if(*p == '\r' || *p == '\n' || *p == 0)
			break;
		else if(*p == '\'' && mode != MODE_MAN)
			encodeRem(&q, &p);
		else if(isdigit(*p) || *p == '.')
			encodeDec(&q, &p);
		else if(*p == '&')
			encodeHex(&q, &p);
		else if(*p == '"')
			encodeStr(&q, &p);
		else if((len = getKeywordFromName(&code, p)) > 0) {
			p += len;
			if(*p == ' ' || *p == '\t')
				p++;
			*q++ = CODE_RESERVED;
			*q++ = code;

			if(
			code == CODE_REM
			)
				encodeRem(&q, &p);
			else if(
			code == CODE_DATA
			)
				encodeData(&q, &p);
			else if(
			code == CODE_RUN ||
			code == CODE_CONT ||
			code == CODE_LIST ||
			code == CODE_LLIST ||
			code == CODE_RENUM ||
			code == CODE_AUTO ||
			code == CODE_DELETE ||
			code == CODE_GOTO ||
			code == CODE_THEN ||
			code == CODE_GOSUB ||
			code == CODE_RETURN ||
			code == CODE_RESTORE ||
			code == CODE_ELSE
			)
				encodeLabel(&q, &p);
		} else if(isalnum(*p))
			while(isalnum(*p))
				*q++ = toupper(*p++);
		else
			*q++ = *p++;
	}

/*
	while(q > dst && *(q - 1) == ' ')
		q--;
*/
	if((int )(q - dst) > 0)
		*q++ = '\r';
	*q = 0;
	return (int )(q - dst);
}

/*
	�s��}������ (insertProg�̉�����)
*/
static int insertLine(uint8 *area, int area_size, int line_no, const uint8 *line, int len, uint8 **ret_p)
{
	int del, err;
	uint8 *p;
	const uint8 *q;

	/* �}����̍s�̃A�h���X�𓾂� */
	if((del = jumpToLineNo((const uint8 **)&p, area, line_no))) {
		/* ���ɓ����s������΍폜���� */
		_delete(area, area_size, p, LINE_SIZE(p));
	}

	/* ��s�Ȃ�Ζ߂� */
	for (q = line; q < line + len && (*q == ' ' || *q == '\t'); q++)
		;
	if(q >= line + len || *q == '\r' || *q == '\n' || *q == 0x1a || *q == 0) {
		if(ret_p != NULL)
			*ret_p = (del ? p: NULL);
		return 1;
	}

	/* �s��}������ */
	if ((err = _insert(area, area_size, p, len + 3)) < 0)
		return err;

	p[0] = (line_no >> 8) & 0xff;
	p[1] = line_no & 0xff;
	p[2] = len;
	memcpy(&p[3], line, len);

	if(ret_p != NULL)
		*ret_p = p;
	return 0;
}

/*
	�v���O������}������
*/
int insertProg(struct Basic *bas, const uint8 *line, int *ret_line_no, uint8 **ret_prog)
{
	int line_no, len, err;
	uint8 buf[256 + 3], *q = buf;
	const uint8 *p = line;

	if(ret_line_no != NULL)
		*ret_line_no = 0;
	if(ret_prog != NULL)
		*ret_prog = NULL;

	/* ��s�Ȃ�Ζ߂� */
	skipBlank(&p);
	if(*p == '\r' || *p == '\n' || *p == 0x1a || *p == 0)
		return ERR_OK;

	/* �s�ԍ��𓾂� */
	if(!isLineNo(&p))
		return ERR_12;
	if((line_no = fetchLineNoOnly(&p)) >= 0xff00)
		return ERR_41;

	/* �v���O�����𒆊ԃR�[�h�ɕϊ����� */
	if(*p == ' ' || *p == '\t')
		p++;
	if((len = encodeProg(q, p, MODE_PRO)) < 0)
		return len;
	if(bas == NULL)
		return ERR_OK;

	/* �v���O������}������ */
	err = insertLine(bas->prog, bas->prog_size, line_no, buf, len, ret_prog);
	if(ret_line_no != NULL) {
		if(err == 0)
			*ret_line_no = line_no;
		else
			*ret_line_no = -1;
	}

	return err >= 0 ? ERR_OK: err;
}

/*
	BASIC�v���O������ǂݍ���
*/
int inportBas(const char *path)
{
	return copyFile(path, pathBasic);
}

/*
	BASIC�v���O��������������
*/
int exportBas(const char *path)
{
	return copyFile(pathBasic, path);
}

/*
	�v���O��������������
*/
static int clearBas(struct Basic *bas)
{
	if(bas == NULL)
		return FALSE;

	*bas->prog = 0xff;
	initFormat(bas);
	return TRUE;
}

/*
	�v���O������ǂݍ���
*/
static int loadBas(struct Basic *bas)
{
	FILE *fp = NULL;
	int i, line_no = 0;
	char buf[256 + 3];

	clearBas(bas);

	for(i = 0; i < sizeof(bas->fp) / sizeof(bas->fp[0]); i++) {
		fclose(bas->fp[i]);
		bas->fp[i] = NULL;
	}

	if((fp = fopen(pathBasic, "r")) == NULL)
		return FALSE;

	while(fgets(buf, sizeof(buf), fp) != NULL) {
		if(!isdigit((unsigned char )buf[0])) {
			char buf_line_no[8];

			line_no = jumpToLast(NULL, bas->prog) + 10;
			sprintf(buf_line_no, "%d", line_no);
			if(strlen(buf) + strlen(buf_line_no) >= sizeof(buf))
				goto fail;

			memmove(buf + strlen(buf_line_no), buf, strlen(buf) + 1);
			memcpy(buf, buf_line_no, strlen(buf_line_no));
		}

		if(insertProg(bas, (uint8 *)buf, NULL, NULL) < 0)
			goto fail;
	}

	if(!feof(fp))
		goto fail;

	fclose(fp);
	return TRUE;

fail:;
	if(fp != NULL)
		fclose(fp);
	return FALSE;
}

/*
	�v���O��������������
*/
static int saveBas(struct Basic *bas)
{
	FILE *fp;
	int len, size, line_no;
	uint8 buf[256];
	const uint8 *prog = bas->prog;

	if((fp = fopen(pathBasic, "w")) == NULL)
		return FALSE;

	for(;;) {
		if((len = decodeLineNo(&line_no, &size, prog)) == 0)
			break;
		prog += len;
		prog += decodeProg(buf, prog);

		fprintf(fp, "%d%.*s\n", line_no, strlen(buf), buf);
	}

	fclose(fp);
	return TRUE;
}

/*
	�v���O���������s����
*/
int runProg(struct Basic *bas, uint8 *buf)
{
	int err, len, size, type = 0;
	uint8 *ans = NULL, prog[256], k;
	const uint8 *p;

	skipBlank(&p);
	p = buf;

	/* ���ԃR�[�h�ɕϊ����� */
	if((len = encodeProg(prog, p, MODE_MAN)) <= 0)
		return 1;
	decodeProg(buf, prog);
	gprintf("%s\r", buf);

	p = prog;
	if((err = runSta1(bas, &p)) == ERR_OK_JUMP || err == ERR_OK_CONT) {
		/* �v���O���������s���� */
		if(err == ERR_OK_CONT)
			goto cont;
		bas->p = p;

		do {
			if((len = decodeLineNo(&bas->line_no, &size, bas->p)) == 0)
				break;
			bas->p += len;
cont:;
			skipBlank(&bas->p);
			if(peekSymbol(&bas->p, "*"))
				fetchLabel(&bas->p);
/*
			printf("LINE=%d\n", bas->line_no);
*/
		} while((err = runLine(bas, &bas->p)) >= 0);

		if(IS_ERROR(err) || err == ERR_BREAK)
			gprintf(" IN %d\r", bas->line_no);

/*
		if(err != ERR_BREAK)
			bas->p = NULL;
		if(bas->p == NULL)
			bas->line_no = 0;
*/

		buf[0] = 0;
		return 0;
	} else if(err == ERR_OK_NEXT || err == ERR_END) {
		/* �}�j���A�����[�h�ł̎��s������I�� */
		if(isLineTerm(&p)) {
			buf[0] = 0;
			return 0;
		}

		err = ERR_10;
	} else if(err == ERR_10) {
		if(*mode & MODE_RUN) {
			/* �l�����߂� */
			p = prog;
			if((err = fetchParam(bas, &ans, &type, &p)) == 0)
				if(!isLineTerm(&p))
					err = ERR_10;
		} else {
			/* �v���O�������[�h */
			err = ERR_12;
		}
	}

	/* �G���[��? */
	if(IS_ERROR(err)) {
		gprintf("\rERROR %d\r", -err);

		for(;;) {
			if((k = getKeycode()) == GKEY_CLS) {
				buf[0] = 0;
				break;
			} else if(k == GKEY_LEFT || k == GKEY_RIGHT)
				break;
		}
		return err;
	} else if(err < 0)
		return err;

	/* ���ʂ�\������ */
	if(type == TYPE_NUM) {
		numCorrect(ans);
		numLet(answer, ans);

		numCorrect9(ans);
		decodeNum(prog, ans);
		gprintf("%24s\r", prog);
	} else {
		strcpy(prog, "");
		gprintf("%s\r", ans);
	}

	if((k = getKeycode()) == GKEY_LEFT || k == GKEY_RIGHT)
		return 0;
	else if(k == GKEY_RETURN)
		strcpy(prog, "");
	strcpy(buf, prog);
	return 0;
}

/*
	BASIC�v���O������\������ (basRun, basPro�̉�����)
*/
static void browseProg(struct Basic *bas, const uint8 *sep, const uint8 *cur)
{
	int pos = 0;
	uint8 buf[512];
	const uint8 *p;

	if(cur != NULL)
		while(*cur != 0xff && pos < lcdCols * lcdRows) {
			cur += decodeLineNoProg(buf, cur, sep);

			for(p = buf; *p != 0; p++) {
				putchr(pos % lcdCols, pos / lcdCols, *p);
				pos++;
			}
			while(pos % lcdCols > 0) {
				putchr(pos % lcdCols, pos / lcdCols, ' ');
				pos++;
			}
		}

	glocate(0, pos / lcdCols);

	while(pos < lcdCols * lcdRows) {
		putchr(pos % lcdCols, pos / lcdCols, ' ');
		pos++;
	}
}

/*
	�w��̍s�ֈړ����� (basRun, basPro�̉�����)
*/
static int jumpLine(struct Basic *bas, uint8 **cur, int line_no)
{
	uint8 *p;

	if(!jumpToLineNo((const uint8 **)&p, bas->prog, line_no))
		return FALSE;

	if(cur != NULL)
		*cur = p;
	return TRUE;
}

/*
	BASIC�C���^�[�v���^(RUN MODE)���N������
*/
int basRun(struct Basic *bas)
{
	int err, newline;
	uint8 *cur, ch, buf[512] = "";

	*mode = MODE_RUN;
	clearCont(bas);

	gcls();
	gprintf("RUN MODE\r");

	while(*mode == MODE_RUN && !isoff()) {
		newline = (buf[0] == 0);

		switch((ch = ggetline(buf, (const uint8 *)(newline ? ">": ""), GETLINE_MAN))) {
		case 0x0c: /* CLS */
			buf[0] = 0;
			gcls();
			break;
		case 0x0d: /* RETURN */
			err = runProg(bas, buf);

			if(err > 0 && newline)
				gprintf("\r");
			break;
		case 0x1e: /* �� */
			if(bas->line_no <= 0)
				break;
			if(!jumpLine(bas, &cur, bas->line_no))
				break;
			browseProg(bas, (const uint8 *)":", cur);
			waitRelease();
			gcls();
			break;
		case 0x1c: /* �� */
		case 0x1d: /* �� */
		case 0x1f: /* �� */
			break;
		default:
			setMode(ch);
			break;
		}
	}
	return 0;
}

/*
	1�s�\������ (basPro�̉�����)
*/
static int dispLine(int row, const uint8 *cur)
{
	int size, len;
	uint8 buf[512], *p = buf;

	decodeLineNoProg(buf, cur, " ");
	size = len = strlen(buf);

	while(row < 0) {
		row++;
		p += lcdCols;
		len -= lcdCols;
	}

	if(len <= 0)
		return size;

	len = (len + lcdCols - 1) / lcdCols * lcdCols;
	if(len > (lcdRows - row) * lcdCols)
		len = (lcdRows - row) * lcdCols;

	glocate(0, row);
	gprintf("%-.*s", len, p);
	return size;
}

/*
	���̍s�ֈړ����� (basPro�̉�����)
*/
static int jumpNext(struct Basic *bas, uint8 **cur, int *pos, int *row)
{
	int len;

	/* ������? */
	if(*cur == NULL || **cur == 0xff)
		return FALSE;

	/* ���̍s�ֈړ����� */
	if(row != NULL) {
		len = dispLine(*row, *cur);

		*row += ((len - 1) / lcdCols + 1);
		while(*row >= lcdRows) {
			scrup();
			(*row)--;
		}
	}

	if(LINE_SIZE(*cur) == 0)
		return FALSE;
	*cur += LINE_SIZE(*cur);

	if(row != NULL) {
		uint8 buf[512];

		decodeLineNoProg(buf, *cur, " ");
		len = strlen(buf);

		if(len > (*pos % lcdCols))
			*pos %= lcdCols;
		else
			*pos = len;
	}
	return TRUE;
}

/*
	�O�̍s�ֈړ����� (basPro�̉�����)
*/
static int jumpPrev(struct Basic *bas, uint8 **cur, int *pos, int *row)
{
	uint8 *prev_cur = *cur, *p;

	/* �擪��? */
	if(*cur == bas->prog)
		return FALSE;

	/* �O�̍s�ֈړ����� */
	if(row != NULL)
		dispLine(*row, *cur);

	for(p = bas->prog; p != prev_cur && p < *cur; jumpNext(bas, &p, NULL, NULL))
		prev_cur = p;
	*cur = prev_cur;

	if(row != NULL) {
		int len, height;
		uint8 buf[512];

		decodeLineNoProg(buf, *cur, " ");
		len = strlen(buf);
		height = len / lcdCols + 1;

		*row -= height;
		while(*row + height < 0) {
			scrdown(0, 0);
			(*row)++;
		}

		*pos = (len / lcdCols) * lcdCols + (*pos % lcdCols);
		if(len < *pos)
			*pos = len;
	}
	return TRUE;
}

/*
	�w��̍s�ֈړ����� (basPro�̉�����)
*/
static int jumpTo(struct Basic *bas, uint8 *prev, uint8 *cur, int *pos, int *row)
{
	/* �ړ����� */
	if(prev > cur) {
		while(prev > cur)
			if(!jumpPrev(bas, &prev, pos, row))
				break;
	} else if(prev < cur) {
		while(prev < cur)
			if(!jumpNext(bas, &prev, pos, row))
				break;
	}

	if(row != NULL) {
		dispLine(*row, cur);
	}
	return TRUE;
}

/*
	�Ō�̍s�ֈړ����� (basPro�̉�����)
*/
static int jumpLast(struct Basic *bas, uint8 **cur, int *row)
{
	uint8 *p = bas->prog, *next_p = bas->prog;

	for(;;) {
		p = next_p;

		if(p >= bas->prog + bas->prog_size)
			break;
		if(IS_LAST(p))
			break;
		if(!jumpNext(bas, &next_p, NULL, NULL))
			break;
		if(IS_LAST(next_p))
			break;
	}

	*cur = p;
	return TRUE;
}

/*
	BASIC�C���^�[�v���^(PROGRAM MODE)���N������
*/
int basPro(struct Basic *bas)
{
	int err, pos = -1, row = -1, mod = FALSE, line_no;
	uint8 ch, buf[512] = "", *cur = NULL, *prev = NULL;

	*mode = MODE_PRO;
	bas->auto_step = 0;
	loadBas(bas);

	gcls();
	gprintf("PROGRAM MODE\r");

	while(*mode == MODE_PRO && !isoff()) {
		if(bas->auto_step > 0 && !jumpLine(bas, NULL, bas->auto_line_no)) { /* �������� */
			int tmp_pos, tmp_row = *curRow, tmp_mod;

			row = pos = -1;
			cur = NULL;

			sprintf((char *)buf, "%d ", bas->auto_line_no);
			tmp_pos = strlen(buf);
			bas->auto_line_no += bas->auto_step;
			ch = ggetline(buf, NULL, GETLINE_PRO, &tmp_pos, &tmp_row, &tmp_mod);
		} else if(cur == NULL) { /* ���͑҂� */
			strcpy(buf, "");
			ch = ggetline(buf, (const uint8 *)">", GETLINE_MAN);
		} else if(pos < 0) { /* �{���� */
			strcpy(buf, "");
			ch = ggetline(buf, NULL, GETLINE_MAN);
		} else { /* �ҏW�� */
			decodeLineNoProg(buf, cur, " ");
			ch = ggetline(buf, NULL, GETLINE_PRO, &pos, &row, &mod);
		}

		prev = cur;

		switch(ch) {
		case 0x0c: /* CLS */
			gcls();
		case 0x05: /* BREAK */
			/* �{���E�ҏW�E�������͏I�� */
			cur = NULL;
			bas->auto_step = 0;
			break;
		case 0x1c: /* �� */
			if(cur == NULL) /* ���͑҂� */
				; /* �������Ȃ� */
			else { /* �{���� */
				row = pos = 0; /* �ҏW�J�n */
				browseProg(bas, (const uint8 *)" ", cur);
			}
			break;
		case 0x0d: /* RETURN */
			if((err = insertProg(bas, buf, &line_no, &cur)) >= 0 && cur != NULL) { /* �}�� */
				/* �ۑ����� */
				saveBas(bas);
				clearCont(bas);

				if(pos < 0) { /* �{���� */
					/* ���͂����s��\������ */
					if(line_no > 0) {
						decodeLineNoProg(buf, cur, ":");
						gprintf("%s", buf);
						moveCursor('\r');
					}
				} else { /* �ҏW�� */
					if(cur != prev)
						jumpTo(bas, prev, cur, &pos, &row);
					else
						jumpNext(bas, &cur, &pos, &row);
					pos = 0;
				}
			} else if(err >= 0) { /* ��s */
				moveCursor('\r');
				pos = bas->auto_step = -1; /* �ҏW�E�������͏I�� */
			} else if((err = runProg(bas, buf)) == ERR_AUTO) { /* �������͊J�n */
				moveCursor('\r');
			} else if(err == ERR_LIST) { /* �{���J�n */
				jumpLine(bas, &cur, bas->line_no);
				clearCont(bas);
				pos = bas->auto_step = -1; /* �ҏW�E�������͏I�� */
			} else if(err > 0) { /* ����I�� */
				saveBas(bas);
				clearCont(bas);
				moveCursor('\r');
				cur = NULL, pos = bas->auto_step = -1; /* �{���E�ҏW�E�������͏I�� */
			} else { /* �G���[ */
				cur = NULL, pos = bas->auto_step = -1; /* �{���E�ҏW�E�������͏I�� */
			}
			break;
		case 0x1e: /* �� */
			if(cur == NULL) { /* ���͑҂� */
				if(bas->line_no <= 0 || !jumpLine(bas, &cur, bas->line_no))
					if(jumpLast(bas, &cur, &row)) /* ������ */
						browseProg(bas, (const uint8 *)":", cur);
			} else if(pos < 0) { /* �{���� */
				jumpPrev(bas, &cur, NULL, NULL); /* �O�̍s�� */
				browseProg(bas, (const uint8 *)":", cur);
			} else { /* �ҏW�� */
				/* �ύX���ꂽ�Ȃ�Α}������ */
				if(mod)
					if(insertProg(bas, buf, &line_no, &cur) >= 0) {
						saveBas(bas);
						clearCont(bas);
					}

				/* �O�̍s�ֈړ����� */
				if(cur != prev)
					jumpTo(bas, prev, cur, &pos, &row);
				jumpPrev(bas, &cur, &pos, &row);
			}
			break;
		case 0x1f: /* �� */
			if(cur == NULL) { /* ���͑҂� */
				if(bas->line_no <= 0 || !jumpLine(bas, &cur, bas->line_no)) {
					cur = bas->prog; /* �擪�� */
					browseProg(bas, (const uint8 *)":", cur);
				}
			} else if(pos < 0) { /* �{���� */
				jumpNext(bas, &cur, NULL, NULL); /* ���̍s�� */
				browseProg(bas, (const uint8 *)":", cur);
			} else { /* �ҏW�� */
				/* �ύX���ꂽ�Ȃ�Α}������ */
				if(mod)
					if(insertProg(bas, buf, &line_no, &cur) >= 0) {
						saveBas(bas);
						clearCont(bas);
					}

				/* ���̍s�ֈړ����� */
				if(cur != prev)
					jumpTo(bas, prev, cur, &pos, &row);
				jumpNext(bas, &cur, &pos, &row);
			}
			break;
		default:
			setMode(ch);
			break;
		}
	}
	return 0;
}

/*
	BASIC�C���^�[�v���^������������
*/
int initBasic(struct Basic *bas)
{
	int i;

	memset(bas, 0, sizeof(*bas));

	for(i = 0; i < sizeof(valueStack) / sizeof(valueStack[0]); i++)
		valueStack[i] = malloc(256);
	valueSp = &valueStack[-1];
	typeSp = &typeStack[-1];

	bas->top = &bas->stack[-1];
	bas->fixed_var = (uint8 (*)[SIZEOF_NUM])&memory[0x7800];

	bas->vars = malloc(sizeof(bas->vars));
	*bas->vars = NULL;

	bas->prog_size = 0x8000;
	bas->prog = malloc(bas->prog_size);
	*bas->prog = 0xff;
	bas->p = NULL;
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
