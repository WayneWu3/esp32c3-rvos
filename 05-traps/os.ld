
#include "platform.h"

ENTRY(_start)

PROVIDE(_stack_size = 0x4000);

MEMORY
{
	irom    (rx)  	: ORIGIN = 0x42000000, LENGTH = LENGTH_IROM
	drom    (r)	: ORIGIN = 0x3c000000, LENGTH = LENGTH_DROM
	ram	(rw)	: ORIGIN = 0x3fc80000, LENGTH = LENGTH_RAM
}

SECTIONS
{
	
  	.header : AT(0){
	     _irom_start = . ;
             LONG(0xaedb041d);
	     LONG(0xaedb041d);
  	}>irom
	

	.text ORIGIN(irom) + 8: {
	     PROVIDE(_text_start = .);
	     *(.text .text.*)
	     PROVIDE(_text_end = .);
	}>irom
	
	. = ALIGN(4);
	_irom_size = . - _irom_start;

	_drom_start = ORIGIN(drom) + _irom_size;
	.rodata _drom_start : AT(_irom_size){
	     PROVIDE(_rodata_start = .);
	     *(.rodata .rodata.*)
	     PROVIDE(_rodata_end = .);
	}>drom

	 . = ALIGN(4);
	_drom_size = . - _drom_start;		

	.data ORIGIN(ram) : AT(_irom_size + _drom_size){
	     PROVIDE(_data_start = .);
	     *(.sdata .sdata.*)
	     *(.data .data.*)
	     PROVIDE(_data_end = .);
	}>ram
	_data_lma = ORIGIN(drom) + LOADADDR(.data);
	_data_size = _data_end - _data_start;

	. = ALIGN(4);
	.bss :{
	     PROVIDE(_bss_start = .);
	     *(.sbss .sbss.*)
	     *(.bss .bss.*)
	     *(COMMON)
	     PROVIDE(_bss_end = .);
	}>ram

  	__global_pointer$ = MIN(_data_start - 0x800, _bss_end - 0x800);

	PROVIDE(_memory_start = ORIGIN(ram));
	PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));

	PROVIDE(_stack_start = ORIGIN(ram) + LENGTH(ram) - _stack_size);
	PROVIDE(_stack_end = _memory_end);

	PROVIDE(_heap_start = _bss_end);
	PROVIDE(_heap_size = _stack_start - _heap_start);
}

PROVIDE(printf = 0x40000040); 
PROVIDE(memset = 0x40000354);
PROVIDE(memcpy = 0x40000358);

