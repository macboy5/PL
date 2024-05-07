import java.util.Scanner;


public class Main {
    static String input;
    static int pos = 0;
    static int x = 0, y = 0, z = 0;
    static boolean bool_x = false, bool_y = false, bool_z = false;
    static boolean is_bool_x = false, is_bool_y = false, is_bool_z = false;
    static int error_occur = 0;
    static char curr_var;
    static StringBuilder temp = new StringBuilder();

    static void syntax_error() {
        error_occur = 1;
    }

    static void program() {
        System.out.print(">> ");

        if (!input.endsWith(";")) {
            System.out.print("syntax error!!");
            return;
        }

        consume_whitespace();
        while (pos < input.length()) {
            if (error_occur == 1) {
                System.out.print("syntax error!!");
                return;
            }
            statement();
            consume_whitespace();
        }

        if (error_occur == 0) {
            System.out.print(temp);
        }

    }

    static void statement() {
        consume_whitespace();
        if (input.charAt(pos) == 'p' && input.charAt(pos + 1) == 'r' && input.charAt(pos + 2) == 'i' &&
                input.charAt(pos + 3) == 'n' && input.charAt(pos + 4) == 't') {
            pos += 5;
            consume_whitespace();
            char variable = var();
            if (error_occur == 1) return;
            consume_whitespace();
            if (input.charAt(pos) != ';') {
                syntax_error();
                return;
            }

            switch (variable) {
                case 'x':
                    if (is_bool_x) {
                        if (bool_x) temp.append("TRUE ");
                        else temp.append("FALSE ");
                    } else temp.append(x).append(" ");
                    break;
                case 'y':
                    if (is_bool_y) {
                        if (bool_y) temp.append("TRUE ");
                        else temp.append("FALSE ");
                    } else temp.append(y).append(" ");
                    break;
                case 'z':
                    if (is_bool_z) {
                        if (bool_z) temp.append("TRUE ");
                        else temp.append("FALSE ");
                    } else temp.append(z).append(" ");
                    break;
            }
            pos++;
        } else {
            char variable = var();
            curr_var = variable;
            consume_whitespace();
            if (input.charAt(pos) == '=') {
                pos++;
                consume_whitespace();
                switch (variable) {
                    case 'x':
                        x = expr();
                        if (error_occur == 1) return;
                        if (is_bool_x) {
                            if (x == 1) bool_x = true;
                            else if (x == 0) bool_x = false;
                        }
                        break;
                    case 'y':
                        y = expr();
                        if (error_occur == 1) return;
                        if (is_bool_y) {
                            if (y == 1) bool_y = true;
                            else if (y == 0) bool_y = false;
                        }
                        break;
                    case 'z':
                        z = expr();
                        if (error_occur == 1) return;
                        if (is_bool_z) {
                            if (z == 1) bool_z = true;
                            else if (z == 0) bool_z = false;
                        }
                        break;
                }

                if (error_occur == 1) return;

                consume_whitespace();
                if (input.charAt(pos) != ';') {
                    syntax_error();
                    return;
                }
                pos++;
            } else {
                syntax_error();
            }
        }
    }

    static int expr() {
        consume_whitespace();
        int origin_pos = pos;
        int result = 0;

        while ((Character.isDigit(input.charAt(pos)) || input.charAt(pos) == ' ') && pos < input.length()) {
            pos++;
        }

        if ((input.charAt(pos) == '+' && input.charAt(pos + 1) == ' ') ||
                (input.charAt(pos) == '-' && input.charAt(pos + 1) == ' ') ||
                (input.charAt(pos) == '*' && input.charAt(pos + 1) == ' ') ||
                (input.charAt(pos) == '/' && input.charAt(pos + 1) == ' ')) {
            pos = origin_pos;
            result = aexpr();
            if (curr_var == 'x') is_bool_x = false;
            if (curr_var == 'y') is_bool_y = false;
            if (curr_var == 'z') is_bool_z = false;
            return result;
        }
        else if ((input.charAt(pos) == '=' && input.charAt(pos + 1) == '=') ||
                (input.charAt(pos) == '!' && input.charAt(pos + 1) == '=') ||
                (input.charAt(pos) == '<' && input.charAt(pos + 1) == ' ') ||
                (input.charAt(pos) == '>' && input.charAt(pos + 1) == ' ') ||
                (input.charAt(pos) == '>' && input.charAt(pos + 1) == '=') ||
                (input.charAt(pos) == '<' && input.charAt(pos + 1) == '=')) {
            pos = origin_pos;
            result = bexpr();
            if (curr_var == 'x') is_bool_x = true;
            if (curr_var == 'y') is_bool_y = true;
            if (curr_var == 'z') is_bool_z = true;
            return result;
        } else {
            syntax_error();
            return 0;
        }
    }

