#include <stdio.h>

#define A(B) C(1, B)
#define C(X, Y, ...) Y ## y
#define B E, F,
#define Ey 1
#define By 2

void main() {
	printf("%d\n", A(B));
	return;
}
