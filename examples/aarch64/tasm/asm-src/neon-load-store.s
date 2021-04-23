movi v30.8b, #1
movi v31.8b, #2
movi v0.8b, #3
st3 {v30.8b, v31.8b, v0.8b}, [x0, #100]
ld3 {v30.8b, v31.8b, v0.8b}, [x0, #100]
ret
