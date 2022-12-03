#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_ASSIGN,  // =
    ND_LVAR,    // ローカル変数
    ND_EQ,  // ==
    ND_NE,  // !=
    ND_LT,  // <
    ND_LE,  // <=
    ND_RETURN,  // return文
    ND_IF,  // if文
    ND_ELSE,    // else文
    ND_WHILE,   // while文
    ND_FOR,     // for文
    ND_NUM, // 整数
    ND_BLOCK, //ブロック
    ND_FUNCCALL, // 関数呼び出し
    ND_FUNCDEF, // 関数定義
    ND_ADDR,    // アドレス参照
    ND_DEREF    // 間接参照
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind; // ノードの型
    Node *lhs;     // 左辺
    Node *rhs;     // 右辺
    int val;       // kindがND_NUMの場合のみ使う
    int offset;    // kindがND_LVARの場合のみ使う

    // "if, "while" or "for" statement
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    char *name;     // 関数名
    int len;        // 関数名の長さ
    Node *args;      // 関数の実引数
    Node *fn;       // 関数定義
};

// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT,    // 識別子
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合、その数値
    char *str;      // トークン文字列
    int len;        // トークンの長さ
};


// 入力プログラム
extern char *user_input;

// 現在着目しているトークン
extern Token *token;

extern Token *tokenize();

extern Node *code[100];
extern void program();

extern Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
extern Node *new_node_num(int val);

extern void gen(Node *node);

extern void error_at(char *loc, char *fmt, ...);
extern void error(char *fmt, ...);