struct A {
	struct {}x;
}b[10][2];

struct B{
	int x; int y;
}c;

struct {int a, b;} fun() {
	struct {
		int aa, bb;
	}x;
//	fun(x);
//	return fun(x);
}

int main() {
	b = c;
	fun().a = 1;
}
