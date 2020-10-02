  mov x1, #5
  mov x2, #7
  mov z1.S, x2
  ptrue p1.S
  st1b z1.S, p1, [x0]
  whilelt p1.S, x1, x2
  ld1rb z1.S, p1/Z, [x0]
  ret