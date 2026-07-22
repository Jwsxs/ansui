.text # for gdb
.globl __check_flag_bit

# %di => least significant bit will be checked here
__check_flag_bit:
    xor %al, %al
_l1:
    bt  $0, %di # bit test 0 (1st least significant bit) on %di
    jnc  _ret_0 # flag is not set
    mov $1, %al
    ret

_ret_0:
    ret