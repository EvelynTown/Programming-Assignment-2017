#include "nemu.h"
#include "cpu/reg.h"
#include "memory/memory.h"
#include "cpu/cpu.h"
#include <stdlib.h>
#include <elf.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, decNUM, REG, VAR, NEQ, AND, OR, DEREF, NEG, hexNUM

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},		// white space
	{"\\+", '+'},
	{"==", EQ},
	{"0[xX][0-9a-fA-F]+",hexNUM},	//hex
	{"[0-9]+",decNUM},		//dec
	{"-",'-'},
	{"\\*",'*'},
	{"\\(",'('},
	{"\\)",')'},
	{"/",'/'},
	{"\\$[a-z]+",REG},		//register
	{"[0-9a-zA-Z_]+",VAR},		//symbol
	{"!=", NEQ},
	{"&&", AND},
	{"\\|\\|", OR},
	{"!",'!'}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for more times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for (i = 0; i < NR_REGEX; i++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if (ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			assert(ret != 0);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

typedef struct Pri {
	int op;
	int priority;
} pri;
pri PRI[] = {
	{263, 0},
	{262, 1},
	{257, 2},
	{261, 2},
	{'+', 3},
	{'-', 3},
	{'*', 4},
	{'/', 4},
	{264, 5},
	{265, 5},
	{'!', 5},
	{'(', 6},
	{')', 6}
};

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;

	nr_token = 0;

	while (e[position] != '\0') {
		/* Try all rules one by one. */
		for (i = 0; i < NR_REGEX; i++) {
			if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {

				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				//	printf("match regex[%d] at position %d with len %d: %.*s\n", i, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i].
				 * Add codes to perform some actions with this token.
				 */


				switch (rules[i].token_type) {
				case 258: case 259: case 260: case 266: 	//decnum,reg,sym
					tokens[nr_token].type = rules[i].token_type;
					assert(substr_len < 32);
					for (int j = 0; j < substr_len; j++) {
						tokens[nr_token].str[j] = *substr_start;
						substr_start++;
					}
					tokens[nr_token].str[substr_len] = '\0';
					nr_token++;
					break;
				case 256: break;
				default: tokens[nr_token].type = rules[i].token_type; nr_token++; break;
				}

				break;
			}
		}

		if (i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
	return true;
}

uint32_t expr(char *e, bool *success) {
	if (!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Implement code to evaluate the expression. */
	for (int i = 0; i < nr_token; i++) {
		if (tokens[i].type == '*' && (i == 0 || tokens[i - 1].type == '+' || tokens[i - 1].type == '*' || tokens[i - 1].type == '/' || tokens[i - 1].type == '(' || tokens[i - 1].type == '!' || tokens[i - 1].type == 261 || tokens[i - 1].type == 262 || tokens[i - 1].type == 263)) {
			tokens[i].type = 264;
		}
		else if (tokens[i].type == '-' && (i == 0 || tokens[i - 1].type == '+' || tokens[i - 1].type == '*' || tokens[i - 1].type == '/' || tokens[i - 1].type == '(' || tokens[i - 1].type == '!' || tokens[i - 1].type == 261 || tokens[i - 1].type == 262 || tokens[i - 1].type == 263)) {
			tokens[i].type = 265;
		}
	}

	/*printf("\nPlease implement expr at expr.c\n");
	assert(0);*/
	return eval(0, nr_token - 1);
}

uint32_t check_parentheses(int p, int q) {
	int count = 0;
	if (tokens[p].type != '(' || tokens[q].type != ')')
		return 1;
	else {
		for (int i = p; i <= q; i++) {
			if (tokens[i].type == '(')
				count++;
			else if (tokens[i].type == ')')
				count--;

			if (count < 0)
				return 1;
			else if (count == 0 && i < q)
				return 1;
			else if (count > 0 && i == q)
				return 1;
		}
		return 0;
	}
}

uint32_t eval(int p, int q) {
	if (p > q) {
		/* Bad expression */
		printf("Bad Expression!\n");
		return 0;
	}
	else if (p == q) {
		/* Single token.
		 * * For now this token should be a number.
		 * * Return the value of the number.
		 * */
		uint32_t val = 0;
		switch (tokens[p].type) {
		case 258:
			val = atoi(tokens[p].str);
			return val;
			break;
		case 266:
			val = strtol(tokens[p].str, NULL, 16);
			return val;
			break;
		case 259:
			if (strcmp(tokens[p].str, "$eax") == 0)
				val = cpu.eax;
			else if (strcmp(tokens[p].str, "$ebx") == 0)
				val = cpu.ebx;
			else if (strcmp(tokens[p].str, "$ecx") == 0)
				val = cpu.ecx;
			else if (strcmp(tokens[p].str, "$edx") == 0)
				val = cpu.edx;
			else if (strcmp(tokens[p].str, "$ebp") == 0)
				val = cpu.ebp;
			else if (strcmp(tokens[p].str, "$esp") == 0)
				val = cpu.esp;
			else if (strcmp(tokens[p].str, "$eip") == 0)
				val = cpu.eip;
			else if (strcmp(tokens[p].str, "$ax") == 0)
				val = cpu.eax & 0xffff;
			else if (strcmp(tokens[p].str, "$bx") == 0)
				val = cpu.ebx & 0xffff;
			else if (strcmp(tokens[p].str, "$cx") == 0)
				val = cpu.ecx & 0xffff;
			else if (strcmp(tokens[p].str, "$dx") == 0)
				val = cpu.edx & 0xffff;
			else if (strcmp(tokens[p].str, "$bp") == 0)
				val = cpu.ebp & 0xffff;
			else if (strcmp(tokens[p].str, "$sp") == 0)
				val = cpu.esp & 0xffff;
			else if (strcmp(tokens[p].str, "$ip") == 0)
				val = cpu.eip & 0xffff;
			else if (strcmp(tokens[p].str, "$ah") == 0)
				val = (cpu.eax & 0xff00) >> 8;
			else if (strcmp(tokens[p].str, "$bh") == 0)
				val = (cpu.ebx & 0xff00) >> 8;
			else if (strcmp(tokens[p].str, "$ch") == 0)
				val = (cpu.ecx & 0xff00) >> 8;
			else if (strcmp(tokens[p].str, "$dh") == 0)
				val = (cpu.edx & 0xff00) >> 8;
			else if (strcmp(tokens[p].str, "$al") == 0)
				val = cpu.eax & 0xff;
			else if (strcmp(tokens[p].str, "$bl") == 0)
				val = cpu.ebx & 0xff;
			else if (strcmp(tokens[p].str, "$cl") == 0)
				val = cpu.ecx & 0xff;
			else if (strcmp(tokens[p].str, "$dl") == 0)
				val = cpu.edx & 0xff;
			return val;
			break;
		case 260:{
			bool success = true;
			return look_up_symtab(tokens[p].str, &success);
			break;
			 }
		default: printf("Something goes wrong!\n"); return 0; break;
		}
	}
	else if (check_parentheses(p, q) == 0) {
		/* The expression is surrounded by a matched pair of parentheses.
		 * * If that is the case, just throw away the parentheses.
		 * */
		return eval(p + 1, q - 1);
	}
	else {
		int count = 0, op = p, tag = op;
		pri operator ={ -1, 100 };
		for (op = p; op <= q; op++) {
			if (tokens[op].type != 258 && tokens[op].type != 259 && tokens[op].type != 260 && tokens[op].type != 266) {
				if (tokens[op].type == '(')
					count++;
				else if (tokens[op].type == ')')
					count--;
				if (count == 0) {
					int i = 0;
					for (i = 0; i < 13; i++) {
						if (PRI[i].op == tokens[op].type)
							break;
					}
					if (operator.priority >= PRI[i].priority) {
						tag = op;
						operator.op = PRI[i].op;
						operator.priority = PRI[i].priority;
					}
				}
			}
		}
		//op = the position of dominant operator in the token expression;
		if(operator.op != 264 && operator.op != 265 && operator.op != '!') {
			uint32_t val1 = eval(p, tag - 1);
			uint32_t val2 = eval(tag + 1, q);

			switch (operator.op) {
			case '+': return val1 + val2; break;
			case '-': return val1 - val2; break;
			case '*': return val1 * val2; break;
			case '/': return val1 / val2; break;
			case 257: return val1 == val2; break;
			case 261: return val1 != val2; break;
			case 262: return val1 && val2; break;
			case 263: return val1 || val2; break;
			default: printf("Something goes wrong!\n"); return 0; break;
			}
		}
		else if (operator.op == 264 || operator.op == 265 || operator.op == '!') {
			uint32_t val = eval(p + 1, q);
			switch (operator.op) {
			case 264: return vaddr_read(val, SREG_DS, sizeof(uint32_t)); break;
			case 265: return val * (-1); break;
			case '!': return !val; break;
			default: printf("Something goes wrong!\n"); return 0; break;
			}
		}
	}
	return 0;
}

