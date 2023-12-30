#include "FS.h"

bool FileManagement::create(int i_number,BlockType blockType,string file_name) {
    /*
     * 新建文件夹、文件其实都是相当于让磁盘分配空间，因此直接调用磁盘的函数即可
     */
    disk->allocateBlock(i_number,blockType,file_name);
}
bool FileManagement::remove(string file_name, int i_number) {
    /*
     * 回收磁盘空间，删除对应inode节点
     */
}
bool FileManagement::recycle_file(int i_number)  {
    disk->modify_file_recycled();
}
string FileManagement::get_dir_content(int i_number) {
    disk->read_file(i_number);
    for(auto i:dir_input_buffer){
        dir_content.append(i.file_name);
        dir_content.push_back('\t');
        dir_content.append(i.modified_time);
        dir_content.push_back('\t');
        //dir_content.append();此处需要增加文件类型
        dir_content.push_back('\n');
    }
    return dir_content;
}
string FileManagement::get_file_content() {
    for (auto i:input_buffer) {
        file_content.push_back(i);
    }
    return file_content;
}

string FileManagement::cd_dir(string file_name) {
    /*
     * 根据文件夹名称进入相应的目录
     * 返回的就是进入目录的相关信息，用string存储，可以在GUI端显示
     */
    for(auto i:dir_input_buffer){
        if(i.file_name==file_name){
            /*
             * 在这里要修改一下当前目录和上级目录
             */
        }
    }
}

void FileManagement::sort_index() {
    /*
     * 使用STL算法，按照关键字进行快速排序
     */
}

void FileManagement::add_index() {
    /*
     * 增加索引项，可以使用插入排序进行插入
     */
}

void FileManagement::delete_index() {
    /*
     * 删除索引项，可以使用二分查找并且删除
     */
}

void FileManagement::print_current_dir() {
    cout<<current_dir;
}



