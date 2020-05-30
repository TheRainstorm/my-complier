int a, b, c=0;
float f1=0.1, f2;

int foo(int param_foo){
    int var_foo;
    return 0;
}

int foo(int param_foo){ //error3 y
    int var_foo;
    return 0;
}

int main(int argc, char argv){

    // a = 1 . . 1;
    // a = 1 +;
    // a = (1 + 2;
    // a = 1+2*(1 +2*6 *(5 + 6);
    // {
    //     int xxx;
    // }

    int var1;
    int argc;   //error1 y
    
    xxx = 1;    //error2 y

    foo2(a);  //error4
    foo(); //error5

    a+f1;//error8
    foo(a)+f1; //error8

    f2 = a; //error9

    return 0;
}