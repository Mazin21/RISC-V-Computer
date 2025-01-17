OUTPUT(riscv)
ENTRY(_start)

MEMORY
{  
    RAM (rwx): ORIGIN = 0x0000, LENGTH = 0xFFFF - 0x0B
}

SECTIONS
{
    __stack_size = 4K;

    .init :
    {
        *(.init)
    } > RAM

    .text :
    {
        *(.text*)
        *(.rodata*)
        PROVIDE( _text_end = .);
      
    } > RAM

    .data : 
    {
        *(.data*)
        PROVIDE( _global_pointer = . + 0x800);
        *(.sdata*)
    } > RAM

    .bss :
    {
        *(.bss*)
    }

    .stack ORIGIN(RAM) + LENGTH(RAM) - __stack_size :
    {
        PROVIDE( _sp = .); 
    } >RAM
}

