#include "kcc.h"

static bool is_alnum(char c);
static bool is_alpha(char c);
static bool startswith(char *p, char *q);
static char *starts_with_reversed(char *p);
static Token *new_token(TokenKind kind, Token *cur, char *str, int len);

static bool is_alnum(char c) {
    return  is_alpha(c) || ('0' <= c && c <= '9');
}

static bool is_alpha(char c) {
    return (('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
            c == '_');
}

static bool startswith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

static char *starts_with_reversed(char *p) {
    static char *kw[] = {"return", "if", "else", "while", "for"};

    for (int i = 0; i < sizeof(kw)/sizeof(*kw) ; i++) {
        int len = strlen(kw[i]);
        if (startswith(p, kw[i]) && !is_alnum(p[len])) {
            return kw[i];
        }
    }
    static char *ops[] = {"==", "!=", "<=", ">="};
    for (int i = 0; i < sizeof(ops)/sizeof(*ops); i++) {
        if (startswith(p, ops[i])) {
            return ops[i];
        }
    }
    static char *types[] = {"int"};
    for (int i = 0; i < sizeof(types)/sizeof(*types); i++) {
        if (startswith(p, types[i])) {
            return types[i];
        }
    }
    return NULL;
}

// 新しいトークンを作成してcurに繋げる
static Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

Token *tokenize() {
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // 空文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }
        // Keywords or multi-letter puctuators
        char *kw = starts_with_reversed(p);
        if (kw) {
            int len = strlen(kw);
            cur = new_token(TK_RESERVED, cur, p, len);
            p += len;
            continue;
        }

        // Single-letter punctuator
        if (strchr("+-*/()<>;={},&", *p)) {
            cur = new_token(TK_RESERVED, cur, p, 1);
            p++;
            continue;
        }
        // Integer literal
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            continue;
        }
        // Identifier
        if (is_alpha(*p)) {
            int len = 0;
            char *tmp = p;
            while(is_alnum(*p)) {
                len++;
                p++;
            }
            cur = new_token(TK_IDENT, cur, tmp, len);
            continue;
        }

        error_at(p, "トークナイズ出来ません");
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;   // トークナイズしたトークンの最初のポインタを返す
}