/*
        SHARP PC-G800 series emulator
        初期化
*/

#include "g800.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* キーボード */
#define KEYBOARD_EN 0 /* 英語 */
#define KEYBOARD_JP 1 /* 日本語 */

/* エミュレートする側のキー名を設定する */
#define SET_TABLE_KEY(t, key, code)                                            \
  setOptTable(t, key, code);                                                   \
  setOptTable(t, "%" key, MODKEY_ALT | code);                                  \
  setOptTable(t, "^" key, MODKEY_CTRL | code);                                 \
  setOptTable(t, "+" key, MODKEY_SHIFT | code)

/* キー割り当て */
struct KeyAssign {
  int key;     /* エミュレートする側のキーコード */
  uint16 gkey; /* PC-G800のキーコード */
};

/* Yes/No */
const static OptTable tableYesNo[] = {{"y", TRUE},  {"yes", TRUE}, {"1", TRUE},
                                      {"n", FALSE}, {"no", FALSE}, {"0", FALSE},
                                      {NULL, 0}};

/* マシン名 */
const static OptTable tableMachine[] = {
    {"e200", MACHINE_E200},          {"g815", MACHINE_G815},
    {"g850", MACHINE_G850},          {"e220", MACHINE_PCE220},
    {"PC-G801", MACHINE_PCG801},     {"PC-E200", MACHINE_PCE200},
    {"PC-G802", MACHINE_PCG802},     {"PC-G803", MACHINE_PCG803},
    {"PC-G805", MACHINE_PCG805},     {"PC-G811", MACHINE_PCG811},
    {"PC-G813", MACHINE_PCG813},     {"PC-G820", MACHINE_PCG820},
    {"PC-G830", MACHINE_PCG830},     {"PC-E220", MACHINE_PCE220},
    {"PC-G815", MACHINE_PCG815},     {"PC-G850", MACHINE_PCG850},
    {"PC-G850S", MACHINE_PCG850S},   {"PC-G850V", MACHINE_PCG850V},
    {"PC-G850VS", MACHINE_PCG850VS}, {NULL, 0}};

/* ブザー */
const static OptTable tableBuzzer[] = {
    {"n", BUZZER_NONE},    {"no", BUZZER_NONE}, {"0", BUZZER_NONE},
    {"y", BUZZER_USE},     {"yes", BUZZER_USE}, {"1", BUZZER_USE},
    {"sync", BUZZER_SYNC}, {"2", BUZZER_SYNC},  {NULL, 0}};

/* キーボード */
const static OptTable tableKeyboard[] = {
    {"en", KEYBOARD_EN}, {"jp", KEYBOARD_JP}, {NULL, 0}};

/* PC-G800のキー名 */
const static OptTable tableGkey[] = {{"off", GKEY_OFF},
                                     {"q", GKEY_Q},
                                     {"w", GKEY_W},
                                     {"e", GKEY_E},
                                     {"r", GKEY_R},
                                     {"t", GKEY_T},
                                     {"y", GKEY_Y},
                                     {"u", GKEY_U},
                                     {"a", GKEY_A},
                                     {"s", GKEY_S},
                                     {"d", GKEY_D},
                                     {"f", GKEY_F},
                                     {"g", GKEY_G},
                                     {"h", GKEY_H},
                                     {"j", GKEY_J},
                                     {"k", GKEY_K},
                                     {"z", GKEY_Z},
                                     {"x", GKEY_X},
                                     {"c", GKEY_C},
                                     {"v", GKEY_V},
                                     {"b", GKEY_B},
                                     {"n", GKEY_N},
                                     {"m", GKEY_M},
                                     {",", GKEY_COMMA},
                                     {"basic", GKEY_BASIC},
                                     {"text", GKEY_TEXT},
                                     {"capslock", GKEY_CAPS},
                                     {"kana", GKEY_KANA},
                                     {"tab", GKEY_TAB},
                                     {"space", GKEY_SPACE},
                                     {"down", GKEY_DOWN},
                                     {"up", GKEY_UP},
                                     {"left", GKEY_LEFT},
                                     {"right", GKEY_RIGHT},
                                     {"ans", GKEY_ANS},
                                     {"0", GKEY_0},
                                     {".", GKEY_PERIOD},
                                     {"=", GKEY_EQUAL},
                                     {"equal", GKEY_EQUAL},
                                     {"+", GKEY_PLUS},
                                     {"return", GKEY_RETURN},
                                     {"l", GKEY_L},
                                     {";", GKEY_SEMICOLON},
                                     {"const", GKEY_CONST},
                                     {"1", GKEY_1},
                                     {"2", GKEY_2},
                                     {"3", GKEY_3},
                                     {"-", GKEY_MINUS},
                                     {"m+", GKEY_MPLUS},
                                     {"i", GKEY_I},
                                     {"o", GKEY_O},
                                     {"insert", GKEY_INSERT},
                                     {"4", GKEY_4},
                                     {"5", GKEY_5},
                                     {"6", GKEY_6},
                                     {"*", GKEY_ASTER},
                                     {"rcm", GKEY_RCM},
                                     {"p", GKEY_P},
                                     {"backspace", GKEY_BACKSPACE},
                                     {"pi", GKEY_PI},
                                     {"7", GKEY_7},
                                     {"8", GKEY_8},
                                     {"9", GKEY_9},
                                     {"/", GKEY_SLASH},
                                     {"(", GKEY_LKAKKO},
                                     {"npr", GKEY_NPR},
                                     {"deg", GKEY_DEG},
                                     {"sqr", GKEY_SQR},
                                     {"squ", GKEY_SQU},
                                     {"^", GKEY_HAT},
                                     {")", GKEY_RKAKKO},
                                     {"rcp", GKEY_RCP},
                                     {"mdf", GKEY_MDF},
                                     {"2ndf", GKEY_2NDF},
                                     {"sin", GKEY_SIN},
                                     {"cos", GKEY_COS},
                                     {"ln", GKEY_LN},
                                     {"log", GKEY_LOG},
                                     {"tan", GKEY_TAN},
                                     {"fe", GKEY_FE},
                                     {"cls", GKEY_CLS},
                                     {"on", GKEY_BREAK},
                                     {"shift", GKEY_SHIFT},
                                     {"reset", GKEY_RESET},
                                     {"trace", GKEY_DEBUG},
                                     {"11pin1", GKEY_11PIN1},
                                     {"11pin2", GKEY_11PIN2},
                                     {"11pin3", GKEY_11PIN3},
                                     {"11pin4", GKEY_11PIN4},
                                     {"11pin5", GKEY_11PIN5},
                                     {"11pin6", GKEY_11PIN6},
                                     {"11pin7", GKEY_11PIN7},
                                     {"11pin8", GKEY_11PIN8},
                                     {"menu", GKEY_MENU},
                                     {"copy", GKEY_COPY},
                                     {"paste", GKEY_PASTE},
                                     {"!", GMODKEY_SHIFT | GKEY_Q},
                                     {"\"", GMODKEY_SHIFT | GKEY_W},
                                     {"hash", GMODKEY_SHIFT | GKEY_E},
                                     {"$", GMODKEY_SHIFT | GKEY_R},
                                     {"%", GMODKEY_SHIFT | GKEY_T},
                                     {"&", GMODKEY_SHIFT | GKEY_Y},
                                     {"'", GMODKEY_SHIFT | GKEY_U},
                                     {"[", GMODKEY_SHIFT | GKEY_A},
                                     {"]", GMODKEY_SHIFT | GKEY_S},
                                     {"{", GMODKEY_SHIFT | GKEY_D},
                                     {"}", GMODKEY_SHIFT | GKEY_F},
                                     {"\\", GMODKEY_SHIFT | GKEY_G},
                                     {"|", GMODKEY_SHIFT | GKEY_H},
                                     {"‾", GMODKEY_SHIFT | GKEY_J},
                                     {"_", GMODKEY_SHIFT | GKEY_K},
                                     {"input", GMODKEY_SHIFT | GKEY_Z},
                                     {"print", GMODKEY_SHIFT | GKEY_X},
                                     {"cont", GMODKEY_SHIFT | GKEY_C},
                                     {"run", GMODKEY_SHIFT | GKEY_V},
                                     {"list", GMODKEY_SHIFT | GKEY_B},
                                     {"save", GMODKEY_SHIFT | GKEY_N},
                                     {"load", GMODKEY_SHIFT | GKEY_M},
                                     {"?", GMODKEY_SHIFT | GKEY_COMMA},
                                     {"asmbl", GMODKEY_SHIFT | GKEY_BASIC},
                                     {"clang", GMODKEY_SHIFT | GKEY_TEXT},
                                     {"contrast", GMODKEY_SHIFT | GKEY_ANS},
                                     {"drg", GMODKEY_SHIFT | GKEY_PERIOD},
                                     {"pnp", GMODKEY_SHIFT | GKEY_RETURN},
                                     {":", GMODKEY_SHIFT | GKEY_SEMICOLON},
                                     {"m-", GMODKEY_SHIFT | GKEY_MPLUS},
                                     {"<", GMODKEY_SHIFT | GKEY_I},
                                     {">", GMODKEY_SHIFT | GKEY_O},
                                     {"delete", GMODKEY_SHIFT | GKEY_INSERT},
                                     {"@", GMODKEY_SHIFT | GKEY_P},
                                     {"rnd", GMODKEY_SHIFT | GKEY_PI},
                                     {"degree", GMODKEY_SHIFT | GKEY_7},
                                     {"minute", GMODKEY_SHIFT | GKEY_8},
                                     {"second", GMODKEY_SHIFT | GKEY_9},
                                     {"xy", GMODKEY_SHIFT | GKEY_LKAKKO},
                                     {"ncr", GMODKEY_SHIFT | GKEY_NPR},
                                     {"dms", GMODKEY_SHIFT | GKEY_DEG},
                                     {"cur", GMODKEY_SHIFT | GKEY_SQR},
                                     {"cub", GMODKEY_SHIFT | GKEY_SQU},
                                     {"pol", GMODKEY_SHIFT | GKEY_HAT},
                                     {"base-n", GMODKEY_SHIFT | GKEY_RKAKKO},
                                     {"fact", GMODKEY_SHIFT | GKEY_RCP},
                                     {"stat", GMODKEY_SHIFT | GKEY_MDF},
                                     {"asn", GMODKEY_SHIFT | GKEY_SIN},
                                     {"acs", GMODKEY_SHIFT | GKEY_COS},
                                     {"exp", GMODKEY_SHIFT | GKEY_LN},
                                     {"ten", GMODKEY_SHIFT | GKEY_LOG},
                                     {"atn", GMODKEY_SHIFT | GKEY_TAN},
                                     {"digit", GMODKEY_SHIFT | GKEY_FE},
                                     {"ca", GMODKEY_SHIFT | GKEY_CLS},
                                     {NULL, 0}};

