// #include <kernel/fs.h>


// FS_Node* fs_root = 0; // The root of the filesystem.

// uint32_t read_fs(FS_Node* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
//     // Has the node got a read callback?
//     if (node->read != 0) {
//         return node->read(node, offset, size, buffer);
//     } else {
//         return 0;
//     }
// }

// uint32_t write_fs(FS_Node* node, uint32_t offset, uint32_t size, uint8_t *buffer) {
//     // Has the node got a write callback?
//     if (node->write != 0)  {
//         return node->write(node, offset, size, buffer);
//     } else {
//         return 0;
//     }
// }

// void open_fs(FS_Node* node, bool read, bool write) {
// 	UNUSED(read);
// 	UNUSED(write);

//     // Has the node got an open callback?
//     if (node->open != 0) {
//         return node->open(node);
//     }
// }

// void close_fs(FS_Node* node) {
//     // Has the node got a close callback?
//     if (node->close != 0) {
//         return node->close(node);
//     }
// }

// Dirent* readdir_fs(FS_Node* node, uint32_t index) {
//     // Is the node a directory, and does it have a callback?
//     if ((node->flags&0x7) == FS_DIRECTORY && node->readdir != 0) {
//     	return node->readdir(node, index);
//     } else {
//         return 0;
//     }
// }

// FS_Node* finddir_fs(FS_Node* node, char *name) {
//     // Is the node a directory, and does it have a callback?
//     if ((node->flags&0x7) == FS_DIRECTORY && node->finddir != 0 ) {
//         return node->finddir(node, name);
//     } else {
//         return 0;
//     }
// }
