/*
        Zilog Z80 Emulator Profiler
*/

#include "z80.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

#if defined(Z80_PROF)

/*
        プロファイラを初期化する
*/
void z80prof_init(Z80stat *z, int use) {
  Z80prof *prof = &z->i.prof;

  if (!use) {
    prof->stack = NULL;
    prof->path = NULL;
    return;
  }

  prof->stack = malloc(sizeof(*prof->stack) * 32);
  prof->cur = NULL;
  prof->record = malloc(sizeof(*prof->record));
  memset(prof->record, 0, sizeof(*prof->record));

  prof->path = malloc(sizeof(prof->path[0]) * 0x100);
  memset(prof->path, 0, sizeof(prof->path[0]) * 0x100);
  prof->pos = NULL;
}

/*
        記録を消去する
*/
void z80prof_clear(Z80stat *z) {
  Z80prof *prof = &z->i.prof;
  int i;

  if (prof->stack == NULL)
    return;

  free(prof->stack);
  free(prof->record);
  prof->cur = NULL;

  for (i = 0; i < 0x100; i++)
    if (prof->path[i] != NULL)
      free(prof->path[i]);
  free(prof->path);
  prof->pos = NULL;

  z80prof_init(z, 1);
}

/*
        呼び出しを記録する
*/
void z80prof_call(Z80stat *z, int call_states) {
  Z80prof *prof = &z->i.prof;

  if (prof->stack == NULL)
    return;

  if (prof->cur == NULL)
    prof->cur = prof->stack;
  else
    prof->cur++;

  prof->cur->bank = z80bank(z, z->r16.pc);
  prof->cur->address = z->r16.pc;
  prof->cur->sp = z->r16.sp;
  prof->cur->states = Z80_STATES(z) + call_states;
}

/*
        経過ステート数を求める (z80prof_retの下請け)
*/
static int z80elapsed(Z80stat *z, int states) {
  int elapsed = Z80_STATES(z) - states;

  if (elapsed >= 0)
    return elapsed;
  else
    return INT_MAX - elapsed;
}

/*
        呼び出し記録を検索する (z80prof_retの下請け)
*/
static Z80record *z80prof_find(Z80prof *prof, uint16 bank, uint16 address) {
  int n;
  Z80record *p;

  for (n = 0, p = prof->record; p->states != 0; p++, n++)
    if (p->bank == bank && p->address == address)
      return p;

  prof->record = realloc(prof->record, (n + 2) * sizeof(*prof->record));
  memset(&prof->record[n + 1], 0, sizeof(prof->record[n + 1]));
  return &prof->record[n];
}

/*
        復帰を記録する
*/
void z80prof_ret(Z80stat *z, int ret_states) {
  Z80prof *prof = &z->i.prof;
  Z80record *record;

  if (prof->stack == NULL)
    return;

  do {
    if (prof->cur == NULL)
      return;

    record = z80prof_find(prof, prof->cur->bank, prof->cur->address);
    record->bank = prof->cur->bank;
    record->address = prof->cur->address;
    record->count++;
    record->states += z80elapsed(z, prof->cur->states - ret_states);

    if (z->r16.pc >= 3 && (prof->path[record->bank][z->r16.pc - 3].code[0] ==
                               0xcd || /* call mn */
                           prof->path[record->bank][z->r16.pc - 3].code[0] ==
                               0xc4 || /* call NZ, mn */
                           prof->path[record->bank][z->r16.pc - 3].code[0] ==
                               0xcc || /* call Z, mn */
                           prof->path[record->bank][z->r16.pc - 3].code[0] ==
                               0xd4 || /* call NC, mn */
                           prof->path[record->bank][z->r16.pc - 3].code[0] ==
                               0xdc || /* call C, mn */
                           prof->path[record->bank][z->r16.pc - 3].code[0] ==
                               0xe4 || /* call PO, mn */
                           prof->path[record->bank][z->r16.pc - 3].code[0] ==
                               0xec || /* call PE, mn */
                           prof->path[record->bank][z->r16.pc - 3].code[0] ==
                               0xf4 || /* call P, mn */
                           prof->path[record->bank][z->r16.pc - 3].code[0] ==
                               0xfc) /* call M, mn */
    )
      prof->path[record->bank][z->r16.pc - 3].sub_states +=
          z80elapsed(z, prof->cur->states - ret_states);
    else if (z->r16.pc >= 1 &&
             (prof->path[record->bank][z->r16.pc - 1].code[0] ==
                  0xc7 || /* rst 00H */
              prof->path[record->bank][z->r16.pc - 1].code[0] ==
                  0xcf || /* rst 08H */
              prof->path[record->bank][z->r16.pc - 1].code[0] ==
                  0xd7 || /* rst 10H */
              prof->path[record->bank][z->r16.pc - 1].code[0] ==
                  0xdf || /* rst 18H */
              prof->path[record->bank][z->r16.pc - 1].code[0] ==
                  0xe7 || /* rst 20H */
              prof->path[record->bank][z->r16.pc - 1].code[0] ==
                  0xef || /* rst 28H */
              prof->path[record->bank][z->r16.pc - 1].code[0] ==
                  0xf7 || /* rst 30H */
              prof->path[record->bank][z->r16.pc - 1].code[0] ==
                  0xff) /* rst 38H */
    )
      prof->path[record->bank][z->r16.pc - 1].sub_states +=
          z80elapsed(z, prof->cur->states - ret_states);

    if (prof->cur == prof->stack)
      prof->cur = NULL;
    else
      prof->cur--;
  } while (prof->cur != NULL && prof->cur->sp < z->r16.sp);
}

/*
        通過を記録する
*/
void z80prof_path(Z80stat *z) {
  Z80prof *prof = &z->i.prof;
  int bank;

  if (prof->stack == NULL)
    return;

  bank = z80bank(z, z->r16.pc);
  if (bank < 0 || bank >= 0x100)
    return;

  if (prof->path[bank] == NULL) {
    prof->path[bank] = malloc(sizeof(prof->path[bank][0]) * 0x10000);
    memset(prof->path[bank], 0, sizeof(prof->path[bank][0]) * 0x10000);
  }

  prof->pos = &prof->path[bank][z->r16.pc];
}

/*
        実行を記録する
*/
void z80prof_exec(Z80stat *z, int states) {
  Z80prof *prof = &z->i.prof;
  int bank, i;

  if (prof->stack == NULL)
    return;
  if (prof->pos == NULL)
    return;

  bank = z80bank(z, z->r16.pc);
  if (bank < 0 || bank >= 0x100)
    return;

  if (prof->pos->count == 0)
    for (i = 0; i < 4; i++)
      prof->pos->code[i] =
          z80read8(z, (int)(prof->pos - &prof->path[bank][0]) + i);

  prof->pos->count++;
  prof->pos->states += states;
}

/*
        条件成立・不成立を記録する
*/
void z80prof_cond(Z80stat *z, int cond) {
  Z80prof *prof = &z->i.prof;

  if (prof->stack == NULL)
    return;
  if (prof->pos == NULL)
    return;
  if (!cond)
    return;

  prof->pos->cond++;
}

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
