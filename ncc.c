#include <stdio.h>
#include <malloc.h>

char  msg[200][10]={
    "IMPORT","module",
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
    "COM","COL","SEM","SCOPE", // , : ;

    "ID","CINT","CDOUBLE","CSTRING","CCHAR",
    "UNKNOW",
    "END"
};


enum Token{
    IMPORT,MODULE,
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
    COM,COL,SEM,SCOPE, // , : ;

    ID,CINT,CDOUBLE,CSTRING,CCHAR,
    UNKNOW,
    END
};

void debug(char * msg)
{
    puts(msg);
}

typedef struct
{
    char filename[4096];
    //双缓冲
    char * buff_alpha;
    char * buff_beta;
    
    char * lex_begin;
    char * forward;
    char * back;

    int line;


    char value[4096];
    int wp;
    enum Token tk;
    int buff_size;
    int finish;
    //源文件
    FILE * source_file;
}Lex;
const char LEX_WHATCHER = 0;


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
void lex_init (Lex *lex, char * filename)
{
    int i = 0;
    for (; *(filename+i) != 0;i++) {
        lex->filename[i] = *(filename+i);
    }
    lex->filename[i] = 0;
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


void lex_error(Lex *lex, char *msg)
{
    lex->value[lex->wp] = 0;
    printf("lexing error at line:%d %s\n",lex->line, msg);
    printf("%s...\n",lex->value);
}

/*
 * 词法分析测试 是否完成
 * 切换缓冲区
 */
void lex_test(Lex * lex)
{
    if (lex->source_file == NULL) {
        lex->finish = 1;
        *lex->forward = LEX_WHATCHER;
        return;
    }

    if (lex->forward == lex->buff_alpha+lex->buff_size-1) {
        fill_buff(lex->buff_beta,lex->buff_size,lex->source_file);
        lex->forward = lex->buff_beta; 
        //debug("切换缓冲");
    } else if (lex->forward == lex->buff_beta+lex->buff_size-1) {
        fill_buff(lex->buff_alpha,lex->buff_size,lex->source_file);
        lex->forward =  lex->buff_alpha; 
        //debug("切换缓冲");
    }else {
        lex->finish = 1;
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

int streq(char *s,char *e)
{
    int i = 0;
    while (s[i] != 0 && e[i] != 0) {
        if (s[i] != e[i]) return 0;
        i++;
    }
    return s[i]==e[i];
}

enum Token lex_keyword(Lex *lex)
{

    char * w = lex->value;
    switch(w[0]) {
    case 'f':
        if (streq(w,"float")) {
            return FLOAT;
        }else if(streq(w,"for")) {
            return FOR;
        }
        break;
    case 'b':
        if (streq(w,"break")) {
            return BREAK;
        }
        break;
    case 'r':
        if (streq(w,"return")) {
            return RETURN;
        }
        break;
    case 'l':
        if (streq(w,"long")) {
            return LONG;
        }
        break;
    case 'u':
        if (streq(w,"unsigned")) {
            return UNSIGNED;
        }
        break;
    case 'w':
        if (streq(w,"while")) {
            return WHILE;
        }
        break;
    case 'd':
        if (streq(w,"double")) {
            return DOUBLE;
        }else if (streq(w,"do")) {
            return DO;
        }else if (streq(w,"default")) {
            return DEFAULT;
        }
        break;
    case 'e':
        if (streq(w,"enum")) {
            return ENUM;
        }else if (streq(w,"extern")) {
            return EXTERN;
        }else if (streq(w,"else")) {
            return ELSE;
        }
        break;
    case 'm':
        if (streq(w,"module")) {
            return MODULE;
        }
    case 's':
        if (streq(w,"short")) {
            return SHORT;
        }else if (streq(w,"struct")) {
            return STRUCT;
        }else if (streq(w,"static")) {
            return STATIC;
        }else if (streq(w,"switch")) {
            return SWITCH;
        }
        break;
    case 'v':
        if (streq(w,"void")) {
            return VOID;
        }
        break;
    case 'i':
        if (streq(w,"import")) {
            return IMPORT;
        }else if (streq(w,"if")) {
            return IF;
        }else if (streq(w,"int")) {
            return INT;
        }
        break;
    case 'c':
        if (streq(w,"char")) {
            return CHAR;
        }else if (streq(w,"const")) {
            return CONST;
        }else if (streq(w,"case")) {
            return CASE;
        }else if (streq(w,"continue")) {
            return CONTINUE;
        }
        break;
    }
    
    return ID;
}
/*
 * 下一个
 */
enum Token lex_next_token (Lex *lex)
{
    int  ch;

    lex->wp = 0;
    lex->lex_begin = lex->forward;
    ch = *lex->forward;
    lex->value[lex->wp++] = ch;
    if (ch == LEX_WHATCHER) {
        lex_test(lex);
        if (lex->finish) return END;
        return lex_next_token(lex);
    }else if (ch == ' ') {
        while (ch == ' ') {
            //putchar('\n');
            lex->forward ++;
            ch = *lex->forward;
        }
        return  lex_next_token(lex);
    }else if (ch == '\n') {
        lex->forward++;
        lex->line ++;
        return  lex_next_token(lex);
    }else if(ch == '(') {
        lex->tk  = LPB;
    }else if(ch == ')') {
        lex->tk = RPB;
    }else if(ch == '[') {
        lex->tk = LBK;
    }else if(ch == ']') {
        lex->tk = RBK;
    }else if(ch == '{') {
        lex->tk = LBS;
    }else if(ch == '}') {
        lex->tk = RBS;
    }else if(ch == ',') {
        lex->tk = COM;
    }else if(ch == ';') {
        lex->tk = SEM;
    }else if(ch == '.') {
        lex->tk = DOT;
    }else if(ch == '&') {
        ch = *(++lex->forward);
        lex->value[lex->wp++] = ch;
        if (ch == '=') {
            lex->tk = AAS;
        }else if (ch == '&') {
            lex->tk = AND;
        }else {
            lex->tk = BAND;
            lex->value[lex->wp-1] = 0;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '|') {
        ch = *(++lex->forward);
        lex->value[lex->wp++] = ch;
        if (ch == '=') {
            lex->tk = OAS;
        }else if (ch == '|') {
            lex->tk = OR;
        }else {
            lex->tk = BOR;
            lex->value[lex->wp-1] = 0;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '^') {
        ch = *(++lex->forward);
        lex->value[lex->wp++] = ch;
        if (ch == '=') {
            lex->tk = XAS;
        }else {
            lex->tk = XOR;
            lex->value[lex->wp-1] = 0;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '!') {
        ch = *(++lex->forward);
        lex->value[lex->wp++] = ch;
        if (ch == '=') {
            lex->tk = NEQ;
        }else {
            lex->tk = NOT;
            lex->value[lex->wp-1] = 0;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '=') {
        ch = *(++lex->forward);
        lex->value[lex->wp++] = ch;
        if (ch == '=') {
            lex->tk = EQ;
        }else {
            lex->tk = ASSIGN;
            lex->value[lex->wp-1] = 0;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == ':') {
        ch = *(++lex->forward);
        lex->value[lex->wp++] = ch;
        if (ch == ':') {
            lex->tk = SCOPE;
        }else {
            lex->tk = COL;
            lex->value[lex->wp-1] = 0;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '+') {
        ch = *(++lex->forward);
        lex->value[lex->wp++] = ch;
        if (ch == '=') {
            lex->tk = PAS;
        }else if (ch == '+') {
            lex->tk = SPLUS;
        }else {
            lex->value[lex->wp-1] = 0;
            lex->tk = PLUS;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '-') {
        ch = *(++lex->forward);
        lex->value[lex->wp++] = ch;
        if (ch == '=') {
            lex->tk = SAS;
        }else if (ch == '-') {
            lex->tk = SSUB;
        }else if (ch == '>') {
            lex->tk = ARROW;
        }else {
            lex->value[lex->wp-1] = 0;
            lex->tk = SUB;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '*') {
        ch = *(++lex->forward);
        lex->value[lex->wp++] = ch;
        if (ch == '=') {
            lex->tk = MAS;
        }else {
            lex->value[lex->wp-1] = 0;
            lex->tk = STAR;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '%') {
        ch = *(++lex->forward);
        lex->value[lex->wp++] = ch;
        if (ch == '=') {
            lex->tk = MODAS;
        }else {
            lex->value[lex->wp-1] = 0;
            lex->tk = MOD;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '>') {
        ch = *(++lex->forward);
        lex->value[lex->wp++] = ch;
        if (ch == '=') {
            lex->tk = GEQ;
        }else if (ch == '>') {
            lex->value[lex->wp++] = ch = *(lex->forward+1);
            if (ch == '=') {
                lex->tk = SRAS;
                lex->forward++;
            }else {
                lex->tk = SR;
            }
        }else {
            lex->value[lex->wp-1] = 0;
            lex->tk = GT;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '<') {
        ch = *(++lex->forward);
        lex->value[lex->wp++] = ch;
        if (ch == '=') {
            lex->tk = LEQ;
        }else if (ch == '<') {
            lex->value[lex->wp++] = ch = *(lex->forward+1);
            if (ch == '=') {
                lex->tk = SLAS;
                lex->forward++;
            }else {
                lex->tk = SL;
            }
        }else {
            lex->tk = LT;
            lex->value[lex->wp-1] = 0;
            lex->forward = lex->lex_begin;
        }
    }else if (ch == '/') {
        ch = *(++lex->forward);
        // for //
        if (ch == '/') {
            while (*lex->forward != '\n') {
                if (ch == LEX_WHATCHER) {
                    lex_test(lex);
                    if (lex->finish) {
                        lex->tk = END;
                        break;
                    }
                }
                lex->forward++;
            }
            return lex_next_token(lex);
        }else if (ch == '=') {
            lex->tk = DAS;
            lex->value[lex->wp++] = ch;
        }else if (ch == '*') {
            ch = *(++lex->forward);
            while (ch != '*' || *(lex->forward+1) != '/' ) {
                if (ch == '\n') lex->line++;
                ch = *(++lex->forward);
                if (ch == LEX_WHATCHER) {
                    lex_test(lex);
                    if (lex->finish) { 
                        lex_error(lex,"expected '*/'");
                        lex->tk = END;
                        break;
                    }
                }
            } 
            lex->forward +=2;
            return lex_next_token(lex);
        }else {
            lex->tk = DIV;
            lex->forward = lex->lex_begin;
        }
    }else if (is_digit(ch)) {
        int point = 0;
        lex->wp--;
        while(is_digit(ch) || ch == '.')  {
            lex->value[lex->wp++] = ch;
            if (ch == '.') point ++;
            ch = *(++lex->forward);
            if (ch == LEX_WHATCHER) {
                lex_test(lex);
                if (lex->finish) { 
                    lex->tk = END;
                    break;
                }
            }
        }
        if (point>1) lex_error(lex,"too much .");
        lex->tk = point == 0?CINT:CDOUBLE; 
        --lex->forward;
    }else if (ch == '\'') {
        lex->tk = CCHAR;
        ch = *(++lex->forward);
        while(ch != '\'' || (lex->value[lex->wp-1] == '\\' && lex->value[lex->wp-2] != '\\')) {
            lex->value[lex->wp++] = ch;
            ch = *(++lex->forward);
            if (ch == LEX_WHATCHER) {
                lex_test(lex);
                if (lex->finish)  {
                    lex_error(lex,"expect \'");
                    lex->tk = END;
                    break;
                }
            }
        }
        lex->value[lex->wp++] = ch;

    }else if(ch == '\"') {
        lex->tk = CSTRING;
        ch = *(++lex->forward);
        while(ch != '\"' || (lex->value[lex->wp-1] == '\\' && lex->value[lex->wp-2] != '\\')) {
            lex->value[lex->wp++] = ch;
            ch = *(++lex->forward);
            if (ch == LEX_WHATCHER) {
                lex_test(lex);
                if (lex->finish) {
                    lex_error(lex,"expected \"");
                    lex->tk = END;
                    break;
                }
            }
        }
        lex->value[lex->wp++] = ch;

    }else if (is_alpha(ch) || ch == '_') {
        lex->wp--;
        while(is_alpha(ch) || is_digit(ch) || ch == '_') {
            lex->value[lex->wp++] = ch;
            ch = *(++lex->forward);
            if (ch == LEX_WHATCHER) {
                lex_test(lex);
                if (lex->finish)  {
                    lex->tk = END;
                    break;
                }
            }
        }
        lex->value[lex->wp] = 0;
        lex->tk = lex_keyword(lex);
        --lex->forward;
    }else {
        lex->tk = UNKNOW;
    }
    lex->value[lex->wp] = 0;
    lex->forward ++;
        //putchar(ch);       
    return lex->tk;
}

void lex_set_look_ahead(Lex *lex)
{
    lex->back = lex->forward;
}

void lex_reset_look_ahead(Lex * lex)
{
    lex->forword = lex->back;
}
void lex_free (Lex * lex)
{
    if (lex) {
        free(lex->buff_alpha);
        free(lex->buff_beta);
        fclose(lex->source_file);
    }
}




/*================语法=============*/



void parse_defination(Lex *lex)
{
    lex_next_token(lex);
    if ()
    
}
void parse( char * file)
{
    Lex lex;
    lex.buff_size = 1024;
    lex_init(&lex,file);
    while(!lex.finish) {
        parse_defination(&lex);
    }
    lex_free(&lex);
}
int main (int argc, char ** argv)
{
    if (argc < 2) {
        printf("no input files\n");
        return 0;
    }
    parse(argv[1]);
}
