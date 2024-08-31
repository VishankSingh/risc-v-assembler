add x3, x4, x7
beq x4, x7, L1
jal x0, L1
sd x5, 12(x6)
lui x9, 0x10000
L1: addi x9, x10, 12

# 007201b3
# 00720863
# 00c0006f
# 00533623
# 100004b7
# 00c50493
