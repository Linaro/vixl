  mov x3, #3
  mov x9, #0
  mov z1.S, x3
  ptrue p1.S, VL4
  st1w z1.S, p1, [x1, x9]
  ld1w z1.S, p1/Z, [x1, #0]
  ret