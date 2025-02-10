ENTRY(_start)

SECTIONS
{
    . = 0x42000000;
    .text :
    {   /* BYTE，SHORT，LONG，QUAD 命令分别存储 1，2，4，8 字节 */
        LONG(0xaedb041d);
        LONG(0xaedb041d);
        *(.text)
        *(.text*)
    }
}
