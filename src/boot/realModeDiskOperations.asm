BITS 16

DISK_DRIVE_NUMBER equ 0x80
DISK_HEAD_NUMBER equ 0x0
DISK_SEGMENT_TO_READ_INTO equ 0x0
DISK_OFFSET_TO_READ_INTO equ 0x7e00
DISK_SECTORS_NUMBER_TO_READ equ 0x10
DISK_SECTOR_TRACK_TO_READ equ 0x0
DISK_SECTOR_TO_READ equ 0x2

; Real Mode Disk helper functions
realModeDiskOperations:

; Reset the disk to first sector
.resetDisk:
    ; Select reset floppy disk function
    mov ah, 0x0

    ; dl has been pre-set by the BIOS to the correct drive value

    ; Call BIOS routine
    int 0x13

    ; If the Carry Flag is present, it means there was an error - retry reset
    jc .resetDisk

    ret

; Read disk in CHS addressing manner
.CHSRead:
    ; First reset drive
    call .resetDisk

    ; Choose the proper sector to read into
    mov ax, DISK_SEGMENT_TO_READ_INTO
    mov es, ax

    ; Choose offset of sector to read ATA into
    mov bx, DISK_OFFSET_TO_READ_INTO

    ; Select CHS sector reading BIOS function
    mov ah, 0x2

    ; Select the number of sectors to read
    mov al, DISK_SECTORS_NUMBER_TO_READ
    
    ; Select sector track to read
    mov ch, DISK_SECTOR_TRACK_TO_READ

    ; Select which sector to read on the track
    mov cl, DISK_SECTOR_TO_READ

    ; Select head number
    mov dh, DISK_HEAD_NUMBER

    ; Call the BIOS routine
    int 0x13

    ; If the Carry Flag is set, it means there was an error - retry read
    jc .CHSRead

    ret


.diskAddressPacketStructure:
    ; Size of packet
    db 0x10

    ; Always zero - reserved
    db 0x0

    ; Number of sectors to transfer
    dw DISK_SECTORS_NUMBER_TO_READ
    
    ; Memory buffer destination address
    dd DISK_OFFSET_TO_READ_INTO

    ; LBA Block to read
    dq  0x1

    ; (EDD-3.0, optional) 64-bit flat address of transfer buffer
    ; used if memory buffer destination address  is 0xffff:0xffff
    dq 0x0

; Read disk in LBA addressing manner
.LBARead:
    ; First reset drive
    call .resetDisk

    ; First, let's verify that LBA is supported (dl already set by BIOS)
    ; IBM/MS Installation Check
    mov ah, 0x41
    mov bx, 0x55aa

    ; Call BIOS function
    int 0x13

    ; If extensions are not supported, Carry Flag will be set
    ; Read with CHS addressing manner instead
    jc .CHSRead

    ; dl already set by BIOS
    ; Select IBM/MS  Extended Read function
    mov ah, 0x42

    ; Select desired Disk Address Packet Structure
    mov si, .diskAddressPacketStructure

    ; Call BIOS function
    int 0x13

    ; If Carry Flag is set, it means we got an error - retry reading
    jc .LBARead

    ret

