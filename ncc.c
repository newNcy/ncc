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

    "ID","CINT","CDOUBLE","CSTRING","CCHAR",
    "UNKNOW",
    "END"
};


enum Token{
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

    int line;
    int chpos;

    char value[4096];
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
        lex->forward =  lex->buff_alpha; 
        //debug("切换缓冲");
    }else {
        lex->finish = 1;
    }
}



void lex_skip(Lex *lex)
{
    char ch = *lex->forward;
    if (ch == LEX_WHATCHER) {
        lex_test(lex);
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
enum Token lex_next_token (Lex *lex)
{
    int  ch;

    enum Token tk;
    int wp = 0;
    lex->lex_begin = lex->forward;
    ch = *lex->forward;
    lex->value[wp++] = ch;
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
        tk  = LPB;
    }else if(ch == ')') {
        tk = RPB;
    }else if(ch == '[') {
        tk = RPB;
    }else if(ch == ']') {
        tk = RPB;
    }else if(ch == '{') {
        tk = RPB;
    }else if(ch == '}') {
        tk = RPB;
    }else if(ch == ',') {
        tk = COM;
    }else if(ch == ':') {
        tk = COL;
    }else if(ch == ';') {
        tk = SEM;
    }else if(ch == '.') {
        tk = DOT;
    }else if(ch == '*') {
        tk = STAR;
    }else if(ch == '&') {
        ch = *(++lex->forward);
        lex->value[wp++] = ch;
        if (ch == '=') {
            tk = AAS;
        }if (ch == '&') {
            tk = AND;
        }else {
            tk = BAND;
            lex->value[wp-1] = 0;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '=') {
        ch = *(++lex->forward);
        lex->value[wp++] = ch;
        if (ch == '=') {
            tk = EQ;
        }else {
            tk = ASSIGN;
            lex->value[wp-1] = 0;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '+') {
        ch = *(++lex->forward);
        lex->value[wp++] = ch;
        if (ch == '=') {
            tk = PAS;
        }else {
            lex->value[wp-1] = 0;
            tk = PLUS;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '-') {
        ch = *(++lex->forward);
        lex->value[wp++] = ch;
        if (ch == '=') {
            tk = SAS;
        }else if (ch == '>') {
            tk = ARROW;
        }else {
            lex->value[wp-1] = 0;
            tk = SUB;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '>') {
        ch = *(++lex->forward);
        lex->value[wp++] = ch;
        if (ch == '=') {
            tk = GEQ;
        }else if (ch == '>') {
            tk = SR;
        }else {
            lex->value[wp-1] = 0;
            tk = GT;
            lex->forward = lex->lex_begin;
        }
    }else if(ch == '<') {
        ch = *(++lex->forward);
        lex->value[wp++] = ch;
        if (ch == '=') {
            tk = LEQ;
        }else if (ch == '<') {
            tk = SL;
        }else {
            tk = LT;
            lex->value[wp-1] = 0;
            lex->forward = lex->lex_begin;
        }
    }else if (ch == '/') {
        ch = *(++lex->forward);
        // for //
        if (ch == '/') {
            while (*lex->forward != '\n') {
                if (ch == LEX_WHATCHER) {
                    lex_test(lex);
                    if (lex->finish)  tk = END;
                    break;
                }
                lex->forward++;
            }
            return lex_next_token(lex);
        }else if (ch == '=') {
            tk = DAS;
            lex->value[wp++] = ch;
        }else if (ch == '*') {
            ch = *(++lex->forward);
            while (ch != '*' || *(lex->forward+1) != '/' ) {
                if (ch == '\n') lex->line++;
                ch = *(++lex->forward);
                if (ch == LEX_WHATCHER) {
                    lex_test(lex);
                    if (lex->finish)  tk = END;
                    break;
                }
            } 
            lex->forward +=2;
            return lex_next_token(lex);
        }else {
            tk = DIV;
            lex->forward = lex->lex_begin;
        }
    }else if (is_digit(ch)) {
        int intiger = 1;
        wp--;
        while(is_digit(ch) || ch == '.')  {
            lex->value[wp++] = ch;
            if (ch == '.') intiger --;
            ch = *(++lex->forward);
            if (ch == LEX_WHATCHER) {
                lex_test(lex);
                if (lex->finish)  tk = END;
                break;
            }
        }
        if (intiger < 0) printf("error: too much '.' in a floating number\n");
        tk = intiger == 0?CINT:CDOUBLE; 
        --lex->forward;
    }else if (ch == '\'') {
        tk = CCHAR;
        ch = *(++lex->forward);
        while(ch != '\'' || (lex->value[wp-1] == '\\' && lex->value[wp-2] != '\\')) {
            lex->value[wp++] = ch;
            ch = *(++lex->forward);
            if (ch == LEX_WHATCHER) {
                lex_test(lex);
                if (lex->finish)  tk = END;
                break;
            }
        }
        lex->value[wp++] = ch;

    }else if(ch == '\"') {
        tk = CSTRING;
        ch = *(++lex->forward);
        while(ch != '\"' || (lex->value[wp-1] == '\\' && lex->value[wp-2] != '\\')) {
            lex->value[wp++] = ch;
            ch = *(++lex->forward);
            if (ch == LEX_WHATCHER) {
                lex_test(lex);
                if (lex->finish)  tk = END;
                break;
            }
        }
        lex->value[wp++] = ch;

    }else if (is_alpha(ch) || ch == '_') {
        tk = ID;
        wp--;
        while(is_alpha(ch) || is_digit(ch) || ch == '_') {
            lex->value[wp++] = ch;
            ch = *(++lex->forward);
            if (ch == LEX_WHATCHER) {
                lex_test(lex);
                if (lex->finish)  tk = END;
                break;
            }
        }
        --lex->forward;
    }else {
        tk = UNKNOW;
    }
    lex->value[wp] = 0;
    lex->forward ++;
        //putchar(ch);       
    return tk;
}

void lex_free (Lex * lex)
{
    if (lex) {
        free(lex->buff_alpha);
        free(lex->buff_beta);
        fclose(lex->source_file);
    }
}

int main (int argc, char ** argv)
{

    Lex lex = {"ncc.c"};
    lex.buff_size = 1024;
    lex_init(&lex);
    int line = lex.line;
#ifdef SOURCE_
    printf("1 ");
    while (!lex.finish) {
        enum Token tk = lex_next_token(&lex);
        if (tk == END) break;
        for (int i = line; i < lex.line;i++) {
            printf("\n%d ",i+1);
        }
        printf("%s ",lex.value);
        line = lex.line;
    }
#else 
    while (!lex.finish) {
        enum Token tk = lex_next_token(&lex);
        if (tk == END) break;
        printf("%d <%s,%s> \n",lex.line,msg[tk],lex.value);
        line = lex.line;
    }
#endif
    lex_free(&lex);
}
