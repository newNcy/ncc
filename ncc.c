#include <stdio.h>
#include <malloc.h>

char  msg[200][10]={
    "IMPORT",
    "VOID","CHAR","SHORT","INT","LONG","FLOAT","DOUBLE","ENUM","STRUCT",
    "UNSIGNED","STATIC","CONST","EXTERN",
    "IF","ELSE","SWITCH","CASE","DEFAULT","FOR","DO","WHILE","CONTINUE","BREAK","RETURN",
    "PLUS","SUB","STAR","DIV","MOD", // + - * / %
    "SPLUS","SSUB", // ++ --
    "ASSIGN","PAS","SAS","MAS","DAS","MODAS", // = += -= *= /= %=
    "EQ","NEQ","GEQ","LEQ","GT","LT", // == != >= <= > <
    "AND","OR","NOT", //&& || !
    "BAND","BOR","BRE","XOR", // & | ~ ^
    "SL","SR", // << >>
    "AAS","OAS","RAS","XAS", // &= |= ~= ^= 
    "SLAS","SRAS", //
    "DOT","ARROW", // . ->

    "LPB","RPB","LBK","RBK","LBS","RBS", // () [] {}
    "COM","COL","SEM", // , : ;

    "ID","CINT","CDOUBLE","CSTRING","CCHAR"
};


enum Ttype{
    IMPORT,
    VOID,CHAR,SHORT,INT,LONG,FLOAT,DOUBLE,ENUM,STRUCT,
    UNSIGNED,STATIC,CONST,EXTERN,
    IF,ELSE,SWITCH,CASE,DEFAULT,FOR,DO,WHILE,CONTINUE,BREAK,RETURN,
    PLUS,SUB,STAR,DIV,MOD, // + - * / %
    SPLUS,SSUB, // ++ --
    ASSIGN,PAS,SAS,MAS,DAS,MODAS, // = += -= *= /= %=
    EQ,NEQ,GEQ,LEQ,GT,LT, // == != >= <= > <
    AND,OR,NOT, //&& || !
    BAND,BOR,BRE,XOR, // & | ~ ^
    SL,SR, // << >>
    AAS,OAS,RAS,XAS, // &= |= ~= ^= 
    SLAS,SRAS, //
    DOT,ARROW, // . ->

    LPB,RPB,LBK,RBK,LBS,RBS, // () [] {}
    COM,COL,SEM, // , : ;

    ID,CINT,CDOUBLE,CSTRING,CCHAR
};

void debug(char * msg)
{
    puts(msg);
}
typedef struct 
{
    enum Ttype type;
}Token;
typedef struct
{
    char filename[4096];
    //双缓冲
    char * buff_alpha;
    char * buff_beta;
    
    char * lex_begin;
    char * forward;

    int line;
    int chpos;

    int buff_size;
    int finish;
    //源文件
    FILE * source_file;
}Lex;
const char LEX_WHATCHER = EOF;


/*
 * 填充缓冲区
 */
void fill_buff(char *buff, int size, FILE * source)
{
    int rc = 0;
    rc = fread(buff,1,size-1,source);
    buff[rc] = LEX_WHATCHER;
}

/*
 *初始化词法分析 
 */
void lex_init (Lex *lex)
{
    if (lex->buff_size <= 0) {
        lex->buff_size = 1024;
    }
    lex->buff_alpha = malloc(lex->buff_size);
    lex->buff_beta = malloc(lex->buff_size);
    lex->line = 1;
    lex->chpos = 1;
    lex->finish = 0;
    lex->source_file = fopen(lex->filename,"r");

    if (lex->source_file == NULL) {
        debug("文件损坏");
        lex->finish = 0;
        return;
    }
    fill_buff(lex->buff_alpha,lex->buff_size,lex->source_file);
    lex->buff_beta[lex->buff_size-1] = LEX_WHATCHER;
    lex->lex_begin = lex->buff_alpha;
    lex->forward = lex->buff_alpha;
}


/*
 * 词法分析测试 是否完成
 * 切换缓冲区
 */
void lex_test(Lex * lex)
{
    if (lex->forward == lex->buff_alpha+lex->buff_size-1) {
        fill_buff(lex->buff_beta,lex->buff_size,lex->source_file);
        lex->forward = lex->buff_beta; 
        //debug("切换缓冲");
    } else if (lex->forward == lex->buff_beta+lex->buff_size-1) {
        fill_buff(lex->buff_alpha,lex->buff_size,lex->source_file);
        lex->forward = lex->buff_alpha; 
        //debug("切换缓冲");
    }else {
        lex->finish = 1;
    }
}



void lex_skip(Lex *lex)
{
    char ch = *lex->forward;

    if (ch == ' ' | ch == '\n') {
        putchar(' ');
    }
    while (ch == ' ' || ch == '\n') {
            //putchar('\n');
        lex->forward ++;
        ch = *lex->forward;
    }
}


int is_alpha(char ch)
{
    return (ch <= 'Z' && ch >= 'A') || (ch <= 'z' && ch >= 'a');
}
int is_digit(char ch)
{
    return (ch <= '9' && ch >= '0');
}

int match(char *s,char *e,char *res)
{
    if (s > e) {
        return 0;
    }
    int i = 0;
    while (s+i <= e) {
        if (*(s+i) != res[i]) return 0;
        i++;
    }
    return 1;
}

/*
 * 下一个
 */
Token lex_next_token (Lex *lex)
{
    int  ch;

    Token tk;
    char cstring [4096] = {0};
    int wp = 0;
    lex_skip(lex);
    lex->lex_begin = lex->forward;
    ch = *lex->forward;
    if (ch == LEX_WHATCHER) {
        lex_test(lex);
        return lex_next_token(lex);
    }else if(ch == '(') {
        tk.type = LPB;
    }else if(ch == ')') {
        tk.type = RPB;
    }if (is_alpha(ch) || ch == '_') {
        tk.type = ID;
        while(is_alpha(ch) || is_digit(ch) || ch == '_') {
            ch = *(++lex->forward);
        }
        --lex->forward;
    }
        //putchar(ch);       
    return tk;
}

void lex_free (Lex * lex)
{
    if (lex) {
        free(lex->buff_alpha);
        free(lex->buff_alpha);
        fclose(lex->source_file);
    }
}

int main (int argc, char ** argv)
{

    FILE * f = fopen("ncc.c","r");
    Lex lex = {"tests/ncc.ncc.c"};
    lex.buff_size = 1024;
    lex.source_file = f;
    lex_init(&lex);
    while (!lex.finish) {
        printf("%s\n",lex_next_token(&lex).type + msg);
    }
    lex_free(&lex);
}
