#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define TableLength 30
#define NUMKEYWORDS 10
int icounter = 1;
//enum tokenType{ EOFILE = -1, LESS_THAN, LESS_THAN_OR_EQUAL,GREATER_THAN,GREATER_THAN_OR_EQUAL,EQUAL,NOT_EQUAL,Int,Char,Float,FUNC,SL};
struct token{
	char lexeme[20];
	int index;
	unsigned int rowno,colno;
	char type[20];
	//int num_args;
	//struct token args;
	char returnType[20];
	//char scope;
};
char nexttype[20];
struct ListElement{
	struct token tok;
	struct ListElement *next;
};
struct ListElement *TABLE[TableLength];
void Initialize(){
	for(int i = 0;i<TableLength;i++){
		TABLE[i] = NULL;
	}
}
int HASH(char *str){
	int sum = 0;
	for(int i = 0;i<strlen(str);i++){
		sum = sum+str[i];
	}
	return (sum%TableLength);
}
struct ListElement* SEARCH(char *str){
	int val = HASH(str);
	if(TABLE[val] == NULL){
		return NULL;
	}
	else{
		struct ListElement * ele = TABLE[val];
		while(ele!= NULL){
			if(strcmp(ele->tok.lexeme,str) == 0){
				return ele;
			}
			ele = ele->next;
		}
		return NULL;
	}
}
void INSERT(struct token tk){
	if(SEARCH(tk.lexeme) != NULL){
		return;
	}
	int val = HASH(tk.lexeme);
	struct ListElement* cur = (struct ListElement*)malloc(sizeof(struct ListElement));
	cur->tok = tk;
	cur->next = NULL;
	if(TABLE[val] == NULL){
		TABLE[val] = cur; //no collision
	}
	else{
		struct ListElement * ele = TABLE[val];
		while(ele->next != NULL){
			ele = ele->next;
		}
		ele->next = cur;
	}
}
void DISPLAY(){
	printf("Index\tName\tType\tR_Type");
	for(int i = 0;i<TableLength;i++){
		struct ListElement * ele = TABLE[i];
		while(ele != NULL){
			printf("%d\t%s\t%s\t%s\n",ele->tok.index,ele->tok.lexeme,ele->tok.type,ele->tok.returnType);
			ele = ele->next;
		}
	}
}
void readFileComplete(FILE *fa, FILE* fb);
int currState(int ch,FILE *fa,FILE *fb);
int spacehandle(char delim,FILE *fa);
int multi_comm_handle(FILE *fa);
int single_comm_handle(FILE *fa);
int newline_handle(FILE *fa,FILE *fb);
int quotes_handle(int ch,FILE *fa,FILE *fb);
int normal_cases_handle(int ch,FILE *fa,FILE *fb);
int check_operator(int op);
int operator_handle(int ch,FILE *fa,FILE *fb);
int word_handle(int ch,FILE *fa,FILE *fb);
int SS_handle(int ch,FILE *fa,FILE *fb);
int numeric_handle(int ch,FILE *fa,FILE *fb);

int LINE = 0;
int COL = 0;

int main(){
	char a[5] = "hi";
    FILE *fa,*fb;
    fa = fopen("fact_test.c","r");
    fb = fopen("out.c","w");
    Initialize();
    if(fa == NULL){
        printf("error opening file\n");
        exit(0);
    }
    readFileComplete(fa,fb);
    DISPLAY();
}

int currState(int ch,FILE *fa,FILE *fb){
    //Used for ignoring quotes, comments, spaces etc.
    while(ch == ' ' || ch == '\t' || ch == '\n' || ch == '/' || ch == '"'){
        if (ch == ' '){
            ch = spacehandle(' ',fa);
        }
        else if(ch == '\t'){
            //printf("tab scanned\n");
            ch = spacehandle('\t',fa);
        }
        else if(ch == '/'){
            ch = getc(fa);
            COL++;
            if(ch == '*'){
                ch = multi_comm_handle(fa);
            }
            else if(ch == '/'){
                ch = single_comm_handle(fa);
            }
            else{
                normal_cases_handle(ch,fa,fb);
            }
        }
        else if(ch == '\n'){
            ch = newline_handle(fa,fb);
        }
        else if(ch == '"'){
            ch = quotes_handle(ch,fa,fb);
        }
    }
    ch = normal_cases_handle(ch,fa,fb);
    return ch;
}

