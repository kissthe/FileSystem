#ifndef FILESYS_FS_H
#define FILESYS_FS_H
#include <iostream>
#include <vector>
#include <string>
#include <map>
#define INODE_NUM 20
#define I_BMAP_NUM 20
#define D_BMAP_NUM 20
#define DATA_BLOCK_NUM 512
using namespace std;
enum BlockType { INODE_BLOCK, FILE_BLOCK, INDIRECT_BLOCK, INDEX_BLOCK };//定义枚举类型，inode、文件、间接块儿、索引块儿

struct Dir_Index{
    char name[20];
    int i_number;
};//目录里面存放的单元

struct Disk_Block{
    bool occupied;
    
    BlockType blockType;//磁盘存储的是哪种类型的数据
    int block_id;//这一块儿属于哪一个文件
    int block_size;
    union {
        char content[256];//存储文件的内容
        //存储间接块儿，可选择合适的数据结构
        Disk_Block* indirect_block[32];
        //存储索引块儿，可选择合适的数据结构
        Dir_Index index[10];
    };
};//磁盘块儿

struct Inode{
    int i_number;//文件的低级名称
    bool recycled;//文件是否放入回收站
    bool file_type;//文件类型 0-文件 1-目录
    int file_size;//文件大小
    string file_name;//文件名
    //修改时间

    int file_block;//分配了多少块儿
    vector<Disk_Block*>disk_pointer;//磁盘指针，会有指向间接块儿的指针
    Disk_Block* direct_block[12];//指向直接快

};//inode节点
class Disk;

class FileManagement{
private:
    /*要有一个变量存放当前目录，进入子目录、返回上层目录时需要对应改变
     * 比如可以用当前的目录的inode节点
     */
    Inode* ope_ptr;//工作指针
    Disk* disk;//磁盘指针
public:
    /*
     * 对文件,目录的一些操作，主要是更改inode属性
     * 新建，修改文件的时候需要对应修改inode属性
     */
    string input_buffer;//输入缓冲区
    Dir_Index dir_input_buffer[20];//目录项输入缓冲区
    
    FileManagement(Inode*root);//构造函数

    bool delete_index(string file_name,int i_number);
    bool add_index(int i_number, BlockType blockType, string file_name);
    string get_dir();//返回当前的目录名
    bool recycle_file(int i_number);//放入回收站



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
    Dir_Index dir_output_buffer[20];//读取的目录项放在这里

    Disk(int blockCount, int blockSize);
    ~Disk();

    
    int allocateBlock_File(int i_number,string file_name, string* input_buffer) ;//返回inode号

    vector<int> findFreeInodes(int num_inodes);
    int findFreeDataBlock();
    vector<int> findFreeDataBlocks(int num_blocks);

    int allocateBlock_Dir(int i_number,string file_name, Dir_Index dir_input_buffer[20]);
    bool deleteBlock();
    void displayDiskStatus();
    bool modify_file_name(string new_name);//更改名称
    bool modify_file_recycled();//把recycled改为true
    void read_file(int i_number);//读取普通文件
    void read_dir(int i_number);//读取目录项

};//磁盘管理模块儿

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
