import stdio.h 
import malloc.h


enum Ttype{
    IMPORT,
    VOID,CHAR,SHORT,INT,LONG,FLOAT,DOUBLE,ENUM,STRUCT,
    UNSIGNED,STATIC,CONST,EXTERN,
    IF,ELSE,SWITCH,CASE,DEFAULT,FOR,DO,WHILE,CONTINUE,BREAK,RETURN,
    PLUS,SUB,STAR,DIV,MOD, // + - * / %
    SPLUS,SSUB, // ++ --
    ASSIGN,PASSIGN,SASSIGN, // = += -=
    EQ,NEQ,GEQ,LEQ,GT,LT, // == != >= <= > <
    BAND,BOR,NOT, //&& || !
    AND,OR,XOR, // & | ^
    SL,SR, // << >>
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


char lex_getch(Lex *lex) 
{
    char ch = *lex->forward;
    return ch;
}
void lex_unget(Lex *lex)
{
    lex->forward --;
}

void lex_skip(Lex *lex)
{
    char ch = *lex->forward;

    if (ch == ' ' | ch == '\n') {
        putchar(' ');
    }
    while (ch == ' ' || ch == '\n') {
        if (ch == '\n');
            //putchar('\n');
        lex->forward ++;
        ch = *lex->forward;
    }
}
/*
 * 下一个
 */
void lex_next_token (Lex *lex)
{
    int  ch;
    char cstring [4096] = {0};
    while(!lex->finish) {
        //skip  
       
        lex_skip(lex);
        ch = *lex->forward;
        if (ch == LEX_WHATCHER) {
            lex_test(lex);
        }else {
            putchar(ch);
        }
        lex->forward++;
    }
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
    Lex lex = {"ncc.c"};
    lex.buff_size = 1024;
    lex.source_file = f;
    lex_init(&lex);
    while (!lex.finish) {
        lex_next_token(&lex);
    }
    lex_free(&lex);
}
