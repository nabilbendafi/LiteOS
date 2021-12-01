/*
 * K20DX256 ARM GCC linker script file
 */

FLASH_SIZE = 0x40000;      /* 256kB */
RAM_SIZE   = 0x10000;      /* 64kB */

STACK_SIZE = 0x400;

/* Highest address of the user mode stack */
_sstack = 0x1FFF8000;    /* start of stack */
_estack = 0x20007FFF;    /*   end of stack */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;  /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

MEMORY
{
  VECTORS          (rx) : ORIGIN = 0x00000000, LENGTH = 0x00000400
  FLASH_PROTECTION (rx) : ORIGIN = 0x00000400, LENGTH = 0x00000010
  FLASH            (rx) : ORIGIN = 0x00000410, LENGTH = 256K - 0x410
  RAM             (rwx) : ORIGIN = 0x1FFF8000, LENGTH = 64K
}

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 * _reset_init : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 * __exidx_start
 * __exidx_end
 * __etext
 * __data_start__
 * __preinit_array_start
 * __preinit_array_end
 * __init_array_start
 * __init_array_end
 * __fini_array_start
 * __fini_array_end
 * __data_end__
 * __bss_start__
 * __bss_end__
 * __end__
 * end
 * __HeapLimit
 * __StackLimit
 * __StackTop
 * __stack
 */
ENTRY(Reset_Handler)

SECTIONS
{
    .isr_vector :
    {
        . = 0;
        __isr_vector = .;
        KEEP(*(.isr_vector))
        *(.text.Reset_Handler)
        *(.text.SystemInit)
         . = ALIGN(8);
    } > VECTORS

    .flash_protect :
    {
        KEEP(*(.kinetis_flash_config_field))
         . = ALIGN(8);
    } > FLASH_PROTECTION

    .text :
    {
        . = ALIGN(4);
        __text_start = .;
        *(.text*)

        . = ALIGN(4);
        _etext = .;        /* define a global symbols at end of code */
        __text_end = _etext;

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        . = ALIGN(4);
        __rodata_start = .;
        *(.rodata*)
        . = ALIGN(4);
        __rodata_end = .;

        KEEP(*(.eh_frame*))
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    __etext = .;

    /* used by the startup to initialize liteos vector */
    _si_liteos_vector_data = LOADADDR(.vector_ram);

    /* Initialized liteos vector sections goes into RAM, load LMA copy after code */
    .vector_ram :
    {
        . = ORIGIN(RAM);
        _s_liteos_vector = .;
        *(.data.vector)    /* liteos vector in ram */
        _e_liteos_vector = .;
    } > RAM AT> FLASH

    /* used by the startup to initialize data */
    _sidata = LOADADDR(.data);

    .data :
    {
        _sdata = .;        /* create a global symbol at data start */
        __data_start__ = .;
        __ram_data_start = _sdata;
        *(vtable)
        *(.data*)

        . = ALIGN(8);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(8);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(8);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        . = ALIGN(4);
	KEEP(*( SORT (.liteos.table.*)));

        . = ALIGN(8);
        /* All data end */
        _edata = .;        /* define a global symbol at data end */
        __data_end__ = _edata;
        __ram_data_end = _edata;

    } > RAM AT> FLASH

     /* Uninitialized data section */
    .bss :
    {
        /* This is used by the startup in order to initialize the .bss section */
        _sbss = .;         /* define a global symbol at bss start */
        __bss_start__ = _sbss;
        __bss_start = _sbss;
        *(.bss*)
        *(COMMON)
	_ebss = .;         /* define a global symbol at bss end */
        __bss_end__ = _ebss;
        __bss_end = __bss_end__;
    } > RAM

    bss_size = __bss_end__ - __bss_start__;

    .heap :
    {
        __end__ = .;
        end = __end__;
        __los_heap_addr_start__ = end;
        *(.heap*)
        . = ORIGIN(RAM) + LENGTH(RAM) - STACK_SIZE - 1;
        __HeapLimit = .;
        __los_heap_addr_end__ =  __HeapLimit;
    } > RAM

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy :
    {
        *(.stack)
    } > RAM

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(RAM) + LENGTH(RAM);
    __StackLimit = __StackTop - STACK_SIZE;
    PROVIDE(__stack = __StackTop);

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
      libc.a ( * )
      libm.a ( * )
      libgcc.a ( * )
    }

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}
