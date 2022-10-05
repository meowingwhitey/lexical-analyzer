/*
*
* 201720857 ���̹������а� �迵ǥ
* 2022-2 �����Ϸ� Programming Project #1
* Lexical Analyzer ����
*
* Reference
* 2���� ���ǳ�Ʈ - LEXICAL ANALYSIS
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TABLE_SIZE 100
#define MAX_BUFFER_SIZE 1000

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

#define FAILED_STATE -1

typedef double DWORD;
typedef int State;
typedef int Bool;

const char TOKEN_TYPES[][11] = {
    "ID", "INT", "REAL", "STRING","ADD_OP", "SUB_OP",
    "MUL_OP", "DIV_OP", "ASSIGN", "COLON", "SEMICOLON"
};
typedef union Value {
    DWORD raw;
}Value;

typedef struct Token{
    int type;
    Value value;
}Token;

Token getNextToken();
void skipWhiteSpace();

Token getId();
Token getNumber();
Token getString();
Token getOperator();
State getMultilineString();

Bool isTokenFail(Token token);
int installId();
int installString();
void retract();
void fail();
char nextChar();
void storeLexeme();
void initLexemeBuffer();
void error(void);
void printSymbolTable();
void printStringTable();

int lexeme_start = 0;
int forward = 0;
int current_line = 0;
const Token FAILED_TOKEN = { -1, -1 };
char* symbol_table[MAX_TABLE_SIZE] = { NULL, };
char* string_table[MAX_TABLE_SIZE] = { NULL, };
char buffer[MAX_BUFFER_SIZE] = { NULL, };
char lexeme[MAX_BUFFER_SIZE] = { NULL, };

int main(int argc, char* argv[]) {
    Token token;
    lexeme_start = 0;
    forward = 0;
    // scan line from stdin
    while (TRUE) {
        // prevent void input
        int prev_length = strlen(buffer);
        gets(buffer + strlen(buffer));
        int current_length = strlen(buffer);
        buffer[strlen(buffer)] = '\n';
        if (current_length - prev_length == 0 || (current_length - prev_length == 1 && buffer[strlen(buffer) - 1] == '\n')) {
            buffer[strlen(buffer) - 1] = NULL;
            continue;
        }

        current_line++;
        // parse token
        while (TRUE) {
            skipWhiteSpace();
            if (lexeme_start == strlen(buffer)) {
                //lexeme_start++; forward++;
                break;
            }
            token = getNextToken();
            if (token.type == ID) {
                printf("<%s, %d> %s\n", TOKEN_TYPES[token.type], (int)token.value.raw, lexeme);
                printSymbolTable();
            }
            else if (token.type == INTEGER) {
                printf("<%s, %d> %s\n", TOKEN_TYPES[token.type], (int)token.value.raw, lexeme);
            }
            else if (token.type == REAL) {
                printf("<%s, %lf> %s\n", TOKEN_TYPES[token.type], token.value.raw, lexeme);
            }
            else if (token.type >= ADD && token.type <= SEMI_COLON) {
                printf("<%s, > %s\n", TOKEN_TYPES[token.type], lexeme);
            }
            else if (token.type == STRING) {
                printf("<%s, %d> %s\n", TOKEN_TYPES[token.type], (int)token.value.raw, lexeme);
                printStringTable();
            }
            else {
                printf("[*] Token Error\n");
            }
            lexeme_start = forward;
        }
        // Line clear
        printf("\n");
    }
    return 0;
}
Token getNextToken() {
    State state = 0;
    Token token = { NULL, 0 };
    initLexemeBuffer();
    while (TRUE) {
        skipWhiteSpace();
        switch (state) {
        case 0:
            token = getId();
            if (isTokenFail(token)) { state = 1; break; }
            return token;
        case 1:
            token = getNumber();
            if (isTokenFail(token)) { state = 2; break; }
            return token;
        case 2:
            token = getOperator();
            if (isTokenFail(token)) { state = 3; break; }
            return token;
        case 3:
            token = getString();
            if (isTokenFail(token)) { state = -1; break; }
            return token;
        default:
            error();
            // Skip Error Character
            forward++;
            lexeme_start++;
            state = 0;
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
                state = 2; ch = nextChar();
                break;
            }
            else {
                state = FAILED_STATE;
                break;
            }
        case 2:
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
                state = 2; ch = nextChar();
                break;
            }
            else if (ch >= '0' && ch <= '9') {
                state = 3; ch = nextChar();
                break;
            }
            else {
                state = 4; //ch = nextChar();
                break;
            }
        case 3:
            if (ch >= '0' && ch <= '9') {
                state = 3; ch = nextChar();
                break;
            }
            else {
                state = 4; //ch = nextChar();
                break;
            }
        case 4:
            retract(); storeLexeme();
            token.type = ID;
            token.value.raw = installId();
            return token;
        default:
            fail();
            return FAILED_TOKEN;
        }
    }
}

Token getNumber() {
    State state = 1;
    Token token = { 0, 0 };
    char ch = NULL;
    ch = nextChar();
    while (TRUE) {
        switch (state) {
        case 1:
            if (ch >= '0' && ch <= '9') {
                state = 2; ch = nextChar();
                break;
            }
            else if( ch == '.') {
                state = 3; ch = nextChar();
                break;
            }
            else {
                state = FAILED_STATE;
                break;
            }
        case 2:
            if (ch >= '0' && ch <= '9') {
                state = 2; ch = nextChar();
                break;
            }
            else if (ch == '.') {
                state = 3; ch = nextChar();
                break;
            }
            else {
                state = 4; //ch = nextChar();
                break;
            }
        case 3:
            if (ch >= '0' && ch <= '9') {
                state = 3; ch = nextChar();
                break;
            }
            else {
                state = 5; //ch = nextChar();
                break;
            }
        case 4:
            retract(); storeLexeme();
            token.type = INTEGER;
            token.value.raw = atoi(lexeme);
            return token;
        case 5:
            retract(); storeLexeme();
            token.type = REAL;
            token.value.raw = strtod(lexeme, NULL);
            return token;
        default:
            fail();
            return FAILED_TOKEN;
        }
    }
}
Token getOperator() {
    State state = 1;
    Token token = { 0, 0 };
    char ch = NULL;
    ch = nextChar();
    while (TRUE) {
        switch (state) {
        case 1:
            if (ch == '+') { state = 2; ch = nextChar(); break; }
            else if (ch == '-') { state = 3; ch = nextChar(); break; }
            else if (ch == '*') { state = 4; ch = nextChar(); break; }
            else if (ch == '/') { state = 5; ch = nextChar(); break; }
            else if (ch == '=') { state = 6; ch = nextChar(); break; }
            else if (ch == ':') { state = 7; ch = nextChar(); break; }
            else if (ch == ';') { state = 8; ch = nextChar(); break; }
            else {
                state = FAILED_STATE;
                break;
            }
        case 2:
            retract(); storeLexeme();
            token.type = ADD;
            return token;
        case 3:
            retract(); storeLexeme();
            token.type = SUB;
            return token;
        case 4:
            retract(); storeLexeme();
            token.type = MUL;
            return token;
        case 5:
            retract(); storeLexeme();
            token.type = DIV;
            return token;
        case 6:
            retract(); storeLexeme();
            token.type = ASSIGN;
            return token;
        case 7:
            retract(); storeLexeme();
            token.type = COLON;
            return token;
        case 8:
            retract(); storeLexeme();
            token.type = SEMI_COLON;
            return token;
        default:
            fail();
            return FAILED_TOKEN;
        }
    }
}
Token getString() {
    State state = 1;
    Token token = { 0, 0 };
    char ch = NULL;
    ch = nextChar();
    while (TRUE) {
        switch (state) {
        case 1:
            if (ch == '\"') {
                state = 2; ch = nextChar();
                break;
            }
            else {
                state = FAILED_STATE;
                break;
            }
        case 2:
            if (ch != '\\' && ch != '\"') {
                state = 2; ch = nextChar();
                break;
            }
            else if (ch == '\\') {
                state = 3; ch = nextChar();
                break;
            }
            else if (ch == '\"') {
                state = 4; ch = nextChar();
                break;
            }
            else {
                state = FAILED_STATE;
                break;
            }
        case 3:
            if (ch == 't' || ch == 'n' || ch == 'a' || ch == '\\' || ch == '\'' || ch == '\"' || ch == 'b' || ch == 'r') {
                state = 2; ch = nextChar();
                break;
            }
            else if (ch == '\0' || ch == '\n') {
                state = 5;
                break;
            }
            else {
                state = FAILED_STATE;
                break;
            }
        case 4:
            retract(); storeLexeme();
            token.type = STRING;
            token.value.raw = installString();
            return token;
        case 5:
            retract();
            state = getMultilineString();
            break;
        default:
            fail();
            return FAILED_TOKEN;
        }
    }
}
State getMultilineString() {
    char ch = NULL;
    // prevent void input
    int prev_length = strlen(buffer);
    gets(buffer + strlen(buffer));
    int current_length = strlen(buffer);
    buffer[strlen(buffer)] = '\n';
    if (current_length - prev_length == 0 || (current_length - prev_length == 1 && buffer[strlen(buffer) - 1] == '\n')) {
        buffer[strlen(buffer) - 1] = NULL;
    }
    return 2;
}
void retract() {
    forward--;
}
void fail() {
    forward = lexeme_start;
}
void initLexemeBuffer() {
    memset(lexeme, NULL, MAX_BUFFER_SIZE);
}
void skipWhiteSpace() {
    char ch = NULL;
    ch = nextChar();
    while (ch == ' ' || ch == '\n') {
        ch = nextChar();
        lexeme_start++;
    }
    retract();
    return;
}
Bool isTokenFail(Token token) {
    if (token.type == FAILED_TOKEN.type)
        return TRUE;
    return FALSE;
}
int installId() {
    static int index_count = -1;
    // check if there is same id
    for (int i = 0; i <= index_count; i++) {
        // limit length of id below 10 when comparing
        int id_length = strlen(symbol_table[i]) > strlen(lexeme) ? strlen(symbol_table[i]) : strlen(lexeme);
        if (id_length > 10) {
            id_length = 10;
        }
        if (strlen(symbol_table[i]) < id_length) {
            continue;
        }
        if (strncmp(symbol_table[i], lexeme, id_length) == 0) {
            return i;
        }
    }
    // install new id
    index_count++;
    // + 1 is for NULL
    int size = sizeof(char) * strlen(lexeme) + 1;
    if (size > 11) {
        size = 11;
    }
    char* symbol = (char*)malloc(size);
    memset(symbol, NULL, size);
    strncpy(symbol, lexeme, size - 1);
    symbol_table[index_count] = symbol;
    return index_count;
}
int installString() {
    static int index_count = -1;
    // check if there is same string
    for (int i = 0; i <= index_count; i++) {
        if (strcmp(string_table[i], lexeme) == 0) {
            return i;
        }
    }
    // install new string
    index_count++;
    // + 1 is for NULL Character
    int size = sizeof(char) * strlen(lexeme) + 1;
    char* string = (char*)malloc(size);
    memset(string, NULL, size);
    strncpy(string, lexeme, size);
    string_table[index_count] = string;
    return index_count;
}
char nextChar() {
    int i = forward++;
    return buffer[i];
}
void storeLexeme() {
    int j = 0;
    for (int i = 0; i + j < forward - lexeme_start; i++) {
        while (TRUE) {
            if (buffer[lexeme_start + i + j] == '\n' || buffer[lexeme_start + i + j] == '\\') {
                j++;
            }
            else break;
        }
        lexeme[i] = buffer[lexeme_start + i + j];
    }
    //strncpy(&lexeme, &buffer[lexeme_start], forward - lexeme_start);
    return;
}
void error(void) {
    printf("Error Line #%d : %c(0x%x) \n", current_line, buffer[forward], buffer[forward]);
    //exit(-1);
    return;
}
void printSymbolTable(){
    int i = 0;
    // print symbol table
    printf("[*] Symbol Table\n");
    while(TRUE){
         if(symbol_table[i] == NULL){
            break;
        }
        printf("Index: %d, Symbol: %s\n", i, symbol_table[i]);
        i++;       
    }
    printf("\n");
    return;
}
void printStringTable(){
    int i = 0;
    // print string table
    printf("[*] String Table\n");
    while(TRUE){
        if(string_table[i] == NULL){
            break;
        }
        printf("Index: %d, String: %s\n", i, string_table[i]);
        i++;
    }
    printf("\n");
    return;
}