void readFileComplete(FILE *fa,FILE *fb){
    //Used for reading all characters till end of file
    int ch = getc(fa);
    COL++;
    while(ch != EOF){
        ch = currState(ch,fa,fb);
    }
}

int spacehandle(char delim,FILE *fa){
    //handles COL value for spaces
    int tem;
    do{
        if(tem == '\t'){
            COL = COL + 4;
        }
        else{
            COL++;
        }
        tem = getc(fa);
    }
    while(tem == delim);
    return tem;
}

int multi_comm_handle(FILE *fa){
    //handles multi line comments
    int tem;
    while(1){
        do{
            tem = getc(fa);
            COL++;
        }
        while(tem != '*');
        tem = getc(fa);
        COL++;
        if(tem == '/'){
            tem = getc(fa);
            COL++;
            return tem;
        }
    }
}

int single_comm_handle(FILE *fa){
    //handles single line comments
    int tem;
    do{
        tem = getc(fa);
        COL++;
    }
    while(tem != '\n');
    COL = 0;
    LINE++;
    tem = getc(fa);
    COL++;
    return tem;
}

int newline_handle(FILE *fa,FILE *fb){
    //handles single or multiple newline characters
    int tem;
    do{
        tem = getc(fa);
        COL = 0;
        LINE++;
    }
    while(tem == '\n');
    fprintf(fb, "\n");
    return tem;
}

int quotes_handle(int ch,FILE *fa,FILE *fb){
    //used for ignoring quotes
    /*int tem;
    do{
        tem = getc(fa);
        COL++;
    }
    while(tem != '"');*/
    int tem;
    char str[50] = "";
    char temp[2];
    temp[1] = '\0';
    temp[0] = ch;
    do{
        strcat(str,temp);
        temp[0] = getc(fa);
        COL++;
    }while(temp[0] != '"');
    fprintf(fb,"<%s,SL> ",str);
    fprintf(stdout,"<%s,SL> ",str);
    printf("<%s,SL> ",str);
    tem = getc(fa);
    return tem;
}

int normal_cases_handle(int ch,FILE *fa,FILE *fb){
    //used for regular lexical analysis and detecting lexemes
    int rch;
    if (check_operator(ch)){
        rch = operator_handle(ch,fa,fb);
    }
    else if((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122) || ch == '_'){
        rch = word_handle(ch,fa,fb);
    }
    else if(ch >= 48 && ch <= 57){
        rch = numeric_handle(ch,fa,fb);
    }
    else{
        rch = SS_handle(ch,fa,fb);
    }
    return rch;
}

int check_operator(int op){
    //checks if its and operator
    if(op == '<' || op == '+' || op == '>' || op == '=' || op == '*' || op == '/' || op == '&' || op == '%' || op == '-' || op == '|' || op == '!'){
        return 1;
    }
    return 0;
}

int numeric_handle(int ch,FILE *fa,FILE *fb){
    //handles numeric constants
    char str[20] = "";
    char temp[2];
    temp[1] = '\0';
    temp[0] = ch;
    do{
        strcat(str,temp);
        temp[0] = getc(fa);
        COL++;
    }
    while((temp[0] >= 48 && temp[0] <= 57));
    fprintf(fb,"<%s,NC> ",str);
    fprintf(stdout,"<%s,NC> ",str);
    printf("<%s,NC> ",str);
    //printf("NUMBER CONST %s ROW %d COL %d\n",str,LINE,COL);
    return temp[0];
}

