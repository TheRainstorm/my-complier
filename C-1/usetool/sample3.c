int foo(int a, int b) {

}

int main() {
    int a, b;
    a = 1 + ((a + b) + 0.12) * 3;
    foo(a, foo(1 + 2, b));

    if(a < b){
       a = 0;
    }else{
        b = 0;
        if(a==b){
            a = 2 *b;
        }
    }

    a = 100;
    while(a > 0){
        b = a%2;
        if(b != 0){
            a = a - 1;
        }else{
            a = a / 2;
        }
    }
}