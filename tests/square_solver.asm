in
pop rax

in
pop rbx

in
pop rcx

push rbx
push rbx
mul

push rax
push rcx
mul

push 4

mul

sub; counting D = b^2 - 4*a*c

pop rdx

push rdx
push 0

ja halt:

push rdx
sqrt

pop rdx
push rdx

push rbx
push -1
mul
pop rcx
push rcx

add

push 2
push rax
mul
pop rax
push rax

div

out

push rcx

push rdx

sub
push rax

div

out

halt:
    hlt