int operator_handle(int ch,FILE *fa,FILE *fb){
    //handles operators
    char str[5] = "";
    char temp[2];
    temp[1] = '\0';
    temp[0] = ch;
    strcat(str,temp);
    temp[0] = (char)getc(fa);
    if(check_operator(temp[0])){
        strcat(str,temp);
        temp[0] = getc(fa);
        COL++;
    }
    fprintf(fb,"<%s,OP> ",str);
    fprintf(stdout,"<%s,OP> ",str);
    printf("<%s,OP> ",str);
    //printf("OPERATOR: %s ROW:%d COL:%ld\n",str,LINE,COL-strlen(str));
    return temp[0];
}

int word_handle(int ch,FILE *fa,FILE *fb){
    //handles keywords and identifiers
    char str[20] = "";
    char temp[2];
    temp[1] = '\0';
    temp[0] = ch;
    do{
        strcat(str,temp);
        temp[0] = getc(fa);
        COL++;
    }
    while((temp[0] >= 65 && temp[0] <= 90) || (temp[0] >= 97 && temp[0] <= 122) || temp[0] == '_' || (temp[0] >=48 && temp[0] <=57));
    char kwds[NUMKEYWORDS][10] = {"return","if","else","for","while","int","float","char","break","do"};
    int i;
    for(i = 0;i < NUMKEYWORDS;i++){
        if(!strcmp(kwds[i],str)){
            fprintf(fb,"<%s,KW> ",str);
            fprintf(stdout,"<%s,KW> ",str);
            printf("<%s,KW> ",str);
            if(!strcmp(str,"int") || !strcmp(str,"float") || !strcmp(str,"char")){
            	strcpy(nexttype,str);
            }
            //printf("KEYWORD %s ROW %d COL %ld\n",str,LINE,COL-strlen(str));
            break;
        }
    }
    /*
    char *lexeme;
	int index;
	unsigned int rowno,colno;
	enum tokenType type;
	int num_args;
	struct token args;
	enum tokenType returnType;
	char scope;
	*/
    if(i == NUMKEYWORDS){
    	struct ListElement* id = SEARCH(str);
    	if(id!=NULL){
    		fprintf(fb,"<%s,%d,ID> ",id->tok.lexeme,id->tok.index);
    		fprintf(stdout,"<%s,%d,ID> ",id->tok.lexeme,id->tok.index);
            printf("<%s,%d,ID> ",id->tok.lexeme,id->tok.index);
    	}
    	else{
    		struct token newid;
    		strcpy(newid.lexeme,str);
    		newid.index = icounter;
    		icounter++;
    		newid.rowno = LINE;
    		newid.colno = COL-strlen(str);
    		if(temp[0] == '('){
    			strcpy(newid.type,"FUNC");
    			strcpy(newid.returnType,nexttype);
    		}
    		else{
    			strcpy(newid.type,nexttype);
    			strcpy(newid.returnType,"NO_VAL");
    		}
    		INSERT(newid);
    		fprintf(fb,"<%s,%d,ID> ",newid.lexeme,newid.index);
    		fprintf(stdout,"<%s,%d,ID> ",newid.lexeme,newid.index);
            printf("<%s,%d,ID> ",newid.lexeme,newid.index);
    	}
        //printf("IDENTIFIER %s ROW %d COL %ld\n",str,LINE,COL-strlen(str));
    }
    return temp[0];
}

int SS_handle(int ch,FILE *fa,FILE *fb){
    //handles special symbols
    char temp;
    if(ch == '#'){
        do{
            temp = getc(fa);
            COL++;
        }
        while(temp != '\n');
        LINE++;
        COL = 0;
        temp = getc(fa);
        return temp;
    }
    else if(ch == '(' || ch == ')' || ch == ',' || ch == ';' || ch == '{' || ch == '}'){
        fprintf(fb,"<%c> ",ch);
        printf("<%c> ",ch);
        //printf("SPECIAL %c ROW %d COL %d\n",ch,LINE,COL);
        temp = getc(fa);
        COL++;
        return temp;
    }
}