/* Linker script for STM32F429x */
MEMORY
{
  rom (rx) : ORIGIN = 0x08000000, LENGTH = 2048K
  ram (rwx) : ORIGIN = 0x20000000, LENGTH = 192K
}

ENTRY(_start)

SECTIONS
{
  .text :
  {
    KEEP(*(.vectors))
    *(.text*)
    *(.rodata*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)
  } >rom

  .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >rom
  .ARM.exidx : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) } >rom

  .data : AT (ADDR(.text) + SIZEOF(.text))
  {
    _data = .;
    *(.data*)
    *(.ramfunc*)
    . = ALIGN(4);
    _edata = .;
  } >ram

  .bss :
  {
    _bss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } >ram

  /DISCARD/ : { *(.note.GNU-stack) }

  end = .;
}
