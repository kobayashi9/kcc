#include "kcc.h"

static int labelseq = 1;
static char funcname[50];
static char funcargs[7][4] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
static void gen_lval(Node *node);

static void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");
    
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");
}


void gen(Node *node) {
    switch (node->kind){
    case ND_RETURN: {
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;
    }
    case ND_IF: {
        int seq = labelseq++;
        if (node->els) {
            gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je  .L.else.%d\n", seq);
            gen(node->then);
            printf("    jmp .L.end.%d\n", seq);
            printf(".L.else.%d:\n", seq);
            gen(node->els);
            printf(".L.end.%d:\n", seq);
        } else {
            gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je  .L.end.%d\n", seq);
            gen(node->then);
            printf(".L.end.%d:\n", seq);
        }
        return;
    }
    case ND_WHILE: {
        int seq = labelseq++;
        printf(".L.continue.%d:\n", seq);
        gen(node->cond);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .L.break.%d\n", seq);
        gen(node->then);
        printf("    jmp .L.continue.%d\n", seq);
        printf(".L.break.%d:\n", seq);
        return;
    }
    case ND_FOR: {
        int seq = labelseq++;
        gen(node->init);
        printf(".L.begin.%d:\n", seq);
        gen(node->cond);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .L.break.%d\n", seq);
        gen(node->then);
        gen(node->inc);
        printf("    jmp .L.begin.%d\n", seq);
        printf(".L.break.%d:\n", seq);
        return;
    }
    case ND_BLOCK: {
        while (node->body != NULL) {
            gen(node->body);
            printf("    pop rax\n");
            node = node->body;
        }
        return;
    }
    }
    
    switch (node->kind) {
    case ND_NUM:
        printf("    push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
        return;
    case ND_FUNCCALL: {
        int funcargsIndex = 0;
        for(int i = 0; i < node->len; i++) {
            funcname[i] = node->name[i];
        }
        funcname[node->len] = '\0';

        while(node->args != NULL &&  funcargsIndex< 6) {
            gen(node->args);
            printf("    pop %s\n", funcargs[funcargsIndex]);
            node = node->args;
            funcargsIndex++;
        }
        printf("    call %s\n", funcname);
        printf("    push rax\n");
        return;
    }
    case ND_FUNCDEF: {
        int funcargsIndex = 0;
        for(int i = 0; i < node->len; i++) {
            funcname[i] = node->name[i];
        }
        funcname[node->len] = '\0';
        printf("%s:\n", funcname);
        printf("    push rbp\n");
        printf("    mov rbp, rsp\n");
        printf("    sub rsp, 208\n");
        Node *argVector = node;
        while(argVector->args != NULL &&  funcargsIndex< 6) {
            gen_lval(argVector->args);
            printf("    push %s\n", funcargs[funcargsIndex]);
            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    mov [rax], rdi\n");
            printf("    push rdi\n");    
            argVector = argVector->args;
            funcargsIndex++;
        }    
        while(node->fn != NULL) {
            gen(node->fn);
            node = node->fn;
        }
        return;
    }
    case ND_ADDR: {
        gen_lval(node->lhs);
        return;
    }
    case ND_DEREF: {
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    }
    }
    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind) {
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("    imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("    cqo\n");
        printf("    idiv rax, rdi\n");
        break;
    case ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_NE:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LT:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    }

    printf("    push rax\n");
}