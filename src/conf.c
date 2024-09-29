/*
        設定ファイル処理(conf.c)
*/

#include "conf.h"
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include "windows.h"
#endif

#if !defined(FALSE)
#define FALSE 0
#endif

#if !defined(TRUE)
#define TRUE 1
#endif

/* 設定ファイルにUTF-8のBOMがあるか? */
static int is_utf8;

/* コメント文字 */
#define COMMENT '#'

/* 関数が戻す文字列の書き込み領域 (win32専用) */
#if defined(_WIN32)
static char _buffer[PATH_MAX];
#endif

/*
        引数の文字列をUTF-8に変換する (win32専用)
*/
#if defined(_WIN32)
char **argvToUTF8(int argc, char *argv[]) {
  int i, len;
  wchar_t *utf16 = malloc(8);
  char **argv_utf8;

  argv_utf8 = malloc(sizeof(char *) * (argc + 1));

  for (i = 0; i < argc; i++) {
    len = (MultiByteToWideChar(CP_ACP, 0, (LPCSTR)argv[i], -1, NULL, 0) + 1) *
          sizeof(wchar_t);
    utf16 = realloc(utf16, len);
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)argv[i], -1, (LPWSTR)utf16, len);

    len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)utf16, -1, NULL, 0, NULL,
                              NULL) +
          1;
    argv_utf8[i] = malloc(len);
    WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)utf16, -1, (LPSTR)argv_utf8[i],
                        len, NULL, NULL);
  }
  argv_utf8[i] = NULL;

  free(utf16);
  return argv_utf8;
}
#endif

/*
        実行ファイルのディレクトリを得る (win32専用) (下請け)
*/
#if defined(_WIN32)
static char *getexedir(void) {
  wchar_t w_module_name[PATH_MAX];
  char *p;

  GetModuleFileNameW(NULL, w_module_name, sizeof(w_module_name));
  WideCharToMultiByte(CP_UTF8, 0, w_module_name, -1, _buffer, sizeof(_buffer),
                      NULL, NULL);

  for (p = _buffer + strlen(_buffer); p > _buffer && *p != '\\'; p--)
    ;
  *p = 0;

  return _buffer;
}
#else
#define getexedir() NULL
#endif

/*
        環境変数の値を得る (win32専用) (下請け)
*/
#if defined(_WIN32)
static const char *ugetenv(const char *varname) {
  wchar_t w_varname[256], *value;

  MultiByteToWideChar(CP_UTF8, 0, varname, -1, w_varname, sizeof(w_varname));
  if ((value = _wgetenv(w_varname)) == NULL) {
    strcpy(_buffer, "");
    return NULL;
  }
  WideCharToMultiByte(CP_UTF8, 0, value, -1, _buffer, sizeof(_buffer), NULL,
                      NULL);
  return _buffer;
}
#define getenv(varname) ugetenv(varname)
#endif

/*
        実行ファイルの引数からオプションを得る (下請け)
*/
static int readArg(Conf *conf, char *argv) {
  int size;
  char *p, *q;

  if (*argv != '-')
    return 0;

  for (p = argv; *p != '=' && *p != '\0'; p++)
    ;
  if (*p == '\0')
    return 0;

  for (q = p; *q != '\0'; q++)
    ;

  size = (int)(p - argv - 1);
  memcpy(conf->key, argv + 1, size);
  *(conf->key + size) = '\0';

  size = (int)(q - p + 1);
  memcpy(conf->value, p + 1, size);
  *(conf->value + size) = '\0';

  *(conf + 1)->key = '\0';
  return 1;
}

/*
        Configファイルをオープンする (下請け)
*/
static FILE *openConfig(const char *file) {
  const char *hide_list[] = {".", "", NULL};
  FILE *fp;
  int i;
  char path[PATH_MAX];
  const char *dir, **hide;
#if defined(_WIN32)
  const wchar_t w_mode[] = {'r', 0};
  wchar_t w_path[PATH_MAX];
#endif

  is_utf8 = FALSE;

  for (hide = hide_list; *hide != NULL; hide++)
    for (i = 0; i < 6; i++) {
      switch (i) {
      case 0:
        dir = ".";
        break;
      case 1:
        dir = getenv("HOME");
        break;
      case 2:
        dir = getenv("APPDATA");
        break;
      case 3:
        dir = getenv("USERPROFILE");
        break;
      case 4:
        dir = getenv("ALLUSERSPROFILE");
        break;
      case 5:
        dir = getexedir();
        break;
      }
      if (dir == NULL)
        continue;

      sprintf(path, "%s/%s%s", dir, *hide, file);
#if defined(_WIN32)
      MultiByteToWideChar(CP_UTF8, 0, path, -1, w_path, sizeof(w_path));
      fp = _wfopen(w_path, w_mode);
#else
      fp = fopen(path, "r");
#endif
      if (fp != NULL)
        return fp;
    }
  return NULL;
}

