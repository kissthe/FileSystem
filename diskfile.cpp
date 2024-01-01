#include "FS.h"
/*
 * 修改意见：
 * 1.直接块儿分配没有问题，但是间接块儿可能会有点问题：就是说间接块儿其实就是一个装满指针的块儿，同一个文件的inode是相同的，因此不用再分配inode
 * 2.其实据我理解你那个索引块就是我们需要的间接块儿的效果。。怪我当时把那个枚举类型命名为索引块。。。我其实想表达的是“索引块儿”是指的那个block里面存放的是目录
 * 也就是说你分配索引块儿的那个功能基本上就是我们想要的分配间接块儿的效果，因此我把那个pointer_to_pointer那个数组注释掉了
 * 3.i_number号和inode数组对应的下标一样，我当时也没说清楚，我的锅
 * 4.就是如果哪里还有问题的话可以问我
 *
 */
using namespace std;

// 函数声明区
vector<int> findFreeInodes(int num_inodes);
int findFreeDataBlock();
vector<int> findFreeDataBlocks(int num_blocks);

// 文件的分配函数
int Disk::allocateBlock_File(string file_name, string* input_buffer)
{
    if(input_buffer== nullptr){
        cout<<"pointer to input_buffer is null"<<endl;
        exit(0);
    }
    int block_id = -1;
    int length = input_buffer->length(); // 缓冲区数据长度 
    //int block_size = 256; // 单个数据块256字节
    int block_size = blockSize;

    double num_blocks = (length + block_size - 1) / block_size; // 计算需要多少个数据块
    int num_blocks_rounded = ceil(num_blocks); // 向上取整

    // 判断需要分配几个inode

    // 默认分配一个inode,一个inode可以记录12个直接块
    int num_inodes = 1;
    if (num_blocks_rounded > 12)
    {
        // 需要分配一个间接块的inode，一个存12个直接块的inode
        num_inodes = 2;
    }
    if (num_blocks_rounded > 12 + 32)
    {
        // 需要分配一个索引块的inode，一个存12个直接块的inode,多个间接块的inode
        num_inodes = 1 + 1 + ceil((num_blocks_rounded - 12) / 32.0);
    }

    // 分配所需的inode
    vector<int> inode_ids = findFreeInodes(num_inodes);
    if (inode_ids.size() < num_inodes)
    {
        cout << "没有足够的可用空闲inode。" << endl;
        return -1;
    }

    for (int i = 0; i < num_inodes; i++)
    {
        int inode_id = inode_ids[i];

        // 标记inode为已占用
        i_bitmap[inode_id] = true;

        // 更新inode的属性
        inodes_blocks[inode_id].i_number = i_number;
        inodes_blocks[inode_id].recycled = false;
        inodes_blocks[inode_id].file_type = 0;//文件类型
        inodes_blocks[inode_id].file_size = 0;
        inodes_blocks[inode_id].file_name = file_name;
        //inodes_blocks[inode_id].file_block = 0;

  //}

  //---------------------分配文件内容------------------------------

        // 如果缓冲区有内容，计算文件大小准备分配
        int length = input_buffer->length();

        if (length) {
            inodes_blocks[inode_id].file_size = length;
        }

        // 分配数据块
        int remaining_blocks = num_blocks_rounded;//数据块的个数
        int block_offset = 0;
        int count = 0;

        // 分配数据块
        //int remaining_blocks = num_blocks_rounded;
        int num_blocks_to_allocate = min(remaining_blocks, 12); // 最多分配12个直接块

        // 分配 num_blocks_to_allocate 个直接块
        vector<int> block_ids = findFreeDataBlocks(num_blocks_to_allocate);

        if (block_ids.size() < num_blocks_to_allocate)
        {
            cout << "没有足够的可用空闲数据块。" << endl;
            return -1;//退出该函数
        }

        for (int i = 0; i < num_blocks_to_allocate; i++)
        {
            int block_id = block_ids[i];

            // 标记数据块为已占用
            d_bitmap[block_id] = true;

            // 更新数据块的属性
            data_blocks[block_id].occupied = true;
            data_blocks[block_id].blockType = FILE_BLOCK;
            //data_blocks[block_id].block_id = i_number;
            //data_blocks[block_id].block_size = blockSize;

            // 将文件内容从输入缓冲区复制到数据块中
            if (input_buffer != nullptr) {
                // 假设 content 是数据块的内容缓冲区
                strcpy(data_blocks[block_id].content, input_buffer->c_str());
                inodes_blocks[inode_id].file_size = input_buffer->size();
            }

            // 更新inode的磁盘指针，将分配的数据块的指针添加到对应的inode的磁盘指针中
            inodes_blocks[inode_id].direct_block[i] = &data_blocks[block_id];
        }

        remaining_blocks -= num_blocks_to_allocate;

        if (num_inodes == 2 && remaining_blocks > 0) //需要分配间接块，不需要分配索引块
        {
            int inode_id2 = inode_ids[1];//用之前找出的空闲inode块的数组中的第二个 
            // 标记inode为已占用
            i_bitmap[inode_id2] = true;
            // 更新inode的属性
            inodes_blocks[inode_id2].i_number = i_number;
            inodes_blocks[inode_id2].recycled = false;
            inodes_blocks[inode_id2].file_type = 0;//文件类型
            inodes_blocks[inode_id2].file_name = file_name;
            inodes_blocks[inode_id2].file_size = 0;
            // 分配一个间接块
            int index_block_id2 = findFreeDataBlock();
            if (index_block_id2 == -1) {
                cout << "没有可用的空闲数据块。" << endl;
                return -1;
            }
            // 标记数据块为已占用
            d_bitmap[index_block_id2] = true;

            int indirect_block_id2=index_block_id2;
            //修改间接块的信息
            data_blocks[indirect_block_id2].occupied = true;
            data_blocks[indirect_block_id2].blockType = INDIRECT_BLOCK; // 间接块
            data_blocks[indirect_block_id2].block_id = i_number;
            data_blocks[indirect_block_id2].block_size = blockSize;

            // 将分配的间接块的指针添加到inode块信息中
            inodes_blocks[1].disk_pointer.push_back(&data_blocks[indirect_block_id2]);
            //分配数据块

            //1.计算需要几个数据块存储数据
            int num_blocks_to_allocate2 = min(remaining_blocks, 32); // 最多分配32个数据块

            // 分配 num_blocks_to_allocate 个间接块
            vector<int> block_ids = findFreeDataBlocks(num_blocks_to_allocate2);
            if (block_ids.size() < num_blocks_to_allocate2)
            {
                cout << "没有足够的可用空闲数据块。" << endl;
                return -1;
            }

            for (int i = 0; i < num_blocks_to_allocate2; i++)
            {
                int block_id2 = block_ids[i];

                // 标记数据块为已占用
                d_bitmap[block_id2] = true;

                // 更新数据块的属性
                data_blocks[block_id2].occupied = true;
                data_blocks[block_id2].blockType = FILE_BLOCK;
                data_blocks[block_id2].block_id = i_number;
                data_blocks[block_id2].block_size = blockSize;

                // 将文件内容从输入缓冲区复制到数据块中
                if (input_buffer != nullptr) {
                    // 假设 content 是数据块的内容缓冲区
                    strcpy(data_blocks[block_id2].content, input_buffer->c_str());
                    inodes_blocks[inode_id2].file_size = input_buffer->size();

                }

                // 将分配的数据块的指针添加到间接块中
                data_blocks[indirect_block_id2].indirect_block[i] = &data_blocks[block_id2];
            }

            remaining_blocks -= num_blocks_to_allocate2;


        }

        if (num_inodes > 2 && remaining_blocks > 0) //需要分配索引块
        {
            // 分配剩余的数据块，这些数据块需要通过间接块中的指针来指向
            remaining_blocks -= 12;
            int num_indirect_blocks = ceil(remaining_blocks / 32.0); // 计算需要的间接块数

            // 分配一个索引块
            int index_block_id = findFreeDataBlock();
            if (index_block_id == -1) {
                cout << "没有可用的空闲数据块。" << endl;
                return -1;
            }
            // 标记数据块为已占用
            d_bitmap[index_block_id] = true;

            // 更新数据块的属性
            data_blocks[index_block_id].occupied = true;
            data_blocks[index_block_id].blockType = INDEX_BLOCK; // 索引块
            data_blocks[index_block_id].block_id = i_number;
            data_blocks[index_block_id].block_size = blockSize;

            // 创建一个新的inode来存放索引块的信息
            int index_inode_id = findFreeInode();
            if (index_inode_id == -1) {
                cout << "没有可用的空闲inode。" << endl;
                return -1;
            }
            // 标记inode为已占用
            i_bitmap[index_inode_id] = true;

            // 更新inode的属性
            inodes_blocks[index_inode_id].i_number = i_number;
            inodes_blocks[index_inode_id].recycled = false;
            inodes_blocks[index_inode_id].file_type = 0;//文件类型
            inodes_blocks[index_inode_id].file_name = file_name;
            inodes_blocks[index_inode_id].file_size = num_indirect_blocks;

            // 将分配的索引块的指针添加到对应的inode的磁盘指针中
            inodes_blocks[inode_id].disk_pointer_pointer.push_back(&data_blocks[index_block_id]);

            // 分配间接块并将指针添加到索引块中
            for (int i = 0; i < num_indirect_blocks; i++)
            {
                // 分配一个间接块
                int indirect_block_id = findFreeDataBlock();
                if (indirect_block_id == -1) {
                    cout << "没有可用的空闲数据块。" << endl;
                    return -1;
                }
                // 标记数据块为已占用
                d_bitmap[indirect_block_id] = true;

                // 更新数据块的属性
                data_blocks[indirect_block_id].occupied = true;
                data_blocks[indirect_block_id].blockType = INDIRECT_BLOCK; // 间接块
                data_blocks[indirect_block_id].block_id = i_number;
                data_blocks[indirect_block_id].block_size = blockSize;

                // 将分配的间接块的指针添加到索引块中
                inodes_blocks[index_inode_id].disk_pointer.push_back(&data_blocks[indirect_block_id]);
            }

            for (int i = 0; i < num_indirect_blocks; i++)
            {
                for (int j = 0; j < min(remaining_blocks, 32); j++)
                {
                    // 分配一个数据块
                    int data_block_id = findFreeDataBlock();
                    if (data_block_id == -1) {
                        cout << "没有可用的空闲数据块。" << endl;
                        return -1;
                    }
                    // 标记数据块为已占用
                    d_bitmap[data_block_id] = true;

                    // 更新数据块的属性
                    data_blocks[data_block_id].occupied = true;
                    data_blocks[data_block_id].blockType = FILE_BLOCK;
                    data_blocks[data_block_id].block_id = i_number;
                    data_blocks[data_block_id].block_size = blockSize;

                    // 将文件内容从输入缓冲区复制到数据块中
                    if (input_buffer != nullptr) {
                        strcpy(data_blocks[data_block_id].content, input_buffer->c_str());
                        inodes_blocks[inode_id].file_size = input_buffer->size();
                    }

                    // 将分配的数据块的指针添加到间接块中
                    int indirect_block_id = inodes_blocks[index_inode_id].disk_pointer[i]->block_id;
                    inodes_blocks[index_inode_id].disk_pointer[i]->indirect_block[j] = &data_blocks[data_block_id];
                }

                remaining_blocks -= 32;
            }
        }
    }
}

