/*
        SHARP PC-G800series emulator
        Memory access for g800
*/

#if !defined(Z80MEMORY_H)
#define Z80MEMORY_H

uint8_t z80read8(const Z80stat *z, uint16 address);
void z80write8(const Z80stat *z, uint16 address, uint8_t value);
uint16 z80read16(const Z80stat *z, uint16 address);
void z80write16(const Z80stat *z, uint16 address, uint16 value);

#endif

/*
        Copyright 2005 ~ 2008 maruhiro
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
