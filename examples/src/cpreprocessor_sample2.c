#include <stdio.h>

#define IFF(cond) CAT(IFF_, cond)
#define CAT(a, ...) a ## __VA_ARGS__
#define IFF_0(t, f) f
#define IFF_1(t, f) t
#define A() B()
#define B() C()
#define C() D()
#define D() 1

void main() {
	IFF(A())(true, false);
}
