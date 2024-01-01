#include "FS.h"

//传入目录信息，缓存区信息 
int Disk::allocateBlock_Dir( string file_name, vector<Dir_Index>dir_input_buffer) {
    int block_id = -1;
    // 找到一个空闲的inode
    int inode_id = findFreeInode();
    if (inode_id == -1) {
        cout << "没有可用的空闲inode。" << endl;
        return -1;
    }

    // 标记inode为已占用
    i_bitmap[inode_id] = true;
    //目录文件类型
        // 更新inode的属性
    inodes_blocks[inode_id].i_number = inode_id;
    inodes_blocks[inode_id].recycled = false;
    inodes_blocks[inode_id].file_type = 1;
    inodes_blocks[inode_id].file_size = blockSize;
    inodes_blocks[inode_id].file_name = file_name;
    inodes_blocks[inode_id].file_block = 2;

    //如果目录缓冲区有内容，计算文件大小准备分配
   //一个块可以存放 Dir_Index index[10]十条，传入数组20,则需要两个块
  // 为inode分配数据块，目录信息比较少，只需要直接块

    if(dir_input_buffer.size()==0)return inode_id;//如果缓冲区为空（创建一个新的文件夹，其实函数只需要到这里就行了）

    int num_blocks = (dir_input_buffer.size() + 9) / 10; // 计算需要的数据块数量，向上取整

    vector<int> free_blocks = findFreeDataBlocks(num_blocks);

    if (!free_blocks.empty()) {
        // free_blocks 不为空，说明找到了足够的空闲数据块
        // 将数据存储到空闲数据块中，修改数据块信息
        int buffer_index = 0; // 用于遍历 dir_input_buffer
        for (int i = 0; i < free_blocks.size(); i++) {
            int x = free_blocks[i];
            d_bitmap[x] = true; // 数据位图修改为占用
            // 设置块的相关属性
            data_blocks[x].occupied = true;
            data_blocks[x].blockType = INDEX_BLOCK;
            data_blocks[x].block_id = inode_id;
            data_blocks[x].block_size = blockSize; // 256 待定
            for (int j = 0; j < 10 && buffer_index < dir_input_buffer.size(); j++) {
                data_blocks[x].index[j] = dir_input_buffer[buffer_index];
                buffer_index++;
            }
        }
    } else {
        // free_blocks 为空，说明没有找到足够的空闲数据块
        cout << "空闲数据块不足。" << endl;
    }
    return inode_id;
}