/*
        ファイルからオプションを得る (下請け)
*/
static int readConfig(FILE *fp, Conf *conf) {
  char buf[160], *p, *q;
#if defined(_WIN32)
  wchar_t w_buf[160];
#endif

  /* 左辺を得る */
  do {
    if (fgets(buf, sizeof(buf), fp) == NULL)
      return 0;
    if (memcmp(buf, "\xef\xbb\xbf", 3) == 0) { /* UTF-8のBOM */
      is_utf8 = TRUE;
      memmove(buf, buf + 3, strlen(buf + 3) + 1);
    }
    for (p = buf; *p == ' ' || *p == '\t'; p++)
      ;
  } while (*p == COMMENT || *p == '\r' || *p == '\n' || *p == 0);

  for (q = p; *q != ' ' && *q != '\t' && *q != '\r' && *q != '\n' && *q != 0;
       q++)
    ;
  *q++ = 0;
  strcpy(conf->key, p);

  /* 右辺を得る */
  for (p = q; *p == ' ' || *p == '\t'; p++)
    ;
  for (q = p; *q != '\r' && *q != '\n' && *q != 0 && *q != COMMENT; q++)
    ;
  if (p < q)
    for (; *(q - 1) == ' ' || *(q - 1) == '\t'; q--)
      ;
  *q++ = 0;
#if defined(_WIN32)
  if (is_utf8)
    strcpy(conf->value, p);
  else {
    MultiByteToWideChar(CP_ACP, 0, p, -1, w_buf, sizeof(w_buf));
    WideCharToMultiByte(CP_UTF8, 0, w_buf, -1, conf->value, sizeof(conf->value),
                        NULL, NULL);
  }
#else
  strcpy(conf->value, p);
#endif

  /* 終端を書き込む */
  *(conf + 1)->key = '\0';
  return 1;
}

/*
        Configファイルを読み込む
*/
Conf *getConfig(Conf *conf, int length, const char *file, int argc,
                char *argv[]) {
  Conf *p = conf, *last = conf + length - 1;
  FILE *fp;
  int i, line;

  strcpy(p->key, "");

  /* 実行ファイルの引数からオプションを得る */
  line = INT_MIN;
  for (i = 1; i < argc && p < last; i++) {
    if (readArg(p, argv[i]))
      (p++)->line = line;
    line++;
  }

  /* ファイルからオプションを得る */
  if (file != NULL) {
    if ((fp = openConfig(file)) == NULL)
      return NULL;

    line = 1;
    while (p < last && readConfig(fp, p))
      (p++)->line = line++;

    fclose(fp);
  }

  return conf;
}

/*
        Configファイルから文字列を取り出す
*/
const char *getOptText(const Conf *conf, const char *key,
                       const char *default_value) {
  const Conf *p;

  for (p = conf; p->key[0] != '\0'; p++)
    if (strcasecmp(p->key, key) == 0)
      return p->value;
  return default_value;
}

/*
        Configファイルから数値を取り出す(10進数)
*/
int getOptInt(const Conf *conf, const char *key, int default_value) {
  const char *p;

  p = getOptText(conf, key, "");
  if (strcmp(p, "") != 0)
    return atoi(p);
  else
    return default_value;
}

/*
        Configファイルから数値を取り出す(16進数)
*/
unsigned int getOptHex(const Conf *conf, const char *key,
                       unsigned int default_value) {
  int x;
  const char *p;

  p = getOptText(conf, key, "");
  if (strcmp(p, "") != 0) {
    sscanf(p, "%x", &x);
    return x;
  } else
    return default_value;
}

/*
        Configファイルから文字列を取り出し, テーブルを利用して数値に変換する
*/
static int textToInt(const OptTable *table, const char *str,
                     int default_value) {
  const OptTable *p;

  for (p = table; p->string != NULL; p++)
    if (strcasecmp(str, p->string) == 0)
      return p->value;
  return default_value;
}
int getOptIntTable(const Conf *conf, const OptTable *table, const char *key,
                   int default_value) {
  return textToInt(table, getOptText(conf, key, ""), default_value);
}

/*
        ~をホームディレクトリに置き換える
*/
char *setHomeDir(char *buf, const char *path) {
  const char *dir;

  if (*path != '~') {
    strcpy(buf, path);
    return buf;
  }

  if ((dir = getenv("HOME")) == NULL)
    if ((dir = getenv("USERPROFILE")) == NULL)
      if ((dir = getexedir()) == NULL)
        dir = ".";

  sprintf(buf, "%s%s", dir, path + 1);
  return buf;
}

/*
        変換テーブルを設定する
*/
void setOptTable(OptTable *table, const char *key, int value) {
  OptTable *o;

  for (o = table; o->string != NULL; o++)
    if (strcasecmp(key, o->string) == 0) {
      o->value = value;
      return;
    }
  o->string = malloc(strlen(key) + 1);
  strcpy(o->string, key);
  o->value = value;
  o++;

  o->string = NULL;
}

/*
        Copyright 2005 ~ 2014 maruhiro
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
