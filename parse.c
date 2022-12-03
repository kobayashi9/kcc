#include "kcc.h"

Node *code[100];

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
    LVar *next; // 次の変数かNULLか
    char *name; // 変数の名前
    int len;    // 名前の長さ
    int offset; // RBPからのoffset
};

// ローカル変数
static LVar *locals;

static LVar *find_lvar(Token *tok);

void program() {
    int i = 0;
    locals = calloc(1, sizeof(LVar));
    locals->offset = 0;
    
    while (!at_eof())
        code[i++] = def_func();
    code[i] = NULL;
}

// 関数定義
Node *def_func() {
    expect("int");
    Token *tok = consume_ident();
    Node *node = calloc(1, sizeof(Node));
    if (tok) {
        expect("(");
        node->kind = ND_FUNCDEF;
        node->name = tok->str;
        node->len = tok->len;
        if(!consume(")")) {
            Node* argVector = node;
            node->args = argVector;
            do {
                expect("int");
                if(token->kind != TK_IDENT) {
                    error_at(tok->str, "変数ではありません。");
                }
                argVector->args = dcl();
                argVector = argVector->args;
            }
            while(consume(","));
            argVector->args = NULL;
            expect(")");
        }
        expect("{");
        Node *fnVector = node;
        node->fn = fnVector;
        while(!consume("}")) {
            fnVector->fn = calloc(1, sizeof(Node));
            fnVector->fn = stmt();
            fnVector = fnVector->fn;
        }
        fnVector->fn = NULL;
        return node;
    } 
}

Node *dcl(){
    Node *node;
    LVar *lvar;
    node = calloc(1, sizeof(Node)); 
    lvar = calloc(1, sizeof(LVar));
    lvar->next = locals;
    lvar->name = token->str;
    lvar->len = token->len;
    lvar->offset = locals->offset + 8;
    node->offset = lvar->offset;
    locals = lvar;   
    node->kind = ND_LVAR;
    token = token->next; 
    return node;
}
 
Node *assign() {
    Node *node = equality();
    if (consume("="))
        node = new_node(ND_ASSIGN, node, assign());
    return node;
}

Node *expr() {
    return assign();
}

Node *stmt() {
    Node *node;

    if (consume("return")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
        expect(";");
        return node;
    }

    if (consume("if")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        if (consume("else"))
            node->els = stmt();
        return node;
    }

    if (consume("while")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        return node;
    }
    
    if (consume("for")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_FOR;
        expect("(");
        
        if (!consume(";")) {
           node->init = expr(); 
        }
        expect(";");
        
        if (!consume(";")) {
            node->cond = expr();
        }
        expect(";");

        if (!consume(")")) {
            node->inc = expr();
        }
        expect(")");

        node->then = stmt();
        return node;
    }
    
    if (consume("{")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        Node *blockVector = node;
        node->body = blockVector;
        while (!consume("}")) {
            blockVector->body = calloc(1, sizeof(Node));
            blockVector->body = stmt();
            blockVector = blockVector->body;
        }
        blockVector->body = NULL;
        return node;
    }

    if (consume("int")) {
        if (token->kind == TK_IDENT) {
            node = dcl();
        } else {
            error_at(token->str, "識別子ではありません");
        }
        expect(";");
        return node;
    }

    node = expr();
    expect(";");
    return node;
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("=="))
            node = new_node(ND_EQ, node, relational());
        else if (consume("!="))
            node = new_node(ND_NE, node, relational());
        else
            return node;
    }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume("<"))
            node = new_node(ND_LT, node, add());
        else if (consume("<="))
            node = new_node(ND_LE, node, add());
        else if (consume(">"))
            node = new_node(ND_LT, add(), node);
        else if (consume(">="))
            node = new_node(ND_LE, add(), node);
        else
            return node;        
        
    }
}
// add = mul ("+" mul | "-" mul)*
Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

// unary = "+"? primary
//       | "-"? unary
//       | "*" unary
//       | "&" unary
//       | primary
Node *unary() {
    if (consume("+"))
        return primary();
    if (consume("-"))
        return new_node(ND_SUB, new_node_num(0), primary());
    if (consume("*"))
        return new_node(ND_DEREF, unary(), NULL);
    if (consume("&"))
        return new_node(ND_ADDR, unary(), NULL);
    return primary();
}

// primary = "(" expr ")" | num
Node *primary() {
    // 次のトークンが"("なら、"(" expr ")"のはず
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();
    if (tok) {
        Node *node = calloc(1, sizeof(Node));
        if(consume("(")){
            node->kind = ND_FUNCTION;
            node->name = tok->str;
            node->len = tok->len;
            if(!consume(")")) {
                Node* argVector = node;
                node->args = argVector;
                do {
                    argVector->args = expr();
                    argVector = argVector->args;
                }
                while(consume(","));
                argVector->args = NULL;
                expect(")");
            }
        } else {
            node->kind = ND_LVAR;

            LVar *lvar = find_lvar(tok);
            if (lvar) {
                node->offset = lvar->offset;
            } else {
                error_at(tok->str, "変数が宣言されていません。");
            }
        }   
        return node;
    }

    // そうでなければ数値のはず
    return new_node_num(expect_number());
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op) {
    if (token->kind != TK_RESERVED || 
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

extern Token *consume_ident() {
    if (token->kind == TK_IDENT) {
        Token *rtn = token;
        token = token->next;
        return rtn;
    }
    return NULL;
}

// 次のトークンが期待している記号の時には、トークンを1つ読み進める。
// それ以外の場合はエラーを報告する。
void expect(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len ||
        memcmp(token->str, op, token->len)) {
        error_at(token->str, "'%s'ではありません", op);
        }
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する、
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

extern bool at_eof() {
    return token->kind == TK_EOF;
}

static LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            return var;
        }
    }
    return NULL;
}