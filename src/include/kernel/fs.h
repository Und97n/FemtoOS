// #pragma once

// #define FS_FILE        0x01
// #define FS_DIRECTORY   0x02
// #define FS_CHARDEVICE  0x03
// #define FS_BLOCKDEVICE 0x04
// #define FS_PIPE        0x05
// #define FS_SYMLINK     0x06
// #define FS_MOUNTPOINT  0x08 // Is the file an active mountpoint?

// typedef struct FS_Node FS_Node;
// typedef struct Dirent Dirent;

// // These typedefs define the type of callbacks - called when read/write/open/close
// // are called.
// typedef uint32_t (*Read_fnc)(FS_Node*, uint32_t,uint32_t, uint8_t*);
// typedef uint32_t (*Write_fnc)(FS_Node*, uint32_t, uint32_t, uint8_t*);
// typedef void (*Open_fnc)(FS_Node*);
// typedef void (*Close_fnc)(FS_Node*);
// typedef Dirent* (*Readdir_fnc)(FS_Node*, uint32_t);
// typedef struct FS_Node* (*Finddir_fnc)(FS_Node*, char* mname);

// struct FS_Node {
//     char name[128];     // The filename.
//     uint32_t mask;        // The permissions mask.
//     uint32_t uid;         // The owning user.
//     uint32_t gid;         // The owning group.
//     uint32_t flags;       // Includes the node type. See #defines above.
//     uint32_t inode;       // This is device-specific - provides a way for a filesystem to identify files.
//     uint32_t length;      // Size of the file, in bytes.
//     uint32_t impl;        // An implementation-defined number.
//     Read_fnc read;
//     Write_fnc write;
//     Open_fnc open; 
//     Close_fnc close;
//     Readdir_fnc readdir;
//     Finddir_fnc finddir;
//     FS_Node* ptr; // Used by mountpoints and symlinks.
// };

// struct Dirent {
//     char name[128]; // Filename.
//     uint32_t ino;     // Inode number. Required by POSIX.
// };

// extern FS_Node* fs_root; // The root of the filesystem.

// // Standard read/write/open/close functions. Note that these are all suffixed with
// // _fs to distinguish them from the read/write/open/close which deal with file descriptors
// // , not file nodes.
// uint32_t read_fs(FS_Node *node, uint32_t offset, uint32_t size, uint8_t* buffer);
// uint32_t write_fs(FS_Node *node, uint32_t offset, uint32_t size, uint8_t* buffer);
// void open_fs(FS_Node *node, bool read, bool write);
// void close_fs(FS_Node *node);
// Dirent* readdir_fs(FS_Node *node, uint32_t index);
// FS_Node* finddir_fs(FS_Node *node, char* name);
