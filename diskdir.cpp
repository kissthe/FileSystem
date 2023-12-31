#include <FS.h>
#include <cstring>
#include <math.h>
using namespace std;

int findFreeInode();
vector<int> findFreeDataBlocks(int num_blocks);

//int allocateBlock_Dir(int i_number, string file_name, Dir_Index dir_input_buffer[20]);
int Disk::allocateBlock_Dir(int i_number,string file_name, Dir_Index dir_input_buffer[20]) {
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
        inodes_blocks[inode_id].file_size = 0;
        inodes_blocks[inode_id].file_name = file_name;
        inodes_blocks[inode_id].file_block = 0;
        

        //如果目录缓冲区有内容，计算文件大小准备分配
      //  int length = dir_input_buffer->length();
      // //如何计算实际目录中有几个文件
        int length=20；//一个目录中最多放20个文件
        int len = length *24；
        if(length){
            inodes_blocks[inode_id].file_size = len;
        }
        //计算一个文件占几个数据块，单个数据块256
        int block_size = 256;
        double num_blocks = (len + block_size - 1) / block_size;
        num_blocks = ceil(num_blocks);

    // 为inode分配数据块，目录信息比较少，只需要直接块
        vector<int> free_blocks = findFreeDataBlocks(num_blocks);
        if (!free_blocks.empty()) {
            // free_blocks 不为空，说明找到了足够的空闲数据块
            //设置两个 Dir_Index数组
            Dir_Index dir_input_buffer1[10];
            Dir_Index dir_input_buffer2[10];
            // 将前十个元素复制到 dir_input_buffer1
            memcpy(dir_input_buffer1, dir_input_buffer, sizeof(Dir_Index) * 10);
            // 将后十个元素复制到 dir_input_buffer2
            memcpy(dir_input_buffer2, dir_input_buffer + 10, sizeof(Dir_Index) * 10);
            // 将数据存储到空闲数据块中，修改数据块信息
            for (int i = 0; i < i < free_blocks.size(); i++) {
                int x = free_blocks[i];
                d_bitmap[x] = true;//标记该数据块已被占用
                int block_index = free_blocks[i];
                Disk_Block& block = disk_blocks[block_index];
                // 设置块的相关属性
                data_blocks[x].occupied = true;
                data_blocks[x].blockType = FILE_BLOCK;
                data_blocks[x].block_id = i_number;
                data_blocks[x].block_size =256;//256待定
                if(i==0）{
               // 将 dir_input_buffer1 的数据复制到第一个数据块的 content
              memcpy(data_blocks[x].content, reinterpret_cast<char*>(dir_input_buffer1), sizeof(Dir_Index) * 10);
                }
                    if (i == 1) {
               // 将 dir_input_buffer2 的数据复制到第二个数据块的 content
                        memcpy(data_blocks[x].content, reinterpret_cast<char*>(dir_input_buffer2), sizeof(Dir_Index) * 10);
                           
              }
                // 更新inode的磁盘指针,将分配的数据块的指针添加到对应的inode的磁盘指针中
                inodes_blocks[inode_id].disk_pointer.push_back(&data_blocks[x]);
            }

            // 在这里进行其他操作，例如更新文件的元数据等
        }
        else {
            // free_blocks 为空，说明没有找到足够的空闲数据块
            cout << "空闲数据块不足。" << endl;
        }


    


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

void Disk::read_file(int i_number) {
    // 遍历 inodes_blocks 数组
    for (int i = 0; i < INODE_NUM; i++) {
        // 找到与 i_number 匹配的 inode
        if (inodes_blocks[i].i_number == i_number) {
            // 检查 inode 是否有足够的磁盘指针
            if (inodes_blocks[i].disk_pointer.size() >= 2) {
                // 获取第一个数据块的位置
                Disk_Block * block1 = inodes_blocks[i].disk_pointer[0];
                // 获取第二个数据块的位置
                Disk_Block*  block2 = inodes_blocks[i].disk_pointer[1];
                Dir_Index dir_input_buffer1[10];
                Dir_Index dir_input_buffer2[10];
                memcpy(dir_input_buffer1, block1.content, sizeof(Dir_Index) * 10);
                memcpy(dir_input_buffer2, block2.content, sizeof(Dir_Index) * 10);
                  // 创建两个临时数组 dir_input_buffer1 和 dir_input_buffer2，用于存储从第一个数据块和第二个数据块中提取的数据。
                 //   从第一个数据块的 content 中提取数据，并将其存储到 dir_input_buffer1 中。
                //    从第二个数据块的 content 中提取数据，并将其存储到 dir_input_buffer2 中。
               //dir_input_buffer1 和 dir_input_buffer2 数组中分别存储了从两个数据块中提取的 content 数据。
               //  输出当前目录下的文件名和代号
                    for (int j = 0; j < 10; j++) {
                        cout << "Name: " << dir_input_buffer1[j].name << ", i_number: " << dir_input_buffer1[j].i_number: << endl;
                    }

                    // 输出 dir_input_buffer2 数组中的内容
                    cout << "dir_input_buffer2:" << endl;
                    for (int j = 0; j < 10; j++) {
                        cout << "Name: " << dir_input_buffer2[j].name << ", i_number:" << dir_input_buffer2[j].i_number: << endl;
                    }
            }
            else {
                // inode 没有足够的磁盘指针，无法获取两个数据块的位置
                cout << "inode 没有足够的磁盘指针，无法获取数据块的位置" << endl;
            }
            return; // 找到匹配的 inode，结束函数
        }
    }

    // 没有找到与 i_number 匹配的 inode
    cout << "没有找到与 i_number 匹配的 inode" << endl;
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

