ENTRY(_entry)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 0x8000000000;
    .text : ALIGN(4096)
    {
        *(.text)
        *(.gnu.linkonce.t*)
    }

    .rodata : ALIGN(4096)
    {
        startCtors = .;
        KEEP(*(SORT(.ctors*)))
        endCtors = .;

        startDtors = .;
        KEEP(*(SORT(.dtors*)))
        endDtors = .;

        *(.rodata*)
        *(.gnu.linkonce.r*)
    }

    .data : ALIGN(4096)
    {
        *(.data)
        *(.gnu.linkonce.d*)
    }

    .bss : ALIGN(4096)
    {
        *(COMMON)
        *(.bss)
        *(.gnu.linkonce.b*)
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
}
 
