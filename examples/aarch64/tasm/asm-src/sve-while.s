  mov x1, #5
  mov x2, #7
  mov z1.s, x2
  ptrue p1.s
  st1b z1.s, p1, [x0]
  whilelt p1.s, x1, x2
  ld1rb z1.s, p1/z, [x0]
  ret