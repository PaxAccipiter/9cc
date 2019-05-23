#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//トークンの型を表す値
enum{
  TK_NUM = 256, //整数トークン
  TK_EOF,       //入力の終わりを表すトークン
};

//トークンの型
typedef struct {
  int ty;
  int val;
  char *input;
} Token

char *user_input;

//トークナイズした結果のトークン列はこの配列に保存する
//100個以上のトークンは来ないものとする。
Token tokens[100];

//エラーを報告するための関数
//printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list
  
int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  char *p = argv[1];

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  printf("  mov rax, %ld\n",strtol(p,&p,0));

  while(*p){
    if (*p == '+'){
      p++;
      printf("  add rax, %ld\n",strtol(p,&p,0));
    }
    else if (*p == '-'){
      p++;
      printf("  sub rax, %ld\n",strtol(p,&p,0));
    }
    else{
      fprintf(stderr,"予期しない文字です。: '%c'\n",*p);
      return 1;
  }
  }
  
  printf("  ret\n");
  return 0;
}
