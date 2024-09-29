/*
	SHARP PC-G800 series Emulator
	IOCSエミュレート
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "g800.h"

/* キーコード -> ASCIIコード変換テーブル */
const static uint8 keycode2ascii_normal[] = {
	0x00,
	0x06,	/* OFF */
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
	0x01,	/* BASIC */
	0x02,	/* TEXT */
	0x14,	/* CAPS */
	0x11,	/* カナ */
	0x0a,	/* TAB */
	0x20,	/* SPACE */
	0x1f,	/* ↓ */

	0x1e,	/* ↑ */
	0x1d,	/* ← */
	0x1c,	/* → */
	0x15,	/* ANS */
	0x30,	/* 0 */
	0x2e,	/* . */
	0x3d,	/* = */
	0x2b,	/* + */
	0x0d,	/* RETURN */
	0x4c,	/* L */
	0x3b,	/* ; */
	0x17,	/* CONST */
	0x31,	/* 1 */
	0x32,	/* 2 */
	0x33,	/* 3 */
	0x2d,	/* - */

	0x1a,	/* M+ */
	0x49,	/* I */
	0x4f,	/* O */
	0x12,	/* INS */
	0x34,	/* 4 */
	0x35,	/* 5 */
	0x36,	/* 6 */
	0x2a,	/* * */
	0x19,	/* R・CM */
	0x50,	/* P */
	0x08,	/* BS */
	0xfe,	/* π */
	0x37,	/* 7 */
	0x38,	/* 8 */
	0x39,	/* 9 */
	0x2f,	/* / */

	0x29,	/* ) */
	0xfe,	/* nPr */
	0xfe,	/* →DEG */
	0xfe,	/* √ */
	0xfe,	/* x^2 */
	0x5e,	/* ^ */
	0x28,	/* ( */
	0xfe,	/* 1/x */
	0xfe,	/* MDF */
	0x10,	/* 2ndF */
	0xfe,	/* sin */
	0xfe,	/* cos */
	0xfe,	/* ln */
	0xfe,	/* log */
	0xfe,	/* tan */
	0x0f,	/* F←→E */

	0x0c,	/* CLS */
	0x05,	/* BREAK */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,

	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,

	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,

	0x00,
	0x06,	/* SHIFT + OFF */
	0x21,	/* SHIFT + Q */
	0x22,	/* SHIFT + W */
	0x23,	/* SHIFT + E */
	0x24,	/* SHIFT + R */
	0x25,	/* SHIFT + T */
	0x26,	/* SHIFT + Y */
	0x27,	/* SHIFT + U */
	0x5b,	/* SHIFT + A */
	0x5d,	/* SHIFT + S */
	0x7b,	/* SHIFT + D */
	0x7d,	/* SHIFT + F */
	0x5c,	/* SHIFT + G */
	0x7c,	/* SHIFT + H */
	0x7e,	/* SHIFT + J */

	0x5f,	/* SHIFT + K */
	0xfe,	/* SHIFT + Z */
	0xfe,	/* SHIFT + X */
	0xfe,	/* SHIFT + C */
	0xfe,	/* SHIFT + V */
	0xfe,	/* SHIFT + B */
	0xfe,	/* SHIFT + N */
	0xfe,	/* SHIFT + M */
	0x3f,	/* SHIFT + , */
	0xf0,	/* SHIFT + BASIC */
	0x03,	/* SHIFT + TEXT */
	0x14,	/* SHIFT + CAPS */
	0x11,	/* SHIFT + カナ */
	0x0a,	/* SHIFT + TAB */
	0x20,	/* SHIFT + SPACE */
	0x1f,	/* SHIFT + ↓ */

	0x1e,	/* SHIFT + ↑ */
	0x1d,	/* SHIFT + ← */
	0x1c,	/* SHIFT + → */
	0xf2,	/* SHIFT + ANS */
	0x30,	/* SHIFT + 0 */
	0x13,	/* SHIFT + . */
	0x45,	/* SHIFT + = */
	0x2b,	/* SHIFT + + */
	0x07,	/* SHIFT + RETURN */
	0x3d,	/* SHIFT + L */
	0x3a,	/* SHIFT + ; */
	0x18,	/* SHIFT + CONST */
	0x31,	/* SHIFT + 1 */
	0x32,	/* SHIFT + 2 */
	0x33,	/* SHIFT + 3 */
	0x16,	/* SHIFT + - */

	0x1b,	/* SHIFT + M+ */
	0x3c,	/* SHIFT + I */
	0x3e,	/* SHIFT + O */
	0x09,	/* SHIFT + INS */
	0x34,	/* SHIFT + 4 */
	0x35,	/* SHIFT + 5 */
	0x36,	/* SHIFT + 6 */
	0x2a,	/* SHIFT + * */
	0x19,	/* SHIFT + R・CM */
	0x40,	/* SHIFT + P */
	0x08,	/* SHIFT + BS */
	0xfe,	/* SHIFT + π */
	0xdf,	/* SHIFT + 7 */
	0x27,	/* SHIFT + 8 */
	0xf8,	/* SHIFT + 9 */
	0x2f,	/* SHIFT + / */

	0xf1,	/* SHIFT + ) */
	0xfe,	/* SHIFT + nPr */
	0xfe,	/* SHIFT + →DEG */
	0xfe,	/* SHIFT + √ */
	0xfe,	/* SHIFT + x^2 */
	0xfe,	/* SHIFT + ^ */
	0xfe,	/* SHIFT + ( */
	0xfe,	/* SHIFT + 1/x */
	0x04,	/* SHIFT + MDF */
	0x10,	/* SHIFT + 2ndF */
	0xfe,	/* SHIFT + sin */
	0xfe,	/* SHIFT + cos */
	0xfe,	/* SHIFT + ln */
	0xfe,	/* SHIFT + log */
	0xfe,	/* SHIFT + tan */
	0x0e,	/* SHIFT + F←→E */

	0x0b,	/* SHIFT + CLS */
	0x05,	/* SHIFT + BREAK */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,

	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00
};
const static uint8 keycode2ascii_caps[] = {
	0x00,
	0x06,	/* CAPS + OFF */
	0x71,	/* CAPS + Q */
	0x77,	/* CAPS + W */
	0x65,	/* CAPS + E */
	0x72,	/* CAPS + R */
	0x74,	/* CAPS + T */
	0x79,	/* CAPS + Y */
	0x75,	/* CAPS + U */
	0x61,	/* CAPS + A */
	0x73,	/* CAPS + S */
	0x64,	/* CAPS + D */
	0x66,	/* CAPS + F */
	0x67,	/* CAPS + G */
	0x68,	/* CAPS + H */
	0x6a,	/* CAPS + J */

	0x6b,	/* CAPS + K */
	0x7a,	/* CAPS + Z */
	0x78,	/* CAPS + X */
	0x63,	/* CAPS + C */
	0x76,	/* CAPS + V */
	0x62,	/* CAPS + B */
	0x6e,	/* CAPS + N */
	0x6d,	/* CAPS + M */
	0x2c,	/* CAPS + , */
	0x01,	/* CAPS + BASIC */
	0x02,	/* CAPS + TEXT */
	0x14,	/* CAPS + CAPS */
	0x11,	/* CAPS + カナ */
	0x0a,	/* CAPS + TAB */
	0x20,	/* CAPS + SPACE */
	0x1f,	/* CAPS + ↓ */

	0x1e,	/* CAPS + ↑ */
	0x1d,	/* CAPS + ← */
	0x1c,	/* CAPS + → */
	0x15,	/* CAPS + ANS */
	0x30,	/* CAPS + 0 */
	0x2e,	/* CAPS + . */
	0x3d,	/* CAPS + = */
	0x2b,	/* CAPS + + */
	0x0d,	/* CAPS + RETURN */
	0x6c,	/* CAPS + L */
	0x3b,	/* CAPS + ; */
	0x17,	/* CAPS + CONST */
	0x31,	/* CAPS + 1 */
	0x32,	/* CAPS + 2 */
	0x33,	/* CAPS + 3 */
	0x2d,	/* CAPS + - */

	0x1a,	/* CAPS + M+ */
	0x69,	/* CAPS + I */
	0x6f,	/* CAPS + O */
	0x12,	/* CAPS + INS */
	0x34,	/* CAPS + 4 */
	0x35,	/* CAPS + 5 */
	0x36,	/* CAPS + 6 */
	0x2a,	/* CAPS + * */
	0x19,	/* CAPS + R・CM */
	0x70,	/* CAPS + P */
	0x08,	/* CAPS + BS */
	0xfe,	/* CAPS + π */
	0x37,	/* CAPS + 7 */
	0x38,	/* CAPS + 8 */
	0x39,	/* CAPS + 9 */
	0x2f,	/* CAPS + / */

	0x29,	/* CAPS + ) */
	0xfe,	/* CAPS + nPr */
	0xfe,	/* CAPS + →DEG */
	0xfe,	/* CAPS + √ */
	0xfe,	/* CAPS + x^2 */
	0x5e,	/* CAPS + ^ */
	0x28,	/* CAPS + ( */
	0xfe,	/* CAPS + 1/x */
	0xfe,	/* CAPS + MDF */
	0x10,	/* CAPS + 2ndF */
	0xfe,	/* CAPS + sin */
	0xfe,	/* CAPS + cos */
	0xfe,	/* CAPS + ln */
	0xfe,	/* CAPS + log */
	0xfe,	/* CAPS + tan */
	0x0f,	/* CAPS + F←→E */

	0x0c,	/* CAPS + CLS */
	0x05,	/* CAPS + BREAK */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,

	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,

	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00
};

/* 次に表示する文字の位置 */
static uint8 *curCol = &memory[0x7920], *curRow = &memory[0x7921];
/* 最後に表示した文字の位置 */
static uint8 *lastCol = &memory[0x7922], *lastRow = &memory[0x7923];
/* 改行しないか? */
static uint8 *noWrap = &memory[0x797d];

/*
	電源を停止する
*/
void poweroff(void)
{
	ioReset = 0;
	z80.r.iff  = 0;
	z80.r.halt = 1;
}

/*
	パターンを表示する
*/
void putpat(uint8 col, uint8 row, const uint8 *pat, uint8 len)
{
	if(cellHeight == 8)
		memcpy(VRAM_CR(col, row), pat, len);
	else {
		uint8 *p, *start = VRAM_CR(col, row);
		const uint8 *q;

		for(p = start, q = pat; p < start + len; p++, q++)
			*p = *q & 0x7f;
	}
}

/*
	rowを消去する
*/
static inline void clrline(uint8 row)
{
	memset(VRAM_CR(0, row), 0, vramWidth - 1);
}

/*
	上にスクロールする
*/
void scrup(void)
{
	uint8 tmp;

	tmp = vram[vramWidth * 7 + lcdWidth];
	vram[vramWidth * 7 + lcdWidth] = vram[vramWidth * 6 + lcdWidth];
	vram[vramWidth * 6 + lcdWidth] = vram[vramWidth * 5 + lcdWidth];
	vram[vramWidth * 5 + lcdWidth] = vram[vramWidth * 4 + lcdWidth];
	vram[vramWidth * 4 + lcdWidth] = vram[vramWidth * 3 + lcdWidth];
	vram[vramWidth * 3 + lcdWidth] = vram[vramWidth * 2 + lcdWidth];
	vram[vramWidth * 2 + lcdWidth] = vram[vramWidth * 1 + lcdWidth];
	vram[vramWidth * 1 + lcdWidth] = vram[vramWidth * 0 + lcdWidth];
	vram[vramWidth * 0 + lcdWidth] = tmp;

	clrline(0);
	switch(machine) {
	case MACHINE_E200:
		memory[0x790d] = (memory[0x790d] + 1) % 8;
		z80outport(NULL, 0x58, (memory[0x790d] << 3) | 0xc0);
		break;
	case MACHINE_G815:
		memory[0x790d] = (memory[0x790d] + 1) % 8;
		z80outport(NULL, 0x50, (memory[0x790d] << 3) | 0xc0);
		break;
	case MACHINE_G850:
		clrline(6);
		clrline(7);
		memory[0x790d] = (memory[0x790d] + 1) % vramRows;
		z80outport(NULL, 0x40, (memory[0x790d] * 8) % (vramRows * 8) | 0x40);
		break;
	}
}

