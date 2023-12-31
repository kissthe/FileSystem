#ifndef FILESYS_FS_H
#define FILESYS_FS_H
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <iomanip>
#include <chrono>
#define INODE_NUM 20
#define I_BMAP_NUM 20
#define D_BMAP_NUM 20
#define DATA_BLOCK_NUM 512
using namespace std;
enum BlockType { INODE_BLOCK, FILE_BLOCK, INDIRECT_BLOCK, INDEX_BLOCK };//定义枚举类型，inode、文件、间接块儿、索引块儿

struct Dir_Index{
    char name[20];
    int index;
};//目录里面存放的单元

struct Disk_Block{
    bool occupied;
    int type;//指明是块儿的类型，是inode还是其它类型，0-inode 1-文件 2-间接块儿 3-索引块儿
    BlockType blockType;//磁盘存储的是哪种类型的数据
    int block_id;//这一块儿属于哪一个文件
    int block_size;
    union {
        char content[256];//存储文件的内容
        //存储间接块儿，可选择合适的数据结构
        Disk_Block* indirect_block[64];
        //存储索引块儿，可选择合适的数据结构
        Dir_Index index[10];
    };
};//磁盘块儿

struct Inode{
    int i_number;//文件的低级名称
    bool recycled;//文件是否放入回收站
    bool file_type;//文件类型
    int file_size;//文件大小
    string file_name;//文件名
    string modified_time;//修改时间
    int file_block;//分配了多少块儿
    vector<Disk_Block*>disk_pointer;//磁盘指针，会有指向间接块儿的指针
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
public:
    /*
     * 对文件,目录的一些操作，主要是更改inode属性
     * 新建，修改文件的时候需要对应修改inode属性
     */
    string input_buffer;//输入缓冲区
    Inode dir_input_buffer[20];//目录项输入缓冲区,增加目录时注意也要在当前目录下增加条目

    FileManagement(Inode*root);//构造函数
    bool remove(string file_name,int i_number);
    bool create(int i_number, BlockType blockType, string file_name);
    string get_using_dir();//返回当前的目录名
    string get_file_content();
    string get_dir_content(int i_number);//获取当下的目录信息，之所以有返回值是因为要返回给GUI那边

    string cd_dir(string file_name);//进入目录

    bool recycle_file(int i_number);//放入回收站

    void add_index();//增加目录项
    void delete_index();//删除目录项
    void sort_index();//将目录项排序

    void print_current_dir();//打印出当前的工作目录
    bool rename_file(int i_number,string new_name);

};

class Disk {

private:

    const int blockCount;  // 磁盘块数量（常量，不可修改）
    const int blockSize;   // 每个磁盘块的大小（常量，不可修改）
    bool i_bitmap[I_BMAP_NUM];
    bool d_bitmap[D_BMAP_NUM];//位示图
    Disk_Block data_blocks[DATA_BLOCK_NUM];  // 固定大小的数组，模拟磁盘块
    Inode inodes_blocks[INODE_NUM];//固定大小的数组，模拟存放inodes的磁盘块儿
    int arr[20]={0};//回收站，如果哪个节点被暂时删除，就把它的inode中的recycled字段置为true，并把inode号放入回收站

public:
    string output_buffer;//读取的文件内容放在这里就行,输出缓冲区
    Inode dir_output_buffer[20];//读取的目录项放在这里

    Disk(int blockCount, int blockSize);
    ~Disk();

    int allocateBlock(int i_number,BlockType type,string file_name,string* input_buffer= nullptr);//返回inode号
    bool deleteBlock(int i_number);
    string modify_time();//修改文件时间，只有保存到磁盘的时候才需要修改
    void displayDiskStatus();
    bool modify_file_name(int i_number,string new_name);//更改名称
    bool modify_file_recycled();//把recycled改为true
    void read_file(int i_number);//读取普通文件
    void read_dir(int i_number);//读取目录项



};//磁盘管理模块儿



#endif //FILESYS_FS_H
