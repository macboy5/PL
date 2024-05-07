# Error handling function
def syntax_error():
    global error_occur
    error_occur = 1
    # print("syntax error!!")



# Recursive-Descent Parser 함수들의 구현

def consume_whitespace():
    global pos
    while pos < len(input_str) and input_str[pos].isspace():
        pos += 1


def program():
    global pos
    print(">> ", end='')

    if input_str[-1] != ';':
        print("syntax error!!")
        return

    consume_whitespace()
    while pos < len(input_str):
        if error_occur == 1:
            print("syntax error!!")
            return
        statement()
        consume_whitespace()

    if error_occur == 0:
        print(temp)


def statement():
    global pos, x, y, z, bool_x, bool_y, bool_z, is_bool_x, is_bool_y, is_bool_z, curr_var, temp
    consume_whitespace()
    if input_str[pos:pos + 5] == 'print':
        pos += 5
        consume_whitespace()
        variable = var()
        if error_occur == 1:
            return
        consume_whitespace()
        if input_str[pos] != ';':
            syntax_error()
            return

        # 여기서부터 출력하는 부분.
        if variable == 'x':
            if is_bool_x:
                if bool_x:
                    temp +="TRUE "
                else:
                    temp +="FALSE "
            else:
                temp += str(x) + " "

        elif variable == 'y':
            if is_bool_y:
                if bool_y:
                    temp += "TRUE "
                else:
                    temp += "FALSE "
            else:
                temp += str(y) + " "

        elif variable == 'z':
            if is_bool_z:
                if bool_z:
                    temp += "TRUE "
                else:
                    temp += "FALSE "
            else:
                temp += str(z) + " "

        pos += 1
        return

    else:  # <var> = <expr> 처리
        variable = var()
        curr_var = variable
        consume_whitespace()
        if input_str[pos] == '=':
            pos += 1
            consume_whitespace()

            if variable == 'x':
                x = expr()
                if error_occur == 1:
                    return
                if is_bool_x:
                    if x == 1:
                        bool_x = True
                    elif x == 0:
                        bool_x = False

            elif variable == 'y':
                y = expr()
                if error_occur == 1:
                    return
                if is_bool_y:
                    if y == 1:
                        bool_y = True
                    elif y == 0:
                        bool_y = False

            elif variable == 'z':
                z = expr()
                if error_occur == 1:
                    return
                if is_bool_z:
                    if z == 1:
                        bool_z = True
                    elif z == 0:
                        bool_z = False

            if error_occur == 1:
                return

            consume_whitespace()
            if input_str[pos] != ';':
                syntax_error()
                return
            pos += 1

        else:
            syntax_error()


def expr():  # <expr> --> <bexpr> | <aexpr>
    global pos, curr_var, is_bool_x, is_bool_y, is_bool_z
    consume_whitespace()
    origin_pos = pos
    result = 0

    while pos < len(input_str) and (input_str[pos].isdigit() or input_str[pos] == ' '):
        pos += 1

    if pos < len(input_str) and (input_str[pos] in ['+', '-', '*', '/']):
        pos = origin_pos
        result = aexpr()
        if curr_var == 'x':
            is_bool_x = False
        if curr_var == 'y':
            is_bool_y = False
        if curr_var == 'z':
            is_bool_z = False
        return result

    elif pos < len(input_str) and (input_str[pos:pos + 2] in ['==', '!=', '< ', '> ', '>=', '<=']):
        pos = origin_pos
        result = bexpr()
        if curr_var == 'x':
            is_bool_x = True
        if curr_var == 'y':
            is_bool_y = True
        if curr_var == 'z':
            is_bool_z = True
        return result

    else:
        syntax_error()
        return 0


def bexpr():
    global pos
    consume_whitespace()
    left = number()
    consume_whitespace()
    op = relop()
    consume_whitespace()
    right = number()

    if error_occur == 1:
        return False

    if op == 1:
        return left == right
    elif op == 2:
        return left != right
    elif op == 3:
        return left < right
    elif op == 4:
        return left > right
    elif op == 5:
        return left <= right
    elif op == 6:
        return  left>= right
    else:
        return False


def aexpr():  # <aexpr> → <term> {( + | - ) <term>}
    global pos
    consume_whitespace()
    result = term()
    consume_whitespace()
    while pos < len(input_str) and (input_str[pos] == '+' or input_str[pos] == '-'):
        op = input_str[pos]
        pos += 1
        consume_whitespace()
        if op == '+':
            result += term()
            if error_occur == 1:
                return 0
        elif op == '-':
            result -= term()
            if error_occur == 1:
                return 0
        consume_whitespace()

    if error_occur == 1:
        return 0
    consume_whitespace()
    return int(result)


def term():  # <term> → <factor> {( * | / ) <factor>}
    global pos
    consume_whitespace()
    result = factor()
    if error_occur == 1:
        return 0
    consume_whitespace()
    while pos < len(input_str) and (input_str[pos] == '*' or input_str[pos] == '/'):
        op = input_str[pos]
        pos += 1
        consume_whitespace()
        if op == '*':
            result *= factor()
            if error_occur == 1:
                return 0
        elif op == '/':
            div_result = factor()
            if error_occur == 1:
                return 0
            if div_result != 0:
                result /= div_result
            else:
                syntax_error()
                return 0
        consume_whitespace()

    consume_whitespace()
    return result


def factor():  # <factor> → <number>
    return number()


def number():  # <number> → <dec> {<dec>}
    global pos
    result = dec()
    while pos < len(input_str) and input_str[pos].isdigit():
        result = result * 10 + dec()
    if error_occur == 1:
        return 0
    return result


def dec():
    global pos
    if pos < len(input_str) and input_str[pos].isdigit():
        digit = int(input_str[pos])  # 문자를 숫자로 변환
        pos += 1
        return digit
    else:
        syntax_error()
        return 0


def var():
    global pos
    if pos < len(input_str) and input_str[pos] in ['x', 'y', 'z']:
        pos += 1
        return input_str[pos - 1]
    else:
        syntax_error()
        return 0


def relop():
    global pos
    if pos < len(input_str):
        if input_str[pos:pos + 2] == '==':
            pos += 2
            return 1
        elif input_str[pos:pos + 2] == '!=':
            pos += 2
            return 2
        elif input_str[pos] == '<' and input_str[pos + 1] == ' ':
            pos += 1
            return 3
        elif input_str[pos] == '>' and input_str[pos + 1] == ' ':
            pos += 1
            return 4
        elif input_str[pos:pos + 2] == '<=':
            pos += 2
            return 5
        elif input_str[pos:pos + 2] == '>=':
            pos += 2
            return 6
        else:
            syntax_error()
            return 0
    else:
        syntax_error()
        return 0


    pos = 0
    x, y, z = 0, 0, 0
    bool_x, bool_y, bool_z = False, False, False
    is_bool_x, is_bool_y, is_bool_z = False, False, False
    error_occur = 0
    curr_var = 'Q'
while True:
    input_str = input(">> ")
    if input_str == "terminate":
        break

    # 초기화
    pos = 0
    x, y, z = 0, 0, 0
    bool_x, bool_y, bool_z = False, False, False
    is_bool_x, is_bool_y, is_bool_z = False, False, False
    error_occur = 0
    curr_var = 'Q'
    temp = ""

    program()
