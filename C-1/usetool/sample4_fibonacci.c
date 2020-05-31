int main() {
    int a, b, n, t;

    n = read();
    a = 1;
    b = 0;
    while (n != 0) {
        t = a + b;
        a = b;
        b = t;

        n = n - 1;
    }

    write(b);
    
    return 0;
}