/* エミュレートする側のキー名 */
static OptTable tableKey[512] = {{"none", 0},
                                 {"backspace", KEY_BACKSPACE},
                                 {"tab", KEY_TAB},
                                 {"clear", KEY_CLEAR},
                                 {"return", KEY_RETURN},
                                 {"enter", KEY_RETURN},
                                 {"pause", KEY_PAUSE},
                                 {"escape", KEY_ESCAPE},
                                 {"space", KEY_SPACE},
                                 {"'", KEY_QUOTE},
                                 {",", KEY_COMMA},
                                 {"-", KEY_MINUS},
                                 {".", KEY_PERIOD},
                                 {"/", KEY_SLASH},
                                 {"0", KEY_0},
                                 {"1", KEY_1},
                                 {"2", KEY_2},
                                 {"3", KEY_3},
                                 {"4", KEY_4},
                                 {"5", KEY_5},
                                 {"6", KEY_6},
                                 {"7", KEY_7},
                                 {"8", KEY_8},
                                 {"9", KEY_9},
                                 {";", KEY_SEMICOLON},
                                 {"=", KEY_EQUALS},
                                 {"[", KEY_LEFTBRACKET},
                                 {"\\", KEY_BACKSLASH},
                                 {"]", KEY_RIGHTBRACKET},
                                 {"`", KEY_BACKQUOTE},
                                 {"a", KEY_A},
                                 {"b", KEY_B},
                                 {"c", KEY_C},
                                 {"d", KEY_D},
                                 {"e", KEY_E},
                                 {"f", KEY_F},
                                 {"g", KEY_G},
                                 {"h", KEY_H},
                                 {"i", KEY_I},
                                 {"j", KEY_J},
                                 {"k", KEY_K},
                                 {"l", KEY_L},
                                 {"m", KEY_M},
                                 {"n", KEY_N},
                                 {"o", KEY_O},
                                 {"p", KEY_P},
                                 {"q", KEY_Q},
                                 {"r", KEY_R},
                                 {"s", KEY_S},
                                 {"t", KEY_T},
                                 {"u", KEY_U},
                                 {"v", KEY_V},
                                 {"w", KEY_W},
                                 {"x", KEY_X},
                                 {"y", KEY_Y},
                                 {"z", KEY_Z},
                                 {"delete", KEY_DELETE},
                                 {"n0", KEY_KP0},
                                 {"n1", KEY_KP1},
                                 {"n2", KEY_KP2},
                                 {"n3", KEY_KP3},
                                 {"n4", KEY_KP4},
                                 {"n5", KEY_KP5},
                                 {"n6", KEY_KP6},
                                 {"n7", KEY_KP7},
                                 {"n8", KEY_KP8},
                                 {"n9", KEY_KP9},
                                 {"n.", KEY_KP_PERIOD},
                                 {"n/", KEY_KP_DIVIDE},
                                 {"n*", KEY_KP_MULTIPLY},
                                 {"n-", KEY_KP_MINUS},
                                 {"n+", KEY_KP_PLUS},
                                 {"nreturn", KEY_KP_ENTER},
                                 {"nenter", KEY_KP_ENTER},
                                 {"n=", KEY_KP_EQUALS},
                                 {"up", KEY_UP},
                                 {"down", KEY_DOWN},
                                 {"right", KEY_RIGHT},
                                 {"left", KEY_LEFT},
                                 {"insert", KEY_INSERT},
                                 {"home", KEY_HOME},
                                 {"end", KEY_END},
                                 {"pageup", KEY_PAGEUP},
                                 {"pagedown", KEY_PAGEDOWN},
                                 {"f1", KEY_F1},
                                 {"f2", KEY_F2},
                                 {"f3", KEY_F3},
                                 {"f4", KEY_F4},
                                 {"f5", KEY_F5},
                                 {"f6", KEY_F6},
                                 {"f7", KEY_F7},
                                 {"f8", KEY_F8},
                                 {"f9", KEY_F9},
                                 {"f10", KEY_F10},
                                 {"f11", KEY_F11},
                                 {"f12", KEY_F12},
                                 {"f13", KEY_F13},
                                 {"f14", KEY_F14},
                                 {"f15", KEY_F15},
                                 {"numlock", KEY_NUMLOCK},
                                 {"capslock", KEY_CAPSLOCK},
                                 {"scrolllock", KEY_SCROLLOCK},
                                 {"rshift", KEY_RSHIFT},
                                 {"lshift", KEY_LSHIFT},
                                 {"rctrl", KEY_RCTRL},
                                 {"lctrl", KEY_LCTRL},
                                 {"ralt", KEY_RALT},
                                 {"lalt", KEY_LALT},
                                 {"mode", KEY_MODE},
                                 {"compose", KEY_COMPOSE},
                                 {"help", KEY_HELP},
                                 {"print", KEY_PRINT},
                                 {"sysreq", KEY_SYSREQ},
                                 {"break", KEY_BREAK},
                                 {"menu", KEY_MENU},
                                 {"power", KEY_POWER},
                                 {"_", KEY_UNDERSCORE},
                                 {"kana", KEY_KANA},
                                 {"yen", KEY_YEN},
                                 {"xfer", KEY_XFER},
                                 {"nfer", KEY_NFER},
                                 {"%backspace", MODKEY_ALT | KEY_BACKSPACE},
                                 {"%tab", MODKEY_ALT | KEY_TAB},
                                 {"%clear", MODKEY_ALT | KEY_CLEAR},
                                 {"%return", MODKEY_ALT | KEY_RETURN},
                                 {"%enter", MODKEY_ALT | KEY_RETURN},
                                 {"%pause", MODKEY_ALT | KEY_PAUSE},
                                 {"%escape", MODKEY_ALT | KEY_ESCAPE},
                                 {"%space", MODKEY_ALT | KEY_SPACE},
                                 {"%'", MODKEY_ALT | KEY_QUOTE},
                                 {"%,", MODKEY_ALT | KEY_COMMA},
                                 {"%-", MODKEY_ALT | KEY_MINUS},
                                 {"%.", MODKEY_ALT | KEY_PERIOD},
                                 {"%/", MODKEY_ALT | KEY_SLASH},
                                 {"%0", MODKEY_ALT | KEY_0},
                                 {"%1", MODKEY_ALT | KEY_1},
                                 {"%2", MODKEY_ALT | KEY_2},
                                 {"%3", MODKEY_ALT | KEY_3},
                                 {"%4", MODKEY_ALT | KEY_4},
                                 {"%5", MODKEY_ALT | KEY_5},
                                 {"%6", MODKEY_ALT | KEY_6},
                                 {"%7", MODKEY_ALT | KEY_7},
                                 {"%8", MODKEY_ALT | KEY_8},
                                 {"%9", MODKEY_ALT | KEY_9},
                                 {"%;", MODKEY_ALT | KEY_SEMICOLON},
                                 {"%=", MODKEY_ALT | KEY_EQUALS},
                                 {"%[", MODKEY_ALT | KEY_LEFTBRACKET},
                                 {"%\\", MODKEY_ALT | KEY_BACKSLASH},
                                 {"%]", MODKEY_ALT | KEY_RIGHTBRACKET},
                                 {"%`", MODKEY_ALT | KEY_BACKQUOTE},
                                 {"%a", MODKEY_ALT | KEY_A},
                                 {"%b", MODKEY_ALT | KEY_B},
                                 {"%c", MODKEY_ALT | KEY_C},
                                 {"%d", MODKEY_ALT | KEY_D},
                                 {"%e", MODKEY_ALT | KEY_E},
                                 {"%f", MODKEY_ALT | KEY_F},
                                 {"%g", MODKEY_ALT | KEY_G},
                                 {"%h", MODKEY_ALT | KEY_H},
                                 {"%i", MODKEY_ALT | KEY_I},
                                 {"%j", MODKEY_ALT | KEY_J},
                                 {"%k", MODKEY_ALT | KEY_K},
                                 {"%l", MODKEY_ALT | KEY_L},
                                 {"%m", MODKEY_ALT | KEY_M},
                                 {"%n", MODKEY_ALT | KEY_N},
                                 {"%o", MODKEY_ALT | KEY_O},
                                 {"%p", MODKEY_ALT | KEY_P},
                                 {"%q", MODKEY_ALT | KEY_Q},
                                 {"%r", MODKEY_ALT | KEY_R},
                                 {"%s", MODKEY_ALT | KEY_S},
                                 {"%t", MODKEY_ALT | KEY_T},
                                 {"%u", MODKEY_ALT | KEY_U},
                                 {"%v", MODKEY_ALT | KEY_V},
                                 {"%w", MODKEY_ALT | KEY_W},
                                 {"%x", MODKEY_ALT | KEY_X},
                                 {"%y", MODKEY_ALT | KEY_Y},
                                 {"%z", MODKEY_ALT | KEY_Z},
                                 {"%delete", MODKEY_ALT | KEY_DELETE},
                                 {"%n0", MODKEY_ALT | KEY_KP0},
                                 {"%n1", MODKEY_ALT | KEY_KP1},
                                 {"%n2", MODKEY_ALT | KEY_KP2},
                                 {"%n3", MODKEY_ALT | KEY_KP3},
                                 {"%n4", MODKEY_ALT | KEY_KP4},
                                 {"%n5", MODKEY_ALT | KEY_KP5},
                                 {"%n6", MODKEY_ALT | KEY_KP6},
                                 {"%n7", MODKEY_ALT | KEY_KP7},
                                 {"%n8", MODKEY_ALT | KEY_KP8},
                                 {"%n9", MODKEY_ALT | KEY_KP9},
                                 {"%n.", MODKEY_ALT | KEY_KP_PERIOD},
                                 {"%n/", MODKEY_ALT | KEY_KP_DIVIDE},
                                 {"%n*", MODKEY_ALT | KEY_KP_MULTIPLY},
                                 {"%n-", MODKEY_ALT | KEY_KP_MINUS},
                                 {"%n+", MODKEY_ALT | KEY_KP_PLUS},
                                 {"%nreturn", MODKEY_ALT | KEY_KP_ENTER},
                                 {"%nenter", MODKEY_ALT | KEY_KP_ENTER},
                                 {"%n=", MODKEY_ALT | KEY_KP_EQUALS},
                                 {"%up", MODKEY_ALT | KEY_UP},
                                 {"%down", MODKEY_ALT | KEY_DOWN},
                                 {"%right", MODKEY_ALT | KEY_RIGHT},
                                 {"%left", MODKEY_ALT | KEY_LEFT},
                                 {"%insert", MODKEY_ALT | KEY_INSERT},
                                 {"%home", MODKEY_ALT | KEY_HOME},
                                 {"%end", MODKEY_ALT | KEY_END},
                                 {"%pageup", MODKEY_ALT | KEY_PAGEUP},
                                 {"%pagedown", MODKEY_ALT | KEY_PAGEDOWN},
                                 {"%f1", MODKEY_ALT | KEY_F1},
                                 {"%f2", MODKEY_ALT | KEY_F2},
                                 {"%f3", MODKEY_ALT | KEY_F3},
                                 {"%f4", MODKEY_ALT | KEY_F4},
                                 {"%f5", MODKEY_ALT | KEY_F5},
                                 {"%f6", MODKEY_ALT | KEY_F6},
                                 {"%f7", MODKEY_ALT | KEY_F7},
                                 {"%f8", MODKEY_ALT | KEY_F8},
                                 {"%f9", MODKEY_ALT | KEY_F9},
                                 {"%f10", MODKEY_ALT | KEY_F10},
                                 {"%f11", MODKEY_ALT | KEY_F11},
                                 {"%f12", MODKEY_ALT | KEY_F12},
                                 {"%f13", MODKEY_ALT | KEY_F13},
                                 {"%f14", MODKEY_ALT | KEY_F14},
                                 {"%f15", MODKEY_ALT | KEY_F15},
                                 {"%mode", MODKEY_ALT | KEY_MODE},
                                 {"%compose", MODKEY_ALT | KEY_COMPOSE},
                                 {"%help", MODKEY_ALT | KEY_HELP},
                                 {"%print", MODKEY_ALT | KEY_PRINT},
                                 {"%sysreq", MODKEY_ALT | KEY_SYSREQ},
                                 {"%break", MODKEY_ALT | KEY_BREAK},
                                 {"%menu", MODKEY_ALT | KEY_MENU},
                                 {"%power", MODKEY_ALT | KEY_POWER},
                                 {"%_", MODKEY_ALT | KEY_UNDERSCORE},
                                 {"%kana", MODKEY_ALT | KEY_KANA},
                                 {"%yen", MODKEY_ALT | KEY_YEN},
                                 {"%xfer", MODKEY_ALT | KEY_XFER},
                                 {"%nfer", MODKEY_ALT | KEY_NFER},
                                 {"^backspace", MODKEY_CTRL | KEY_BACKSPACE},
                                 {"^tab", MODKEY_CTRL | KEY_TAB},
                                 {"^clear", MODKEY_CTRL | KEY_CLEAR},
                                 {"^return", MODKEY_CTRL | KEY_RETURN},
                                 {"^enter", MODKEY_CTRL | KEY_RETURN},
                                 {"^pause", MODKEY_CTRL | KEY_PAUSE},
                                 {"^escape", MODKEY_CTRL | KEY_ESCAPE},
                                 {"^space", MODKEY_CTRL | KEY_SPACE},
                                 {"^'", MODKEY_CTRL | KEY_QUOTE},
                                 {"^,", MODKEY_CTRL | KEY_COMMA},
                                 {"^-", MODKEY_CTRL | KEY_MINUS},
                                 {"^.", MODKEY_CTRL | KEY_PERIOD},
                                 {"^/", MODKEY_CTRL | KEY_SLASH},
                                 {"^0", MODKEY_CTRL | KEY_0},
                                 {"^1", MODKEY_CTRL | KEY_1},
                                 {"^2", MODKEY_CTRL | KEY_2},
                                 {"^3", MODKEY_CTRL | KEY_3},
                                 {"^4", MODKEY_CTRL | KEY_4},
                                 {"^5", MODKEY_CTRL | KEY_5},
                                 {"^6", MODKEY_CTRL | KEY_6},
                                 {"^7", MODKEY_CTRL | KEY_7},
                                 {"^8", MODKEY_CTRL | KEY_8},
                                 {"^9", MODKEY_CTRL | KEY_9},
                                 {"^;", MODKEY_CTRL | KEY_SEMICOLON},
                                 {"^=", MODKEY_CTRL | KEY_EQUALS},
                                 {"^[", MODKEY_CTRL | KEY_LEFTBRACKET},
                                 {"^\\", MODKEY_CTRL | KEY_BACKSLASH},
                                 {"^]", MODKEY_CTRL | KEY_RIGHTBRACKET},
                                 {"^`", MODKEY_CTRL | KEY_BACKQUOTE},
                                 {"^a", MODKEY_CTRL | KEY_A},
                                 {"^b", MODKEY_CTRL | KEY_B},
                                 {"^c", MODKEY_CTRL | KEY_C},
                                 {"^d", MODKEY_CTRL | KEY_D},
                                 {"^e", MODKEY_CTRL | KEY_E},
                                 {"^f", MODKEY_CTRL | KEY_F},
                                 {"^g", MODKEY_CTRL | KEY_G},
                                 {"^h", MODKEY_CTRL | KEY_H},
                                 {"^i", MODKEY_CTRL | KEY_I},
                                 {"^j", MODKEY_CTRL | KEY_J},
                                 {"^k", MODKEY_CTRL | KEY_K},
                                 {"^l", MODKEY_CTRL | KEY_L},
                                 {"^m", MODKEY_CTRL | KEY_M},
                                 {"^n", MODKEY_CTRL | KEY_N},
                                 {"^o", MODKEY_CTRL | KEY_O},
                                 {"^p", MODKEY_CTRL | KEY_P},
                                 {"^q", MODKEY_CTRL | KEY_Q},
                                 {"^r", MODKEY_CTRL | KEY_R},
                                 {"^s", MODKEY_CTRL | KEY_S},
                                 {"^t", MODKEY_CTRL | KEY_T},
                                 {"^u", MODKEY_CTRL | KEY_U},
                                 {"^v", MODKEY_CTRL | KEY_V},
                                 {"^w", MODKEY_CTRL | KEY_W},
                                 {"^x", MODKEY_CTRL | KEY_X},
                                 {"^y", MODKEY_CTRL | KEY_Y},
                                 {"^z", MODKEY_CTRL | KEY_Z},
                                 {"^delete", MODKEY_CTRL | KEY_DELETE},
                                 {"^n0", MODKEY_CTRL | KEY_KP0},
                                 {"^n1", MODKEY_CTRL | KEY_KP1},
                                 {"^n2", MODKEY_CTRL | KEY_KP2},
                                 {"^n3", MODKEY_CTRL | KEY_KP3},
                                 {"^n4", MODKEY_CTRL | KEY_KP4},
                                 {"^n5", MODKEY_CTRL | KEY_KP5},
                                 {"^n6", MODKEY_CTRL | KEY_KP6},
                                 {"^n7", MODKEY_CTRL | KEY_KP7},
                                 {"^n8", MODKEY_CTRL | KEY_KP8},
                                 {"^n9", MODKEY_CTRL | KEY_KP9},
                                 {"^n.", MODKEY_CTRL | KEY_KP_PERIOD},
                                 {"^n/", MODKEY_CTRL | KEY_KP_DIVIDE},
                                 {"^n*", MODKEY_CTRL | KEY_KP_MULTIPLY},
                                 {"^n-", MODKEY_CTRL | KEY_KP_MINUS},
                                 {"^n+", MODKEY_CTRL | KEY_KP_PLUS},
                                 {"^nreturn", MODKEY_CTRL | KEY_KP_ENTER},
                                 {"^nenter", MODKEY_CTRL | KEY_KP_ENTER},
                                 {"^n=", MODKEY_CTRL | KEY_KP_EQUALS},
                                 {"^up", MODKEY_CTRL | KEY_UP},
                                 {"^down", MODKEY_CTRL | KEY_DOWN},
                                 {"^right", MODKEY_CTRL | KEY_RIGHT},
                                 {"^left", MODKEY_CTRL | KEY_LEFT},
                                 {"^insert", MODKEY_CTRL | KEY_INSERT},
                                 {"^home", MODKEY_CTRL | KEY_HOME},
                                 {"^end", MODKEY_CTRL | KEY_END},
                                 {"^pageup", MODKEY_CTRL | KEY_PAGEUP},
                                 {"^pagedown", MODKEY_CTRL | KEY_PAGEDOWN},
                                 {"^f1", MODKEY_CTRL | KEY_F1},
                                 {"^f2", MODKEY_CTRL | KEY_F2},
                                 {"^f3", MODKEY_CTRL | KEY_F3},
                                 {"^f4", MODKEY_CTRL | KEY_F4},
                                 {"^f5", MODKEY_CTRL | KEY_F5},
                                 {"^f6", MODKEY_CTRL | KEY_F6},
                                 {"^f7", MODKEY_CTRL | KEY_F7},
                                 {"^f8", MODKEY_CTRL | KEY_F8},
                                 {"^f9", MODKEY_CTRL | KEY_F9},
                                 {"^f10", MODKEY_CTRL | KEY_F10},
                                 {"^f11", MODKEY_CTRL | KEY_F11},
                                 {"^f12", MODKEY_CTRL | KEY_F12},
                                 {"^f13", MODKEY_CTRL | KEY_F13},
                                 {"^f14", MODKEY_CTRL | KEY_F14},
                                 {"^f15", MODKEY_CTRL | KEY_F15},
                                 {"^mode", MODKEY_CTRL | KEY_MODE},
                                 {"^compose", MODKEY_CTRL | KEY_COMPOSE},
                                 {"^help", MODKEY_CTRL | KEY_HELP},
                                 {"^print", MODKEY_CTRL | KEY_PRINT},
                                 {"^sysreq", MODKEY_CTRL | KEY_SYSREQ},
                                 {"^break", MODKEY_CTRL | KEY_BREAK},
                                 {"^menu", MODKEY_CTRL | KEY_MENU},
                                 {"^power", MODKEY_CTRL | KEY_POWER},
                                 {"^_", MODKEY_CTRL | KEY_UNDERSCORE},
                                 {"^kana", MODKEY_CTRL | KEY_KANA},
                                 {"^yen", MODKEY_CTRL | KEY_YEN},
                                 {"^xfer", MODKEY_CTRL | KEY_XFER},
                                 {"^nfer", MODKEY_CTRL | KEY_NFER},
                                 {"+backspace", MODKEY_SHIFT | KEY_BACKSPACE},
                                 {"+tab", MODKEY_SHIFT | KEY_TAB},
                                 {"+clear", MODKEY_SHIFT | KEY_CLEAR},
                                 {"+return", MODKEY_SHIFT | KEY_RETURN},
                                 {"+enter", MODKEY_SHIFT | KEY_RETURN},
                                 {"+pause", MODKEY_SHIFT | KEY_PAUSE},
                                 {"+escape", MODKEY_SHIFT | KEY_ESCAPE},
                                 {"+space", MODKEY_SHIFT | KEY_SPACE},
                                 {"+'", MODKEY_SHIFT | KEY_QUOTE},
                                 {"+,", MODKEY_SHIFT | KEY_COMMA},
                                 {"+-", MODKEY_SHIFT | KEY_MINUS},
                                 {"+.", MODKEY_SHIFT | KEY_PERIOD},
                                 {"+/", MODKEY_SHIFT | KEY_SLASH},
                                 {"+0", MODKEY_SHIFT | KEY_0},
                                 {"+1", MODKEY_SHIFT | KEY_1},
                                 {"+2", MODKEY_SHIFT | KEY_2},
                                 {"+3", MODKEY_SHIFT | KEY_3},
                                 {"+4", MODKEY_SHIFT | KEY_4},
                                 {"+5", MODKEY_SHIFT | KEY_5},
                                 {"+6", MODKEY_SHIFT | KEY_6},
                                 {"+7", MODKEY_SHIFT | KEY_7},
                                 {"+8", MODKEY_SHIFT | KEY_8},
                                 {"+9", MODKEY_SHIFT | KEY_9},
                                 {"+;", MODKEY_SHIFT | KEY_SEMICOLON},
                                 {"+=", MODKEY_SHIFT | KEY_EQUALS},
                                 {"+[", MODKEY_SHIFT | KEY_LEFTBRACKET},
                                 {"+\\", MODKEY_SHIFT | KEY_BACKSLASH},
                                 {"+]", MODKEY_SHIFT | KEY_RIGHTBRACKET},
                                 {"+`", MODKEY_SHIFT | KEY_BACKQUOTE},
                                 {"+a", MODKEY_SHIFT | KEY_A},
                                 {"+b", MODKEY_SHIFT | KEY_B},
                                 {"+c", MODKEY_SHIFT | KEY_C},
                                 {"+d", MODKEY_SHIFT | KEY_D},
                                 {"+e", MODKEY_SHIFT | KEY_E},
                                 {"+f", MODKEY_SHIFT | KEY_F},
                                 {"+g", MODKEY_SHIFT | KEY_G},
                                 {"+h", MODKEY_SHIFT | KEY_H},
                                 {"+i", MODKEY_SHIFT | KEY_I},
                                 {"+j", MODKEY_SHIFT | KEY_J},
                                 {"+k", MODKEY_SHIFT | KEY_K},
                                 {"+l", MODKEY_SHIFT | KEY_L},
                                 {"+m", MODKEY_SHIFT | KEY_M},
                                 {"+n", MODKEY_SHIFT | KEY_N},
                                 {"+o", MODKEY_SHIFT | KEY_O},
                                 {"+p", MODKEY_SHIFT | KEY_P},
                                 {"+q", MODKEY_SHIFT | KEY_Q},
                                 {"+r", MODKEY_SHIFT | KEY_R},
                                 {"+s", MODKEY_SHIFT | KEY_S},
                                 {"+t", MODKEY_SHIFT | KEY_T},
                                 {"+u", MODKEY_SHIFT | KEY_U},
                                 {"+v", MODKEY_SHIFT | KEY_V},
                                 {"+w", MODKEY_SHIFT | KEY_W},
                                 {"+x", MODKEY_SHIFT | KEY_X},
                                 {"+y", MODKEY_SHIFT | KEY_Y},
                                 {"+z", MODKEY_SHIFT | KEY_Z},
                                 {"+delete", MODKEY_SHIFT | KEY_DELETE},
                                 {"+n0", MODKEY_SHIFT | KEY_KP0},
                                 {"+n1", MODKEY_SHIFT | KEY_KP1},
                                 {"+n2", MODKEY_SHIFT | KEY_KP2},
                                 {"+n3", MODKEY_SHIFT | KEY_KP3},
                                 {"+n4", MODKEY_SHIFT | KEY_KP4},
                                 {"+n5", MODKEY_SHIFT | KEY_KP5},
                                 {"+n6", MODKEY_SHIFT | KEY_KP6},
                                 {"+n7", MODKEY_SHIFT | KEY_KP7},
                                 {"+n8", MODKEY_SHIFT | KEY_KP8},
                                 {"+n9", MODKEY_SHIFT | KEY_KP9},
                                 {"+n.", MODKEY_SHIFT | KEY_KP_PERIOD},
                                 {"+n/", MODKEY_SHIFT | KEY_KP_DIVIDE},
                                 {"+n*", MODKEY_SHIFT | KEY_KP_MULTIPLY},
                                 {"+n-", MODKEY_SHIFT | KEY_KP_MINUS},
                                 {"+n+", MODKEY_SHIFT | KEY_KP_PLUS},
                                 {"+nreturn", MODKEY_SHIFT | KEY_KP_ENTER},
                                 {"+nenter", MODKEY_SHIFT | KEY_KP_ENTER},
                                 {"+n=", MODKEY_SHIFT | KEY_KP_EQUALS},
                                 {"+up", MODKEY_SHIFT | KEY_UP},
                                 {"+down", MODKEY_SHIFT | KEY_DOWN},
                                 {"+right", MODKEY_SHIFT | KEY_RIGHT},
                                 {"+left", MODKEY_SHIFT | KEY_LEFT},
                                 {"+insert", MODKEY_SHIFT | KEY_INSERT},
                                 {"+home", MODKEY_SHIFT | KEY_HOME},
                                 {"+end", MODKEY_SHIFT | KEY_END},
                                 {"+pageup", MODKEY_SHIFT | KEY_PAGEUP},
                                 {"+pagedown", MODKEY_SHIFT | KEY_PAGEDOWN},
                                 {"+f1", MODKEY_SHIFT | KEY_F1},
                                 {"+f2", MODKEY_SHIFT | KEY_F2},
                                 {"+f3", MODKEY_SHIFT | KEY_F3},
                                 {"+f4", MODKEY_SHIFT | KEY_F4},
                                 {"+f5", MODKEY_SHIFT | KEY_F5},
                                 {"+f6", MODKEY_SHIFT | KEY_F6},
                                 {"+f7", MODKEY_SHIFT | KEY_F7},
                                 {"+f8", MODKEY_SHIFT | KEY_F8},
                                 {"+f9", MODKEY_SHIFT | KEY_F9},
                                 {"+f10", MODKEY_SHIFT | KEY_F10},
                                 {"+f11", MODKEY_SHIFT | KEY_F11},
                                 {"+f12", MODKEY_SHIFT | KEY_F12},
                                 {"+f13", MODKEY_SHIFT | KEY_F13},
                                 {"+f14", MODKEY_SHIFT | KEY_F14},
                                 {"+f15", MODKEY_SHIFT | KEY_F15},
                                 {"+mode", MODKEY_SHIFT | KEY_MODE},
                                 {"+compose", MODKEY_SHIFT | KEY_COMPOSE},
                                 {"+help", MODKEY_SHIFT | KEY_HELP},
                                 {"+print", MODKEY_SHIFT | KEY_PRINT},
                                 {"+sysreq", MODKEY_SHIFT | KEY_SYSREQ},
                                 {"+break", MODKEY_SHIFT | KEY_BREAK},
                                 {"+menu", MODKEY_SHIFT | KEY_MENU},
                                 {"+power", MODKEY_SHIFT | KEY_POWER},
                                 {"+_", MODKEY_SHIFT | KEY_UNDERSCORE},
                                 {"+kana", MODKEY_SHIFT | KEY_KANA},
                                 {"+yen", MODKEY_SHIFT | KEY_YEN},
                                 {"+xfer", MODKEY_SHIFT | KEY_XFER},
                                 {"+nfer", MODKEY_SHIFT | KEY_NFER},
                                 {NULL, 0}};

