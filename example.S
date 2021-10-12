    .arch armv5t

    .comm   y, 4
    .comm   n, 4
    .text
    .align  2
    .section    .rodata
    .align  2
    
x:
     .global 	x
     .align	4
     .size	x, 4
     .word	5
     
array:
    .global array
    .align  4
    .size array, 20
    .word   2
    .word   6
    .word   13
    .word   3
    .word   99
_str0:
    .ascii  "%d\n\0"
    .align 2

_str1:
    .ascii  "%d\0"
    .align  2

_str2:
    .ascii  "result is %d \0"
    .text
    .align  2

func:
    str     fp, [sp,#-4]！ 
    mov     fp, sp
    sub     sp, sp, #12

    str     r0, [fp, #-8] @将[fp+8]处的值放进r0
    str     r1, [fp, #-12]
    cmp     r0, r1
    blt     .L2
    add     r0, r0, r1
    b       .L3

.L2:
    mov     r2, r0
    mul     r0, r2, r1
.L3:

    add     sp, fp, #0
    ldr     fp, [sp], #4
    bx      lr

    .global main
main:
    push    {fp,lr}
    add     fp, sp, #4
    mov     r3 , #0    @初始化sum
    ldr     r1 , _bridge+8  
    ldr     r0 , _bridge+16
    bl      __isoc99_scanf  @输入n,r0是str1,r1是&n
    ldr     r3, _bridge+8
    ldr     r1, [r3]    @r1是n
    mov     r2 , #1
    mov     r3 , #0
loop:
    add  r3 , r3 ,r2    @sum = sum + i
    add r2 , r2 ,#1    @i++
    cmp  r2 , r1      @判断 i 是否等于val       if(i==val)
    bgt  end            @若大于 跳转至END处       break
    b    loop           @若小于等于 跳转至LOOP处进入下次循环     else continue
end:
    ldr     r0, _bridge+12  @_str0
    mov     r1, r3          @_要打印的放进r1
    bl      printf
    ldr     r1, _bridge+4   @r1=&y
    ldr     r0, _bridge+16
    bl      __isoc99_scanf  @输入y,r0是str1,r1是&y
    ldr     r3, _bridge+4
    ldr     r2, _bridge
    ldr     r0, [r2]    @r0是x
    ldr     r1, [r3]    @r1是y
    bl      func    @调用func
    mov     r1, r0
    ldr     r0, _bridge+20
    bl      printf  @r0是"result is %d"，r1是result
    mov     r0, #0
    
    ldr     r0, =array @把array[0]的地址存到r0里,没有=就是存值
    mov     r1, #0      @初始化i
    mov     r2, #1      @初始化结果mul
arrLoop:
    ldr     r3，[r0],#4 @数组中的值拿出来存进r3,并将r0+4写入r0
    mul     r2,  r3, r2
    add     r1, r1, #1
    cmp     r1, #5
    bne     arrLoop
    ldr     r0, _bridge+12  @_str0
    mov     r1, r2          @_要打印的放进r1
    bl      printf
    pop     {fp, pc}

_bridge:
    .word   x
    .word   y
    .word   n
    .word   _str0
    .word   _str1
    .word   _str2