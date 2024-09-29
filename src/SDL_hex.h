/*
	IntelHEXå`éÆèàóùÉwÉbÉ_(SDL_hex.h)
*/

#if !defined(SDLHEX_H)
#define SDLHEX_H

#include <stdio.h>
#include "SDL.h"

Sint64 SDLHex_Load_RW(SDL_RWops *, void *, Sint64 *, size_t, int, int);
Sint64 SDLHex_LoadAbs_RW(SDL_RWops *, void *, Sint64 *, size_t, int, int);
Sint64 SDLHex_Load(const char *, void *, Sint64 *, size_t, int);
Sint64 SDLHex_LoadAbs(const char *, void *, Sint64 *, size_t, int);
Sint64 SDLHex_Save_RW(SDL_RWops *, const void *, Sint64, size_t, int);
Sint64 SDLHex_SaveAbs_RW(SDL_RWops *, const void *, Sint64, size_t, int);
Sint64 SDLHex_Save(const char *, const void *, Sint64, size_t);
Sint64 SDLHex_SaveAbs(const char *, const void *, Sint64, size_t);

#endif

/*
	Copyright 2014 maruhiro
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