/*分配函数中用到的子函数（查询空闲磁盘块）*/
//找空闲inode 的函数
int Disk::findFreeInode() {
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
vector<int> Disk::findFreeInodes(int num_inodes)
{
    vector<int> free_inodes;

    // 遍历inode位视图
    for (int i = 0; i < I_BMAP_NUM; i++) {
        // 如果找到一个空闲的inode
        if (!i_bitmap[i]) {
            // 将inode的索引添加到结果中
            free_inodes.push_back(i);

            // 如果已经找到足够数量的inode，结束循环
            if (free_inodes.size() == num_inodes) {
                break;
            }
        }
    }

    // 如果没有找到足够数量的空闲inode，返回一个空的向量
    if (free_inodes.size() < num_inodes) {
        free_inodes.clear();
    }

    return free_inodes;
}

//找空闲数据块 的函数
int Disk::findFreeDataBlock()
{
    // 遍历数据块位视图
    for (int i = 0; i < D_BMAP_NUM; i++) {
        // 如果找到一个空闲的数据块
        if (!d_bitmap[i]) {
            // 返回数据块的索引
            return i;
        }
    }

    // 没有找到空闲的数据块
    return -1;
}

vector<int> Disk::findFreeDataBlocks(int num_blocks)
{
    /*
     * 在位视图中寻找空余的块儿，找到足够的块儿的话就返回这些块儿的号
     */
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