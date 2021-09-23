ENTRY(_entry)
OUTPUT_FORMAT(elf64-x86-64)
SECTIONS
{
    . = 0x90000000;
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

    . = ALIGN(8);
 
      .data : ALIGN(4096)
    {
        *(.data)
        *(.gnu.linkonce.d*)
    }

    startTData = .;
   .tdata :
    {
        *(.tdata)
        *(.gnu.linkonce.td*)
    }
    endTData = .;

   .tbss :
    {
        *(.tbss)
        *(.gnu.linkonce.tb*)
        . = ALIGN(8);
    }
    endTBss = . + SIZEOF(.tbss);


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
 
