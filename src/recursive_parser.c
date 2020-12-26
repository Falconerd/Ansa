#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


double parse_sum();
double parse_product();
double parse_factor();
double parse_number(int neg);
double parse_formula();

char* x = NULL;

double parse_sum() {
	double left = parse_product();
	while (*x == '+' || *x == '-') {
		int add = (*x == '+');
		++x;
		double right = parse_product();
		if (add) {
			left = left + right;
		} else {
			left = left - right;
		}
	}
	return left;
}

double parse_product() {
	double left = parse_factor();
	while (*x == '*' || *x == '/' || *x == '%') {
		char op = *x;
		++x;
		double right = parse_factor();
		if (op == '*') {
			left = left * right;
		} else if (op == '/') {
			left = left / right;
		} else if (op == '%') {
			// @TODO: does this do funky shit?
			left = (int)left % (int)right;
		}
	}
	return left;
}

double parse_factor() {
	int neg = 0;
	if (*x == '-') {
		neg = 1;
		++x;
	}
	if (*x >= '0' && *x <= '9') {
		return parse_number(neg);
	} else if (*x == '(') {
		++x;
		double result = parse_sum();
		if (*x == ')') {
			++x;
			return result;
		}
	} else {
		printf("Expected digit but found %c\n", *x);
	}
}

double parse_number(int neg) {
	double number = 0;
	while (*x >= '0' && *x <= '9') {
		number = number * 10;
		number = number + (*x - '0');
		++x;
	}
	if (*x == '.') {
		++x;
		double weight = 1;
		while (*x >= '0' && *x <= '9') {
			weight = weight / 10;
			double scaled = (*x - '0') * weight;
			number = number + scaled;
			++x;
		}
	}
	if (neg) {
		number = -number;
	}
	return number;
}

double parse_formula() {
	double result = parse_sum();
	if (*x == '\0') {
		return result;
	}
	printf("Expected end of input but found %c\n", *x);
}

int main() {
	char* src = "3+5*(3-2)+(7*3-2)";
	x = src;
	assert(parse_formula() == 27.0);

	char* src2 = "1-2+4/(3-2)*3";
	x = src2;
	assert(parse_formula() == 11.0);

	char* src3 = "3*(4*3/2)+17.5/3";
	x = src3;
	double result = parse_formula();
	assert(result > 23.82);
	assert(result < 23.84);
	return 0;
}
