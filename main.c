#include "kcc.h"

Token *token;

char *user_input;

static char *read_file(char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        error("cannot open %s: %s", path, strerror(errno));
    }
    int filemax = 10 * 1024 * 1024;
    char *buf = malloc(filemax);
    fread(buf, 1, filemax, fp);
    return buf;
}


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // トークナイズする
    // user_input = read_file(argv[1]);
    user_input = argv[1];
    token = tokenize();
    program();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");

    // 先頭の式から順にコード生成
    for (int i = 0; code[i]; i++) {
        gen(code[i]);

        // 式の評価結果としてスタックに一つの値が残っている
        // はずなので、スタックが溢れないようにポップしておく
        printf("    pop rax\n");
    }

    // エピローグ
    // 最後の式の結果がRAXに残っているのでそれが返り値になる
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
    return 0;
}

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// エラー個所を報告する
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}