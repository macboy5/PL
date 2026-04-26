# Recursive-Descent Parser 구현 과제

EBNF로 정의된 간단한 언어를 위한 파서 구현 과제입니다.

- **과제 1**: 간단한 산술/불린 표현식 파서를 C++, Java, Python으로 구현
- **과제 2**: 변수 선언, do-while 반복문이 추가된 확장 파서를 C++로 구현

두 과제 모두 한 줄 입력 단위로 동작하며, `terminate` 입력 시 종료됩니다.

---

## 파일 구조

```
PL/
├── 구현과제 1/
│   ├── 구현과제1 명세서.pdf
│   ├── main.cpp          # C++ 구현
│   ├── Main.java         # Java 구현
│   └── RD_PARSER.py      # Python 구현
└── 구현과제 2/
    ├── 구현과제2 명세서.pdf
    ├── main.cpp          # C++ 구현
    └── CMakeLists.txt
```

---

## Recursive Descent Parser란?

Recursive Descent Parser는 EBNF 문법의 각 비단말 기호(non-terminal)를 함수 하나로 대응시켜 구현하는 하향식(top-down) 파싱 방법입니다.

파서는 최상위 규칙인 `program()`에서 시작하여 문법 계층을 따라 함수 호출을 재귀적으로 내려갑니다. 예를 들어 `program()` → `statement()` → `expr()` → `aexpr()` → `term()` → `factor()` → `number()` → `dec()` 순으로 호출이 깊어집니다. 가장 아래 단계인 `dec()`나 `var()` 같은 함수에서 실제 입력 토큰이 기대한 것과 일치하는지 검사하고, 일치하지 않으면 `syntax_error()`를 호출합니다.

에러가 발생하면 플래그(`error_occur`)를 세우고, 호출 스택을 따라 거슬러 올라오면서 각 함수가 에러 플래그를 확인하여 파싱을 중단합니다. 이 구조 덕분에 문법 규칙과 코드 구조가 1:1로 대응되어 가독성이 높고 구현이 직관적입니다.

```
program()
  └─ statement()
       └─ expr()
            ├─ aexpr()
            │    └─ term()
            │         └─ factor()
            │              └─ number()
            │                   └─ dec()  ← 여기서 실제 토큰 검사
            └─ bexpr()
                 ├─ number() ← 실제 토큰 검사
                 ├─ relop()  ← 실제 토큰 검사
                 └─ number() ← 실제 토큰 검사
```

---

## 구현과제 1

### 문법 (EBNF)

```
<program>   → {<statement>}
<statement> → <var> = <expr> ; | print <var> ;
<expr>      → <bexpr> | <aexpr>
<bexpr>     → <number> <relop> <number>
<relop>     → == | != | < | > | <= | >=
<aexpr>     → <term> {( + | - ) <term>}
<term>      → <factor> {( * | / ) <factor>}
<factor>    → <number>
<number>    → <dec> {<dec>}
<dec>       → 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
<var>       → x | y | z
```

### 동작 방식

**입력**
- `>> ` 프롬프트에서 한 줄씩 코드를 입력받습니다.
- 토큰 사이에는 공백이 1개 이상 있어야 합니다.
- 세미콜론(`;`)으로 끝나지 않으면 즉시 `syntax error!!`를 출력합니다.
- 한 줄에 여러 statement를 세미콜론으로 구분하여 입력할 수 있습니다.
- 입력 한 줄이 처리되면 모든 변수 상태가 초기화됩니다.

**변수**
- 사용 가능한 변수는 `x`, `y`, `z`로 한정됩니다.
- 초기화되지 않은 변수는 기본값 `0`을 가집니다.
- 각 변수는 마지막으로 대입된 표현식의 종류를 기억합니다.
  - `<aexpr>`이 대입된 경우 `print` 시 정수값 출력
  - `<bexpr>`이 대입된 경우 `print` 시 `TRUE` 또는 `FALSE` 출력

**expr 분기**
- `<expr>`은 `<bexpr>`와 `<aexpr>` 중 하나인데, 첫 토큰만 봐서는 구분이 불가능합니다 (둘 다 숫자로 시작).
- 따라서 숫자와 공백을 건너뛰며 전방 탐색(lookahead)을 수행한 뒤, 다음에 오는 연산자가 관계 연산자(`== != < > <= >=`)이면 `bexpr()`, 산술 연산자(`+ - * /`)이면 `aexpr()`로 분기합니다.

**aexpr 연산자 우선순위**
- `<aexpr>`(덧셈/뺄셈)과 `<term>`(곱셈/나눗셈)을 별도 함수로 분리하여 곱셈과 나눗셈이 덧셈과 뺄셈보다 먼저 계산되도록 합니다.
- 같은 우선순위 내에서는 왼쪽 결합성(Left Associative)으로 계산됩니다.

**에러 처리**
- 문법에 맞지 않는 입력이나 0으로 나누는 경우 `syntax error!!`를 출력합니다.

### 실행 방법

```bash
# C++
g++ -o parser "구현과제 1/main.cpp" && ./parser

# Java
javac "구현과제 1/Main.java" && java -cp "구현과제 1" Main

# Python
python "구현과제 1/RD_PARSER.py"
```

### 실행 예시

