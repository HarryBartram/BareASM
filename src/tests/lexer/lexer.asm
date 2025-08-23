[bits 64]
[section .text]

#define SYS_EXIT "60"
#define VALUE_2 "2"
#define SEG_REG "ds"

start {
    xor ds, ds:
    call program:
    jmp exit:
}

; Starting with 1, add 2 repeatly until 5 is reached
program {
    mov eax, <value1+1-1>@SEG_REG:
    mov rbx, VALUE_2:
.adding:
    add rax, rbx:
.compare:
    cmp rax, 5:
    jne .adding:
    ret:
}

exit {
    mov rdi, rax:
    mov rax, SYS_EXIT:
    syscall:
}

[section .data]

value1 {
    db 0x01:
}

stringTest {
    db "Hello \t world!":
}