in ; input
;out
pop rax
;push rax
;out
;jmp 0

in
pop rbx
;push rbx
;out

in
pop rcx
;push rbx
;out

push rbx
push rbx
mul
;out

push rax
push rcx
mul
;out

push 4
mul

sub

out

hlt
