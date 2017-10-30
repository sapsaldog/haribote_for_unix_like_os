/* Script for -z combreloc: combine and sort reloc sections */
OUTPUT_FORMAT("elf32-i386", "elf32-i386",
	      "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(HariMain)
SEARCH_DIR("../../tools/golibc");
SECTIONS
{
  . = 0x00280000;
  .text : { *(.text) }
  .data : { *(.data) }
  .rodata : {
    *(.rodata)
    hankaku = _binary_hankaku_bin_start;
  }
  .bss : { *(.bss) }
}
