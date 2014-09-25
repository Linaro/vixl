Extending the disassembler
==========================

The output of the disassembler can be extended and customized. This may be
useful for example to add comments and annotations to the disassembly or print
aliases for register names.

The general procedure to achieve this is to create a sub-class of
`Disassembler` and override the appropriate virtual functions.

The `Disassembler` class provides virtual methods that implement how specific
disassembly elements are printed. See
[src/a64/disasm-a64.h](/src/a64/disasm-a64.h) for details.  At the time of
writing, these are

    virtual void AppendRegisterNameToOutput(const Instruction* instr,
                                            CPURegister::RegisterType reg_type,
                                            unsigned reg_code,
                                            unsigned reg_size);
    virtual void AppendPCRelativeOffsetToOutput(const Instruction* instr,
                                                int64_t offset);
    virtual void AppendAddressToOutput(const Instruction* instr,
                                       const void* addr);
    virtual void AppendCodeAddressToOutput(const Instruction* instr,
                                           const void* addr);
    virtual void AppendDataAddressToOutput(const Instruction* instr,
                                           const void* addr);

They can be overridden for example to use different register names and annotate
code addresses.

More complex modifications can be performed by overriding the visitor functions
of the disassembler. The VIXL `Decoder` uses a visitor pattern implementation,
so the `Disassembler` (as a sub-class of `DecoderVisitor`) must provide a
visitor function for each sub-type of instructions. The complete list of
visitors is defined by the macro `VISITOR_LIST` in
[src/a64/decoder-a64.h](/src/a64/decoder-a64.h).

The [/examples/custom-disassembler.h](/examples/custom-disassembler.h) and
[/examples/custom-disassembler.cc](/examples/custom-disassembler.cc) example
files show how the methods can be overridden to use different register names,
annotate code addresses, and add comments:

    VIXL disasm:   add x10, x16, x17
    custom disasm: add x10, ip0, ip1 // add/sub to x10

    VIXL disasm:   cbz x10, #+0x28 (addr 0x7fff8843bf6c)
    custom disasm: cbz x10, #+0x28 (addr 0x7fff8843bf6c) (function: foo)


One can refer to the implementation of visitor functions for the `Disassembler`
(in [src/a64/disasm-a64.cc](/src/a64/disasm-a64.cc)) or even for the `Simulator`
(in [src/a64/simulator-a64.cc](/src/a64/simulator-a64.cc)) to see how to extract
information from instructions.
