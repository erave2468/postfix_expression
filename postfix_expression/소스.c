#include<stdio.h>
#include<stdlib.h>
#define MALLOC(p,s)\
	if (!((p) = malloc(s))) {\
		fprintf(stderr, "Insufficient memory");\
		exit(EXIT_FAILURE);\
	}

char* input; //�Է¹��� ���ڿ�
char output[100]; //����� ���ڿ�
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
stackLink top[10]; //0�� ��ȯ��, 1�� ����

void push(element item,int n) { //���� ���� �Լ�
	stackLink temp;
	MALLOC(temp, sizeof(*temp));
	temp->data = item;
	temp->link = top[n];
	top[n] = temp; // �� ��带 ������ �� ���� �߰�
}
element pop(int n) {
	element item;
	stackLink temp = top[n]; //���� ���� ���� �� �� ���
	if (!temp) {
		return (element) { -1, 'e' }; // ������ ������� -1�� e�� ���� ���� ��ȯ
	}
	item = top[n]->data;
	top[n] = top[n]->link; // ������ �� �� ��带 ���� ���� ����
	free(temp); //���� ��� �޸� ����
	return item;
}

precedence getToken(char *symbol,int *n) { //���ڿ��� ��ū���� ��ȯ
	*symbol = input[(*n)++];
	switch (*symbol) {
	case '(': return lparen;
	case ')': return rparen;
	case '+': return plus;
	case '-': return minus;
	case '*': return times;
	case '/': return divide;
	case '%': return mod;
	case '\0': return eos; //���ڿ� ��
	default: return operand; //�ǿ�����
	};
}
char printToken(precedence token) { //��ū�� ���ڷ� ��ȯ
	switch (token) {
	case lparen: return '(';
	case rparen: return ')';
	case plus: return '+';
	case minus: return '-';
	case times: return '*';
	case divide: return '/';
	case mod: return '%';
	case eos: return '\0'; //���ڿ� ��
	default: return 'e'; //����
	}
}
void postfix() {
	char symbol;
	precedence token;
	push((element) { eos, 0 },0); // ���ÿ� eos ����
	int n = 0; // �Է� ���ڿ��� �ε���
	int i = 0; // ��� ���ڿ��� �ε���
	for (token = getToken(&symbol, &n); token != eos; token = getToken(&symbol, &n)) {
		if (token == operand) { //�ǿ����ڸ�
			output[i++] = symbol; //��� ���ڿ��� ����
		}
		else if (token == rparen) { //������ ��ȣ��
			while ((top[0]->data.type != lparen) && (top[0]->data.type != eos)) {
				output[i++] = printToken(top[0]->data.type); //���ÿ��� ������ ��� ���ڿ��� ����
				pop(0); 
			}
			pop(0); //���� ��ȣ�� ������� ����
		}
		else { //�����ڸ�
			while (isp[top[0]->data.type] >= icp[token]) { //�켱������ ũ�ų� ������
				output[i++] = printToken(top[0]->data.type); //���ÿ��� ������ ��� ���ڿ��� ����
				pop(0); 
			}
			push((element) { token, symbol },0); //���ÿ� ����
		}
	}
	while (top[0]->data.type != eos) { //���ÿ� �����ִ� �����ڵ��� ��� ���ڿ��� ����
		output[i++] = printToken(top[0]->data.type);
		pop(0); //���ÿ��� ������
	}
	if (i > 0 && output[i - 1] != '\0') { //��� ���ڿ��� ������� ������
		output[i++] = '\0'; //���ڿ� ���� �� ���� �߰�
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
		if (token == operand) { //�ǿ����ڸ�
			push((element) { operand, symbol - '0' }, 1); //���ÿ� ����
		}
		else { //�����ڸ�
			op2.num = pop(1).num; 
			op1.num = pop(1).num; //���ÿ��� �� ���� �ǿ����� ������
			switch (token) {
			case plus: //����
				push((element) { operand, op1.num + op2.num }, 1);
				break;
			case minus: //����
				push((element) { operand, op1.num - op2.num }, 1);
				break;
			case times: //����
				push((element) { operand, op1.num* op2.num }, 1);
				break;
			case divide: //������
				if (op2.num == 0) { //0���� ������ ���� ó��
					fprintf(stderr, "0���� ���� �� �����ϴ�.\n");
					exit(EXIT_FAILURE);
				}
				push((element) { operand, op1.num / op2.num }, 1);
				break;
			case mod: //������
				if (op2.num == 0) { //0���� ������ ���� ó��
					fprintf(stderr, "0���� ���� �� �����ϴ�.\n");
					exit(EXIT_FAILURE);
				}
				push((element) { operand, op1.num% op2.num }, 1);
				break;
			}
		}
		token = getToken(&symbol, &n); //���� ��ū �б�
	}
	return pop(1).num; //���ÿ��� ���� ��� ��ȯ
}
int main() {
	precedence one;
	char symbol;
	char put[100];
	while (1) {
		printf("���� ǥ������� ��ȯ�� ������ �Է��ϼ��� (�����Ϸ��� 'exit' �Է�): ");
		scanf_s("%s", put, 100); //���ڿ� �Է�
		if (strcmp(put, "exit") == 0) {
			break; // 'exit' �Է� �� ����
		}
		input = put; //�Է� ���ڿ� ����
		postfix(); //���� ǥ������� ��ȯ
		printf("���� ǥ���: ");
		printf("%s\n", output); //��� ���ڿ� ���
		int result = eval(); //���� ǥ��� ���
		printf("���: %d\n", result); //��� ���

	}
	
}
