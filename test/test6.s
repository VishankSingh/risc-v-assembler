beq x1, x2, L1
jal x0, L2

L1: xor a5, a3, a7
lui x9, 0x10000

L2: add t1, x8, s10
  
lbu x1,-20(x1)

sub x0,x0,x0