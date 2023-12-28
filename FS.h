#ifndef FILESYS_FS_H
#define FILESYS_FS_H
#include <iostream>
#include <array> //用来存储定量字符，模拟磁盘容量
#include <vector>
#include <string>
#include <map>
#define INODE_NUM 20
#define I_BMAP_NUM 20
#define D_BMAP_NUM 20
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
        array<char,256> content;//存储文件的内容
        //存储间接块儿，可选择合适的数据结构
        array<Disk_Block*,64>indirect_block;
        //存储索引块儿，可选择合适的数据结构
        array<Dir_Index,10>index;
    };
};//磁盘块儿

struct Inode{
    int i_number;//文件的低级名称
    bool file_type;//文件类型
    int file_size;//文件大小
    string file_name;//文件名
    //修改时间
    int file_block;//分配了多少块儿
    vector<Disk_Block*>disk_pointer;//磁盘指针，会有指向间接块儿的指针
};//inode节点

class Disk {
public:
    Disk(int blockCount, int blockSize);
    ~Disk();

    Disk_Block* allocateBlock();//传入新建文件的i_number,file_type;并创建对应的inode和属性
    Disk_Block* deleteBlock();
    void displayDiskStatus();

private:
    const int blockCount;  // 磁盘块数量（常量，不可修改）
    const int blockSize;   // 每个磁盘块的大小（常量，不可修改）
    bool i_bitmap[I_BMAP_NUM];
    bool d_bitmap[D_BMAP_NUM];//位示图
    array<Disk_Block, 100> data_blocks;  // 固定大小的数组，模拟磁盘块
    array<Inode,INODE_NUM> inodes_blocks;//存储inodes
};//磁盘管理模块儿

class FileManagement{
private:
    /*要有一个变量存放当前目录，进入子目录、返回上层目录时需要对应改变
     * 比如可以用当前的目录的inode节点
     */
     Inode* ope_ptr;//工作指针
public:
    /*
     * 对文件,目录的一些操作，主要是更改inode属性
     * 新建，修改文件的时候需要对应修改inode属性
     */
    FileManagement(Inode*root);//构造函数
    void add_index(string file_name,int i_number);
    void delete_index(string file_name,int i_number);
    string get_dir();//返回当前的目录名

};

class User {
private:
    int user_id;
    string current_path;  // 用户的当前工作目录
    Inode* userRoot;  // 指向用户根目录的指针

public:
    User() : current_path("/"), userRoot(nullptr) {
        // 用户登录时初始化文件系统
        initializeFileSystem();
    }
    // 初始化文件系统的方法
    void initializeFileSystem() {
        // 在这里实现文件系统初始化逻辑
        // 例如，读取超级块，初始化用户根目录等
        userRoot = createRootDirectory();  // 创建用户根目录
    }
    // 创建用户根目录的方法
    Inode* createRootDirectory() {
        // 实现创建用户根目录的逻辑
        // 可能需要分配inode、更新磁盘位图等
        return nullptr;  // 返回用户根目录的指针
    }
    //create()--创建文件夹，文件名,并随机分配一个inumber号传入
    /*
     * list()--显示当前目录
cd()--进入目录
mkdir()--新建目录
rename()--重命名
copy()--复制
move()--移动
remove()--删除
edit()--读取并可以编辑
//User的这些操作必须经过FileManagement完成
     */
};

#endif //FILESYS_FS_H
