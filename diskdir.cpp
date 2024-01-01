#include "FS.h"


//传入目录信息，缓存区信息 
int Disk::allocateBlock_Dir( string file_name, Dir_Index dir_input_buffer[20]) {
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
    inodes_blocks[inode_id].i_number = i_number;
    inodes_blocks[inode_id].recycled = false;
    inodes_blocks[inode_id].file_type = 1;
    inodes_blocks[inode_id].file_size = blockSize;
    inodes_blocks[inode_id].file_name = file_name;
    inodes_blocks[inode_id].file_block = 2;
    //如果目录缓冲区有内容，计算文件大小准备分配
   //一个块可以存放 Dir_Index index[10]十条，传入数组20,则需要两个块
  // 为inode分配数据块，目录信息比较少，只需要直接块
    int num_blocks = 2;
    vector<int> free_blocks = findFreeDataBlocks(num_blocks);//取得两个块儿
    if (!free_blocks.empty()) {
        // free_blocks 不为空，说明找到了足够的空闲数据块
        // 将数据存储到空闲数据块中，修改数据块信息
        for (int i = 0; i < free_blocks.size(); i++) {
            int x = free_blocks[i];
            d_bitmap[x] = true;//数据位图修改为占用 
            // 设置块的相关属性
            data_blocks[x].occupied = true;
            data_blocks[x].blockType = INDEX_BLOCK;
            data_blocks[x].block_id = i_number;
            data_blocks[x].block_size = blockSize;//256待定
            if (i == 0){
                // 将前十个元素赋值给数据块1的 Dir_Index index[10]
                 for (int j = 0; j < 10; j++) {
                     data_blocks[x].index[j] = dir_input_buffer[j];
                 }

                 inodes_blocks[inode_id].direct_block[0] = &data_blocks[x];//指向直接块 

                }
                if (i == 1) {
                    // 将后十个元素赋值给第二个数据块的 Dir_Index index[10]
                    for (int j = 0; j < 10; j++) {
                        data_blocks[x].index[j] = dir_input_buffer[j + 10];
                    }
                    inodes_blocks[inode_id].direct_block[1] = &data_blocks[x];//指向直接块 


                    // 在这里进行其他操作，例如更新文件的元数据等
                }
        }

    }
    else {
        // free_blocks 为空，说明没有找到足够的空闲数据块
        cout << "空闲数据块不足。" << endl;
    }
    return inode_id;
}


void Disk::read_file(int i_number) {//读目录信息
    // 遍历 inodes_blocks 数组
        // 找到与 i_number 匹配的 inode
            // 读取第一个数据块的 Dir_Index 数组内容
            Disk_Block* block1 = inodes_blocks[i_number].direct_block[0];
            for (int j = 0; j < 10; j++) {
                Dir_Index index = block1->index[j];
                cout << "Name: " << index.name << ",i_number: " << index.i_number << endl;
            }

            // 读取第二个数据块的 Dir_Index 数组内容
            Disk_Block* block2 = inodes_blocks[i_number].direct_block[1];
            for (int j = 0; j < 10; j++) {
                Dir_Index index = block2->index[j];
                cout << "Name: " << index.name << " i_number: " << index.i_number << endl;
            }

        // 没有找到与 i_number 匹配的 inode
        cout << "没有找到与 i_number 匹配的 inode,该文件不存在" << endl;


}


bool modify_file_name(int i_number, string new_name) {
    // 遍历 inodes_blocks 数组
    for (int i = 0; i < INODE_NUM; i++) {
        // 找到与 i_number 匹配的 inode
        if (inodes_blocks[i].i_number == i_number) {
            // 比较新的文件名与当前文件名是否相同
            if (inodes_blocks[i].name == new_name) {//不知道这个文件属于哪个文件夹，所以无法判断在同一个文件夹下，有没有同名的文件，bug
                cout << "新文件名与当前文件名相同，无需修改。" << endl;
                return false;
            }

            // 修改文件名
            inodes_blocks[i].name = new_name;
            cout << "文件名修改成功。" << endl;
            return true;
        }
    }

    // 没有找到与 i_number 匹配的 inode
    cout << "没有找到与 i_number 匹配的 inode" << endl;
    return false;
}





