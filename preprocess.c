#include "kcc.h"

bool is_alpha(char c);

bool is_alnum(char c) {
    return  is_alpha(c) || ('0' <= c && c <= '9');
}

bool is_alpha(char c) {
    return (('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
            c == '_');
}

bool startswith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

char *starts_with_reversed(char *p) {
    static char *kw[] = {"return", "if", "else", "while", "for"};
    
    for (int i = 0; i < sizeof(kw)/sizeof(*kw) ; i++) {
        int len = strlen(kw[i]);
        if (startswith(p, kw[i]) && !is_alnum(p[len])) {
            return kw[i];
        }
    }
    static char *ops[] = {"==", "!=", "<=", ">="};
    for (int i = 0; i < sizeof(ops)/sizeof(*ops); i++) {
        int len = strlen(ops[i]);
        if (startswith(p, ops[i]) && !is_alnum(p[len])) {
            return ops[i];
        }
    }
    return NULL;
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
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
        if (strchr("+-*/()<>;=", *p)) {
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