/*
	下にスクロールする
*/
void scrdown(uint8 row, uint8 col)
{
	int n;
	uint8 r;

	n = lcdWidth - row * cellWidth;
	for(r = vramRows - 1; r != row; r--)
		memcpy(VRAM_CR(col, r), VRAM_CR(col, r - 1), n);
	clrline(r);
}

/*
	画面全体を消去する
*/
void clrall(void)
{
	uint8 row;

	for(row = 0; row != vramCols; row++)
		clrline(row);
}

/*
	最初の文字を表示する
*/
void putchr(uint8 col, uint8 row, uint8 chr)
{
	uint8 pat[7];

	if(row >= lcdRows || col >= lcdCols)
		return;

	memset(pat, 0, cellWidth);
	memcpy(pat, &font[(int)chr * 5], 5);
	putpat(col, row, pat, cellWidth);
}

/*
	次の行を求める
*/
int nextRow(uint8 *col, uint8 *row)
{
	*col = 0;
	if(*row >= lcdRows - 1) {
		*row = lcdRows - 1;
		return TRUE;
	} else {
		(*row)++;
		return FALSE;
	}
}

/*
	次の列を求める
*/
int nextCol(uint8 *col, uint8 *row)
{
	if(*col >= lcdCols - 1)
		return nextRow(col, row);
	else {
		(*col)++;
		return FALSE;
	}
}

/*
	前の列を求める
*/
void prevCol(uint8 *col, uint8 *row)
{
	if(*col == 0) {
		if(*row == 0)
			return;
		*col = lcdCols - 1;
		(*row)--;
	} else
		(*col)--;
}

/*
	2つ目以降の文字を表示する
*/
int putchrNext(uint8 *col, uint8 *row, uint8 chr)
{
	if(nextCol(col, row)) {
		scrup();
		putchr(*col, *row, chr);
		return TRUE;
	} else {
		putchr(*col, *row, chr);
		return FALSE;
	}
}

/*
	文字列を表示する
*/
int putstr(uint8 col, uint8 row, void *str, ...)
{
	va_list v;
	char buf[512], *p;

	va_start(v, str);
	vsprintf(buf, str, v);
	va_end(v);

	for(p = buf; *p != 0; p++) {
		putchr(col++, row, *p);

		if(col >= lcdCols && *p != 0) {
			col = 0;
			row++;
		}
	}
	return row;
}

/*
	表示位置を決める
*/
void glocate(uint8 col, uint8 row)
{
	*curCol = col;
	*curRow = row;
	*noWrap = 1;
}

/*
	カーソルを移動する
*/
int moveCursor(uint8 ctrl)
{
	if(ctrl == 0x0d) { /* RETURN */
		*curCol = 0;
		(*curRow)++;
	} else if(ctrl == 0x1c) { /* → */
		if((int8 )++*curCol >= lcdCols) {
			*curCol = 0;
			(*curRow)++;
		}
	} else if(ctrl == 0x1d) { /* ← */
		if((int8 )--*curCol < 0) {
			*curCol = lcdCols - 1;
			(*curRow)--;
		}
	}

	if((int8 )*curRow < 0) {
		*curRow = 0;
		scrdown(0, 0);
		return -1;
	} else if((int8 )*curRow >= lcdRows) {
		*curRow = lcdRows - 1;
		scrup();
		return 1;
	} else
		return 0;
}

/*
	1文字表示する
*/
int gputchr(uint8 chr)
{
	int scroll;

	if(*noWrap == 0) {
		*curCol = 0;
		*curRow = *lastRow + 1;
	}
	if(*curRow >= lcdRows) {
		*curCol = 0;
		*curRow = lcdRows - 1;
		scrup();
		scroll = TRUE;
	} else
		scroll = FALSE;
	*lastCol = *curCol;
	*lastRow = *curRow;
	*noWrap = 1;

	if(chr == 0)
		return scroll;

	putchr(*curCol, *curRow, chr);
	updateLCD();

	(*curCol)++;
	if(*curCol >= lcdCols) {
		*curCol = 0;
		(*curRow)++;
	}
	return scroll;
}

/*
	文字列を表示する
*/
void gprintf(const char *str, ...)
{
	va_list v;
	uint8 buf[512], *p;

	va_start(v, str);
	vsprintf((char *)buf, str, v);
	va_end(v);

	for(p = buf; *p != 0; p++)
		if(*p >= 0x20)
			gputchr(*p);
		else if(*p == 0x0d)
			*noWrap = 0;
		else if(*p == 0x1c || *p == 0x1d)
			moveCursor(*p);
		else
			gputchr(' ');
}

/*
	画面全体を消去する
*/
void gcls()
{
	clrall();
	glocate(0, 0);
}

/*
	LCD上にドットがあるか調べる
*/
uint8 point(int16 x, int16 y)
{
	/*
	printf("POINT(%d,%d)\n", x, y);
	fflush(stdout);
	*/

	if(x < 0 || y < 0 || x >= lcdWidth || y >= lcdHeight)
		return 0;

	return *VRAM_XY(x, y);
}

/*
	LCD上に点を描く
*/
void pset(int16 x, int16 y, uint8 mode)
{
	uint8 mask, *p;

	if(x < 0 || y < 0 || x >= lcdWidth || y >= lcdHeight)
		return;
	if(machine == MACHINE_E200 && (y % 8 == 7))
		return;

	p = VRAM_XY(x, y);
	mask = 1 << (y % 8);

	switch(mode) {
	case 0:
		*p &= ~mask;
		break;
	case 1:
		*p |= mask;
		break;
	default:
		*p ^= mask;
		break;
	}
}

/*
	ステータスを描く
*/
void putstatus(int status, int on_off)
{
	uint8 *p = VRAM_CR(lcdCols, machineInfo[machine]->status_row[status]);
	uint8 mask = machineInfo[machine]->status_mask[status];

	if(on_off)
		*p |= mask;
	else
		*p &= ~mask;
}

/*
	16bitの変数を交換する
*/
static inline void swap16(void *x, void *y)
{
	uint16 tmp;

	tmp = *(uint16 *)x;
	*(uint16 *)x  = *(uint16 *)y;
	*(uint16 *)y  = tmp;
}

/*
	長方形のクリッピングを行う
*/
static void clip(int16 *x1, int16 *y1, int16 *x2, int16 *y2)
{
	*x1 = (*x1 < 0 ? 0: (*x1 >= lcdWidth  ? lcdWidth  - 1: *x1));
	*y1 = (*y1 < 0 ? 0: (*y1 >= lcdHeight ? lcdHeight - 1: *y1));
	*x2 = (*x2 < 0 ? 0: (*x2 >= lcdWidth  ? lcdWidth  - 1: *x2));
	*y2 = (*y2 < 0 ? 0: (*y2 >= lcdHeight ? lcdHeight - 1: *y2));
	if(*x1 > *x2) 
		swap16(x1, x2);
	if(*y1 > *y2)
		swap16(y1, y2);
}

/*
	左シフトする (下請け)
*/
static uint16 rotate(uint16 *mask)
{
	uint16 prev = *mask;

	if((*mask = (*mask << 1)) == 0)
		*mask = 0x0001;
	return prev;
}

/*
	LCD上に線を描く
*/
void line(int16 x1, int16 y1, int16 x2, int16 y2, uint8 mode, uint16 pat)
{
	int dx, dx0, dy, dy0, e;
	int16 x, y;
	uint16 mask = 0x0001;

	/*
	printf("LINE(%d,%d)-(%d,%d),%d\n", x1, y1, x2, y2, mode & 0x03);
	fflush(stdout);
	*/

	dx0 = x2 - x1; dx = (dx0 > 0 ? dx0: -dx0);
	dy0 = y2 - y1; dy = (dy0 > 0 ? dy0: -dy0);

	if(dx > dy) {
		if(dx0 < 0) {
			swap16(&x1, &x2);
			swap16(&y1, &y2);
			dy0 = -dy0;
		}
		for(x = x1, y = y1, e = 0; x <= x2; x++) {
			e += dy;
			if(e > dx) {
				e -= dx;
				y += (dy0 > 0 ? 1: -1);
			}
			if(pat & rotate(&mask))
				pset(x, y, mode);
		}
	} else {
		if(dy0 < 0) {
			swap16(&x1, &x2);
			swap16(&y1, &y2);
			dx0 = -dx0;
		}
		for(y = y1, x = x1, e = 0; y <= y2; y++) {
			e += dx;
			if(e > dy) {
				e -= dy;
				x += (dx0 > 0 ? 1: -1);
			}
			if(pat & rotate(&mask))
				pset(x, y, mode);
		}
	}
}

/*
	LCD上に四角を描く
*/
void box(int16 x1, int16 y1, int16 x2, int16 y2, uint8 mode, uint16 pat)
{
	int dx = (x2 > x1 ? x2 - x1: x1 - x2), dy = (y2 > y1 ? y2 - y1: y1 - y2);

	/*
	printf("BOX(%d,%d)-(%d,%d),%d\n", x1, y1, x2, y2, mode);
	fflush(stdout);
	*/

	if(dx == 0 || dy == 0)
		line(x1, y1, x2, y2, mode, pat);
	else {
		if(dx == 0)
			pset(x1, y1, mode);
		else {
			line(x1, y1, x2, y1, mode, pat);
			line(x1, y2, x2, y2, mode, pat);
		}

		if(dy <= 1)
			return;

		if(dx == 0)
			pset(x2, y2, mode);
		else {
			line(x1, y1 + 1, x1, y2 - 1, mode, pat);
			line(x2, y1 + 1, x2, y2 - 1, mode, pat);
		}
	}
}

/*
	LCD上に塗りつぶした四角を描く
*/
void boxfill(int16 x1, int16 y1, int16 x2, int16 y2, uint8 mode, uint16 pat)
{
	uint16 i, j;
	uint16 mask = 0x0001;

	/*
	printf("BOXFILL(%d,%d)-(%d,%d),%d\n", x1, y1, x2, y2, mode);
	fflush(stdout);
	*/

	clip(&x1, &y1, &x2, &y2);
	for(j = y1; j <= y2; j++)
		for(i = x1; i <= x2; i++)
			if(pat & rotate(&mask))
				pset(i, j, mode);
}

/*
	LCD上にパターンを描く
*/
static void gprint(int16 x, int16 y, uint8 pat)
{
	/*
	printf("GPRINT(%d,%d),%02x\n", x, y, pat);
	fflush(stdout);
	*/

	pset(x, y - 7, (pat & 0x01 ? 1: 0));
	pset(x, y - 6, (pat & 0x02 ? 1: 0));
	pset(x, y - 5, (pat & 0x04 ? 1: 0));
	pset(x, y - 4, (pat & 0x08 ? 1: 0));
	pset(x, y - 3, (pat & 0x10 ? 1: 0));
	pset(x, y - 2, (pat & 0x20 ? 1: 0));
	pset(x, y - 1, (pat & 0x40 ? 1: 0));
	pset(x, y - 0, (pat & 0x80 ? 1: 0));
}

