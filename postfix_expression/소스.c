#include<stdio.h>
#include<stdlib.h>
#define MALLOC(p,s)\
	if (!((p) = malloc(s))) {\
		fprintf(stderr, "Insufficient memory");\
		exit(EXIT_FAILURE);\
	}

char* input; //입력받을 문자열
char output[100]; //출력할 문자열
typedef enum precedence { lparen, rparen, plus, minus, times, divide, mod, eos, operand }precedence;
int isp[] = { 0, 19, 12, 12, 13, 13, 13, 0 };
int icp[] = { 20, 0, 12, 12, 13, 13, 13, 0 };

typedef struct element {
	precedence type;
	int num;
} element;
typedef struct stack* stackLink;
typedef struct stack {
	element data;
	int key;
	stackLink link;
} stack;
stackLink top[10]; //0번 변환용, 1번 계산용

void push(element item,int n) { //스택 삽입 함수
	stackLink temp;
	MALLOC(temp, sizeof(*temp));
	temp->data = item;
	temp->link = top[n];
	top[n] = temp; // 새 노드를 스택의 맨 위에 추가
}
element pop(int n) {
	element item;
	stackLink temp = top[n]; //꺼낼 노드는 스택 맨 위 노드
	if (!temp) {
		return (element) { -1, 'e' }; // 스택이 비었으면 -1과 e를 갖는 원소 반환
	}
	item = top[n]->data;
	top[n] = top[n]->link; // 스택의 맨 위 노드를 다음 노드로 변경
	free(temp); //꺼낸 노드 메모리 해제
	return item;
}

precedence getToken(char *symbol,int *n) { //문자열을 토큰으로 변환
	*symbol = input[(*n)++];
	switch (*symbol) {
	case '(': return lparen;
	case ')': return rparen;
	case '+': return plus;
	case '-': return minus;
	case '*': return times;
	case '/': return divide;
	case '%': return mod;
	case '\0': return eos; //문자열 끝
	default: return operand; //피연산자
	};
}
char printToken(precedence token) { //토큰을 문자로 변환
	switch (token) {
	case lparen: return '(';
	case rparen: return ')';
	case plus: return '+';
	case minus: return '-';
	case times: return '*';
	case divide: return '/';
	case mod: return '%';
	case eos: return '\0'; //문자열 끝
	default: return 'e'; //에러
	}
}
void postfix() {
	char symbol;
	precedence token;
	push((element) { eos, 0 },0); // 스택에 eos 삽입
	int n = 0; // 입력 문자열의 인덱스
	int i = 0; // 출력 문자열의 인덱스
	for (token = getToken(&symbol, &n); token != eos; token = getToken(&symbol, &n)) {
		if (token == operand) { //피연산자면
			output[i++] = symbol; //출력 문자열에 삽입
		}
		else if (token == rparen) { //오른쪽 괄호면
			while ((top[0]->data.type != lparen) && (top[0]->data.type != eos)) {
				output[i++] = printToken(top[0]->data.type); //스택에서 꺼내서 출력 문자열에 삽입
				pop(0); 
			}
			pop(0); //왼쪽 괄호는 출력하지 않음
		}
		else { //연산자면
			while (isp[top[0]->data.type] >= icp[token]) { //우선순위가 크거나 같으면
				output[i++] = printToken(top[0]->data.type); //스택에서 꺼내서 출력 문자열에 삽입
				pop(0); 
			}
			push((element) { token, symbol },0); //스택에 삽입
		}
	}
	while (top[0]->data.type != eos) { //스택에 남아있는 연산자들을 출력 문자열에 삽입
		output[i++] = printToken(top[0]->data.type);
		pop(0); //스택에서 꺼내기
	}
	if (i > 0 && output[i - 1] != '\0') { //출력 문자열이 비어있지 않으면
		output[i++] = '\0'; //문자열 끝에 널 문자 추가
	}
}
int eval() {
	precedence token;
	char symbol;
	element op1, op2;
	int n = 0;
	input = output;
	token = getToken(&symbol, &n);
	while (token != eos) {
		if (token == operand) { //피연산자면
			push((element) { operand, symbol - '0' }, 1); //스택에 삽입
		}
		else { //연산자면
			op2.num = pop(1).num; 
			op1.num = pop(1).num; //스택에서 두 개의 피연산자 꺼내기
			switch (token) {
			case plus: //덧셈
				push((element) { operand, op1.num + op2.num }, 1);
				break;
			case minus: //뺄셈
				push((element) { operand, op1.num - op2.num }, 1);
				break;
			case times: //곱셈
				push((element) { operand, op1.num* op2.num }, 1);
				break;
			case divide: //나눗셈
				if (op2.num == 0) { //0으로 나누기 에러 처리
					fprintf(stderr, "0으로 나눌 수 없습니다.\n");
					exit(EXIT_FAILURE);
				}
				push((element) { operand, op1.num / op2.num }, 1);
				break;
			case mod: //나머지
				if (op2.num == 0) { //0으로 나누기 에러 처리
					fprintf(stderr, "0으로 나눌 수 없습니다.\n");
					exit(EXIT_FAILURE);
				}
				push((element) { operand, op1.num% op2.num }, 1);
				break;
			}
		}
		token = getToken(&symbol, &n); //다음 토큰 읽기
	}
	return pop(1).num; //스택에서 최종 결과 반환
}
int main() {
	precedence one;
	char symbol;
	char put[100];
	while (1) {
		printf("후위 표기법으로 변환할 수식을 입력하세요 (종료하려면 'exit' 입력): ");
		scanf_s("%s", put, 100); //문자열 입력
		if (strcmp(put, "exit") == 0) {
			break; // 'exit' 입력 시 종료
		}
		input = put; //입력 문자열 설정
		postfix(); //후위 표기법으로 변환
		printf("후위 표기법: ");
		printf("%s\n", output); //출력 문자열 출력
		int result = eval(); //후위 표기법 계산
		printf("결과: %d\n", result); //결과 출력

	}
	
}
