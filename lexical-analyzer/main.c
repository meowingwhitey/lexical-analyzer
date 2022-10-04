/*
*
* 201720857 사이버보안학과 김영표
* 2022-2 컴파일러 Programming Project #1
* Lexical Analyzer 구현
* 
* Reference
* 2주차 강의노트 - LEXICAL ANALYSIS
* 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TABLE_SIZE 1000
#define MAX_BUFFER_SIZE 100

#define ID 0
#define INTEGER 1
#define REAL 2
#define STRING 3
#define ADD 4
#define SUB 5
#define MUL 6
#define DIV 7
#define ASSIGN 8
#define COLON 9
#define SEMI_COLON 10

#define TRUE 1
#define FALSE 0

#define FAIL_STATE -1
const char TOKEN_TYPES[][11] = {
    "ID", "INT", "REAL", "STRING","ADD_OP", "SUB_OP", 
    "MUL_OP", "DIV_OP", "ASSIGN", "COLON", "SEMICOLON" 
};

typedef struct Token{
    int type;
    int value;
}Token;

typedef int State;
typedef int Bool;

void error(void) {
    printf("[*] State Error! \n");
    exit(-1);
}

Token getNextToken();
void skipWhiteSpace();
Token getId();
Token getNumber();
Token getString();
Token getOperator();
Bool isTokenFail(Token token);
int installId();

char nextChar();

int lexeme_start = 0;
int forward = 0;
const Token FAIL_TOKEN = { -1, -1 };
char* symbol_list[MAX_TABLE_SIZE] = { NULL, };
char* string_list[MAX_TABLE_SIZE] = { NULL, };
char buffer[MAX_BUFFER_SIZE] = { NULL, };
char lexeme[MAX_BUFFER_SIZE] = { NULL, };

int main(int argc, char* argv[]) {
    Token token;

    // scan line from stdin
    while (TRUE) {
        scanf("%s", buffer);
        lexeme_start = 0;
        forward = 0;
        // parse token
        while (TRUE) {
            token = getNextToken();
            strncpy(&lexeme, &buffer[lexeme_start], forward - lexeme_start);
            printf("<%s, %d> %s\n", TOKEN_TYPES[token.type], token.value, lexeme);
            lexeme_start = forward;
            if (lexeme_start >= strlen(buffer)) {
                break;
            }
        }
    }
    return 0;
}
char nextChar() {
    forward++;
    return buffer[forward];
}
Token getNextToken() {
    State state = 0;
    Token token = { NULL, NULL };
    while (TRUE) {
        skipWhiteSpace();
        switch (state) {
        case 0:
            token = getId();
            if (isTokenFail(token)) { state = 6; break; }
            return token;
        case 6:
            //token = getNumber();
            if (isTokenFail(token)) { state = -1; break; }
            return token;
        default:
            error();
        }
    }
}
Token getId() {
    State state = 1;
    Token token = {0, 0};
    char ch = NULL;
    ch = nextChar();
    while (TRUE) {
        switch (state) {
        case 1:
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
                state = 2;
                ch = nextChar();
                break;
            }
            else {
                state = FAIL_STATE;
                break;
            }
        case 2:
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
                state = 2;
                ch = nextChar();
                break;
            }
            else if (ch >= '0' && ch <= '9') {
                state = 3;
                ch = nextChar();
                break;
            }
            else {
                state = 4;
                ch = nextChar();
                break;
            }
        case 3:
            if (ch >= '0' && ch <= '9') {
                state = 3;
                ch = nextChar();
                break;
            }
            else {
                state = 4;
                ch = nextChar();
                break;
            }
        case 4:       
            token.type = ID;
            token.value = installId();
            return token;
        default:
            return FAIL_TOKEN;
        }
    }
}

void skipWhiteSpace() {
    char ch = NULL; 
    while ((ch = nextChar()) == ' ') {}
    forward--;
    return;
}
Bool isTokenFail(Token token) {
    if (token.type == FAIL_TOKEN.type)
        return TRUE;
    return FALSE;
}
int installId() {
    static int index_count = -1;
    // check if there is same id
    for (int i = 0; i <= index_count; i++) {
        // limit length of id below 10 when comparing
        int id_length = (forward - lexeme_start) - 1;
        if (id_length > 10) {
            id_length = 10;
        }
        if (strlen(symbol_list[i]) < id_length) {
            continue;
        }
        if (strncmp(symbol_list[i], &buffer[lexeme_start], id_length) == 0) {
            return i;
        }
    }

    // install new id 
    index_count++;
    int size = sizeof(char) * (forward - lexeme_start);
    char* symbol = (char*)malloc(size);
    strcpy(symbol, &buffer[lexeme_start], size);
    symbol_list[index_count] = symbol;
    return index_count;
}