/* キー割り当てのデフォルト */
static struct KeyAssign keyAssign[128] = {{KEY_BACKSPACE, GKEY_BACKSPACE},
                                          {KEY_TAB, GKEY_TAB},
                                          {KEY_CLEAR, GKEY_CLS},
                                          {KEY_RETURN, GKEY_RETURN},
                                          {KEY_PAUSE, GKEY_BREAK},
                                          {KEY_ESCAPE, GKEY_OFF},
                                          {KEY_SPACE, GKEY_SPACE},
                                          {KEY_QUOTE, GKEY_ASTER},
                                          {KEY_COMMA, GKEY_COMMA},
                                          {KEY_MINUS, GKEY_MINUS},
                                          {KEY_PERIOD, GKEY_PERIOD},
                                          {KEY_SLASH, GKEY_SLASH},
                                          {KEY_0, GKEY_0},
                                          {KEY_1, GKEY_1},
                                          {KEY_2, GKEY_2},
                                          {KEY_3, GKEY_3},
                                          {KEY_4, GKEY_4},
                                          {KEY_5, GKEY_5},
                                          {KEY_6, GKEY_6},
                                          {KEY_7, GKEY_7},
                                          {KEY_8, GKEY_8},
                                          {KEY_9, GKEY_9},
                                          {KEY_SEMICOLON, GKEY_SEMICOLON},
                                          {KEY_EQUALS, GKEY_EQUAL},
                                          {KEY_LEFTBRACKET, GKEY_LKAKKO},
                                          {KEY_BACKSLASH, 0},
                                          {KEY_RIGHTBRACKET, GKEY_RKAKKO},
                                          {KEY_A, GKEY_A},
                                          {KEY_B, GKEY_B},
                                          {KEY_C, GKEY_C},
                                          {KEY_D, GKEY_D},
                                          {KEY_E, GKEY_E},
                                          {KEY_F, GKEY_F},
                                          {KEY_G, GKEY_G},
                                          {KEY_H, GKEY_H},
                                          {KEY_I, GKEY_I},
                                          {KEY_J, GKEY_J},
                                          {KEY_K, GKEY_K},
                                          {KEY_L, GKEY_L},
                                          {KEY_M, GKEY_M},
                                          {KEY_N, GKEY_N},
                                          {KEY_O, GKEY_O},
                                          {KEY_P, GKEY_P},
                                          {KEY_Q, GKEY_Q},
                                          {KEY_R, GKEY_R},
                                          {KEY_S, GKEY_S},
                                          {KEY_T, GKEY_T},
                                          {KEY_U, GKEY_U},
                                          {KEY_V, GKEY_V},
                                          {KEY_W, GKEY_W},
                                          {KEY_X, GKEY_X},
                                          {KEY_Y, GKEY_Y},
                                          {KEY_Z, GKEY_Z},
                                          {KEY_DELETE, GKEY_BACKSPACE},
                                          {KEY_KP0, GKEY_0},
                                          {KEY_KP1, GKEY_1},
                                          {KEY_KP2, GKEY_2},
                                          {KEY_KP3, GKEY_3},
                                          {KEY_KP4, GKEY_4},
                                          {KEY_KP5, GKEY_5},
                                          {KEY_KP6, GKEY_6},
                                          {KEY_KP7, GKEY_7},
                                          {KEY_KP8, GKEY_8},
                                          {KEY_KP9, GKEY_9},
                                          {KEY_KP_PERIOD, GKEY_PERIOD},
                                          {KEY_KP_DIVIDE, GKEY_SLASH},
                                          {KEY_KP_MULTIPLY, GKEY_ASTER},
                                          {KEY_KP_MINUS, GKEY_MINUS},
                                          {KEY_KP_PLUS, GKEY_PLUS},
                                          {KEY_KP_ENTER, GKEY_RETURN},
                                          {KEY_KP_EQUALS, GKEY_EQUAL},
                                          {KEY_UP, GKEY_UP},
                                          {KEY_DOWN, GKEY_DOWN},
                                          {KEY_RIGHT, GKEY_RIGHT},
                                          {KEY_LEFT, GKEY_LEFT},
                                          {KEY_INSERT, GKEY_INSERT},
                                          {KEY_HOME, GKEY_CLS},
                                          {KEY_END, 0},
                                          {KEY_PAGEUP, 0},
                                          {KEY_PAGEDOWN, 0},
                                          {KEY_F1, 0},
                                          {KEY_F2, 0},
                                          {KEY_F3, 0},
                                          {KEY_F4, 0},
                                          {KEY_F5, 0},
                                          {KEY_F6, 0},
                                          {KEY_F7, 0},
                                          {KEY_F8, 0},
                                          {KEY_F9, 0},
                                          {KEY_F10, 0},
                                          {KEY_F11, 0},
                                          {KEY_F12, 0},
                                          {KEY_F13, 0},
                                          {KEY_F14, 0},
                                          {KEY_F15, 0},
                                          {KEY_NUMLOCK, 0},
                                          {KEY_CAPSLOCK, GKEY_CAPS},
                                          {KEY_SCROLLOCK, 0},
                                          {KEY_RSHIFT, GKEY_SHIFT},
                                          {KEY_LSHIFT, GKEY_SHIFT},
                                          {KEY_RCTRL, 0},
                                          {KEY_LCTRL, 0},
                                          {KEY_RALT, 0},
                                          {KEY_LALT, 0},
                                          {KEY_MODE, 0},
                                          {KEY_COMPOSE, GKEY_MENU},
                                          {KEY_HELP, GKEY_MENU},
                                          {KEY_PRINT, 0},
                                          {KEY_SYSREQ, 0},
                                          {KEY_BREAK, GKEY_BREAK},
                                          {KEY_MENU, GKEY_MENU},
                                          {KEY_POWER, GKEY_OFF},
                                          {0, 0}};

