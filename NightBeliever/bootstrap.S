.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set AOUT_KLUDGE, 1<<16         # aout kludge
# this is the Multiboot 'flag' field
.set FLAGS,    ALIGN | MEMINFO | AOUT_KLUDGE
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

.text
.align 4
multiboot_header:
.long MAGIC
.long FLAGS
.long CHECKSUM
.long multiboot_header
.long multiboot_header
.long bss
.long end_bss
.long start

.text
.extern __Z10entrypointv
.global start
#.type _start, @function
start:
	movl $stack_top, %esp
	cli
	call __Z10entrypointv
	cli
	hlt
.Lhang:
	jmp .Lhang

.section __DATA, __data
.long 0xDEADBEEF # Prevent __data from ending up after __ass

.section __DATA, __ass
bss:
.skip 16384 # 16 KiB
stack_top:

.section __EATA, __FAKE
end_bss:
