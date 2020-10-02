  movz x0, #17
  sub x1, x0, #0xff
  add x1, x1, #10
  add x1, x1, x0
  orr x1, x1, #2
  ret