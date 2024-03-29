#include "FS.h"

void FileManagement::print_current_dir() {
    cout<<"current_dir:"<<current_dir<<endl;
}
void FileManagement::update_buffer() {
    disk->read_dir(ope_inode);
    //cout<<"updating buffer now!"<<endl;
    dir_input_buffer=disk->dir_output_buffer;
    parent_inode=disk->parent_inode_number;
    disk->dir_output_buffer.clear();//读取完毕后清空磁盘缓冲区
}

void FileManagement::update_file_buffer(int file_number){
    disk->read_file(file_number);
    input_buffer=disk->output_buffer;//缓冲区的传递
    disk->output_buffer.clear();//清空原来的缓冲区
}

void FileManagement::print_dir_details() {
//    cout<<"ope_node:"<<ope_inode<<endl;
//    cout<<"current_dir_child_number:"<<disk->get_CurDirChild_number(ope_inode)<<endl;
    cout<<"FileName"<<"\t"<<"ModifiedTime"<<"\t"<<"Type"<<"\t"<<"Size"<<endl;
    update_buffer();
    //cout<<"update success!"<<endl;

    if(dir_input_buffer.size()==0){
        cout<<"current file is empty"<<endl;
        return;
    }
    for(auto i:dir_input_buffer){
        //if(i.i_number!=dir_input_buffer.front().i_number){
            if(i.file_type==FILE_TYPE){
                cout<<i.file_name<<"\t"<<i.modified_time<<"\t"<<"File"<<"\t"<<i.file_size<<endl;
            } else{
                cout<<i.file_name<<"\t"<<i.modified_time<<"\t"<<"Dir"<<"\t"<<endl;
            }
        //}
    }

}

void FileManagement::print_sort_dir(int choice) {
    update_buffer();
    if(dir_input_buffer.empty()){
        cout<<"current file is empty"<<endl;
        return;
    }
    vector<Inode>tmp=dir_input_buffer;
    sort_index(choice,tmp);
    cout<<"FileName"<<"\t"<<"ModifiedTime"<<"\t"<<"Type"<<"\t"<<"Size"<<endl;
    for(auto i:tmp){
        //if(i.i_number!=dir_input_buffer.front().i_number){
        if(i.file_type==FILE_TYPE){
            cout<<i.file_name<<"\t"<<i.modified_time<<"\t"<<"File"<<"\t"<<i.file_size<<endl;
        } else{
            cout<<i.file_name<<"\t"<<i.modified_time<<"\t"<<"Dir"<<"\t"<<endl;
        }
        //}
    }

}

bool FileManagement::rename_file(string old_name, string new_name) {
    /*
     * 修改文件名
     */
    for(auto i:dir_input_buffer){
        if(old_name==i.file_name){
            for(auto j:dir_input_buffer){
                if(new_name==j.file_name){
                    cout<<"重复"<<endl;
                    return false;
                }
            }
            return disk->modify_file_name(i.i_number,new_name);
        }
    }
    return false;
}

bool FileManagement::create(FileType file_type,string file_name) {
    /*
     * 新建文件夹、文件其实都是相当于让磁盘分配空间，因此直接调用磁盘的函数即可
     * true代表普通文件,false代表目录文件
     */
    update_buffer();
    for(auto i:dir_input_buffer){
        if(i.file_name==file_name){
            cout<<"file_name already exists!"<<endl;
            return false;
        }
    }
    int ci_number;
    if(file_type==FILE_TYPE){
        string tmp;
        cin.get();
        //cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout <<"input file content:";
        getline(cin,tmp);

        ci_number= disk->allocateBlock_File(file_name,&tmp);
        disk->add_index_in_dir(ope_inode,ci_number);
    } else{
       ci_number= disk->new_allocate_dir(file_name);
       //cout<<"allocate finish:"<<ci_number<<endl;
       disk->add_index_in_dir(ope_inode,ci_number);
       disk->add_index_in_dir(ci_number,ope_inode);

    }
}

bool FileManagement::remove(string file_name) {
    /*
     * 回收磁盘空间，删除对应inode节点
     */
    update_buffer();
    for(auto i:dir_input_buffer){
        if(i.file_name==file_name){
            disk->delete_index_in_dir(ope_inode,i.i_number);
            return disk->deleteBlock(i.i_number);
        }
    }
    cout<<"could not find the file/dir"<<endl;
    return false;
}
bool FileManagement::recycle_file(string file_name)  {

    for(auto i:dir_input_buffer){
        if(i.file_name==file_name){
            return disk->modify_file_recycled(i.i_number);
        }
    }
    return false;
}


string FileManagement::get_file_content(string file_name) {
    update_buffer();
    int file_number=-1;
    for(auto i:dir_input_buffer){
        if(i.file_name==file_name&&i.file_type==FILE_TYPE){
            file_number=i.i_number;
            break;
        }
    }
    if(file_number==-1){
        return "file did not exist!";
    }
    update_file_buffer(file_number);
    file_content=input_buffer;
    if(!file_content.empty()){
        //cout<<"file content:"<<file_content<<endl;
        return file_content;
    } else{
        cout<<"the file is empty!!"<<endl;
    }

    return file_content;
}

