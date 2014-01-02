#include <stdio.h>

#define A(B) C(1, B)
#define C(X, Y, ...) Y ## y(H)
#define B E, F
#define Ey(Z) G(Z)
#define By G(H)
#define G(H) I(2, H)
#define H J, K
#define I(L, M, ...) M ## aa
#define Kaa 4
#define Jaa 6

void main() {
	printf("%d\n", A(B));
	return;
}
