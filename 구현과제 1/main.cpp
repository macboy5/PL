#include <iostream>
#include <string>
#include <cstdlib>


using namespace std;

// 전역 변수
string input;
size_t pos = 0;
int x=0, y=0,z=0;
bool bool_x=false, bool_y=false, bool_z=false;
bool is_bool_x=false, is_bool_y=false, is_bool_z=false;
int error_occur=0;
char curr_var;
string temp;

// 함수 선언
void program();
void statement();
int expr();
bool bexpr();
int aexpr();
int term();
int factor();
int number();
int dec();
char var();
int relop();


// Error handling function

void syntax_error() {
//    cout << "syntax error!!";
    error_occur = 1;
//    // Clear input buffer
//    while (getchar() != '\n');
}


// isspace() : 공백이거나 개행문자 등이 들어가면 0이 아닌 수 반환.
void consume_whitespace() {
    while (isspace(input[pos])) {
        pos++;
        if (pos >= input.length()) break; // 입력의 끝에 도달한 경우 종료
    }
}

// Recursive-Descent Parser 함수들의 구현
void program() {
    cout <<">> ";

    if(input[input.length()-1] != ';') {
        cout << "syntax error!!";
        return ;
    }

    consume_whitespace();
    while (pos < input.length() ) { // 문자열의 끝에 도달할 때까지 반복
        if(error_occur == 1) {
            cout <<"syntax error!!";
            return;
        }
        statement();
        consume_whitespace();
    }


    if(error_occur == 0) cout << temp ;

}

void statement() {
    consume_whitespace();
    // print <var>;의 경우를 처리.
    if(input[pos] == 'p' && input[pos + 1] == 'r' && input[pos + 2] == 'i' && input[pos + 3] == 'n' && input[pos + 4] == 't' ){
        pos+=5;
        consume_whitespace();
        char variable = var();
        if(error_occur == 1) return;
        consume_whitespace();
        if(input[pos] != ';'){
            syntax_error();
            return;
        }

        //여기서부터 출력하는 부분.
        switch (variable) {
            case 'x':
                if(is_bool_x){
                    if(bool_x) temp.append("TRUE ");
                    else temp.append("FALSE ");
                }
                else temp.append(to_string(x) +" ");

                break;
            case 'y':
                if (is_bool_y){
                    if(bool_y) temp.append("TRUE ");
                    else temp.append("FALSE ");;
                }
                else temp.append(to_string(y) +" ");
                break;
            case 'z':
                if (is_bool_z){
                    if(bool_z) temp.append("TRUE ");
                    else temp.append("FALSE ");;
                }
                else temp.append(to_string(z) +" ");
                break;
        }
        pos++;
        return;

    }
    else{ //<var> = <expr> ; 처리
        char variable =var();
        curr_var = variable;
//        if(error_occur == 1) return;
        consume_whitespace();
        if(input[pos] == '='){
            pos++;
            consume_whitespace();

            switch (variable) {
                case 'x' :
                    x = expr();
                    if(error_occur == 1) return;
                    if(is_bool_x){
                        if(x==1) bool_x = true;
                        else if(x==0) bool_x = false;
                    }
                    break;
                case 'y' :
                    y = expr();
                    if(error_occur == 1) return;
                    if(is_bool_y){
                        if(y == 1) bool_y = true;
                        else if(y == 0) bool_y = false;
                    }
                    break;
                case 'z' :
                    z = expr();
                    if(error_occur == 1) return;
                    if(is_bool_z){
                        if(z == 1) bool_z = true;
                        else if(z == 0) bool_z = false;
                    }

                    break;
            }

            if(error_occur == 1) return;

            consume_whitespace();
            if(input[pos] != ';'){
                syntax_error();
                return;
            }
            pos++;
        }
        else{
            syntax_error();
            return;
        }

    }

}

int expr() { // <expr> --> <bexpr> | <aexpr>
    consume_whitespace();

    int origin_pos = pos;
    int result = 0;

    // <relop> 혹은 operator가 나올 때 까지 이동.
    //그러니까 숫자이거나 빈칸이면 계속 진행하다가 relop op만나면 탈출..
    while( ( isdigit(input[pos]) || input[pos] == ' ')  && pos<input.length()) {
        pos++;
    }

    if(input[pos] == '+' && input[pos+1] ==' ' ||
       input[pos] == '-' && input[pos+1] ==' '||
       input[pos] == '*' && input[pos+1] ==' ' ||
       input[pos] == '/' && input[pos+1] ==' '){
        pos = origin_pos;
        result = aexpr();
        if(curr_var == 'x') is_bool_x = false;
        if(curr_var == 'y') is_bool_y = false;
        if(curr_var == 'z') is_bool_z = false;
        return result;
    }
    else if(input[pos] == '=' && input[pos+1] == '=' ||
            input[pos] == '!' && input[pos+1] == '=' ||
            input[pos] == '<' && input[pos+1] == ' ' ||
            input[pos] == '>' && input[pos+1] == ' ' ||
            input[pos] == '>' && input[pos+1] == '=' ||
            input[pos] == '<' && input[pos+1] == '='  ){
        pos = origin_pos;
        result = bexpr();
        if(curr_var == 'x') is_bool_x = true;
        if(curr_var == 'y') is_bool_y = true;
        if(curr_var == 'z') is_bool_z = true;
        return result;

    }
    else{
        syntax_error();
        return 0;
    }
}


