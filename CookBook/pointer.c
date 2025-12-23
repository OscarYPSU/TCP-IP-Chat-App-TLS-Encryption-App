#include <stdio.h>


int var=10;
int *pointer = &var;


int main(){
    printf("Value of var: %d\n", pointer);
}