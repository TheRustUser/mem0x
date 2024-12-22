# Mem0x Implementations

These are things to implement for mem0x.

### Todo

- [ ] Read Sectors that are after 512 Bytes.
- [ ] Create Memory Map, set up E820.
- [ ] Check Protected Mode CPUID Instruction & Check if Long Mode is Available on the System.
- [ ] Enable Paging, and Set up the GDT, IDT to Jump to Long Mode.
- [ ] Jump to kernel_start (C code).

### In Progress

- [ ] Set up GDT and enter to Protected Mode.

### Done ✓

- [x] Implement BIOS Print in Read Mode.
- [x] Enable A20 Line
