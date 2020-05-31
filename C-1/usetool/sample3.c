int main() {
    int a, b;
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