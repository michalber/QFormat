# QFormat

Someday there will be cool description :sweat_smile:

## Description
This is a cool implementation of of Fixed Point number format which is able to perform calculations at compile time!

## Example
Consider simple example where you define `0.3f` in `Q31` format.
```cpp
int main()
{  
    QFormat<Q31> num = QFormat<Q31>{0.3f};
    num += QFormat<Q15>{0.43f / 2};
    
    return num.mQformatInt;
}
```

Without any optimizations compiler (`gcc 10.2 x86-64`) will produce this assembly.
```
QFormat<QType<int, 1ul, true> >::operator+=(QFormat<QType<int, 1ul, true> > const&):
        push    rbp
        mov     rbp, rsp
        mov     QWORD PTR [rbp-8], rdi
        mov     QWORD PTR [rbp-16], rsi
        mov     rax, QWORD PTR [rbp-8]
        mov     edx, DWORD PTR [rax]
        mov     rax, QWORD PTR [rbp-16]
        mov     eax, DWORD PTR [rax]
        add     edx, eax
        mov     rax, QWORD PTR [rbp-8]
        mov     DWORD PTR [rax], edx
        mov     rax, QWORD PTR [rbp-8]
        pop     rbp
        ret
main:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     DWORD PTR [rbp-12], 0
        mov     DWORD PTR [rbp-12], 644245120
        lea     rax, [rbp-2]
        movss   xmm0, DWORD PTR .LC0[rip]
        mov     rdi, rax
        call    QFormat<QType<short, 1ul, true> >::QFormat(float) [complete object constructor]
        lea     rdx, [rbp-2]
        lea     rax, [rbp-8]
        mov     rsi, rdx
        mov     rdi, rax
        call    QFormat<QType<int, 1ul, true> >::QFormat<QType<short, 1ul, true> >(QFormat<QType<short, 1ul, true> > const&)
        lea     rdx, [rbp-8]
        lea     rax, [rbp-12]
        mov     rsi, rdx
        mov     rdi, rax
        call    QFormat<QType<int, 1ul, true> >::operator+=(QFormat<QType<int, 1ul, true> > const&)
        mov     eax, DWORD PTR [rbp-12]
        leave
        ret
QFormat<QType<short, 1ul, true> >::QFormat(float) [base object constructor]:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     QWORD PTR [rbp-8], rdi
        movss   DWORD PTR [rbp-12], xmm0
        mov     edx, DWORD PTR [rbp-12]
        mov     rax, QWORD PTR [rbp-8]
        movd    xmm0, edx
        mov     rdi, rax
        call    QFormat<QType<short, 1ul, true> >::ToQFormat(float) const
        mov     rdx, QWORD PTR [rbp-8]
        mov     WORD PTR [rdx], ax
        nop
        leave
        ret
QFormat<QType<int, 1ul, true> >::QFormat<QType<short, 1ul, true> >(QFormat<QType<short, 1ul, true> > const&):
        push    rbp
        mov     rbp, rsp
        mov     QWORD PTR [rbp-24], rdi
        mov     QWORD PTR [rbp-32], rsi
        mov     DWORD PTR [rbp-8], 16
        mov     rax, QWORD PTR [rbp-32]
        movzx   eax, WORD PTR [rax]
        cwde
        sal     eax, 16
        mov     DWORD PTR [rbp-4], eax
        mov     rax, QWORD PTR [rbp-32]
        movzx   eax, WORD PTR [rax]
        test    ax, ax
        jle     .L9
        or      DWORD PTR [rbp-4], 32768
        jmp     .L10
.L9:
        or      DWORD PTR [rbp-4], 32767
.L10:
        mov     rax, QWORD PTR [rbp-24]
        mov     edx, DWORD PTR [rbp-4]
        mov     DWORD PTR [rax], edx
        nop
        pop     rbp
        ret
QFormat<QType<short, 1ul, true> >::ToQFormat(float) const:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     QWORD PTR [rbp-8], rdi
        movss   DWORD PTR [rbp-12], xmm0
        movss   xmm1, DWORD PTR [rbp-12]
        movss   xmm0, DWORD PTR .LC1[rip]
        mulss   xmm1, xmm0
        movd    eax, xmm1
        movd    xmm0, eax
        call    std::round(float)
        cvttss2si       eax, xmm0
        leave
        ret
.LC0:
        .long   1046227190
.LC1:
        .long   1191182336
```

But when optimisation is enabled..
```
main:
        mov     eax, 1105979008
        ret
```

Magic! :nerd_face:
