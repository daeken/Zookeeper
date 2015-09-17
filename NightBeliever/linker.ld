/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = 0xC0000000;

	.text BLOCK(4K) : ALIGN(4K) {
		PROVIDE(_begin = .);
		*(.multiboot)
		LONG(0);
		*(.text)
		PROVIDE(_etext = .);
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
		PROVIDE(_edata = .);
	}

	/* Uninitialized data */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
		PROVIDE(_ebss = .);
	}

	/DISCARD/ :{
		*(.note*)
		*(.indent)
		*(.comment)
		*(.shstrtab)
		*(.symtab)
		*(.strtab)
		*(.eh_frame)
	}
}
