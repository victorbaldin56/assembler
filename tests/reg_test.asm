in ; input
;out
pop rax
;push rax
;out
;jmp rax + -5

in
pop rbx
;push rbx
;out

in
pop rcx
;push rbx
;out

push rbx
;out
push rbx
;out
mul
;out

push rax
;out
push rcx
mul
;out

push 4
mul

sub

pop  [5]
push [5]
out

hlt
