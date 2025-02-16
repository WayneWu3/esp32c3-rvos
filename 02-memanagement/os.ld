
#include "platform.h"

ENTRY(_start)

MEMORY
{
	irom    (rx)  	: ORIGIN = 0x42000000, LENGTH = LENGTH_IROM
	drom    (r)	: ORIGIN = 0x3c000000, LENGTH = LENGTH_DROM
	ram	(rw)	: ORIGIN = 0x3fc80000, LENGTH = LENGTH_RAM
}

SECTIONS
{
  	.header : AT(0){
	     _irom_start = .  ;
             LONG(0xaedb041d);
	     LONG(0xaedb041d);
  	}>irom

	.text ORIGIN(irom) + 8:{
	     PROVIDE(_text_start = .);
	     *(.text .text.*)
	     PROVIDE(_text_end = .);
	}>irom
	
	. = ALIGN(4096);
	_irom_size = . - _irom_start;

	_drom_start = ORIGIN(drom) + _irom_size;
	.rodata _drom_start : AT(_irom_size){
	     PROVIDE(_rodata_start = .);
	     *(.rodata .rodata.*)
	     PROVIDE(_rodata_end = .);
	}>drom
	_drom_size = . - _drom_start;		

	.data ORIGIN(ram) : AT(_irom_size + _drom_size){
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
