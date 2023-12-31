#include <FS.h>
#include <cstring>
#include <math.h>
using namespace std;

int findFreeInode();
vector<int> findFreeDataBlocks(int num_blocks);

int Disk::allocateBlock(int i_number, BlockType type, string file_name, string* input_buffer) {
    int block_id = -1;

    // 找到一个空闲的inode
    int inode_id = findFreeInode();
    if (inode_id == -1) {
        cout << "没有可用的空闲inode。" << endl;
        return -1;
    }
    
    // 标记inode为已占用
    i_bitmap[inode_id] = true;
    
    //根据文件类型，选择不同分配策略
    //type == FILE_BLOCK,即文件类型
    if(type == FILE_BLOCK){
        // 更新inode的属性
        inodes_blocks[inode_id].i_number = i_number;
        inodes_blocks[inode_id].recycled = false;
        inodes_blocks[inode_id].file_type = (type == FILE_BLOCK);
        inodes_blocks[inode_id].file_size = 0;
        inodes_blocks[inode_id].file_name = file_name;
        inodes_blocks[inode_id].file_block = 0;
        
        //如果缓冲区有内容，计算文件大小准备分配
        int length = input_buffer->length();

        if(length){
            inodes_blocks[inode_id].file_size = length;
        }
        //计算一个文件占几个数据块，单个数据块256
        int block_size = 256;
        double num_blocks = (length + block_size - 1) / block_size;
        num_blocks = ceil(num_blocks);
    }

    // 修改时间(没写)

    // 为inode分配一个数据块（还要考虑多个数据块，用直接块就行，还是还需要间接块还是索引块）
    //block_id = findFreeDataBlocks();
    if (block_id == -1) {
        cout << "没有可用的空闲数据块。" << endl;
        return -1;
    }
    
    // 标记数据块为已占用
    d_bitmap[block_id] = true;
    
    // 更新数据块的属性
    data_blocks[block_id].occupied = true;
    data_blocks[block_id].type = 0;
    data_blocks[block_id].blockType = type;
    data_blocks[block_id].block_id = i_number;
    data_blocks[block_id].block_size = blockSize;
    
    // 如果是文件块，将内容从输入缓冲区复制到数据块中
    if (type == FILE_BLOCK && input_buffer != nullptr) {
        strcpy(data_blocks[block_id].content, input_buffer->c_str());
        inodes_blocks[inode_id].file_size = input_buffer->size();
    }
    
    // 更新inode的磁盘指针,将分配的数据块的指针添加到对应的inode的磁盘指针中
    inodes_blocks[inode_id].disk_pointer.push_back(&data_blocks[block_id]);
    


    //找空闲inode 的函数
    int findFreeInode() {
    // 遍历inode位视图
    for (int i = 0; i < I_BMAP_NUM; i++) {
        // 如果找到一个空闲的inode
        if (!i_bitmap[i]) {
            // 返回inode的索引
            return i;
        }
    }
    
    // 没有找到空闲的inode
    return -1;
}

    //找空闲数据块 的函数
    vector<int> findFreeDataBlocks(int num_blocks) {
        vector<int> free_blocks;
    
    // 遍历数据块位视图
    for (int i = 0; i < D_BMAP_NUM; i++) {
        // 如果找到一个空闲的数据块
        if (!d_bitmap[i]) {
            // 将数据块索引添加到空闲块列表中
            free_blocks.push_back(i);
            
            // 如果已经找到了足够的空闲块
            if (free_blocks.size() == num_blocks) {
                // 返回所有相关的数据块索引
                return free_blocks;
            }
        }
    }
    
    // 如果没有找到足够的空闲块，则返回一个空的列表
    return vector<int>();
}
    
    return inode_id;
}