```
>> x = ( 12 + 3 ;
syntax error!!
>> x = 12 == 3 print x ;
syntax error!!
>> x = 12 == 3 ; print x ;
FALSE
>> z = 100 * 3 ; y = 42 * 5 ; y = 12 != 3 ; print y ;
TRUE
>> x = 12 == 3 ; y = 10 + 5 * 3 ; print y ; print x ;
25 FALSE
>> y = 10 * 5 - 3 ; z = 5 - 2 + 8 / 2 ; print y ; print z ;
47 7
>> print x ;
0
>> x = 9 / 3 / 3 ; print x ;
1
>> x = 0 <= 0 ; y = 100 - 50 * 2 ; print x ; print y ;
TRUE 0
>> x = 99 != 99 ; y = 7 * 8 - 6 ; z = 1 <= 2 ; print z ; print x ; print y ;
TRUE FALSE 50
>> terminate
```

---

## 구현과제 2

### 문법 (EBNF)

```
<program>     → {<declaration>} {<statement>}
<declaration> → <type> <var> ;
<statement>   → <var> = <aexpr> ;
              | print <bexpr> ;
              | print <aexpr> ;
              | do '{' {<statement>} '}' while ( <bexpr> ) ;
<bexpr>       → <relop> <aexpr> <aexpr>
<relop>       → == | != | < | > | <= | >=
<aexpr>       → <term> {( + | - | * | / ) <term>}
<term>        → <number> | <var> | ( <aexpr> )
<type>        → int
<number>      → <dec>{<dec>}
<dec>         → 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
<var>         → <alphabet>{<alphabet>}
<alphabet>    → a | b | c | ... | z
```

### 동작 방식

**토크나이저 기반 파싱**
- 입력 문자열을 먼저 공백 기준으로 분리하여 토큰 목록(`vector<string>`)을 만든 뒤, 토큰 단위로 파싱합니다.
- 과제 1이 문자 단위로 입력을 읽은 것과 달리, 미리 토큰을 분리해두기 때문에 각 파싱 함수가 `get_token()` 호출만으로 다음 토큰을 가져올 수 있어 구현이 단순해집니다.
- 토큰 사이에 공백이 없으면 하나의 토큰으로 붙어서 인식되어 `Syntax Error!!`가 발생합니다.

**변수 선언**
- 모든 변수는 `int <var> ;` 형태로 먼저 선언해야 사용할 수 있습니다.
- 선언된 변수는 `0`으로 초기화되며, 선언되지 않은 변수를 사용하면 `Syntax Error!!`입니다.
- `int` 이외의 타입(예: `float`, `double`) 선언 시 `Syntax Error!!`
- 선언부는 문장부보다 앞에 위치해야 합니다. 문장 사이에 선언이 등장하면 `Syntax Error!!`

**bexpr — 전위(prefix) 표기**
- 과제 1의 중위 표기(`number relop number`)와 달리 전위 표기(`relop aexpr aexpr`)를 사용합니다.
  - 예: `> x 10` → x > 10
- `print` 다음에 오는 첫 토큰이 관계 연산자이면 `bexpr()`, 그 외이면 `aexpr()`로 분기합니다.

**aexpr — 연산자 우선순위 없음**
- `+`, `-`, `*`, `/` 모두 동일한 우선순위로 왼쪽 결합성으로 계산됩니다.
- 우선순위가 필요한 경우 괄호 `( <aexpr> )`를 사용해야 합니다.
  - 예: `k + 100 * 3 / 2 - 1` → `((((k + 100) * 3) / 2) - 1) = 149`

**do-while**
- `do { ... } while ( <bexpr> ) ;` 형태입니다.
- 블록을 먼저 실행한 뒤 조건을 검사하며, 조건이 거짓이 될 때까지 반복합니다.
- 블록 시작 시점의 토큰 인덱스를 저장해두고, 매 반복마다 해당 위치로 되돌아가 블록을 재파싱·재실행합니다.

**출력**
- `print` 결과는 순서대로 누적되었다가 파싱이 정상 완료된 후 공백으로 구분하여 한 줄에 출력됩니다.
- 에러 발생 시 즉시 `Syntax Error!!`를 출력하고 이후 출력은 모두 무시됩니다.

### 실행 방법

```bash
g++ -o parser2 "구현과제 2/main.cpp" && ./parser2
```

### 실행 예시

```
>>  int variable ; variable = 365 ;
>>  ab = 12 ;
Syntax Error!!
>>  float ab ;
Syntax Error!!
>>  int k = 2 ;
Syntax Error!!
>>  int k ; int j ; k = 3 ; j = 20 ; int a ; print k + j ;
Syntax Error!!
>>  int k ; int j ; k = 3 ; j = 20 ; print k + j ;
23
>>  int k ; print k + 100 * 3 / 2 - 1 ;
149
>>  int x ; x = 10 + 5 - ( 2 + 3 - 5 * 10 ) ; print x ;
15
>>  int x ; x = 10 + 5 * 2 ; print x ; print > 10 x ;
30 FALSE
>>  int k ; int j ; k = 3 ; j = 20 ; do { print k + ( j - 1 ) * 10 ; k = k - 1 ; } while ( > k + 10 10 ) ; print == k 0 ;
220 210 200 TRUE
>>  int i ; int j ; i = 0 ; j = 0 ; do { j = j + i ; i = i + 1 ; } while ( < i * ( 5 - 4 ) 5 ) ; print i ; print j ;
5 10
>>  int a ; int b ; a = 6 ; b = 2 ; print a / b * b ;
6
>>  int x ; x = 5 - 10 ; print x ;
-5
>>  int a ; int b ; a = 1 ; b = 2 ; print == a + b - 1 b ;
TRUE
>>  int a ; print != a 0 ;
FALSE
>>  int a ; int b ; a = 1 ; b = 1 ; do { print a ; a = a + 1 ; } while ( <= a 3 ) ; print b ;
1 2 3 1
>>  int a ; int b ; a = 2 ; b = 10 ; do { b = b - a ; } while ( > b 0 ) ; print b ;
-2
>>  terminate
```
