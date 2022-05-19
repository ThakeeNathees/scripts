
:: Create a new hello world C file as main.c

@echo off

setlocal EnableDelayedExpansion
set source=#include ^<stdio.h^>^

^

int main(int argc, char^*^* argv) {^

^

  printf("Hello World!\n");^

  return 0;^

}^


echo !source! > main.c
