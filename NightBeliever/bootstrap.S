.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set AOUT_KLUDGE, 1<<16         # aout kludge
# this is the Multiboot 'flag' field
.set FLAGS,    ALIGN | MEMINFO | AOUT_KLUDGE
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

.section .multiboot
.align 4
multiboot_header:
.long MAGIC
.long FLAGS
.long CHECKSUM
.long multiboot_header
.extern _begin
.long _begin
.extern _edata
.long _edata
.extern _ebss
.long _ebss
.long _start

.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.extern _Z10entrypointv
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp
	cli
	call _Z10entrypointv
	cli
	hlt
.Lhang:
	jmp .Lhang

.size _start, . - _start
