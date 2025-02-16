set disassemble-next-line on
b _start
b start_kernel
target extended-remote :3333
#mon reset halt
#maintenance flush register-cache
c
