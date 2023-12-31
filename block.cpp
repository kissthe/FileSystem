#include "FS.h"
void Disk::read_file(int i_number) {
    /*
     * 知道具体分配策略才能够知道怎么读
     */
    //for(auto iter:)
}
void Disk::read_dir(int i_number) {
    /*
     * 将目录的信息仿佛缓冲区中
     */
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
}

void Disk::add_index(int i_number) {
    /*
     * 添加子目录的话不用重新分配
     */
    for(int i=0;i<12;i++){
        if(inodes_blocks[i_number].dir_arr[i]== nullptr){
            //这里需要重新找一个空闲块
            //inodes_blocks[i_number].dir_arr[i]=;

            //处理完毕后break

        } else{
            if(inodes_blocks[i_number].dir_arr[i]->index.size()<10){
                //处理，注意此事要写入inode中
            }
        }


    }
}