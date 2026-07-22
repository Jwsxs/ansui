.text # for gdb
.globl __check_flag_bit

# %di => least significant bit will be checked here
# si => is by default null, if anything other than 0 then shift %si
__check_flag_bit:
    xor %al, %al
_l1:
    bt  %si, %di # bit test 0 (1st least significant bit) on %di
    jnc  _ret_0 # flag is not set
    mov $1, %al
    ret

_ret_0:
    ret