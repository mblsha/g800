/*
	SHARP PC-G800 series emulator
*/

#if !defined(DEPEND_H)
#define DEPEND_H

#include <stdio.h>
#include "SDL.h"
#include "sdlxpm.h"

/*
	マクロ
*/
/* エミュレートするマシンのキーコード */
#if SDL_MAJOR_VERSION == 2
#define printf	SDL_Log
#endif

/* PC(エミュレートする側)のキーコード */
#if SDL_MAJOR_VERSION == 2
#define KEY_NONE	0
#define KEY_BACKSPACE	SDL_SCANCODE_BACKSPACE
#define KEY_TAB	SDL_SCANCODE_TAB
#define KEY_CLEAR	SDL_SCANCODE_NUMLOCKCLEAR
#define KEY_RETURN	SDL_SCANCODE_RETURN
#define KEY_PAUSE	SDL_SCANCODE_PAUSE
#define KEY_ESCAPE	SDL_SCANCODE_ESCAPE
#define KEY_SPACE	SDL_SCANCODE_SPACE
#define KEY_QUOTE	SDL_SCANCODE_APOSTROPHE
#define KEY_COMMA	SDL_SCANCODE_COMMA
#define KEY_MINUS	SDL_SCANCODE_MINUS
#define KEY_PERIOD	SDL_SCANCODE_PERIOD
#define KEY_SLASH	SDL_SCANCODE_SLASH
#define KEY_0	SDL_SCANCODE_0
#define KEY_1	SDL_SCANCODE_1
#define KEY_2	SDL_SCANCODE_2
#define KEY_3	SDL_SCANCODE_3
#define KEY_4	SDL_SCANCODE_4
#define KEY_5	SDL_SCANCODE_5
#define KEY_6	SDL_SCANCODE_6
#define KEY_7	SDL_SCANCODE_7
#define KEY_8	SDL_SCANCODE_8
#define KEY_9	SDL_SCANCODE_9
#define KEY_SEMICOLON	SDL_SCANCODE_SEMICOLON
#define KEY_EQUALS	SDL_SCANCODE_EQUALS
#define KEY_LEFTBRACKET	SDL_SCANCODE_LEFTBRACKET
#define KEY_BACKSLASH	SDL_SCANCODE_BACKSLASH
#define KEY_RIGHTBRACKET	SDL_SCANCODE_RIGHTBRACKET
#define KEY_BACKQUOTE	SDL_SCANCODE_GRAVE
#define KEY_A	SDL_SCANCODE_A
#define KEY_B	SDL_SCANCODE_B
#define KEY_C	SDL_SCANCODE_C
#define KEY_D	SDL_SCANCODE_D
#define KEY_E	SDL_SCANCODE_E
#define KEY_F	SDL_SCANCODE_F
#define KEY_G	SDL_SCANCODE_G
#define KEY_H	SDL_SCANCODE_H
#define KEY_I	SDL_SCANCODE_I
#define KEY_J	SDL_SCANCODE_J
#define KEY_K	SDL_SCANCODE_K
#define KEY_L	SDL_SCANCODE_L
#define KEY_M	SDL_SCANCODE_M
#define KEY_N	SDL_SCANCODE_N
#define KEY_O	SDL_SCANCODE_O
#define KEY_P	SDL_SCANCODE_P
#define KEY_Q	SDL_SCANCODE_Q
#define KEY_R	SDL_SCANCODE_R
#define KEY_S	SDL_SCANCODE_S
#define KEY_T	SDL_SCANCODE_T
#define KEY_U	SDL_SCANCODE_U
#define KEY_V	SDL_SCANCODE_V
#define KEY_W	SDL_SCANCODE_W
#define KEY_X	SDL_SCANCODE_X
#define KEY_Y	SDL_SCANCODE_Y
#define KEY_Z	SDL_SCANCODE_Z
#define KEY_DELETE	SDL_SCANCODE_DELETE
#define KEY_KP0	SDL_SCANCODE_KP_0
#define KEY_KP1	SDL_SCANCODE_KP_1
#define KEY_KP2	SDL_SCANCODE_KP_2
#define KEY_KP3	SDL_SCANCODE_KP_3
#define KEY_KP4	SDL_SCANCODE_KP_4
#define KEY_KP5	SDL_SCANCODE_KP_5
#define KEY_KP6	SDL_SCANCODE_KP_6
#define KEY_KP7	SDL_SCANCODE_KP_7
#define KEY_KP8	SDL_SCANCODE_KP_8
#define KEY_KP9	SDL_SCANCODE_KP_9
#define KEY_KP_PERIOD	SDL_SCANCODE_KP_PERIOD
#define KEY_KP_DIVIDE	SDL_SCANCODE_KP_DIVIDE
#define KEY_KP_MULTIPLY	SDL_SCANCODE_KP_MULTIPLY
#define KEY_KP_MINUS	SDL_SCANCODE_KP_MINUS
#define KEY_KP_PLUS	SDL_SCANCODE_KP_PLUS
#define KEY_KP_ENTER	SDL_SCANCODE_KP_ENTER
#define KEY_KP_EQUALS	SDL_SCANCODE_KP_EQUALS
#define KEY_UP	SDL_SCANCODE_UP
#define KEY_DOWN	SDL_SCANCODE_DOWN
#define KEY_RIGHT	SDL_SCANCODE_RIGHT
#define KEY_LEFT	SDL_SCANCODE_LEFT
#define KEY_INSERT	SDL_SCANCODE_INSERT
#define KEY_HOME	SDL_SCANCODE_HOME
#define KEY_END	SDL_SCANCODE_END
#define KEY_PAGEUP	SDL_SCANCODE_PAGEUP
#define KEY_PAGEDOWN	SDL_SCANCODE_PAGEDOWN
#define KEY_F1	SDL_SCANCODE_F1
#define KEY_F2	SDL_SCANCODE_F2
#define KEY_F3	SDL_SCANCODE_F3
#define KEY_F4	SDL_SCANCODE_F4
#define KEY_F5	SDL_SCANCODE_F5
#define KEY_F6	SDL_SCANCODE_F6
#define KEY_F7	SDL_SCANCODE_F7
#define KEY_F8	SDL_SCANCODE_F8
#define KEY_F9	SDL_SCANCODE_F9
#define KEY_F10	SDL_SCANCODE_F10
#define KEY_F11	SDL_SCANCODE_F11
#define KEY_F12	SDL_SCANCODE_F12
#define KEY_F13	SDL_SCANCODE_F13
#define KEY_F14	SDL_SCANCODE_F14
#define KEY_F15	SDL_SCANCODE_F15
#define KEY_NUMLOCK	SDL_SCANCODE_NUMLOCKCLEAR
#define KEY_CAPSLOCK	SDL_SCANCODE_CAPSLOCK
#define KEY_SCROLLOCK	SDL_SCANCODE_SCROLLLOCK
#define KEY_RSHIFT	SDL_SCANCODE_RSHIFT
#define KEY_LSHIFT	SDL_SCANCODE_LSHIFT
#define KEY_RCTRL	SDL_SCANCODE_RCTRL
#define KEY_LCTRL	SDL_SCANCODE_LCTRL
#define KEY_RALT	SDL_SCANCODE_RALT
#define KEY_LALT	SDL_SCANCODE_LALT
#define KEY_MODE	SDL_SCANCODE_MODE
#define KEY_COMPOSE	SDL_SCANCODE_APPLICATION
#define KEY_HELP	SDL_SCANCODE_HELP
#define KEY_PRINT	SDL_SCANCODE_PRINTSCREEN
#define KEY_SYSREQ	SDL_SCANCODE_SYSREQ
#define KEY_BREAK	SDL_SCANCODE_STOP
#define KEY_MENU	SDL_SCANCODE_MENU
#define KEY_POWER	SDL_SCANCODE_POWER
#define KEY_UNDERSCORE	SDL_SCANCODE_INTERNATIONAL1
#define KEY_KANA	SDL_SCANCODE_INTERNATIONAL2
#define KEY_YEN	SDL_SCANCODE_INTERNATIONAL3
#define KEY_XFER	SDL_SCANCODE_INTERNATIONAL4
#define KEY_NFER	SDL_SCANCODE_INTERNATIONAL5
#define KEY_LAST	SDL_NUM_SCANCODES
#elif SDL_MAJOR_VERSION == 1
#define KEY_NONE	0
#define KEY_BACKSPACE	SDLK_BACKSPACE
#define KEY_TAB	SDLK_TAB
#define KEY_CLEAR	SDLK_CLEAR
#define KEY_RETURN	SDLK_RETURN
#define KEY_PAUSE	SDLK_PAUSE
#define KEY_ESCAPE	SDLK_ESCAPE
#define KEY_SPACE	SDLK_SPACE
#define KEY_QUOTE	SDLK_QUOTE
#define KEY_COMMA	SDLK_COMMA
#define KEY_MINUS	SDLK_MINUS
#define KEY_PERIOD	SDLK_PERIOD
#define KEY_SLASH	SDLK_SLASH
#define KEY_0	SDLK_0
#define KEY_1	SDLK_1
#define KEY_2	SDLK_2
#define KEY_3	SDLK_3
#define KEY_4	SDLK_4
#define KEY_5	SDLK_5
#define KEY_6	SDLK_6
#define KEY_7	SDLK_7
#define KEY_8	SDLK_8
#define KEY_9	SDLK_9
#define KEY_SEMICOLON	SDLK_SEMICOLON
#define KEY_EQUALS	SDLK_EQUALS
#define KEY_LEFTBRACKET	SDLK_LEFTBRACKET
#define KEY_BACKSLASH	SDLK_BACKSLASH
#define KEY_RIGHTBRACKET	SDLK_RIGHTBRACKET
#define KEY_BACKQUOTE	SDLK_BACKQUOTE
#define KEY_A	SDLK_a
#define KEY_B	SDLK_b
#define KEY_C	SDLK_c
#define KEY_D	SDLK_d
#define KEY_E	SDLK_e
#define KEY_F	SDLK_f
#define KEY_G	SDLK_g
#define KEY_H	SDLK_h
#define KEY_I	SDLK_i
#define KEY_J	SDLK_j
#define KEY_K	SDLK_k
#define KEY_L	SDLK_l
#define KEY_M	SDLK_m
#define KEY_N	SDLK_n
#define KEY_O	SDLK_o
#define KEY_P	SDLK_p
#define KEY_Q	SDLK_q
#define KEY_R	SDLK_r
#define KEY_S	SDLK_s
#define KEY_T	SDLK_t
#define KEY_U	SDLK_u
#define KEY_V	SDLK_v
#define KEY_W	SDLK_w
#define KEY_X	SDLK_x
#define KEY_Y	SDLK_y
#define KEY_Z	SDLK_z
#define KEY_DELETE	SDLK_DELETE
#define KEY_KP0	SDLK_KP0
#define KEY_KP1	SDLK_KP1
#define KEY_KP2	SDLK_KP2
#define KEY_KP3	SDLK_KP3
#define KEY_KP4	SDLK_KP4
#define KEY_KP5	SDLK_KP5
#define KEY_KP6	SDLK_KP6
#define KEY_KP7	SDLK_KP7
#define KEY_KP8	SDLK_KP8
#define KEY_KP9	SDLK_KP9
#define KEY_KP_PERIOD	SDLK_KP_PERIOD
#define KEY_KP_DIVIDE	SDLK_KP_DIVIDE
#define KEY_KP_MULTIPLY	SDLK_KP_MULTIPLY
#define KEY_KP_MINUS	SDLK_KP_MINUS
#define KEY_KP_PLUS	SDLK_KP_PLUS
#define KEY_KP_ENTER	SDLK_KP_ENTER
#define KEY_KP_EQUALS	SDLK_KP_EQUALS
#define KEY_UP	SDLK_UP
#define KEY_DOWN	SDLK_DOWN
#define KEY_RIGHT	SDLK_RIGHT
#define KEY_LEFT	SDLK_LEFT
#define KEY_INSERT	SDLK_INSERT
#define KEY_HOME	SDLK_HOME
#define KEY_END	SDLK_END
#define KEY_PAGEUP	SDLK_PAGEUP
#define KEY_PAGEDOWN	SDLK_PAGEDOWN
#define KEY_F1	SDLK_F1
#define KEY_F2	SDLK_F2
#define KEY_F3	SDLK_F3
#define KEY_F4	SDLK_F4
#define KEY_F5	SDLK_F5
#define KEY_F6	SDLK_F6
#define KEY_F7	SDLK_F7
#define KEY_F8	SDLK_F8
#define KEY_F9	SDLK_F9
#define KEY_F10	SDLK_F10
#define KEY_F11	SDLK_F11
#define KEY_F12	SDLK_F12
#define KEY_F13	SDLK_F13
#define KEY_F14	SDLK_F14
#define KEY_F15	SDLK_F15
#define KEY_NUMLOCK	SDLK_NUMLOCK
#define KEY_CAPSLOCK	SDLK_CAPSLOCK
#define KEY_SCROLLOCK	SDLK_SCROLLOCK
#define KEY_RSHIFT	SDLK_RSHIFT
#define KEY_LSHIFT	SDLK_LSHIFT
#define KEY_RCTRL	SDLK_RCTRL
#define KEY_LCTRL	SDLK_LCTRL
#define KEY_RALT	SDLK_RALT
#define KEY_LALT	SDLK_LALT
#define KEY_MODE	SDLK_MODE
#define KEY_COMPOSE	SDLK_COMPOSE
#define KEY_HELP	SDLK_HELP
#define KEY_PRINT	SDLK_PRINT
#define KEY_SYSREQ	SDLK_SYSREQ
#define KEY_BREAK	SDLK_BREAK
#define KEY_MENU	SDLK_MENU
#define KEY_POWER	SDLK_POWER
#define KEY_LAST	SDLK_LAST
#define KEY_UNDERSCORE	SDLK_UNDERSCORE
#define KEY_KANA	0
#define KEY_YEN	0
#define KEY_XFER	0
#define KEY_NFER	0
#endif

#define MODKEY_ALT	0x1000
#define MODKEY_CTRL	0x2000
#define MODKEY_SHIFT	0x4000

#endif

/*
	Copyright 2005 ~ 2008 maruhiro
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
