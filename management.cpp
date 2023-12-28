#include "FS.h"
FileManagement::FileManagement(Inode *root) :ope_ptr(root){};
bool FileManagement::add_index(int i_number,BlockType blockType,string file_name) {
    /*
     * 新建文件夹、文件其实都是相当于让磁盘分配空间，因此直接调用磁盘的函数即可
     */
    disk->allocateBlock(i_number,blockType,file_name);
}
bool FileManagement::recycle_file(int i_number)  {
    disk->modify_file_recycled();
}
string FileManagement::get_dir() {

}