    static int bexpr() {
        consume_whitespace();
        int left = number();
        consume_whitespace();
        int op = relop();
        consume_whitespace();
        int right = number();

        if (error_occur == 1) return 0;

        switch (op) {
            case 1:
                if(left == right ) return 1;
                else return 0;
            case 2:
                if(left != right ) return 1;
                else return 0;
            case 3:
                if(left < right ) return 1;
                else return 0;
            case 4:
                if(left > right ) return 1;
                else return 0;
            case 5:
                if(left <= right ) return 1;
                else return 0;
            case 6:
                if(left >= right ) return 1;
                else return 0;
            default:
                return 0;
        }
    }

    static int aexpr() {
        consume_whitespace();
        int result = term();
        consume_whitespace();
        while (input.charAt(pos) == '+' || input.charAt(pos) == '-') {
            char op = input.charAt(pos);
            pos++;
            consume_whitespace();
            if (op == '+') {
                result += term();
                if (error_occur == 1) return 0;
            } else if (op == '-') {
                result -= term();
                if (error_occur == 1) return 0;
            }
        }
        if (error_occur == 1) return 0;
        consume_whitespace();
        return result;
    }

    static int term() {
        consume_whitespace();
        int result = factor();
        if (error_occur == 1) return 0;
        consume_whitespace();
        while (input.charAt(pos) == '*' || input.charAt(pos) == '/') {
            char op = input.charAt(pos);
            pos++;
            consume_whitespace();
            if (op == '*') {
                result *= factor();
                if (error_occur == 1) return 0;
            } else if (op == '/') {
                int div = factor();
                if (error_occur == 1) return 0;
                if (div != 0) {
                    result /= div;
                } else {
                    syntax_error();
                    return 0;
                }
            }
            consume_whitespace();
        }
        consume_whitespace();
        return result;
    }

    static int factor() {
        return number();
    }

    static int number() {
        int result = dec();
        while (Character.isDigit(input.charAt(pos))) {
            result = result * 10 + dec();
        }
        if (error_occur == 1) return 0;
        return result;
    }

    static int dec() {
        if (Character.isDigit(input.charAt(pos))) {
            int digit = Character.getNumericValue(input.charAt(pos));
            pos++;
            return digit;
        } else {
            syntax_error();
        }
        return 0;
    }

    static char var() {
        if (input.charAt(pos) == 'x' || input.charAt(pos) == 'y' || input.charAt(pos) == 'z') {
            return input.charAt(pos++);
        } else {
            syntax_error();
            return 0;
        }
    }

    static int relop() {
        if (input.charAt(pos) == '=' && input.charAt(pos + 1) == '=') {
            pos += 2;
            return 1;
        } else if (input.charAt(pos) == '!' && input.charAt(pos + 1) == '=') {
            pos += 2;
            return 2;
        } else if (input.charAt(pos) == '<') {
            if (input.charAt(pos + 1) == '=') {
                pos += 2;
                return 5;
            } else if (input.charAt(pos + 1) == ' ') {
                pos++;
                return 3;
            } else {
                syntax_error();
                return 0;
            }
        } else if (input.charAt(pos) == '>') {
            if (input.charAt(pos + 1) == '=') {
                pos += 2;
                return 6;
            } else if (input.charAt(pos + 1) == ' ') {
                pos++;
                return 4;
            } else {
                syntax_error();
                return 0;
            }
        } else {
            syntax_error();
            return 0;
        }
    }

    static void consume_whitespace() {
        while (pos < input.length() && Character.isWhitespace(input.charAt(pos))) {
            pos++;
        }
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        while (true) {
            System.out.print(">> ");
            input = scanner.nextLine();
            if (input.equals("terminate")) break;

            // 초기화
            pos = 0;
            x = 0;
            y = 0;
            z = 0;
            bool_x = false;
            bool_y = false;
            bool_z = false;
            is_bool_x = false;
            is_bool_y = false;
            is_bool_z = false;
            error_occur = 0;
            curr_var = 'Q';
            temp.setLength(0);

            program();
            System.out.println();
        }
        scanner.close();
    }
}
