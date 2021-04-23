  mov x3, #3
  mov x9, #0
  mov z1.s, x3
  ptrue p1.s, vl4
  st1w z1.s, p1, [x0, x9]
  ld1w z1.s, p1/z, [x0, #0]
  ret