bool bexpr() {
    consume_whitespace();
    int left = number();
//    if(error_occur == 1) return 0;
    consume_whitespace();
    int op = relop();
    consume_whitespace();
    int right = number();

    if(error_occur == 1) return 0;

    switch (op) {
        case 1:
            return left == right;
        case 2:
            return left != right;
        case 3:
            return left < right;
        case 4:
            return left > right;
        case 5:
            return left <= right;
        case 6:
            return left >= right;
        default:
            return false;
    }

}

int aexpr() { //<aexpr> → <term> {( + | - ) <term>}
    consume_whitespace();
    int result = term();
//    if (error_occur == 1) return 0;
    consume_whitespace();
    while (input[pos] == '+' || input[pos] == '-') {
        char op = input[pos];
        pos++;
        consume_whitespace();
        if(op == '+'){
            result += term();
            if (error_occur == 1) return 0;
        }
        else if(op == '-'){
            result -= term();
            if (error_occur == 1) return 0;
        }

        consume_whitespace();
    }

    if(error_occur == 1) return 0;
    consume_whitespace();
    return result;
}

int term() { //<term> → <factor> {( * | / ) <factor>}
    consume_whitespace();
    int result = factor();
    if(error_occur == 1) return 0;
    consume_whitespace();
    while (input[pos] == '*' || input[pos] == '/') {
        char op = input[pos];
        pos++;
        consume_whitespace();
        if(op == '*'){
            result *= factor();
            if(error_occur == 1) return 0;
        }
        else if(op == '/'){
            int div = factor();
            if(error_occur == 1) return 0;
            if(div !=0){
                result /= div;
            }
            else{
                syntax_error();
                return 0;
            }
        }

        consume_whitespace();
    }
//    if(error_occur == 1) return 0;
    consume_whitespace();
    return result;
}

int factor() { //<factor> → <number>
    return number();
}

int number() { //<number> → <dec> {<dec>}
    int result = dec();
//    if(error_occur == 1) return 0;
    while(isdigit(input[pos])){
        result = result * 10 + dec();
//        if(error_occur == 1) return 0;
    }
    if(error_occur == 1) return 0;
    return result;
}

int dec() {
    if (isdigit(input[pos])) {
        int digit = input[pos] - '0'; // 수정: 문자를 숫자로 변환
        pos++;
        return digit;
    }
    else {
        syntax_error();
    }
    return 0;
}

char var() {
    if(input[pos] == 'x' || input[pos] == 'y' || input[pos] == 'z') {
        return input[pos++];
    }
    else {
        syntax_error();
        return 0;
    }
}

int relop() {
    if (input[pos] == '=' && input[pos + 1] == '=') {
        pos += 2;
        return 1;
    }
    else if (input[pos] == '!' && input[pos + 1] == '=') {
        pos += 2;
        return 2;
    }
    else if (input[pos] == '<') {
        if (input[pos + 1] == '=') {
            pos += 2;
            return 5;
        }
        else if(input[pos+1] == ' '){
            pos++;
            return 3;
        }
        else{
            syntax_error();
            return 0;
        }
    }
    else if (input[pos] == '>') {
        if (input[pos + 1] == '=') {
            pos += 2;
            return 6;
        }
        else if(input[pos+1] == ' '){
            pos++;
            return 4;
        }
        else{
            syntax_error();
            return 0;
        }
    }
    else {
        syntax_error();
        return 0;
    }
}


int main() {
    while (true) {
        cout << ">> ";
        getline(cin, input);
        if (input == "terminate") break;

        //초기화.
        pos = 0;
        x=0, y=0,z=0;
        bool_x=false, bool_y=false, bool_z=false;
        is_bool_x=false, is_bool_y=false, is_bool_z=false;
        error_occur=0;
        curr_var='Q';
        temp ="";

        program();
        cout <<"\n";
    }

    return 0;
}
