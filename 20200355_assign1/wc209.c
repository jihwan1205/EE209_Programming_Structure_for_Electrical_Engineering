#include<stdio.h>
#include<ctype.h>
#include<assert.h>

enum DFAState {DEFAULT, WORD, SLASH, COMMENT1, COMMENT2, COMMENT2STAR};
int line=0, word=0, character=0; // count number of newline, word, character each
int errorline; // where error occurs in COMMENT2
int wordstate=0; // if true, reduce word 

enum DFAState state = DEFAULT;  //  initial state

void func_default(char c){
    if(c=='/'){ // input is slash
        state = SLASH;
        character++;
        word++;
        wordstate = 1; // true in order to subtract word if comment starts
    }
    else if(isspace(c)){ // input is whitespace
        character++;
        if(c=='\n') line++;
    }
    else{ // input is nonspace
        state = WORD;
        character++;
        word++;
    }
}
void func_word(char c){
    if(c=='/'){ // input is slash
        state = SLASH;
        character++;
    }
    else if(isspace(c)){ // input is whitespace
        state = DEFAULT;
        character++;
        if(c=='\n') line++;
    }
    else{ // input is nonspace
        character++;
    }
}
void func_slash(char c){
    if(c=='*'){ // start slash asterix comment
        state = COMMENT2;
        character--;
        errorline = line;
        if(wordstate==1){
                word--; // do not count word because comment starts
                wordstate = 0; // back to default
        }
    }
    else if(c=='/'){ // start slash slash comment
        state = COMMENT1;
        character--;
        if(wordstate==1){
            word--; // do not count as word because comment starts
            wordstate = 0; // back to default
        }
    }
    else if(isspace(c)){ // input is whitespace
        state = DEFAULT;
        character++;
        if(c=='\n') line++;
    }
    else{ // input is nonspace
        state = WORD;
        character++;
    }
}
void func_comment1(char c){
    if(c=='\n'){ // end comment
        state = DEFAULT;
        line++;
        character++;
    }
    else state = COMMENT1; 
}
void func_comment2(char c){
    if(c=='*') state = COMMENT2STAR; // closes comment if next character is slash
    else{
        if(c=='\n') { // input is newline
            line++;
            character++;
        }
    }
}
void func_comment2star(char c){
    if(c=='/'){ // close comment and count comment as space
        state = DEFAULT;
        character++; 
    }
    else{ // still in comment
        state = COMMENT2;
        if(c=='\n'){ // input is newline
            line++;
            character++;
        }
    }
}
int main(void){
    int c; // character from stdin    
    while((c=getchar())!=EOF){
        switch(state){
            case DEFAULT: // outside word, slash, comment
                func_default(c);
                break;
            case WORD: // inside word not in comment
                func_word(c);
                break;
            case SLASH: // character before is slash
                func_slash(c);
                break;
            case COMMENT1: // slash slash comment
                func_comment1(c);
                break;
            case COMMENT2: // slash asterix comment
                func_comment2(c);
                break;
            case COMMENT2STAR: // asterix comes after start of COMMENT2
                func_comment2star(c);
                break;
            default: // defensive coding
                assert(0);
                break;
        }
    }
    if(state==DEFAULT||state==WORD||state==SLASH||state==COMMENT1) 
        fprintf(stdout, "%d %d %d\n",line, word, character); // print to stdout
    else if(state==COMMENT2||state==COMMENT2STAR) // slash asterix comment not finished
        fprintf(stderr, "Error: line %d: unterminated comment\n",errorline+1); // print to stderr
    return 0;
}