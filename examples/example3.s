label1: label3: add x0,x0,x0
    addi x0,x0,2047
    add x0,x0, x0
    bge x0,x0,label4
(label2:) ()
  ()()()  bge s9,x0,0b10
label4:
    jalr x0, 0(x1)
    jal x0, label3
    