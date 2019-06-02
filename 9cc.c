#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//globals
//入力列
char* user_input;

//トークナイズした結果のトークン列はこの配列に保存する
//100個以上のトークンは来ないものとする。
Token tokens[100];
//トークン列のどこをみているのか
int pos = 0;

//defines
//トークンの型を表す値
enum{
  TK_NUM = 256, //整数トークン
  TK_EOF,       //入力の終わりを表すトークン
};

//トークンの型
//type
//value(tyがTK_NUMの場合、その数値)
//トークンの文字列
typedef struct {
  int ty;
  int val;
  char* input;//いるのか??
} Token;

//ノードの型を表す値
enum{
  ND_NUM, //整数ノード
};

typedef struct Node {
  int ty ; //演算子かND_NUM
  struct Node *lhs; //left hand side 左辺
  struct Node *rhs; //right hand side 右辺
  int val; //tyがND_NUMの場合のみ使われる
} Node;

//functions
void error(char *fmt, ...);
void error_at(char *loc, char *msg);
void tokenize();
Node* new_node(int ty, Node* lhs, Node *rhs);
Node* new_node_num(int val);
int consume(int ty);
Node* term();
Node* mul();
Node* expr();
void gen(Node* node);

//エラーを報告するための関数
//printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);
  vfprintf(stderr,fmt,ap);
  fprintf(stderr,"\n");
  va_end(ap);
  exit(1);
}

//エラー個所を報告する関数。
//locってなんやろ
void error_at(char *loc, char *msg){
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");//pos個数の空白を出力
  fprintf(stderr, "^ %s\n",msg);
  exit(1);
}

//user_inputが指している文字列を
//トークンに分割してtokensに保存する
void tokenize(){
  char* p = user_input;
  int i = 0;
  while (*p) {
    //空白文字をスキップ
    if(isspace(*p)){
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)){
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p,&p,10);//別にここでpは次のトークンか空白に移動しているからp++はいらない。
      i++;
      continue;
    }

    error_at(p,"トークナイズできません。");
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}


Node* new_node(int ty, Node* lhs, Node *rhs){
  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node* new_node_num(int val){
  Node* node = mallloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

int consume(int ty){
  if(token[pos].ty != ty) return 0;
  pos++;
  return 1;
}

Node* term(){
  if(consume('(')){
    Node* node = expr();
    if(!consume(')')){
      error_at(tokens[pos].input,"開き括弧に対する閉じ括弧がありません。")
    }
    return node;
  }

  //そうでないなら数値のはず
  if(tokens[pos].ty == TK_NUM) return new_node_num(tokens[pos++].val);

  error_at(tokens[pos].input, "数値でも開き括弧でもないトークンです。");
}

Node* mul(){
  Node* node = term();//int* a int *a はどちらも同じもの。int *aだと、aのdereferenceがint型のように見えて分かりやすそう?

  for(;;){
    if(consume('*')) node = new_node('*',node,term());
    else if(consume('/')) node = new_node('/',node,term());
    else return node;
  }
}
Node* expr(){
  Node *node = mul();//一つ目の文字

  for(;;){
    if(consume('+')) node = new_node('+',node,mul());
    else if(consume('-')) node = new_node('-',node,mul());
    else return node;
  }
}

//ASTをアセンブリにしましょう
void gen(Node* node){
  if(node->ty == ND_NUM){
    printf("  push %d\n",node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("pop rdi\n");
  printf("pop rax\n");

  switch(node->ty){
    case '+':
      printf("  add rax, rdi\n");
      break;
    case '-':
      printf("  sub rax, rdi\n");
      break:
    case '*':
      printf("  imul rdi\n");//imulはraxとrdiの席をrdxとraxに入れる.
      break;
    case '/':
      printf("  cqo\n");//これはraxを拡張してくれるもの.
      printf("  idiv rdi\n");//idivはrdxとraxの128bitをrdiで割って結果をraxにセットする。
  }

  printf("  push rax\n");
}
int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  user_input = argv[1];

  //トークナイズする
  tokenize(argv[1]);
  Node* node = expr();

  //アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  //ASTを走査してアセンブリを出力
  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
