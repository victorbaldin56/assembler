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

push rbx + 2
;out
push rbx + 2
;out
mul
;out

push rax + 1
;out
push rcx + 1
mul
;out

push 4
mul

sub

out

hlt
