#include <stdio.h>

#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(X) X DEFER(EMPTY)()
#define EXPAND(id) id
#define A() 123

#define B B ## C
#define BC BC ## D
#define BCD BCD ## E

#define Z
#define F() 456
#define G() F
#define H() G F()
#define I() H
#define K I () () () ()

void main() {
	EXPAND(DEFER(A))();
	DEFER(A)    ();

	OBSTRUCT(A)();

	OBSTRUCT(A);

	EXPAND(OBSTRUCT(A)());
	EXPAND(EXPAND(OBSTRUCT(A)()));
	B;
	K;
}
