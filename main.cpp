#include <iostream>

int main(int argc, char **argv) {
    if (argc != 2) {
        return 1;
    }

    printf(".intel_syntax noprefix\n");
    printf(".globl _main\n");
    printf("_main:\n");
    printf("  mov rax, %d\n", atoi(argv[1]));
    printf("  ret\n");
    return 0;
}
