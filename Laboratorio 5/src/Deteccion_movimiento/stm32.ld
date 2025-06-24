/*
 * Linker script para STM32F429 (adaptado del genérico libopencm3)
 */

/* Enforce emission of the vector table */
EXTERN(vector_table)

/* Entry point */
ENTRY(reset_handler)

/* Define memory regions */
MEMORY
{
    /* STM32F429 tiene típicamente: */
    rom (rx)  : ORIGIN = 0x08000000, LENGTH = 2048K  /* 2 MB Flash */
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 256K   /* 256 KB SRAM */
}

/* Sections definition */
SECTIONS
{
    .text : {
        *(.vectors)            /* Vector table */
        *(.text*)              /* Código */
        . = ALIGN(4);
        *(.rodata*)            /* Datos sólo lectura */
        . = ALIGN(4);
    } > rom

    /* C++ static constructors/destructors */
    .preinit_array : {
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;
    } > rom

    .init_array : {
        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;
    } > rom

    .fini_array : {
        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;
    } > rom

    .ARM.extab : {
        *(.ARM.extab*)
    } > rom

    .ARM.exidx : {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > rom

    . = ALIGN(4);
    _etext = .;

    .noinit (NOLOAD) : {
        *(.noinit*)
    } > ram
    . = ALIGN(4);

    .data : {
        _data = .;
        *(.data*)
        *(.ramtext*)
        . = ALIGN(4);
        _edata = .;
    } > ram AT > rom
    _data_loadaddr = LOADADDR(.data);

    .bss : {
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > ram

    /* Descarta sección de excepciones C++ no usadas */
    /DISCARD/ : { *(.eh_frame) }

    . = ALIGN(4);
    end = .;
}

/* Define _stack pointer at the end of RAM */
PROVIDE(_stack = ORIGIN(ram) + LENGTH(ram));
