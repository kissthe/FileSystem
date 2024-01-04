#include "FS.h"
string Disk::get_file_name(int i_number) {
    return inodes_blocks[i_number].file_name;
}
void Disk::read_file(int i_number) {
    for(auto i:inodes_blocks[i_number].direct_block){
        if(i!= nullptr){
            output_buffer+=i->content;
        }
    }
    if(inodes_blocks[i_number].disk_pointer.size()==1){
        for(int i=0;inodes_blocks[i_number].disk_pointer[0]->indirect_block[i]!= nullptr;i++){
            output_buffer+=inodes_blocks[i_number].disk_pointer[0]->indirect_block[i]->content;
        }
    }else{
        /*
         * 存在二级索引的情况
         */
        for(int i=1;i<inodes_blocks[i_number].disk_pointer.size();i++){
            for(int j=0;j<32&&inodes_blocks[i_number].disk_pointer[i]->indirect_block[j]!= nullptr;j++){
                output_buffer+=inodes_blocks[i_number].disk_pointer[i]->indirect_block[j]->content;
            }
        }
    }

}
void Disk::read_dir(int i_number) {
    /*
     * 将目录的信息放入缓冲区中
     */

    parent_inode_number=inodes_blocks[i_number].direct_block[0]->index[0];//记录一下上级节点
    for(auto i:inodes_blocks[i_number].direct_block){
        if(i!= nullptr){
            for(int j=0;j<i->index_number;j++){
                if(i->index[j]!=parent_inode_number){//这样的话就不会把上级节点放入缓冲区中
                        dir_output_buffer.push_back(inodes_blocks[i->index[j]]);//放入目录对应的缓冲区中
                }
            }
        }

    }
}
string Disk::modify_time() {
    // 获取当前时间点
    auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // 将时间点转换为本地时间结构
    std::tm* localTime = std::localtime(&currentTime);

    // 创建一个字符数组来存储时间字符串
    const int bufferSize = 80;
    char buffer[bufferSize];

    // 使用 std::strftime 将时间结构格式化为字符串
    std::strftime(buffer, bufferSize, "%Y/%m/%d %H:%M", localTime);
    return buffer;//返回当前日期时间字符串
}

bool Disk::modify_file_name(int i_number, string new_name) {
    /*
     * 把对应文件/文件夹的inode信息更改
     */
    inodes_blocks[i_number].file_name=new_name;
    return true;
}

void Disk::add_index_in_dir(int pi_number,int ci_number) {
    /*
     * 让pi_number的目录下面有ci_number的条目
     * 这俩number都是inode号
     */

    for(int i=0;i<12;i++){
        //if(inodes_blocks[pi_number].direct_block[i]!= nullptr){}
        //int count = inodes_blocks[pi_number].direct_block[i]->index_number;//当前磁盘块儿存放的目录数量

        if(inodes_blocks[pi_number].direct_block[i]== nullptr){

            //这里需要重新找一个空闲块并且指向它，并在空闲块儿中添加目录
            inodes_blocks[pi_number].direct_block[i]=&data_blocks[findFreeDataBlock()];
            //
            inodes_blocks[pi_number].direct_block[i]->index[0]=ci_number;//把新创建的目录，文件的i_number号加入进去
            //
            inodes_blocks[pi_number].direct_block[i]->index_number++;//存放目录的数量+1
            break;
        } else if(inodes_blocks[pi_number].direct_block[i]->index_number<64){
            /*
             * 如果块儿不是空的话，试着在他里面填充一个新的目录
             */
            int count = inodes_blocks[pi_number].direct_block[i]->index_number;

            inodes_blocks[pi_number].direct_block[i]->index[count]=ci_number;

            inodes_blocks[pi_number].direct_block[i]->index_number++;
            count++;

            break;
        } else cout<<"add index in dir failed"<<endl;
    }
    inodes_blocks[pi_number].modified_time=modify_time();//更改上级目录的修改时间
    //cout<<"add index in dir success!"<<endl;
}
void Disk::delete_index_in_dir(int pi_number, int ci_number) {
    /*
     * 删除pi_number下面ci_number对应的条目
     */
    for(int i=0;i<12;i++){
        if(inodes_blocks[pi_number].direct_block[i]== nullptr)return;
        int count = inodes_blocks[pi_number].direct_block[i]->index_number;//当前磁盘块儿存放的目录数量
            /*
             * 一个一个指针进行条目的查找
             */
            for(int j=0;j<count;j++){
                if(inodes_blocks[pi_number].direct_block[i]->index[j]==ci_number){
                    /*
                     * 找到匹配的了，需要删除该元素，并把后面的元素往前移动一格
                     */
                    for(int k=j;k<count-1;k++){
                        inodes_blocks[pi_number].direct_block[i]->index[k]=inodes_blocks[pi_number].direct_block[i]->index[k+1];
                    }
                    inodes_blocks[pi_number].direct_block[i]->index_number--;
                    break;
                }
            }

    }
}


bool Disk::modify_file_recycled(int i_number) {
    inodes_blocks[i_number].recycled= true;//改为回收状态
}

int Disk::get_CurDirChild_number(int i_number) {
    int sum=1;
    for(auto i:inodes_blocks[i_number].direct_block){
        if(i!= nullptr)sum+=i->index_number-1;
    }
    return sum;
}

Disk::Disk(int blockCount,int blockSize) : blockCount(blockCount), blockSize(blockSize) {
    // 初始化位示图,根目录一定要初始化了
    i_bitmap[0]= 1;
    for (int i = 1; i < I_BMAP_NUM; ++i) {
        i_bitmap[i] = 0;
    }
    d_bitmap[0]= 1;
    for (int i = 1; i < D_BMAP_NUM; ++i) {
        d_bitmap[i] = 0;
    }

    //初始化inode区域,要有根目录的文件头
    inodes_blocks[0].file_name="/";
    inodes_blocks[0].i_number=0;
    inodes_blocks[0].recycled= false;
    inodes_blocks[0].modified_time="2024/01/01 00:00";
    inodes_blocks[0].direct_block[0]=&data_blocks[0];//默认把第一块儿分给根目录
    inodes_blocks[0].file_type=DIRECTORY_TYPE;
    //初始化数据区域，要有根目录存在
    data_blocks[0].index_number=1;//存放了根目录，因此数量为1
    data_blocks[0].occupied= true;
    data_blocks[0].blockType=INDEX_BLOCK;
    data_blocks[0].index[0]=0;//根目录的上级目录还是根目录

    for (int i = 1; i < DATA_BLOCK_NUM; ++i) {
        data_blocks[i].occupied = false;
        /*
         * 把存放数据的区域全部置为0
         */
        memset(data_blocks[i].content,0,256);
        memset(data_blocks[i].index,0,64);
        memset(data_blocks[i].indirect_block, 0,32);
        // 根据需要对其他字段进行初始化
    }

    // 初始化回收站数组
    for (int i = 0; i < 20; ++i) {
        arr[i] = 0;
    }

}


Disk::~Disk() {
    // 在析构函数中进行资源清理工作，如果有的话
    // 例如，释放动态分配的内存等
    // 根据需要，在析构函数中添加其他清理逻辑
}
