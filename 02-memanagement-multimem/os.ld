
#include "platform.h"

ENTRY(_start)

MEMORY
{
	irom    (rx)  : ORIGIN = 0x42000000, LENGTH = LENGTH_IROM
	ram	(rwxa): ORIGIN = 0x3FC80000, LENGTH = LENGTH_RAM
}

SECTIONS
{
    	. = 0x42000000;
  	.header :{
             LONG(0xaedb041d);
	     LONG(0xaedb041d);
  	}>irom

	. = ORIGIN(ram);
	.text :{
	     PROVIDE(_text_start = .);
	     *(.text .text.*)
	     PROVIDE(_text_end = .);
	}>ram

	.rodata :{
	     PROVIDE(_rodata_start = .);
	     *(.rodata .rodata.*)
	     PROVIDE(_rodata_end = .);
	}>ram
	
	.data :{
	     . = ALIGN(4096);
	     PROVIDE(_data_start = .);
	     *(.sdata .sdata.*)
	     *(.data .data.*)
	     PROVIDE(_data_end = .);
	}>ram

	.bss :{
	     PROVIDE(_bss_start = .);
	     *(.sbss .sbss.*)
	     *(.bss .bss.*)
	     *(COMMON)
	     PROVIDE(_bss_end = .);
	}>ram

	PROVIDE(_memory_start = ORIGIN(ram));
	PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));

	PROVIDE(_heap_start = _bss_end);
	PROVIDE(_heap_size = _memory_end - _heap_start);
}
PROVIDE(printf = 0x40000040); 
