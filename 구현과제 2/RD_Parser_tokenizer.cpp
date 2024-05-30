#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <sstream>

using namespace std;

// 전역 변수
//tokens는 token list...
vector<string> tokens;
size_t token_pos = 0;
int error_occur = 0;
vector<string> declared_vars;
vector<int> declared_vars_values;
vector<string> outputs;

// 함수 선언
void program();
void declaration();
void statement();
bool bexpr();
int aexpr();
int term();
int number();
string var();
int relop();
void type();

// Error handling function
void syntax_error() {
    error_occur = 1;
}

//배열 내에서 찾으면 1 없으면 0 리턴
int is_declared_var(string var_name) {
    return find(declared_vars.begin(), declared_vars.end(), var_name) != declared_vars.end();
}

//찾은 변수의 값을 return
int get_var_value(string var_name) {
    for (size_t i = 0; i < declared_vars.size(); i++) {
        if (declared_vars[i] == var_name) {
            return declared_vars_values[i];
        }
    }
    return 0; // 변수를 찾지 못한 경우 0으로 초기화된 값 반환
}

//변수 값 초기화
void set_var_value(string var_name, int value) {
    for (size_t i = 0; i < declared_vars.size(); i++) {
        if (declared_vars[i] == var_name) {
            declared_vars_values[i] = value;
            return;
        }
    }
}

// Tokenizer 함수
void tokenize_input(string &input) {
    string token = "";
    for (char c : input) {
        if (c == ' ') {
            if (!token.empty()) {
                tokens.push_back(token);
                token = "";
            }
        } else {
            token += c;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
}

//현재 위치에 있는 토큰을 반환.
string get_token() {
    if (token_pos < tokens.size()) {
        return tokens[token_pos++];
    } else {
        return "";
    }
}

void match_token(const string& expected) {
    string token = get_token();
    if (token != expected) {
        syntax_error();
    }
}

// <program> → {<declaration>} {<statement>}
void program() {
    bool has_declaration = false;
    bool has_statement = false;

    while (token_pos < tokens.size()) {
        string token = get_token();
        if(token == "int") {
            token_pos--;
            declaration();
            if (error_occur == 1) {
                cout << "syntax error!!\n";
                return;
            }
            has_declaration = true;
        }
        else {
            token_pos--;
            break;
        }


    }

    while(token_pos < tokens.size()){
         string token = get_token();
         if(token.empty()) break;
         token_pos--;
         statement();
        if (error_occur == 1) {
            cout << "syntax error!!\n";
            return;
        }
        else if (error_occur == 2) {
            cout << "division error\n";
            return;
        }
         has_statement = true;
    }

    if (!has_declaration && !has_statement) {
        cout << "syntax error!!\n";
        return;
    }

    if (error_occur == 0) {
//        for (const auto& output : outputs) {
//            cout << output << " ";
//        }

        if(!outputs.empty()) {
            for (int i = 0; i < outputs.size() - 1; i++) {
                    cout << outputs[i] << " ";
            }
            cout << outputs[outputs.size() - 1] << "\n";

        }
        else return;
    }

}

// <declaration> → <type> <var> ;
void declaration() {
    type();
    string var_name = var();
    if(error_occur == 1 ) return ;

    declared_vars.push_back(var_name);
    declared_vars_values.push_back(0); //선언시 0으로 값 초기화.

    match_token(";");

}

// <statement> → <var> = <aexpr> ; | print <bexpr> ; | print <aexpr> ; | do '{' {<statement>} '}' while ( <bexpr> ) ;
void statement() {
    string token = get_token();

    if (token == "print") {
        token = get_token();
        token_pos--;
        if (token == "==" || token == "!=" || token == "<" || token == ">" || token == "<=" || token == ">=") {
            bool result = bexpr();
            if (error_occur == 1) return;
            outputs.push_back(result ? "TRUE" : "FALSE");
            match_token(";");
        } else {
            int result = aexpr();
            if (error_occur == 1) return;
            outputs.push_back(to_string(result));
            match_token(";");
        }
    }
    else if (token == "do") {
        match_token("{");
        size_t origin_pos = token_pos;

        do {
            token_pos = origin_pos;
            while (true) {
                string next_token = get_token();
                if (next_token == "}") break;
                token_pos--;
                statement();
                if (error_occur == 1 || error_occur == 2) return;
            }
            match_token("while");
            match_token("(");
            bool result = bexpr();
            if (error_occur == 1) return;
            match_token(")");
            match_token(";");

            if (!result) break;


        }while(true);

    }


    else {
        token_pos --;
        string variable = var();
        if(error_occur == 1 ) return ;
        if (is_declared_var(variable)) {
            match_token("=");

            int value = aexpr();
            if(error_occur == 1 ) return ;
            set_var_value(variable, value);

            match_token(";");
        }
        else {
            syntax_error();
            return;
        }
    }
}

// <bexpr> → <relop> <aexpr> <aexpr>
bool bexpr() {
    int op = relop();
    if(error_occur == 1 ) return false;

    int left = aexpr();
    if(error_occur == 1 ) return false;
    int right = aexpr();
    if(error_occur == 1 ) return false;

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
            syntax_error();
            return false;
    }
}

// <aexpr> → <term> {( + | - | * | / ) <term>}
int aexpr() {
    int result = term();
    if(error_occur == 1 ) return -1;

    while (true) {
        string op = get_token();
        if (op != "+" && op != "-" && op != "*" && op != "/") {
            token_pos--;
            break;
        }

        int right = term();
        if(error_occur == 1 ) return -1;

        if (op == "+") {
            result += right;
        }
        else if (op == "-") {
            result -= right;
        }
        else if (op == "*") {
            result *= right;
        }
        else if (op == "/") {
            if (right != 0) {
                result /= right;
            }
            else {
                error_occur = 2;
                return 0;
            }
        }
    }

    return result;
}

// <term> → <number> | <var> | ( <aexpr> )
int term() {
    string token = get_token();
    int result = 0;

    if (isdigit(token[0])) {
        token_pos--;
        result = number();
        if(error_occur == 1 ) return -1;
    }
    else if (isalpha(token[0])) {
        if(is_declared_var(token)) {
          result = get_var_value(token);
        }else{
            syntax_error();
            return -1;
        }
    }
    else if (token == "(") {
        result = aexpr();
        if(error_occur == 1 ) return -1;
        match_token(")");
    }
    else {
        syntax_error();
        return -1;
    }

    if(error_occur == 1 ) return -1;
    else return result;
}

int number() {
    string token = get_token();
    if(token.length()>10 || token.empty()){
        syntax_error();
        return -1;
    }
    else{
        return stoi(token);
    }
}

string var() {
    string token = get_token();
    if (token.empty() || token.length()>10 || token.find_first_not_of("abcdefghijklmnopqrstuvwxyz") != string::npos) {
        syntax_error();
        return "!INVALID!";
    }
    else return token;
}

int relop() {
    string token = get_token();
    if (token == "==") return 1;
    if (token == "!=") return 2;
    if (token == "<") return 3;
    if (token == ">") return 4;
    if (token == "<=") return 5;
    if (token == ">=") return 6;
    syntax_error();
    return -1;
}

void type() {
    match_token("int");
}

int main() {
    while (true) {
        cout << ">>  ";
        string input;
        getline(cin, input);
        if (input == "terminate") break;

        // 초기화
        tokens.clear();
        token_pos = 0;
        error_occur = 0;
        declared_vars.clear();
        declared_vars_values.clear();
        outputs.clear();

        tokenize_input(input);
        program();
        //cout << "\n";

    }

    return 0;
}
