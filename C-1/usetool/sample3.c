int foo(int a, int b) {

}

int main() {
    int a, b;
    // a = 0;
    // b = a;
    a = 1 + ((a + b) + 0.12) * 3;
    // foo(a, b);
    foo(a, foo(1 + 2, b));

    return 0;
}