jmp main

Factorial:
    pop rax ; save current n to rax
    push rax
    push rax
    push 1
    je end ; condition of the recursion end

    push rax + -1
;    push rax + -1
;    out

    call Factorial ; recursive call

    mul ; fact(n) = n*fact(n - 1)

    pop rax

    end:
        push rax
        ret

main:
    in
    pop rax
    push rax + 1
    call Factorial
    out

hlt