/*
        キー割り当てのデフォルトを変更する
*/
static void setKeyAssign(int key, uint16 gkey) {
  struct KeyAssign *a;

  for (a = keyAssign; a->key != 0; a++)
    if (a->key == key) {
      a->gkey = gkey;
      return;
    }

  a->key = key;
  a->gkey = gkey;
  (++a)->key = 0;
}

/*
        パス名からディレクトリ名の長さを得る
*/
static int getDirLength(const char *path) {
  const char *p;

  for (p = path + strlen(path) - 1; p > path; p--)
    if (*p == '\\' || *p == ':' || *p == '/')
      return (int)(p - path) + 1;
  return 0;
}

/*
        終了処理を行う
*/
static void quit(void) {
#if defined(Z80_PROF)
  if (*pathSioIn != 0)
    loadSym(pathProg);
  else if (*pathProg != 0)
    loadSym(pathProg);

  writeProfFile(".\\prof.tsv");
  writePathFile(".\\path.tsv");
#endif

  quitDepend();
}

/*
        初期化する
*/
int init(int argc, char *argv[]) {
  Conf conf[256];
  const Machineinfo *m;
  const OptTable *o;
  const struct KeyAssign *a;
  int i, key, trace, success;

  /* 読込ファイルと開始アドレス */
  beginProg = 0;
  for (i = 1; i < argc; i++)
    if (*argv[i] != '-') {
      strcpy(pathProg, argv[i]);
      for (i = i + 1; i < argc; i++)
        if (*argv[i] != '-')
          sscanf(argv[i], "%x", &beginProg);
    }

  success = (getConfig(conf, sizeof(conf) / sizeof(conf[0]), "g800config", argc,
                       argv) != NULL);

  /* 設定 */
  trace = getOptIntTable(conf, tableYesNo, "debug", FALSE);
  if ((freqUpdateIO = getOptInt(conf, "refresh", 60)) > 360)
    freqUpdateIO = 360;
  setHomeDir(pathSioIn, getOptText(conf, "sio_in", ""));
  setHomeDir(pathSioOut, getOptText(conf, "sio_out", ""));
  noWait = getOptIntTable(conf, tableYesNo, "no_wait", FALSE);
  serialTest = getOptIntTable(conf, tableYesNo, "serial_test", FALSE);
  useSoftwareKey = getOptIntTable(conf, tableYesNo, "software_key", FALSE);

  /* メモリ */
  setHomeDir(pathRAM, getOptText(conf, "ram_path", ""));
  setHomeDir(dirROM, getOptText(conf, "rom_dir", ""));
  useROM = (strcmp(dirROM, "") != 0);
  if (getOptIntTable(conf, tableYesNo, "exram", FALSE))
    exram = malloc(0x8000);
  else
    exram = NULL;

  /* BASIC */
  useBasic = getOptIntTable(conf, tableYesNo, "sim_basic", TRUE);
  setHomeDir(pathBasic, getOptText(conf, "basic_path", ""));
  if (strcmp(pathBasic, "") == 0)
    sprintf(pathBasic, "%.*sbasic.txt", getDirLength(pathBasic), pathBasic);

  /* エミュレーション */
  machine = getOptIntTable(conf, tableMachine, "machine", MACHINE_G850);
  if (machine & 0xff00) {
    machineSub = machine & 0x00ff;
    machine = ((machine & 0xff00) >> 8) - 1;
  } else if (machine == MACHINE_E200)
    machineSub = MACHINE_PCE200 & 0x00ff;
  else if (machine == MACHINE_G815)
    machineSub = MACHINE_PCG815 & 0x00ff;
  else if (machine == MACHINE_G850)
    machineSub = MACHINE_PCG850 & 0x00ff;
  emulateIOCS = getOptIntTable(conf, tableYesNo, "emulate_iocs", !useROM);
  if ((lcdScales = getOptInt(conf, "lcd_scales", 2)) == 1)
    lcdScales = 2;
  freqCPU = getOptInt(conf, "clock", 0) * 1000;

  /* LCD */
  zoom = getOptInt(conf, "zoom", 3);
  colorBack = getOptHex(conf, "lcd_back", 0xcceebb);
  colorOff = getOptHex(conf, "lcd_off", 0xaaddbb);
  colorOn = getOptHex(conf, "lcd_on", 0x002211);

  /* キーボード */
  switch (getOptIntTable(conf, tableKeyboard, "keyboard", KEYBOARD_EN)) {
  case KEYBOARD_JP:
    SET_TABLE_KEY(tableKey, ":", KEY_QUOTE);
    SET_TABLE_KEY(tableKey, "^", KEY_EQUALS);
    SET_TABLE_KEY(tableKey, "@", KEY_LEFTBRACKET);
    SET_TABLE_KEY(tableKey, "]", KEY_BACKSLASH);
    SET_TABLE_KEY(tableKey, "[", KEY_RIGHTBRACKET);
    setKeyAssign(KEY_EQUALS, GKEY_HAT);
    break;
  case KEYBOARD_EN:
  default:
    break;
  }

  /* キー割り当て(デフォルト) */
  memset(keyConv, 0, sizeof(keyConv));
  memset(keyConvAlt, 0, sizeof(keyConvAlt));
  memset(keyConvCtrl, 0, sizeof(keyConvCtrl));
  memset(keyConvShift, 0, sizeof(keyConvShift));
  for (a = keyAssign; a->key != 0; a++)
    switch (a->key & 0xf000) {
    case MODKEY_ALT:
      keyConvAlt[a->key & 0xfff] = a->gkey;
      break;
    case MODKEY_CTRL:
      keyConvCtrl[a->key & 0xfff] = a->gkey;
      break;
    case MODKEY_SHIFT:
      keyConvShift[a->key & 0xfff] = a->gkey;
      break;
    default:
      keyConv[a->key & 0xfff] = a->gkey;
      break;
    }

  /* キー割り当て(ユーザー定義) */
  for (o = tableGkey; o->string != NULL; o++) {
    key = getOptIntTable(conf, tableKey, o->string, KEY_NONE);
    switch (key & 0xf000) {
    case MODKEY_ALT:
      keyConvAlt[key & 0xfff] = o->value;
      break;
    case MODKEY_CTRL:
      keyConvCtrl[key & 0xfff] = o->value;
      break;
    case MODKEY_SHIFT:
      keyConvShift[key & 0xfff] = o->value;
      break;
    default:
      keyConv[key & 0xfff] = o->value;
      break;
    }
  }
  for (key = KEY_NONE + 1; key <= KEY_LAST; key++)
    if (keyConvShift[key] != GKEY_NONE && !(keyConvShift[key] & GMODKEY_SHIFT))
      keyConvShift[key] |= GMODKEY_NOSHIFT;

  /* ジョイスティック */
  useJoy = getOptIntTable(conf, tableYesNo, "use_joy", TRUE);
  joyUp = getOptIntTable(conf, tableGkey, "joy_up", GKEY_UP);
  joyDown = getOptIntTable(conf, tableGkey, "joy_down", GKEY_DOWN);
  joyLeft = getOptIntTable(conf, tableGkey, "joy_left", GKEY_LEFT);
  joyRight = getOptIntTable(conf, tableGkey, "joy_right", GKEY_RIGHT);
  joyButton[0] = getOptIntTable(conf, tableGkey, "joy_button1", GKEY_SPACE);
  joyButton[1] = getOptIntTable(conf, tableGkey, "joy_button2", GKEY_RETURN);
  joyButton[2] = getOptIntTable(conf, tableGkey, "joy_button3", GKEY_NONE);
  joyButton[3] = getOptIntTable(conf, tableGkey, "joy_button4", GKEY_NONE);
  joyButton[4] = getOptIntTable(conf, tableGkey, "joy_button5", GKEY_NONE);
  joyButton[5] = getOptIntTable(conf, tableGkey, "joy_button6", GKEY_NONE);
  joyButton[6] = getOptIntTable(conf, tableGkey, "joy_button7", GKEY_NONE);
  joyButton[7] = getOptIntTable(conf, tableGkey, "joy_button8", GKEY_NONE);
  joyButton[8] = getOptIntTable(conf, tableGkey, "joy_button9", GKEY_NONE);
  joyButton[9] = getOptIntTable(conf, tableGkey, "joy_button10", GKEY_NONE);
  joyButton[10] = getOptIntTable(conf, tableGkey, "joy_button11", GKEY_NONE);
  joyButton[11] = getOptIntTable(conf, tableGkey, "joy_button12", GKEY_NONE);

  /* 「閉じる」をOFFキーとして扱うか? */
  closeAsOff = getOptIntTable(conf, tableYesNo, "close_as_off", FALSE);

  /* LCD */
  if (zoom < 0)
    zoom = 2;
  m = machineInfo[machine];
  cellWidth = m->cell_width;
  cellHeight = m->cell_height;
  lcdCols = m->lcd_cols;
  lcdRows = m->lcd_rows;
  vramCols = m->vram_cols;
  vramRows = m->vram_rows;
  lcdWidth = cellWidth * lcdCols;
  lcdHeight = 8 * lcdRows;
  vramWidth = m->vram_width;
  vramHeight = m->vram_height;
  statesKeyStrobeClear = m->states_keystrobe_clear;
  freqCPU = (freqCPU != 0 ? freqCPU : m->cpu_clocks);

  /* シリアル入出力 */
  if (strcmp(pathSioOut, "") != 0)
    sioSave(pathSioOut);
  if (strcmp(pathSioIn, "") != 0)
    sioLoad(pathSioIn);

  /* ブザー */
  buzzer = getOptIntTable(conf, tableBuzzer, "buzzer", TRUE);
  if (buzzer != BUZZER_NONE) {
    soundBufferSize = getSoundBufferSize(freqUpdateIO);
    freqUpdateIO = FREQ_SOUND / soundBufferSize;
    soundReadBuffer = calloc(1, soundBufferSize * 2);
    soundWriteBuffer = soundReadBuffer + soundBufferSize;
  }

  /* ファイルフィルタを使うか? */
  useFileFilter = getOptIntTable(conf, tableYesNo, "file_filter", FALSE);

  /* 説明画像のファイル名 */
  pathInfoImage = getOptText(conf, "info_image", NULL);

  /* 自動入力 */
  if (getOptText(conf, "auto", NULL) != NULL) {
    char buf[256];

    sprintf(buf, "%s\n", getOptText(conf, "auto", NULL));
    setAutoKeyText(buf, FALSE);
  }

  initDepend();
  atexit(quit);
  z80init(&z80);
  z80prof_init(&z80, getOptIntTable(conf, tableYesNo, "prof", FALSE));
  z80.m = memory;
  z80.i.trace = trace;
  z80.i.emulate_subroutine = emulateIOCS;
  lcdContrast = (machine == MACHINE_G850 ? 0 : 0x0f);
  updateLCDContrast();

  return success;
}

/*
        Copyright 2005 ‾ 2017 maruhiro
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