/*
	押されているキーを得る(waitなし)
*/
uint8 peekKeycode(void)
{
	const static uint8 zero[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int len;
	uint8 k, *p;

	if(keyBreak)
		return GKEY_BREAK;
	if(memcmp(keyMatrix, zero, 10) == 0)
		return 0;

	for(p = keyMatrix, k = 1; *p == 0; p++, k += 8)
		;
	switch(*p) {
	case 0x01:
		break;
	case 0x02:
		k += 1; break;
	case 0x04:
		k += 2; break;
	case 0x08:
		k += 3; break;
	case 0x10:
		k += 4; break;
	case 0x20:
		k += 5; break;
	case 0x40:
		k += 6; break;
	case 0x80:
		k += 7; break;
	default:
		return GKEY_DOUBLE;
	}

	len = (int )(&keyMatrix[9] - p);
	if(len != 0 && memcmp(p + 1, zero, len) != 0)
		return GKEY_DOUBLE;
	return k | (keyShift ? 0x80: 0);
}

/*
	押されているキーを得る(waitあり)
*/
uint8 getKeycode(void)
{
	uint8 keycode;

	updateLCD();

	while(peekKeycode() != 0) {
		updateKey();
		updateLCD();
		delay(1000 / freqUpdateIO);
	}
	while((keycode = peekKeycode()) == 0 || keycode == GKEY_DOUBLE) {
		updateKey();
		updateLCD();
		delay(1000 / freqUpdateIO);
	}
	return keycode;
}

/*
	キーを離すまで待つ
*/
void waitRelease(void)
{
	do {
		updateKey();
		updateLCD();
		delay(1000 / freqUpdateIO);
	} while(peekKeycode() != GKEY_NONE);
}

/*
	キーコードをASCIIに変換する
*/
uint8 keycode2ascii(uint8 keycode, int is_normal)
{
	if(is_normal)
		return keycode2ascii_normal[keycode];
	else
		return keycode2ascii_caps[keycode & 0x7f];
}

/*
*/
int setMode(uint8 ch)
{
	if(ch == 0x00)
		memory[0x7902] = 0x00;
	else if(ch == 0x01) /* BASIC */
		memory[0x7902] = (memory[0x7902] ^ 0x60) & 0x60;
	else if(ch == 0x02) /* TEXT */
		;
	else if(ch == 0x03) /* C */
		;
	else if(ch == 0x04) /* STAT */
		;
	else if(ch == 0x06) /* OFF */
		poweroff();
	else if(ch == 0xf0) /* ASMBL */
		;
	else if(ch == 0xf1) /* BASE-n */
		;
	else if(ch == 0xf2) /* コントラスト */
		;
	else
		return FALSE;

	return TRUE;
}

/*
	カーソルを表示する (getChrcodeの下請け)
*/
static void putCursor(uint8 col, uint8 row, const uint8 *off, const uint8 *on, int count)
{
	int cursor = count % freqUpdateIO;

	if(col >= lcdCols) {
		col = 0;
		row++;
	}

	if(col < lcdCols && row < lcdRows) {
		if(cursor == 0) {
			putpat(col, row, on, 5);
			updateLCD();
		} else if(cursor == freqUpdateIO / 2) {
			putpat(col, row, off, 5);
			updateLCD();
		}
	}

	delay(1000 / freqUpdateIO);
}

#define REPEAT_START_DELAY	200	/* キーを最初に押してから文字が出るまでの待ち時間 */
#define REPEAT_DELAY	100	/* 文字が出る間隔 */

/*
	キーコードを得る(オートリピートあり・ウェイトなし) (getChrcodeの下請け)
*/
static uint8 peekKeycodeRepeat(void)
{
	static int repeat_counter = 0;
	static int repeat_start_counter = 0;
	static uint8 old_key;
	static uint8 last_key = GKEY_NONE;

	updateKey();
	updateLCD();

	old_key = last_key;
	last_key = peekKeycode();

	if(last_key == GKEY_DOWN || last_key == GKEY_UP || last_key == GKEY_LEFT || last_key == GKEY_RIGHT || last_key == GKEY_BACKSPACE) {
		if(last_key != old_key || last_key == GKEY_NONE || last_key == GKEY_DOUBLE) {
			repeat_counter = REPEAT_DELAY * freqUpdateIO / 1000;
			repeat_start_counter = REPEAT_START_DELAY * freqUpdateIO / 1000;
		}

		if(last_key != old_key)
			return last_key;
		if(repeat_start_counter-- > 0)
			return GKEY_NONE;
		if(repeat_counter-- > 0)
			return GKEY_NONE;
		repeat_counter = REPEAT_DELAY * freqUpdateIO / 1000;
		return last_key;
	} else {
		if((last_key & 0x7f) != (old_key & 0x7f))
			return last_key;
		else
			return GKEY_NONE;
	}
}

/*
	キー入力を1文字得る (ggetchrの下請け)
*/
uint8 getChrcode(int cursor)
{
	const struct RomanKana {
		const uint8 *roman;
		const uint8 *kana;
	} *r, table[] = {
		{ (const uint8 *)"A", (const uint8 *)"\xb1" },	/* ア */
		{ (const uint8 *)"I", (const uint8 *)"\xb2" },	/* イ */
		{ (const uint8 *)"U", (const uint8 *)"\xb3" },	/* ウ */
		{ (const uint8 *)"E", (const uint8 *)"\xb4" },	/* エ */
		{ (const uint8 *)"O", (const uint8 *)"\xb5" },	/* オ */
		{ (const uint8 *)"YE", (const uint8 *)"\xb2\xaa" },	/* イェ */
		{ (const uint8 *)"KA", (const uint8 *)"\xb6" },	/* カ */
		{ (const uint8 *)"KI", (const uint8 *)"\xb7" },	/* キ */
		{ (const uint8 *)"KU", (const uint8 *)"\xb8" },	/* ク */
		{ (const uint8 *)"KE", (const uint8 *)"\xb9" },	/* ケ */
		{ (const uint8 *)"KO", (const uint8 *)"\xba" },	/* コ */
		{ (const uint8 *)"CA", (const uint8 *)"\xb6" },	/* カ */
		{ (const uint8 *)"CU", (const uint8 *)"\xb8" },	/* ク */
		{ (const uint8 *)"CO", (const uint8 *)"\xba" },	/* コ */
		{ (const uint8 *)"QA", (const uint8 *)"\xb8\xa7" },	/* クァ */
		{ (const uint8 *)"QI", (const uint8 *)"\xb8\xa8" },	/* クィ */
		{ (const uint8 *)"QU", (const uint8 *)"\xb8" },	/* ク */
		{ (const uint8 *)"QE", (const uint8 *)"\xb8\xaa" },	/* クェ */
		{ (const uint8 *)"QO", (const uint8 *)"\xb8\xab" },	/* クォ */
		{ (const uint8 *)"KYA", (const uint8 *)"\xb7\xac" },	/* キャ */
		{ (const uint8 *)"KYI", (const uint8 *)"\xb7\xa8" },	/* キィ */
		{ (const uint8 *)"KYU", (const uint8 *)"\xb7\xad" },	/* キュ */
		{ (const uint8 *)"KYE", (const uint8 *)"\xb7\xaa" },	/* キェ */
		{ (const uint8 *)"KYO", (const uint8 *)"\xb7\xae" },	/* キョ */
		{ (const uint8 *)"SA", (const uint8 *)"\xbb" },	/* サ */
		{ (const uint8 *)"SI", (const uint8 *)"\xbc" },	/* シ */
		{ (const uint8 *)"SU", (const uint8 *)"\xbd" },	/* ス */
		{ (const uint8 *)"SE", (const uint8 *)"\xbe" },	/* セ */
		{ (const uint8 *)"SO", (const uint8 *)"\xbf" },	/* ソ */
		{ (const uint8 *)"SHA", (const uint8 *)"\xbc\xac" },	/* シャ */
		{ (const uint8 *)"SHI", (const uint8 *)"\xbc" },	/* シ */
		{ (const uint8 *)"SHU", (const uint8 *)"\xbc\xad" },	/* シュ */
		{ (const uint8 *)"SHE", (const uint8 *)"\xbc\xaa" },	/* シェ */
		{ (const uint8 *)"SHO", (const uint8 *)"\xbc\xae" },	/* ショ */
		{ (const uint8 *)"SYA", (const uint8 *)"\xbc\xac" },	/* シャ */
		{ (const uint8 *)"SYI", (const uint8 *)"\xbc" },	/* シ */
		{ (const uint8 *)"SYU", (const uint8 *)"\xbc\xad" },	/* シュ */
		{ (const uint8 *)"SYE", (const uint8 *)"\xbc\xaa" },	/* シェ */
		{ (const uint8 *)"SYO", (const uint8 *)"\xbc\xae" },	/* ショ */
		{ (const uint8 *)"TA", (const uint8 *)"\xc0" },	/* タ */
		{ (const uint8 *)"TI", (const uint8 *)"\xc1" },	/* チ */
		{ (const uint8 *)"TU", (const uint8 *)"\xc2" },	/* ツ */
		{ (const uint8 *)"TE", (const uint8 *)"\xc3" },	/* テ */
		{ (const uint8 *)"TO", (const uint8 *)"\xc4" },	/* ト */
		{ (const uint8 *)"TSA", (const uint8 *)"\xc2\xa7" },	/* ツァ */
		{ (const uint8 *)"TSI", (const uint8 *)"\xc2\xa8" },	/* ツィ */
		{ (const uint8 *)"TSU", (const uint8 *)"\xc2" },	/* ツ */
		{ (const uint8 *)"TSE", (const uint8 *)"\xc2\xaa" },	/* ツェ */
		{ (const uint8 *)"TSO", (const uint8 *)"\xc2\xab" },	/* ツォ */
		{ (const uint8 *)"CHA", (const uint8 *)"\xc1\xac" },	/* チャ */
		{ (const uint8 *)"CHI", (const uint8 *)"\xc1" },	/* チ */
		{ (const uint8 *)"CHU", (const uint8 *)"\xc1\xad" },	/* チュ */
		{ (const uint8 *)"CHE", (const uint8 *)"\xc1\xaa" },	/* チェ */
		{ (const uint8 *)"CHO", (const uint8 *)"\xc1\xac" },	/* チョ */
		{ (const uint8 *)"TYA", (const uint8 *)"\xc1\xac" },	/* チャ */
		{ (const uint8 *)"TYI", (const uint8 *)"\xc1\xa8" },	/* チィ */
		{ (const uint8 *)"TYU", (const uint8 *)"\xc1\xad" },	/* チュ */
		{ (const uint8 *)"TYE", (const uint8 *)"\xc1\xaa" },	/* チェ */
		{ (const uint8 *)"TYO", (const uint8 *)"\xc1\xac" },	/* チョ */
		{ (const uint8 *)"CYA", (const uint8 *)"\xc1\xac" },	/* チャ */
		{ (const uint8 *)"CYI", (const uint8 *)"\xc1\xa8" },	/* チィ */
		{ (const uint8 *)"CYU", (const uint8 *)"\xc1\xad" },	/* チュ */
		{ (const uint8 *)"CYE", (const uint8 *)"\xc1\xaa" },	/* チェ */
		{ (const uint8 *)"CYO", (const uint8 *)"\xc1\xac" },	/* チョ */
		{ (const uint8 *)"NA", (const uint8 *)"\xc5" },	/* ナ */
		{ (const uint8 *)"NI", (const uint8 *)"\xc6" },	/* ニ */
		{ (const uint8 *)"NU", (const uint8 *)"\xc7" },	/* ヌ */
		{ (const uint8 *)"NE", (const uint8 *)"\xc8" },	/* ネ */
		{ (const uint8 *)"NO", (const uint8 *)"\xc9" },	/* ノ */
		{ (const uint8 *)"NYA", (const uint8 *)"\xc6\xac" },	/* ニャ */
		{ (const uint8 *)"NYI", (const uint8 *)"\xc6\xa8" },	/* ニィ */
		{ (const uint8 *)"NYU", (const uint8 *)"\xc6\xad" },	/* ニュ */
		{ (const uint8 *)"NYE", (const uint8 *)"\xc6\xaa" },	/* ニェ */
		{ (const uint8 *)"NYO", (const uint8 *)"\xc6\xac" },	/* ニョ */
		{ (const uint8 *)"HA", (const uint8 *)"\xca" },	/* ハ */
		{ (const uint8 *)"HI", (const uint8 *)"\xcb" },	/* ヒ */
		{ (const uint8 *)"HU", (const uint8 *)"\xcc" },	/* フ */
		{ (const uint8 *)"HE", (const uint8 *)"\xcd" },	/* ヘ */
		{ (const uint8 *)"HO", (const uint8 *)"\xce" },	/* ホ */
		{ (const uint8 *)"FA", (const uint8 *)"\xcd\xa7" },	/* ファ */
		{ (const uint8 *)"FI", (const uint8 *)"\xcd\xa8" },	/* フィ */
		{ (const uint8 *)"FU", (const uint8 *)"\xcd" },	/* フ */
		{ (const uint8 *)"FE", (const uint8 *)"\xcd\xaa" },	/* フェ */
		{ (const uint8 *)"FO", (const uint8 *)"\xcd\xab" },	/* フォ */
		{ (const uint8 *)"HYA", (const uint8 *)"\xcb\xac" },	/* ヒャ */
		{ (const uint8 *)"HYI", (const uint8 *)"\xcb\xa8" },	/* ヒィ */
		{ (const uint8 *)"HYU", (const uint8 *)"\xcb\xad" },	/* ヒュ */
		{ (const uint8 *)"HYE", (const uint8 *)"\xcb\xaa" },	/* ヒェ */
		{ (const uint8 *)"HYO", (const uint8 *)"\xcb\xac" },	/* ヒョ */
		{ (const uint8 *)"MA", (const uint8 *)"\xcf" },	/* マ */
		{ (const uint8 *)"MI", (const uint8 *)"\xd0" },	/* ミ */
		{ (const uint8 *)"MU", (const uint8 *)"\xd1" },	/* ム */
		{ (const uint8 *)"ME", (const uint8 *)"\xd2" },	/* メ */
		{ (const uint8 *)"MO", (const uint8 *)"\xd3" },	/* モ */
		{ (const uint8 *)"MYA", (const uint8 *)"\xd0\xac" },	/* ミャ */
		{ (const uint8 *)"MYI", (const uint8 *)"\xd0\xa8" },	/* ミィ */
		{ (const uint8 *)"MYU", (const uint8 *)"\xd0\xad" },	/* ミュ */
		{ (const uint8 *)"MYE", (const uint8 *)"\xd0\xaa" },	/* ミェ */
		{ (const uint8 *)"MYO", (const uint8 *)"\xd0\xac" },	/* ミョ */
		{ (const uint8 *)"YA", (const uint8 *)"\xd4" },	/* ヤ */
		{ (const uint8 *)"YI", (const uint8 *)"\xb2" },	/* イ */
		{ (const uint8 *)"YU", (const uint8 *)"\xd5" },	/* ユ */
		{ (const uint8 *)"YO", (const uint8 *)"\xd6" },	/* ヨ */
		{ (const uint8 *)"RA", (const uint8 *)"\xd7" },	/* ラ */
		{ (const uint8 *)"RI", (const uint8 *)"\xd8" },	/* リ */
		{ (const uint8 *)"RU", (const uint8 *)"\xd9" },	/* ル */
		{ (const uint8 *)"RE", (const uint8 *)"\xda" },	/* レ */
		{ (const uint8 *)"RO", (const uint8 *)"\xdb" },	/* ロ */
		{ (const uint8 *)"LA", (const uint8 *)"\xd7" },	/* ラ */
		{ (const uint8 *)"LI", (const uint8 *)"\xd8" },	/* リ */
		{ (const uint8 *)"LU", (const uint8 *)"\xd9" },	/* ル */
		{ (const uint8 *)"LE", (const uint8 *)"\xda" },	/* レ */
		{ (const uint8 *)"LO", (const uint8 *)"\xdb" },	/* ロ */
		{ (const uint8 *)"RYA", (const uint8 *)"\xd8\xac" },	/* リャ */
		{ (const uint8 *)"RYI", (const uint8 *)"\xd8\xa8" },	/* リィ */
		{ (const uint8 *)"RYU", (const uint8 *)"\xd8\xad" },	/* リュ */
		{ (const uint8 *)"RYE", (const uint8 *)"\xd8\xaa" },	/* リェ */
		{ (const uint8 *)"RYO", (const uint8 *)"\xd8\xae" },	/* リョ */
		{ (const uint8 *)"LYA", (const uint8 *)"\xd8\xac" },	/* リャ */
		{ (const uint8 *)"LYI", (const uint8 *)"\xd8\xa8" },	/* リィ */
		{ (const uint8 *)"LYU", (const uint8 *)"\xd8\xad" },	/* リュ */
		{ (const uint8 *)"LYE", (const uint8 *)"\xd8\xaa" },	/* リェ */
		{ (const uint8 *)"LYO", (const uint8 *)"\xd8\xae" },	/* リョ */
		{ (const uint8 *)"WA", (const uint8 *)"\xdc" },	/* ワ */
		{ (const uint8 *)"WO", (const uint8 *)"\xa6" },	/* ヲ */
		{ (const uint8 *)"VA", (const uint8 *)"\xb3\xde\xa7" },	/* ヴァ */
		{ (const uint8 *)"VI", (const uint8 *)"\xb3\xde\xa8" },	/* ヴィ */
		{ (const uint8 *)"VU", (const uint8 *)"\xb3\xde" },	/* ヴ */
		{ (const uint8 *)"VE", (const uint8 *)"\xb3\xde\xaa" },	/* ヴェ */
		{ (const uint8 *)"VO", (const uint8 *)"\xb3\xde\xab" },	/* ヴォ */
		{ (const uint8 *)"GA", (const uint8 *)"\xb6\xde" },	/* ガ */
		{ (const uint8 *)"GI", (const uint8 *)"\xb7\xde" },	/* ギ */
		{ (const uint8 *)"GU", (const uint8 *)"\xb8\xde" },	/* グ */
		{ (const uint8 *)"GE", (const uint8 *)"\xb9\xde" },	/* ゲ */
		{ (const uint8 *)"GO", (const uint8 *)"\xba\xde" },	/* ゴ */
		{ (const uint8 *)"GYA", (const uint8 *)"\xb7\xde\xac" },	/* ギャ */
		{ (const uint8 *)"GYI", (const uint8 *)"\xb7\xde\xa8" },	/* ギィ */
		{ (const uint8 *)"GYU", (const uint8 *)"\xb7\xde\xad" },	/* ギュ */
		{ (const uint8 *)"GYE", (const uint8 *)"\xb7\xde\xaa" },	/* ギェ */
		{ (const uint8 *)"GYO", (const uint8 *)"\xb7\xde\xac" },	/* ギョ */
		{ (const uint8 *)"ZA", (const uint8 *)"\xbb\xde" },	/* ザ */
		{ (const uint8 *)"ZI", (const uint8 *)"\xbc\xde" },	/* ジ */
		{ (const uint8 *)"ZU", (const uint8 *)"\xbd\xde" },	/* ズ */
		{ (const uint8 *)"ZE", (const uint8 *)"\xbe\xde" },	/* ゼ */
		{ (const uint8 *)"ZO", (const uint8 *)"\xbf\xde" },	/* ゾ */
		{ (const uint8 *)"JA", (const uint8 *)"\xbc\xde\xac" },	/* ジャ */
		{ (const uint8 *)"JI", (const uint8 *)"\xbc\xde" },	/* ジ */
		{ (const uint8 *)"JU", (const uint8 *)"\xbc\xde\xad" },	/* ジュ */
		{ (const uint8 *)"JE", (const uint8 *)"\xbc\xde\xaa" },	/* ジェ */
		{ (const uint8 *)"JO", (const uint8 *)"\xbc\xde\xac" },	/* ジョ */
		{ (const uint8 *)"JYA", (const uint8 *)"\xbc\xde\xac" },	/* ジャ */
		{ (const uint8 *)"JYI", (const uint8 *)"\xbc\xde\xa8" },	/* ジィ */
		{ (const uint8 *)"JYU", (const uint8 *)"\xbc\xde\xad" },	/* ジュ */
		{ (const uint8 *)"JYE", (const uint8 *)"\xbc\xde\xaa" },	/* ジェ */
		{ (const uint8 *)"JYO", (const uint8 *)"\xbc\xde\xac" },	/* ジョ */
		{ (const uint8 *)"ZYA", (const uint8 *)"\xbc\xde\xac" },	/* ジャ */
		{ (const uint8 *)"ZYI", (const uint8 *)"\xbc\xde\xa8" },	/* ジィ */
		{ (const uint8 *)"ZYU", (const uint8 *)"\xbc\xde\xad" },	/* ジュ */
		{ (const uint8 *)"ZYE", (const uint8 *)"\xbc\xde\xaa" },	/* ジェ */
		{ (const uint8 *)"ZYO", (const uint8 *)"\xbc\xde\xac" },	/* ジョ */
		{ (const uint8 *)"DA", (const uint8 *)"\xc0\xde" },	/* ダ */
		{ (const uint8 *)"DI", (const uint8 *)"\xc1\xde" },	/* ヂ */
		{ (const uint8 *)"DU", (const uint8 *)"\xc2\xde" },	/* ヅ */
		{ (const uint8 *)"DE", (const uint8 *)"\xc3\xde" },	/* デ */
		{ (const uint8 *)"DO", (const uint8 *)"\xc4\xde" },	/* ド */
		{ (const uint8 *)"DHA", (const uint8 *)"\xc3\xde\xac" },	/* デャ */
		{ (const uint8 *)"DHI", (const uint8 *)"\xc3\xde\xa8" },	/* ディ */
		{ (const uint8 *)"DHU", (const uint8 *)"\xc3\xde\xad" },	/* デュ */
		{ (const uint8 *)"DHE", (const uint8 *)"\xc3\xde\xaa" },	/* デェ */
		{ (const uint8 *)"DHO", (const uint8 *)"\xc3\xde\xac" },	/* デョ */
		{ (const uint8 *)"DYA", (const uint8 *)"\xc1\xde\xac" },	/* ヂャ */
		{ (const uint8 *)"DYI", (const uint8 *)"\xc1\xde\xa8" },	/* ヂィ */
		{ (const uint8 *)"DYU", (const uint8 *)"\xc1\xde\xad" },	/* ヂュ */
		{ (const uint8 *)"DYE", (const uint8 *)"\xc1\xde\xaa" },	/* ヂェ */
		{ (const uint8 *)"DYO", (const uint8 *)"\xc1\xde\xac" },	/* ヂョ */
		{ (const uint8 *)"BA", (const uint8 *)"\xca\xde" },	/* バ */
		{ (const uint8 *)"BI", (const uint8 *)"\xcb\xde" },	/* ビ */
		{ (const uint8 *)"BU", (const uint8 *)"\xcc\xde" },	/* ブ */
		{ (const uint8 *)"BE", (const uint8 *)"\xcd\xde" },	/* ベ */
		{ (const uint8 *)"BO", (const uint8 *)"\xce\xde" },	/* ボ */
		{ (const uint8 *)"BYA", (const uint8 *)"\xcb\xde\xac" },	/* ビャ */
		{ (const uint8 *)"BYI", (const uint8 *)"\xcb\xde\xa8" },	/* ビィ */
		{ (const uint8 *)"BYU", (const uint8 *)"\xcb\xde\xad" },	/* ビュ */
		{ (const uint8 *)"BYE", (const uint8 *)"\xcb\xde\xaa" },	/* ビェ */
		{ (const uint8 *)"BYO", (const uint8 *)"\xcb\xde\xac" },	/* ビョ */
		{ (const uint8 *)"PA", (const uint8 *)"\xca\xdf" },	/* パ */
		{ (const uint8 *)"PI", (const uint8 *)"\xcb\xdf" },	/* ピ */
		{ (const uint8 *)"PU", (const uint8 *)"\xcc\xdf" },	/* プ */
		{ (const uint8 *)"PE", (const uint8 *)"\xcd\xdf" },	/* ペ */
		{ (const uint8 *)"PO", (const uint8 *)"\xce\xdf" },	/* ポ */
		{ (const uint8 *)"PYA", (const uint8 *)"\xcb\xdf\xac" },	/* ピャ */
		{ (const uint8 *)"PYI", (const uint8 *)"\xcb\xdf\xa8" },	/* ピ */
		{ (const uint8 *)"PYU", (const uint8 *)"\xcb\xdf\xad" },	/* ピュ */
		{ (const uint8 *)"PYE", (const uint8 *)"\xcb\xdf\xaa" },	/* ピ */
		{ (const uint8 *)"PYO", (const uint8 *)"\xcb\xdf\xac" },	/* ピョ */
		{ (const uint8 *)"N'", (const uint8 *)"\xdd" },	/* ン */
		{ (const uint8 *)".", (const uint8 *)"\xa1" },	/* 。 */
		{ (const uint8 *)"(", (const uint8 *)"\xa2" },	/* 「 */
		{ (const uint8 *)")", (const uint8 *)"\xa3" },	/* 」 */
		{ (const uint8 *)",", (const uint8 *)"\xa4" },	/* 、 */
		{ (const uint8 *)"+", (const uint8 *)"\xa5" },	/* ・ */
		{ (const uint8 *)"-", (const uint8 *)"\xb0" },	/* ー (独自拡張) */
		{ (const uint8 *)"X", (const uint8 *)"" },
		{ NULL }
	};
	const struct DaiSho {
		const uint8 *dai;
		const uint8 *sho;
	} *d, dai_sho[] = {
		{ (const uint8 *)"\xb1", (const uint8 *)"\xa7" },	/* ア → ァ */
		{ (const uint8 *)"\xb2", (const uint8 *)"\xa8" },	/* イ → ィ */
		{ (const uint8 *)"\xb3", (const uint8 *)"\xa9" },	/* ウ → ゥ */
		{ (const uint8 *)"\xb4", (const uint8 *)"\xaa" },	/* エ → ェ */
		{ (const uint8 *)"\xb5", (const uint8 *)"\xab" },	/* オ → ォ */
		{ (const uint8 *)"\xd4", (const uint8 *)"\xac" },	/* ヤ → ャ */
		{ (const uint8 *)"\xd5", (const uint8 *)"\xad" },	/* ユ → ュ */
		{ (const uint8 *)"\xd6", (const uint8 *)"\xae" },	/* ヨ → ョ */
		{ (const uint8 *)"\xc2", (const uint8 *)"\xaf" },	/* ツ → ッ */
		{ NULL }
	};
	const struct Function {
		uint8 key;
		uint8 code;
	} *f, func[] = {
		{ GKEY_PI, CODE_PI },
		{ GKEY_NPR, CODE_NPR },
		{ GKEY_DEG, CODE_DEG },
		{ GKEY_SQR, CODE_SQR },
		{ GKEY_SQU, CODE_SQU },
		{ GKEY_RCP, CODE_RCP },
		{ GKEY_MDF, CODE_MDF },
		{ GKEY_SIN, CODE_SIN },
		{ GKEY_COS, CODE_COS },
		{ GKEY_LN, CODE_LN },
		{ GKEY_LOG, CODE_LOG },
		{ GKEY_TAN, CODE_TAN },
		{ GKEY_Z | 0x80, CODE_INPUT },
		{ GKEY_X | 0x80, CODE_PRINT },
		{ GKEY_C | 0x80, CODE_CONT },
		{ GKEY_V | 0x80, CODE_RUN },
		{ GKEY_B | 0x80, CODE_LIST },
		{ GKEY_N | 0x80, CODE_SAVE },
		{ GKEY_M | 0x80, CODE_LOAD },
		{ GKEY_PI | 0x80, CODE_RND },
		{ GKEY_NPR | 0x80, CODE_NCR },
		{ GKEY_DEG | 0x80, CODE_DMS },
		{ GKEY_SQR | 0x80, CODE_CUR },
		{ GKEY_SQU | 0x80, CODE_CUB },
		{ GKEY_HAT | 0x80, CODE_POL },
		{ GKEY_LKAKKO | 0x80, CODE_REC },
		{ GKEY_RCP | 0x80, CODE_FACT },
		{ GKEY_SIN | 0x80, CODE_ASN },
		{ GKEY_COS | 0x80, CODE_ASC },
		{ GKEY_LN | 0x80, CODE_EXP },
		{ GKEY_LOG | 0x80, CODE_TEN },
		{ GKEY_TAN | 0x80, CODE_ATN },
		{ 0 }
	};
	static uint8 queue[8] = { 0 }, roman[4] = { 0, 0, 0, 0 };
	int cursor_count = 0, roman_row;
	uint8 key, ch, *v_cursor, *v_roman, cursor_back[5], cursor_off[5], cursor_on[5], roman_back[5 * 6];

	/* カーソル位置の文字を保存する */
	if(*curCol < lcdCols && *curRow < lcdRows)
		v_cursor = VRAM_CR(*curCol, *curRow);
	else
		v_cursor = NULL;
	if(v_cursor != NULL)
		memcpy(cursor_back, v_cursor, sizeof(cursor_back));

	/* 変換中ローマ字の位置の文字を保存する */
	if(*curRow != lcdRows - 1)
		roman_row = lcdRows - 1;
	else
		roman_row = 0;

	v_roman = VRAM_CR(lcdCols - 5, roman_row);
	memcpy(roman_back, v_roman, cellWidth * 5);

	/* カーソルの形状を設定する */
	if(cursor == CURSOR_NEW) {
		memset(cursor_off, 0x40, sizeof(cursor_off));
		memset(cursor_on, 0x40, sizeof(cursor_on));
	} else if(cursor == CURSOR_OVER) {
		memcpy(cursor_off, cursor_back, sizeof(cursor_off));
		memset(cursor_on, 0xff, sizeof(cursor_on));
	} else if(cursor == CURSOR_INS) {
		const uint8 pat[] = { 0x08, 0x1c, 0x3e, 0x7f, 0x00 };
		memcpy(cursor_off, cursor_back, sizeof(cursor_off));
		memcpy(cursor_on, pat, sizeof(cursor_on));
	} else {
		memcpy(cursor_off, cursor_back, sizeof(cursor_off));
		memcpy(cursor_on, cursor_back, sizeof(cursor_on));
	}

	while(queue[0] == 0) {
		/* 変換中のローマ字を表示する */
		if(roman[0] == 0)
			memcpy(v_roman, roman_back, cellWidth * 5);
		else
			putstr(lcdCols - 5, roman_row, "[%-3s]", roman);

		/* ステータスを表示する */
		putstatus(STATUS_CAPS, memory[0x7901] & 0x02);
		putstatus(STATUS_KANA, memory[0x7901] & 0x04);
		putstatus(STATUS_SYO, memory[0x7901] & 0x08);
		putstatus(STATUS_2NDF, memory[0x7901] & 0x10);

		putstatus(STATUS_TEXT, memory[0x7902] & 0x08);
		putstatus(STATUS_CASL, memory[0x7902] & 0x10);
		putstatus(STATUS_PRO, memory[0x7902] & 0x20);
		putstatus(STATUS_RUN, memory[0x7902] & 0x40);

		if(memory[0x7903] == 0x10) { /* RAD */
			putstatus(STATUS_DE, FALSE);
			putstatus(STATUS_G, FALSE);
			putstatus(STATUS_RAD, TRUE);
		} else if(memory[0x7903] == 0x30) { /* GRAD */
			putstatus(STATUS_DE, FALSE);
			putstatus(STATUS_G, TRUE);
			putstatus(STATUS_RAD, TRUE);
		} else if(memory[0x7903] == 0x60) { /* DEG */
			putstatus(STATUS_DE, TRUE);
			putstatus(STATUS_G, TRUE);
			putstatus(STATUS_RAD, FALSE);
		}
		updateLCD();

		/* 押したキーを得る */
		while(key = peekKeycodeRepeat(), key == GKEY_NONE || key == GKEY_DOUBLE)
			putCursor(*curCol, *curRow, cursor_off, cursor_on, cursor_count++);
		if(memory[0x7901] & 0x10) {
			if(key != GKEY_2NDF)
				memory[0x7901] &= ~0x10;
			key |= 0x80;
		}
		if((memory[0x7901] & 0x04) && key == (GKEY_KANA | 0x80))
			ch = 0xb0; /* ー */
		else
			ch = keycode2ascii(key, memory[0x7901] & 0x02);

		/* 押したキーを処理する */
		if(ch == 0x10) {
			/* 2ndFをロック・解除する */
			memory[0x7901] ^= 0x10;
		} else if(ch == 0x11) {
			memset(roman, 0, sizeof(roman));

			/* カナをロック・解除する */
			memory[0x7901] ^= 0x04;

			/* 小を解除する */
			memory[0x7901] &= ~0x08;
		} else if(ch == 0x13) {
			/* DRGを変える */
			if(memory[0x7903] == 0x10)
				memory[0x7903] = 0x30;
			else if(memory[0x7903] == 0x30)
				memory[0x7903] = 0x60;
			else
				memory[0x7903] = 0x10;
		} else if(ch == 0x14) {
			if(memory[0x7901] & 0x04) {
				/* 小をロック・解除する */
				memory[0x7901] ^= 0x08;
			} else {
				/* CAPSをロック・解除する */
				memory[0x7901] ^= 0x02;
			}
		} else if(ch == 0xfe) {
			/* ファンクションキーからキーワードを得る */
			for(f = func; f->key != 0; f++)
				if(key == f->key)
					getKeywordFromCode(queue, f->code);
		} else if(!(memory[0x7901] & 0x04)) {
			/* 押したキーの文字をキューに入れる */
			queue[0] = ch;
			queue[1] = 0;
			memset(roman, 0, sizeof(roman));
		} else {
			/* ローマ字の入力を得る */
			if(!isalpha(ch) && ch != '\'')
				memset(roman, 0, sizeof(roman));
			if(roman[0] == 0)
				roman[0] = ch;
			else if(roman[1] == 0)
				roman[1] = ch;
			else
				roman[2] = ch;

			/* ローマ字・カナ変換テーブルから検索し一致すればキューに入れる */
			for(r = table; r->roman != NULL; r++)
				if(memicmp(roman, r->roman, strlen(roman)) == 0) {
					if(strlen(roman) == strlen(r->roman)) {
						strcpy(queue, r->kana);

						if(memory[0x7901] & 0x08) {
							for(d = dai_sho; d->dai != NULL; d++)
								if(stricmp(queue, d->dai) == 0)
									strcpy(queue, d->sho);
							memory[0x7901] &= ~0x08;
						}

						memset(roman, 0, sizeof(roman));
					}
					break;
				}

			/* 一致するローマ字がなかった場合の処理を行う */
			if(r->roman == NULL) {
				if(!isalpha(ch)) {
					queue[0] = ch;
					queue[1] = 0;
					memset(roman, 0, sizeof(roman));
				} else {
					if(toupper(roman[0]) == toupper(roman[1])) {
						queue[0] = 0xaf; /* ッ */
						queue[1] = 0;
					} else if(toupper(roman[0]) == 'N' || toupper(roman[0]) == 'M') {
						queue[0] = 0xdd; /* ン */
						queue[1] = 0;
					}
					roman[0] = ch;
					roman[1] = 0;
				}
			}
		}
	}

	/* カーソルを消す */
	if(v_cursor != NULL)
		memcpy(v_cursor, cursor_back, sizeof(cursor_back));

	/* 変換中のローマ字を消す */
	memcpy(v_roman, roman_back, cellWidth * 5);

	/* 文字を戻す */
	ch = queue[0];
	memmove(queue, queue + 1, sizeof(queue) - 1);
	queue[sizeof(queue) - 1] = 0;
	return ch;
}

/*
	キー入力を1文字得る (ggetchrの下請け)
*/
uint8 ggetchr(void)
{
	return getChrcode(CURSOR_NONE);
}

/*
	ggetlineの状態
*/
struct ggetline_stat {
	int base_col, base_row, cur_pos, mod;
	uint8 *buf;
};

/*
	バッファ位置から座標を得る (ggetlineの下請け)
*/
static int _pos_to_cr(struct ggetline_stat *stat, uint8 *col, uint8 *row, int pos)
{
	int z, x, y;

	if(stat->base_col < 0) {
		gputchr(0);
		stat->base_col = *curCol;
		stat->base_row = (int8 )*curRow;
	}

	z = stat->base_col + pos;
	y = z / lcdCols + stat->base_row;
	x = z % lcdCols;

	if(col != NULL)
		*col = x;
	if(row != NULL)
		*row = y;
	if(y < 0 || y >= lcdRows)
		return FALSE;
	return TRUE;
}

/*
	画面を再描画する (ggetlineの下請け)
*/
static void _refresh(struct ggetline_stat *stat)
{
	int pos;
	uint8 col, row;

	for(pos = 0; stat->buf[pos] != 0; pos++)
		if(_pos_to_cr(stat, &col, &row, pos))
			putchr(col, row, stat->buf[pos]);
}

/*
	下にスクロールする (ggetlineの下請け)
*/
static void _scroll_down(struct ggetline_stat *stat)
{
	stat->base_row++;
	scrdown(0, 0);
	_refresh(stat);
}

/*
	カーソルを左に動かす (ggetlineの下請け)
*/
static int _moveleft(struct ggetline_stat *stat)
{
	uint8 col, row;

	if(stat->cur_pos <= 0)
		return FALSE;
	stat->cur_pos--;

	while(!_pos_to_cr(stat, &col, &row, stat->cur_pos))
		_scroll_down(stat);

	glocate(col, row);
	return TRUE;
}

/*
	カーソルを上に動かす (ggetlineの下請け)
*/
static int _moveup(struct ggetline_stat *stat)
{
	int i;

	if(stat->cur_pos < lcdCols)
		return FALSE;

	for(i = 0; i < lcdCols; i++)
		_moveleft(stat);
	return TRUE;
}

/*
	先頭へ飛ぶ (ggetlineの下請け)
*/
static void _jumpfirst(struct ggetline_stat *stat)
{
	while(stat->cur_pos > 0)
		_moveleft(stat);
}

/*
	上にスクロールする (ggetlineの下請け)
*/
static void _scroll_up(struct ggetline_stat *stat)
{
	stat->base_row--;
	scrup();
	_refresh(stat);
}

/*
	カーソルを右に動かす (ggetlineの下請け)
*/
static int _moveright(struct ggetline_stat *stat)
{
	uint8 col, row;

	if(stat->buf[stat->cur_pos] == 0)
		return FALSE;
	stat->cur_pos++;

	while(!_pos_to_cr(stat, &col, &row, stat->cur_pos))
		_scroll_up(stat);

	glocate(col, row);
	return TRUE;
}

/*
	カーソルを下に動かす (ggetlineの下請け)
*/
static int _movedown(struct ggetline_stat *stat)
{
	int i;

	if(stat->cur_pos + lcdCols >= strlen(stat->buf))
		return FALSE;

	for(i = 0; i < lcdCols; i++)
		_moveright(stat);
	return TRUE;
}

/*
	末尾へ飛ぶ (ggetlineの下請け)
*/
static void _jumplast(struct ggetline_stat *stat)
{
	while(stat->buf[stat->cur_pos] != 0)
		_moveright(stat);
}

/*
	バッファに文字を書き込む (ggetlineの下請け)
*/
static void _putchr(struct ggetline_stat *stat, uint8 c)
{
	int last = (stat->buf[stat->cur_pos] == 0);
	uint8 col, row;

	stat->mod = TRUE;

	if(_pos_to_cr(stat, &col, &row, stat->cur_pos))
		putchr(col, row, c);

	stat->buf[stat->cur_pos] = c;
	if(last)
		stat->buf[stat->cur_pos + 1] = 0;

	_moveright(stat);
}

/*
	バッファにTABを書き込む (ggetlineの下請け)
*/
static void _puttab(struct ggetline_stat *stat)
{
	do {
		_putchr(stat, ' ');
	} while(stat->cur_pos % 7 > 0);
}

/*
	バッファに文字を挿入する (ggetlineの下請け)
*/
static void _inschr(struct ggetline_stat *stat, uint8 c)
{
	stat->mod = TRUE;

	memmove(&stat->buf[stat->cur_pos + 1], &stat->buf[stat->cur_pos], strlen(&stat->buf[stat->cur_pos]) + 1);
	_refresh(stat);

	_putchr(stat, c);
}

/*
	バッファにTABを挿入する (ggetlineの下請け)
*/
static void _instab(struct ggetline_stat *stat)
{
	do {
		_inschr(stat, ' ');
	} while(stat->cur_pos % 7 > 0);
}

/*
	バッファの文字を削除する (ggetlineの下請け)
*/
static int _del(struct ggetline_stat *stat)
{
	uint8 col, row;

	if(stat->buf[stat->cur_pos] == 0)
		return FALSE;

	stat->mod = TRUE;

	if(_pos_to_cr(stat, &col, &row, strlen(stat->buf) - 1))
		putchr(col, row, ' ');

	memmove(&stat->buf[stat->cur_pos], &stat->buf[stat->cur_pos + 1], strlen(&stat->buf[stat->cur_pos + 1]) + 1);
	_refresh(stat);
	return TRUE;
}

/*
	バッファの文字を削除する (ggetlineの下請け)
*/
static int _bs(struct ggetline_stat *stat)
{
	if(!_moveleft(stat))
		return FALSE;

	return _del(stat);
}

/*
	バッファを消去する (ggetlineの下請け)
*/
static void _clear(struct ggetline_stat *stat)
{
	_jumplast(stat);

	while(_bs(stat))
		;
}

/*
	プロンプトを表示する (ggetlineの下請け)
*/
static void _putprompt(struct ggetline_stat *stat, const uint8 *prompt)
{
	uint8 col, row;

	if(prompt == NULL)
		return;

	if(!_pos_to_cr(stat, &col, &row, 0))
		return;
	glocate(col, row);
	gprintf("%s", prompt);
	glocate(col, row);
}

/*
	プロンプトを消す (ggetlineの下請け)
*/
static void _clearprompt(struct ggetline_stat *stat, const uint8 *prompt)
{
	int pos;
	uint8 col, row;

	if(prompt == NULL)
		return;

	for(pos = 0; prompt[pos] != 0; pos++)
		if(_pos_to_cr(stat, &col, &row, pos))
			putchr(col, row, ' ');
	_refresh(stat);
}

/*
	文字列の入力を得る
*/
uint8 ggetline(uint8 *buf, const uint8 *prompt, int mode, ...)
{
	va_list v;
	struct ggetline_stat stat;
	int ins = FALSE, first = TRUE, cursor = CURSOR_NONE, *pos = NULL, *row = NULL, *mod = NULL;
	uint8 c, str[32], *p;

	stat.base_col = -1;
	stat.base_row = -1;
	stat.cur_pos = 0;
	stat.buf = buf;

	putstatus(STATUS_BUSY, FALSE);
	_putprompt(&stat, prompt);

	if(mode == GETLINE_PRO) {
		int len;
		uint8 cur_col, cur_row;

		va_start(v, mode);
		pos = (int *)va_arg(v, int *);
		row = (int *)va_arg(v, int *);
		mod = (int *)va_arg(v, int *);
		va_end(v);

		len = strlen(buf);
		if(len >= lcdCols && *pos > len)
			*pos -= lcdCols;

		stat.base_col = 0;
		stat.base_row = *row;
		stat.cur_pos = *pos;
		*mod = stat.mod = FALSE;
		_pos_to_cr(&stat, &cur_col, &cur_row, stat.cur_pos);

		if((int8 )cur_row < 0)
			do {
				_scroll_down(&stat);
				cur_row++;
			} while((int8 )cur_row < 0);
		else if((int8 )cur_row >= lcdRows)
			do {
				_scroll_up(&stat);
				cur_row--;
			} while((int8 )cur_row >= lcdRows);

		_refresh(&stat);
		glocate(cur_col, cur_row);

		first = FALSE;
	}

	for(;;) {
		if(first)
			cursor = CURSOR_NONE;
		else if(stat.buf[stat.cur_pos] == 0)
			cursor = CURSOR_NEW;
		else if(ins)
			cursor = CURSOR_INS;
		else
			cursor = CURSOR_OVER;

		switch((c = getChrcode(cursor))) {
		case 0x00: /* NUL */
			break;
		case 0x01: /* BASIC */
			return c;
		case 0x02: /* TEXT */
			return c;
		case 0x03: /* C */
			return c;
		case 0x04: /* STAT */
			return c;
		case 0x05: /* BREAK */
/*
			if(mode == GETLINE_RUN)
				break;
*/
			return c;
		case 0x06: /* OFF */
			return c;
		case 0x07: /* P←→NP */
			break;
		case 0x08: /* BS */
			_bs(&stat);
			break;
		case 0x09: /* DEL */
			_del(&stat);
			break;
		case 0x0a: /* TAB */
			if(first)
				_clear(&stat);
			if(ins)
				_instab(&stat);
			else
				_puttab(&stat);
			break;
		case 0x0b: /* CA */
			break;
		case 0x0c: /* CLS */
			_clear(&stat);
			return 0x0c;
		case 0x0d: /* RETURN */
			if(mode == GETLINE_PRO) {
				*pos = strlen(buf);
				*row = stat.base_row;
				*mod = stat.mod;
				while(*row < 0) {
					_scroll_down(&stat);
					(*row)++;
				}
				glocate(0, *row);
			} else if(first && stat.buf[0] != 0)
				_clear(&stat);
			else
				_jumpfirst(&stat);
			gputchr(0);
			return 0x0d;
		case 0x0e: /* DIGIT */
			break;
		case 0x0f: /* F←→E */
			break;
		case 0x10: /* 2ndF */
			break;
		case 0x11: /* カナ */
			break;
		case 0x12: /* INS */
			ins = !ins;
			break;
		case 0x13: /* DRG */
			break;
		case 0x14: /* CAPS */
			break;
		case 0x15: /* ANS */
			if(decodeNum(str, &memory[0x79a0]) < 0)
				break;
			for(p = str; *p != 0; p++)
				if(ins)
					_inschr(&stat, *p);
				else
					_putchr(&stat, *p);
			break;
		case 0x16: /* (-) */
			break;
		case 0x17: /* CONST */
			break;
		case 0x18: /* SHIFT + CONST */
			/* ??? */
			break;
		case 0x19: /* R・CM */
			break;
		case 0x1a: /* M+ */
			break;
		case 0x1b: /* M- */
			break;
		case 0x1c: /* → */
			if(buf[0] == 0)
				return c;
			if(first) {
				_refresh(&stat);
				_jumpfirst(&stat);
			} else
				_moveright(&stat);
			break;
		case 0x1d: /* ← */
			if(buf[0] == 0)
				return c;
			if(first) {
				_refresh(&stat);
				_jumplast(&stat);
			} else
				_moveleft(&stat);
			break;
		case 0x1e: /* ↑ */
			if(buf[0] == 0)
				return c;
			if(!_moveup(&stat))
				if(mode == GETLINE_PRO) {
					*pos = stat.cur_pos % lcdCols;
					*row = stat.base_row;
					*mod = stat.mod;
					while(*row < 0) {
						_scroll_down(&stat);
						(*row)++;
					}
					glocate(0, *row);
					return c;
				}
			break;
		case 0x1f: /* ↓ */
			if(buf[0] == 0)
				return c;
			if(!_movedown(&stat))
				if(mode == GETLINE_PRO) {
					*pos = (stat.cur_pos / lcdCols) * lcdCols + stat.cur_pos % lcdCols;
					*row = stat.base_row;
					*mod = stat.mod;
					while(*row >= lcdRows) {
						_scroll_up(&stat);
						(*row)--;
					}
					glocate(0, *row);
					return c;
				}
			break;
		case 0xf0: /* ASMBL */
		case 0xf1: /* BASE-n */
		case 0xf2: /* コントラスト */
			return c;
		default: /* 文字 */
			if(first) {
				if(
				c == '+' ||
				c == '-' ||
				c == '*' ||
				c == '/' ||
				c == '\\' ||
				c == '^' ||
				c == '<' ||
				c == '=' ||
				c == '>'
				) {
					_refresh(&stat);
					_jumplast(&stat);
				} else
					_clear(&stat);
			}

			if(ins)
				_inschr(&stat, c);
			else
				_putchr(&stat, c);
		}

		if(first)
			_clearprompt(&stat, prompt);
		first = FALSE;
	}
}

/*
	数値(16進数)を得る
*/
static int ishex(uint8 ascii)
{
	return 
	ascii == 0x30 ||
	ascii == 0x31 ||
	ascii == 0x32 ||
	ascii == 0x33 ||
	ascii == 0x34 ||
	ascii == 0x35 ||
	ascii == 0x36 ||
	ascii == 0x37 ||
	ascii == 0x38 ||
	ascii == 0x39 ||
	ascii == 0x61 || ascii == 0x41 ||
	ascii == 0x62 || ascii == 0x42 ||
	ascii == 0x63 || ascii == 0x43 ||
	ascii == 0x64 || ascii == 0x44 ||
	ascii == 0x65 || ascii == 0x45 ||
	ascii == 0x66 || ascii == 0x46;
}
static void gethex(void *num, int length, uint8 col, uint8 row)
{
	int n, first = TRUE;
	char buf[8];
	uint8 k, x = length * 2;

	if(length == sizeof(uint8))
		putstr(col, row, "%02X", *(uint8 *)num);
	else
		putstr(col, row, "%04X", *(uint16 *)num);

	while((k = getKeycode()) != GKEY_RETURN) {
		if(k == GKEY_CLS || first) {
			first = FALSE;
			while(x > 0)
				putchr(col + --x, row, 0x20);
		}
		if(ishex(keycode2ascii_normal[k])) {
			if(x >= length * 2)
				x--;
			buf[x] = keycode2ascii_normal[k];
			putchr(col + x, row, buf[x]);
			x++;
		}
	}
	buf[x] = 0;

	if(x == 0)
		return;
	sscanf(buf, "%x", &n);
	if(length == sizeof(uint8))
		*(uint8 *)num = n;
	else
		*(uint16 *)num = n;
}

/*
	レジスタ破壊
*/
static inline uint16 destroy16(void)
{
	static uint32 rnd = 0xffffffff;
	
	rnd = rnd * 65541 + 1;
	return rnd >> 16;
}
static inline uint8 destroy8(void)
{
	return destroy16() >> 8;
}

/*
	全レジスタを表示する
*/
static inline int iocs_bd03(Z80stat *z)
{
	clrall();
	putstr(0, 0, "PC=%04X  AF=%02X %02X", z->r16.pc, z->r.a, z->r.f);
	putstr(0, 1, "SP=%04X  BC=%02X %02X", z->r16.sp, z->r.b, z->r.c);
	putstr(0, 2, "IX=%04X  DE=%02X %02X", z->r16.ix, z->r.d, z->r.e);
	putstr(0, 3, "IY=%04X  HL=%02X %02X", z->r16.iy, z->r.h, z->r.l);
	getKeycode();

	return 1000;
}

/*
	少し待つ (PC-G850専用)
*/
static inline int iocs_8aad(Z80stat *z)
{
	return 1500;
}

/*
	グラフィック処理 (PC-G815専用)
*/
static inline int iocs_02_f9f8(Z80stat *z)
{
	z->r.a = point(z->r16.hl, z->r16.de);
	z->r.c = 1 << (z->r16.de % 8);
	return 1000;
}
static inline int iocs_0d_c76e(Z80stat *z)
{
	pset(z->r16.hl, z->r16.de, z->m[0x7f0f]);
	return 1000;
}
static inline int iocs_0d_c5fc(Z80stat *z)
{
	int16 x = z->m[0x7968] << 8U | z->m[0x7967];
	int16 y = z->m[0x796a] << 8U | z->m[0x7969];

	line(z->r16.hl, z->r16.de, x, y, z->m[0x7f0f], 0xffff);
	return 6000;
}
static inline int iocs_0d_c4a9(Z80stat *z)
{
	int16 x1 = z->r16.hl;
	int16 y1 = z->r16.de;
	int16 x2 = z->m[0x7968] << 8U | z->m[0x7967];
	int16 y2 = z->m[0x796a] << 8U | z->m[0x7969];

	z->r16.de = y2;
	box(x1, y1, x2, y2, z->m[0x7f0f], 0xffff);
	return 8000;
}
static inline int iocs_0d_c442(Z80stat *z)
{
	int16 x1 = z->r16.hl;
	int16 y1 = z->r16.de;
	int16 x2 = z->m[0x7968] << 8U | z->m[0x7967];
	int16 y2 = z->m[0x796a] << 8U | z->m[0x7969];

	z->r16.de = y2;
	box(x1, y1, x2, y2, z->m[0x777f], 0xffff);
	return 10000;
}
static inline int iocs_0d_c532(Z80stat *z)
{
	int16 x1 = z->r16.hl;
	int16 y1 = z->r16.de;
	int16 x2 = z->m[0x7968] << 8U | z->m[0x7967];
	int16 y2 = z->m[0x796a] << 8U | z->m[0x7969];

	z->r16.de = y2;
	boxfill(x1, y1, x2, y2, z->m[0x7f0f], 0xffff);
	return 10000;
}
static inline int iocs_0d_c595(Z80stat *z)
{
	int16 x = z->m[0x7968] << 8U | z->m[0x7967];
	int16 y = z->m[0x796a] << 8U | z->m[0x7969];

	line(z->r16.hl, z->r16.de, x, y, z->m[0x777f], 0xffff);
	return 6000;
}
static inline int iocs_02_f892(Z80stat *z)
{
	int16 x = z->m[0x79dc] << 8U | z->m[0x79db];
	int16 y = z->m[0x79de] << 8U | z->m[0x79dd];

	if(++(z->m[0x79db]) == 0)
		++(z->m[0x79dc]);
	z->r16.hl = x;
	z->r16.de = y & 0xff80;
	gprint(x, y, z->r.a);
	return 4000;
}
static inline int iocs_9490(Z80stat *z)
{
	uint16 address;
	uint8 page;

	if(useROM)
		return -1;

	page    = z->m[z->r16.pc + 3];
	address = z->m[z->r16.pc + 5] << 8U | z->m[z->r16.pc + 4];
	
	z->r16.pc = (z->m[z->r16.sp + 1] << 8U | z->m[z->r16.sp]) - 3;
	z->r16.sp += 2;

	switch(page) {
	case 0x02:
		switch(address) {
		case 0xf892:
			return iocs_02_f892(z);
		case 0xf9f8:
			return iocs_02_f9f8(z);
		}
		break;
	case 0x0d:
		switch(address) {
		case 0xc76e:
			return iocs_0d_c76e(z);
		case 0xc5fc:
			return iocs_0d_c5fc(z);
		case 0xc4a9:
			return iocs_0d_c4a9(z);
		case 0xc442:
			return iocs_0d_c442(z);
		case 0xc532:
			return iocs_0d_c532(z);
		case 0xc595:
			return iocs_0d_c595(z);
		}
		break;
	}

	/*
	printf("page=%02x address=%04x\n", page, address);
	fflush(stdout);
	*/

	return 1000;
}

/*
	グラフィック処理 (PC-G850専用)
*/
static inline int iocs_0e_ffca(Z80stat *z)
{
	z->r.a = point(z->r16.hl, z->r16.de);
	z->r.c = 1 << (z->r16.de % 8);
	return 1000;
}
static inline int iocs_0d_ffd0(Z80stat *z)
{
	pset(z->r16.hl, z->r16.de, z->m[0x777f]);
	return 1000;
}
static inline int iocs_0d_ffd3(Z80stat *z)
{
	int16 x = z->m[0x7968] << 8U | z->m[0x7967];
	int16 y = z->m[0x796a] << 8U | z->m[0x7969];

	line(z->r16.hl, z->r16.de, x, y, z->m[0x777f], 0xffff);
	return 5000;
}
static inline int iocs_0d_ffd6(Z80stat *z)
{
	int16 x1 = z->r16.hl;
	int16 y1 = z->r16.de;
	int16 x2 = z->m[0x7968] << 8U | z->m[0x7967];
	int16 y2 = z->m[0x796a] << 8U | z->m[0x7969];
	
	z->r16.de = y2;
	box(x1, y1, x2, y2, z->m[0x777f], 0xffff);
	return 6000;
}
static inline int iocs_0d_ffd9(Z80stat *z)
{
	int16 x1 = z->r16.hl;
	int16 y1 = z->r16.de;
	int16 x2 = z->m[0x7968] << 8U | z->m[0x7967];
	int16 y2 = z->m[0x796a] << 8U | z->m[0x7969];
	
	z->r16.de = y2;
	boxfill(x1, y1, x2, y2, z->m[0x777f], 0xffff);
	return 10000;
}
static inline int iocs_0e_ffa3(Z80stat *z)
{
	int16 x = z->m[0x79dc] << 8U | z->m[0x79db];
	int16 y = z->m[0x79de] << 8U | z->m[0x79dd];

	if(++(z->m[0x79db]) == 0)
		++(z->m[0x79dc]);
	z->r16.hl = x;
	z->r16.de = y & 0xff80;
	gprint(x, y, z->r.a);
	return 3000;
}
static inline int iocs_bb6b(Z80stat *z)
{
	uint16 address;
	uint8 page;

	if(useROM)
		return -1;

	page    = z->m[z->r16.pc + 3];
	address = z->m[z->r16.pc + 5] << 8U | z->m[z->r16.pc + 4];

	z->r16.pc = (z->m[z->r16.sp + 1] << 8U | z->m[z->r16.sp]) - 3;
	z->r16.sp += 2;

	switch(page) {
	case 0x0d:
		switch(address) {
		case 0xc76e:
		case 0xffd0:
			return iocs_0d_ffd0(z);
		case 0xc595:
		case 0xffd3:
			return iocs_0d_ffd3(z);
		case 0xc442:
		case 0xffd6:
			return iocs_0d_ffd6(z);
		case 0xc4cb:
		case 0xffd9:
			return iocs_0d_ffd9(z);
		}
		break;
	case 0x0e:
		switch(address) {
		case 0xca08:
		case 0xffca:
			return iocs_0e_ffca(z);
		case 0xc92e:
		case 0xffa3:
			return iocs_0e_ffa3(z);
		}
		break;
	}

	/*
	printf("page=%02x address=%04x\n", page, address);
	fflush(stdout);
	*/

	return 1000;
}

/*
	割り込み先 (PC-G850専用)
*/
static inline int iocs_bc37(Z80stat *z)
{
	z->r.halt = 0;
	z->r.iff  = 3;
	interruptMask = 0x0f;

	return 1000;
}

/*
	押されているキーのASCIIコードを得る(waitあり) (PC-G850専用)
*/
static inline int iocs_bcc4(Z80stat *z)
{
	waitRelease();

	z->r.a = ggetchr();
	z->r.f |= 0x01;

	z->r.f |= destroy8() & 0xfe;
	z->r.b = 0;
	z->r.c = destroy8();
	z->r16.hl   = destroy16();
	z->r16.bc_d = destroy16();
	z->r16.de_d = destroy16();
	z->r16.hl_d = destroy16();

/*
	if(k == GKEY_BASIC || k == GKEY_TEXT)
		z->r16.sp = z->i.stack_under + 2;
*/

	return 100000;
}

/*
	押されているキーを得る(waitなし)
*/
static inline int iocs_be53(Z80stat *z)
{
	z->r.a = peekKeycode();
	if(z->r.a) 
		z->r.f |= 0x01;	/* ? */
	else
		z->r.f &= ~0x01;	/* ? */
	
	if(z->r.f & 0x01)
		z->r.b = destroy8();
	z->r.f |= destroy8() & 0xfe;
	z->r16.bc_d = destroy16();
	z->r16.de_d = destroy16();
	z->r16.hl_d = destroy16();

	switch(machine) {
	case MACHINE_E200:
	case MACHINE_G815:
		return 18000;
	case MACHINE_G850:
	default:
		return 30000;
	}
}

/*
	キーコードをASCIIコードに変換する
*/
static inline int iocs_be56(Z80stat *z)
{
	if(z->m[0x78f0] & 0x08) {
		z->r.b = z->r.a;
		z->r.f = 0x10;
	} else {
		z->r.a = keycode2ascii(z->r.a, z->m[0x7901] & 0x02);
		z->r.f = 0x44;
	}

	return 500;
}

/*
	1文字表示する(記号を含む)
*/
static inline int iocs_be5f(Z80stat *z)
{
	if(z->r.e >= vramCols || z->r.d >= vramRows)
		return 100;
	
	putchr(z->r.e, z->r.d, z->r.a);

	z->r16.af = destroy16();
	z->r.b   = 0;
	z->r.c   = destroy8();
	z->r16.hl = destroy16();

	return 1800;
}

/*
	1文字表示する(記号を含まない)
*/
static inline int iocs_be62(Z80stat *z)
{
	if(z->r.e >= vramCols || z->r.d >= vramRows)
		return 100;
	
	putchr(z->r.e, z->r.d, (z->r.a > 0x20 ? z->r.a: 0x20));

	z->r16.af = destroy16();
	z->r.b   = 0;
	z->r.c   = destroy8();
	z->r16.hl = destroy16();
	
	return 1800;
}

/*
	下にスクロールする
*/
static inline int iocs_be65(Z80stat *z)
{
	if(z->r.e >= vramCols || z->r.d >= vramRows)
		return 100;

	scrdown(z->r.e, z->r.d);

	z->r16.af = destroy16();
	z->r16.bc = 0x0000;
	z->r16.de = destroy16();
	z->r16.hl = destroy16();
	
	return 5000;
}

/*
	押されているキーを得る(waitあり)
*/
static inline int iocs_bcfd(Z80stat *z)
{
	z->r.a = getKeycode();
	z->r.f |= 0x01;

	z->r.f |= destroy8() & 0xfe;
	z->r16.bc_d = destroy16();
	z->r16.de_d = destroy16();
	z->r16.hl_d = destroy16();

	return 100000;
}

/*
	16進数2桁のキー入力を得る
*/
static inline int iocs_bd09(Z80stat *z)
{
	clrall();
	putstr(0, 0, "A=");
	gethex(&z->r.a, sizeof(z->r.a), 2, 0);
	clrall();

	z->r16.bc_d = destroy16();
	z->r16.de_d = destroy16();
	z->r16.hl_d = destroy16();

	return 100000;
}

/*
	16進数4桁のキー入力を得る
*/
static inline int iocs_bd0f(Z80stat *z)
{
	clrall();
	putstr(0, 0, "HL=");
	gethex(&z->r16.hl, sizeof(z->r16.hl), 3, 0);
	clrall();

	z->r16.af   = 0x0000;
	z->r16.bc_d = destroy16();
	z->r16.de_d = destroy16();
	z->r16.hl_d = destroy16();

	return 100000;
}

/*
	押されているキーを得る(waitあり)
*/
static inline int iocs_bfcc(Z80stat *z)
{
	z->r.a = getKeycode();
	z->r.f |= 0x01;

	z->r.f |= destroy8() & 0xfe;

	return 100000;
}

/*
	パターンを表示する
*/
static inline int iocs_bfd0(Z80stat *z)
{
	int state, width, b;

	if(machine == MACHINE_E200 || machine == MACHINE_G815)
		width = lcdWidth;
	else
		width = lcdWidth + 1;

	if((int )z->r.e * cellWidth + z->r.b > width) {
		if((b = width - (int )z->r.e * cellWidth) < 0)
			b = 0;
		z->r.b = b;
	}

	if(z->r.e >= vramCols || z->r.d >= vramRows || z->r.b == 0)
		return 100;

	putpat(z->r.e, z->r.d, &z->m[z->r16.hl], z->r.b);

	state = 100 + 170 * z->r.b;
	z->r.a = z->m[z->r16.hl];
	z->r.e += z->r.b;
	z->r16.hl += z->r.b - 1;
	z->r.b = 0;

	z->r.f = destroy8();

	return state;
}

/*
	上にスクロールする
*/
static inline int iocs_bfeb(Z80stat *z)
{
	scrup();

	z->r16.af = 0x0044;
	z->r.b = 0;
	z->r16.hl = destroy16();
	
	return 5000;
}

/*
	n個の文字を表示する
*/
static inline int iocs_bfee(Z80stat *z)
{
	int state;

	if(z->r.e >= vramCols || z->r.d >= vramRows || z->r.b == 0)
		return 100;

	state = 100 + 1800 * z->r.b;

	putchr(z->r.e, z->r.d, z->r.a);
	while(--z->r.b)
		putchrNext(&z->r.e, &z->r.d, z->r.a);

	z->r.a   = 0;
	z->r.f   = destroy8();
	z->r16.hl = destroy16();

	return state;
}

/*
	文字列を表示する
*/
static inline int iocs_bff1(Z80stat *z)
{
	int state;

	if(z->r.e >= vramCols || z->r.d >= vramRows || z->r.b == 0)
		return 100;

	state = 100 + 1800 * z->r.b;

	z->r.c = 0;
	putchr(z->r.e, z->r.d, z->m[z->r16.hl]);
	while(--z->r.b)
		if(putchrNext(&z->r.e, &z->r.d, z->m[++z->r16.hl]))
			z->r.c++;

	z->r16.af = destroy16();

	return state;
}

/*
	起動する
*/
static inline int iocs_bff4(Z80stat *z)
{
	z->r16.sp = 0x7ff6;
	z->m[0x7902] = 0x40;
	return 0;
}

/*
	電源を切る
*/
static inline int iocs_c110(Z80stat *z)
{
	poweroff();
	return 0;
}

/*
	IOCSコールをエミュレートする
*/
int z80subroutine(Z80stat *z, uint16 address)
{
	if(address < 0x8000 && address > 0x0040)
		return -1;

	switch(address) {
	case 0x0030:
		return iocs_bd03(z);
	case 0xbcfd:
		return iocs_bcfd(z);
	case 0xbe53:
		return iocs_be53(z);
	case 0xbe56:
		return iocs_be56(z);
	case 0xbe5f:
		return iocs_be5f(z);
	case 0xbe62:
		return iocs_be62(z);
	case 0xbe65:
		return iocs_be65(z);
	case 0xbd03:
		return iocs_bd03(z);
	case 0xbd09:
		return iocs_bd09(z);
	case 0xbd0f:
		return iocs_bd0f(z);
	case 0xbfcc:
		return iocs_bfcc(z);
	case 0xbfcd:
		return iocs_bfcc(z);
	case 0xbfd0:
		return iocs_bfd0(z);
	case 0xbfeb:
		return iocs_bfeb(z);
	case 0xbfee:
		return iocs_bfee(z);
	case 0xbff1:
		return iocs_bff1(z);
	case 0xbff4:
		return iocs_bff4(z);
	case 0xc110:
		return iocs_c110(z);
	}
	
	switch(machine) {
	case MACHINE_E200:
		break;
	case MACHINE_G815:
		switch(address) {
		case 0x93cd:
		case 0x9490:
			return iocs_9490(z);
		}
		break;
	case MACHINE_G850:
		switch(address) {
		case 0x8aad:
			return iocs_8aad(z);
		case 0x93cb:
		case 0x93cd:
		case 0xbb6b:
			return iocs_bb6b(z);
		case 0xbc37:
			return iocs_bc37(z);
		case 0xbcc4:
			return iocs_bcc4(z);
		}
		break;
	}

	if(address >= 0x8000) {
#ifdef WARN_UNKNOWN_IO
		printf("UNKNOWN CALL %04x\n", address);
#endif
		return (useROM ? -1: 100);
	} else
		return -1;
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
