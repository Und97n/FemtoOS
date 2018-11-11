// #pragma once

// #include <kernel/fs.h>

// typedef struct {
//     uint32_t nfiles; // The number of files in the ramdisk.
// } Initrd_Header;

// typedef struct {
//     uint8_t magic;     // Magic number, for error checking.
//     char name[64];  // Filename.
//     uint32_t offset;   // Offset in the initrd that the file starts.
//     uint32_t length;   // Length of the file.
// } Initrd_File_Header;

// // Initialises the initial ramdisk. It gets passed the address of module,
// // and returns a completed filesystem node.
// FS_Node* initialise_initrd(uint32_t location);
