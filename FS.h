#ifndef FILESYS_FS_H
#define FILESYS_FS_H
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <cctype>
#include <cstdlib>
#define INODE_NUM 100
#define I_BMAP_NUM 100
#define D_BMAP_NUM 512
#define DATA_BLOCK_NUM 512
#define BLOCK_SIZE 256
using namespace std;
enum BlockType { INODE_BLOCK, FILE_BLOCK, INDIRECT_BLOCK, INDEX_BLOCK };//定义枚举类型，inode、文件、间接块儿、索引块儿
enum FileType {FILE_TYPE,DIRECTORY_TYPE};

//struct Dir_Index{
//    //char name[20];
//    int index;
//};//目录里面存放的单元

struct Disk_Block{
    bool occupied;
    BlockType blockType;//磁盘存储的是哪种类型的数据
    int block_id;//这一块儿属于哪一个文件
    int block_size;
    int index_number;//意思就是存储的index的数量，默认为0
    union {
        char content[256];//存储文件的内容
        Disk_Block* indirect_block[32];  //存储间接块儿
        int index[64];   //存放目录内容,这里不容易知道存放多少个
    };
};//磁盘块儿

struct Inode{
    int i_number;//文件的低级名称
    bool recycled;//文件是否放入回收站，放入回收站的话就无法读取
    FileType file_type;//文件类型
    int file_size;//文件大小
    string file_name;//文件名
    string modified_time;//修改时间
    int file_block;//分配了多少块儿
    vector<Disk_Block*>disk_pointer;//磁盘指针，会有指向间接块儿的指针
    //vector<Disk_Block*>disk_pointer_pointer;//磁盘指针，指向索引块指针
    Disk_Block* direct_block[12];//指向直接块
};//inode节点
class Disk;

class FileManagement{
private:
    /*要有一个变量存放当前目录，进入子目录、返回上层目录时需要对应改变
     * 比如可以用当前的目录的inode节点
     */
    int ope_inode;//当前的inode号
    int parent_inode;//上一级inode号

    Disk* disk;//磁盘指针
    string dir_content;//存放得到的目录项信息
    string file_content;//存放得到的文件内容
    string current_dir;//当前处于的路径

    Inode copy_tmp;//临时变量
    vector<Inode>copy_number;//拷贝的文件头
    string copy_file_content;//拷贝的文件内容

    bool is_cut;
    int cut_parent_number;//剪切专用



public:
    /*
     * 对文件,目录的一些操作，主要是更改inode属性
     * 新建，修改文件的时候需要对应修改inode属性
     */
    string input_buffer;//输入缓冲区
    //Inode dir_input_buffer[20];//目录项输入缓冲区,增加目录时注意也要在当前目录下增加条目
    vector<Inode> dir_input_buffer;

    FileManagement(Disk* disk);//构造函数
    bool remove(string file_name);
    bool create(FileType file_type, string file_name);
    string get_file_content(string  file_name);

    void print_sort_dir(int choice);//以不同排序方式呈现


    void cd_dir(string file_name);//进入目录

    bool recycle_file(string file_name);//放入回收站

    void print_dir_details();//打印出来当前的目录


    void print_current_dir();//打印出当前的工作目录
    bool rename_file(string old_name,string new_name);

    void show_disk_status();

    void sort_index(int choice,vector<Inode>&tmp);

    void update_buffer();//更新操作，每次读之前都得用该操作一次
    void go_back();//返回上一级目录
    void update_file_buffer(int file_number);
    void copy_directory();
    void copy_file();
    void copy(string file_name);//复制
    void paste();//粘贴
    void cut(string file_name);//剪切
};

class Disk {
private:

    const int blockCount;  // 磁盘块数量（常量，不可修改）
    const int blockSize;   // 每个磁盘块的大小（常量，不可修改）
    int i_bitmap[I_BMAP_NUM];
    int d_bitmap[D_BMAP_NUM];//位示图
    Disk_Block data_blocks[DATA_BLOCK_NUM];  // 固定大小的数组，模拟磁盘块
    Inode inodes_blocks[INODE_NUM];//固定大小的数组，模拟存放inodes的磁盘块儿
    int arr[20]={0};//回收站，如果哪个节点被暂时删除，就把它的inode中的recycled字段置为true，并把inode号放入回收站

public:
    string output_buffer;//读取的文件内容放在这里就行,输出缓冲区
    vector<Inode> dir_output_buffer;//读取的目录项放在这里
    int parent_inode_number;//存储上级的i_number号

    Disk(int blockCount,int blockSize) ;
    ~Disk();

    int allocateBlock_File(string file_name,string* input_buffer= nullptr);//返回inode号，分配文件
    int allocateBlock_Dir( string file_name, vector<Inode>dir_input_buffer,int pi_number);//返回inode号，分配目录
    bool deleteBlock(int i_number);
    string modify_time();//修改文件时间，只有保存到磁盘的时候才需要修改
    void displayDiskStatus();
    bool modify_file_name(int i_number,string new_name);//更改名称
    bool modify_file_recycled(int i_number);//把recycled改为true
    void read_file(int i_number);//读取普通文件
    void read_dir(int i_number);//读取目录项
    int get_CurDirChild_number(int i_number);//获取当前目录下的元素个数

    int findFreeDataBlock();
    vector<int> findFreeInodes(int num_inodes);
    int findFreeInode();
    vector<int> findFreeDataBlocks(int num_blocks);
    void add_index_in_dir(int pi_number,int ci_number);
    void delete_index_in_dir(int pi_number,int ci_number);
    int new_allocate_dir(string file_name);
    string get_file_name(int i_number);
};//磁盘管理模块儿



#endif //FILESYS_FS_H
