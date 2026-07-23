#   Size: 3 bytes, 1 byte alignment
#   27.94% on C 3 whole comparisons
#   5.52% on ASM 1 simpl comparison

#   Free'd up to 22.5% of total exe
#   time for other kernel work.

.text
.globl _icmpPixels

# edi => 1st that's the 4-byte pxa
# esi => 2nd that's the 4-byte prev_pxa
_icmpPixels:
    xor %rax, %rax
    xor $0x0FFF, %esi
    xor $0x0FFF, %edi
.l1:
    cmp %esi, %edi
    jne .ret_diff
    mov $1, %rax
    ret

.ret_diff:
    mov $0, %rax
    ret

