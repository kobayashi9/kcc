# kcc C Compiler
kcc is a toy C compiler.
It is the goal of this project that it becomes to be able to do self-hosting.

## Status
note: kcc is now being developed.

It supports the follwing functions.
- int type
- Local variable
- Four arithmetic operations
- for, while, if, else, and return statement
- Function call
- Function definition

Currently, it can compile the following program which calcurates fibonacci sequence with the recursive call.

``` c
int fib(int n){
    if(n == 0){
        return 0;
    } else if(n == 1) {
        return 1;
    } else {
        return fib(n-1)+ fib(n-2);
    }
}

int main(){
    return fib(10);
}
```

## Usage
- Build
```
$ make
```
- Test
```
$ make test
```

## Reference
- https://www.sigbus.info/compilerbook 