void FileManagement::cd_dir(string file_name) {
    //bool exist= false;
    /*
     * 根据文件夹名称进入相应的目录
     * 返回的就是进入目录的相关信息，用string存储，可以在GUI端显示
     */
    update_buffer();
    bool flag= false;
    for(auto i:dir_input_buffer){
        if(i.file_name==file_name){
            /*
             * 在这里要修改一下当前目录和上级目录
             * 在这里要修改一下当前目录和上级目录的inode号
             */
            flag= true;
            parent_inode=ope_inode;
            ope_inode=i.i_number;
            current_dir.push_back('/');
            current_dir+=disk->get_file_name(ope_inode);
            //exist= true;
            break;
        }
    }
    if(flag== false){
        cout<<"目录不存在"<<endl;
        return;
    }

    disk->read_file(ope_inode);
    return;
}

void FileManagement::go_back() {
    update_buffer();
    string tmp=disk->get_file_name(ope_inode);
    if(ope_inode!=0)//如果不是根目录的话
        for(int i=0;i<=tmp.size();i++)current_dir.pop_back();
    ope_inode=parent_inode;

}

// 按照文件名进行比较
bool compareByName(const Inode& a, const Inode& b) {
    return a.file_name[0] < b.file_name[0];
}

// 按照文件大小进行比较
bool compareBySize(const Inode& a, const Inode& b) {
    return a.file_size < b.file_size;
}

// 按照修改日期进行比较
bool compareByModifiedTime(const Inode& a, const Inode& b) {
    return a.modified_time.compare(b.modified_time) < 0;
}

void FileManagement::show_disk_status() {
    disk->displayDiskStatus();
}

void FileManagement::sort_index(int choice,vector<Inode>&tmp) {
    /*
     * 删除索引项，可以使用二分查找并且删除
     * 使用STL算法，按照关键字进行快速排序
     */
    switch (choice) {
        case 1:
            sort(tmp.begin(), tmp.end(), compareByName);
            break;
        case 2:
            sort(tmp.begin(), tmp.end(), compareBySize);
            break;
        case 3:
            sort(tmp.begin(), tmp.end(), compareByModifiedTime);
            break;
    }
}


FileManagement::FileManagement(Disk*root):disk(root){
    //刚开始工作在根目录
    ope_inode=0;
    parent_inode=0;
    current_dir="root";
    copy_tmp ={-1};//用-1来表示当前剪切板为空
}

void FileManagement::copy(string file_name) {
    is_cut= false;
    bool flag= false;
    update_buffer();
    for(auto i:dir_input_buffer){
        if(i.file_name==file_name){
            flag= true;
            if(i.file_type==FILE_TYPE){
                copy_tmp=i;
                copy_file_content= get_file_content(file_name);
            } else{
                copy_tmp=i;
            /*
             * 这里可能还会需要添加一些东西
             */
            }
        }
    }
    if(!flag){
        cout<<"file/dir does not exist!";
        return;
    }
    cout<<"copy finish!"<<endl;
}
void FileManagement::cut(string file_name) {
    is_cut= true;
    bool flag= false;
    update_buffer();
    for(auto i:dir_input_buffer){
        if(i.file_name==file_name){
            flag= true;
            if(i.file_type==FILE_TYPE){
                copy_tmp=i;
                copy_file_content= get_file_content(file_name);
                cut_parent_number=ope_inode;
                break;
            } else{
                copy_tmp=i;
                cut_parent_number=ope_inode;
                break;
                /*
                 * 这里可能还会需要添加一些东西
                 */
            }
        }
    }
    if(flag== false){
        cout<<"file/dir does not exist!";
        return;
    }
    cout<<"copy finish!"<<endl;
}
void FileManagement::copy_file(){
    disk->read_file(copy_tmp.i_number);
    copy_file_content=disk->output_buffer;//缓冲区的传递
    disk->output_buffer.clear();//清空原来的缓冲区
    int ci_number= disk->allocateBlock_File(copy_tmp.file_name,&copy_file_content);
    disk->add_index_in_dir(ope_inode,ci_number);
}


void FileManagement::copy_directory(){
    create(DIRECTORY_TYPE,copy_tmp.file_name);//先创建一个目录
    cd_dir(copy_tmp.file_name);//进入目录

    disk->read_dir(copy_tmp.i_number);
    copy_number=disk->dir_output_buffer;
    disk->dir_output_buffer.clear();

    /*
     * 读取子目录，来获得子目录信息
     */
    for(auto i:copy_number){
        /*
         * 子目录递归复制
         */
        copy_tmp=i;
        if(copy_tmp.file_type==DIRECTORY_TYPE){
            copy_directory();
        } else{
            copy_file();
        }
    }
    go_back();//复制完毕则返回上一级

}

void FileManagement::paste() {
    if(copy_tmp.i_number==-1){
        cout<<"clipboard is empty!"<<endl;
        return;
    }
    int flag=copy_tmp.i_number;
    //if(copy_number.empty())return;
    if(copy_tmp.file_type==FILE_TYPE){
        copy_file();
    } else copy_directory();
    if(is_cut){//如果是剪切的话删除原来的块儿
        disk->delete_index_in_dir(cut_parent_number,flag);
        disk->deleteBlock(flag);
    }
    copy_tmp.i_number=-1;//将剪切板置为空

}





// 为了示例目的，这里没有对 disk 进行初始化
// 你可能需要根据实际情况在构造函数中进行 disk 的初始化

