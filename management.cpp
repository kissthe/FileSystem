#include "FS.h"

void FileManagement::print_current_dir() {
    cout<<"current_dir:"<<current_dir<<endl;
}
void FileManagement::update_buffer() {
    disk->read_dir(ope_inode);
    //cout<<"updating buffer now!"<<endl;
    dir_input_buffer=disk->dir_output_buffer;
    parent_inode=disk->parent_inode_number;
    disk->dir_output_buffer.clear();//��ȡ��Ϻ���մ��̻�����
}

void FileManagement::update_file_buffer(int file_number){
    disk->read_file(file_number);
    input_buffer=disk->output_buffer;//�������Ĵ���
    disk->output_buffer.clear();//���ԭ���Ļ�����
}

void FileManagement::print_dir_details() {
    cout<<"ope_node:"<<ope_inode<<endl;
    cout<<"current_dir_child_number:"<<disk->get_CurDirChild_number(ope_inode)<<endl;
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

bool FileManagement::rename_file(string old_name, string new_name) {
    /*
     * �޸��ļ���
     */
    for(auto i:dir_input_buffer){
        if(old_name==i.file_name){
            for(auto j:dir_input_buffer){
                if(new_name==j.file_name){
                    cout<<"�ظ�"<<endl;
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
     * �½��ļ��С��ļ���ʵ�����൱���ô��̷���ռ䣬���ֱ�ӵ��ô��̵ĺ�������
     * true������ͨ�ļ�,false����Ŀ¼�ļ�
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
        cin.get();
        //cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout <<"input file content:";
        getline(cin,input_buffer);

        ci_number= disk->allocateBlock_File(file_name,&input_buffer);
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
     * ���մ��̿ռ䣬ɾ����Ӧinode�ڵ�
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
        cout<<"file did not exist!"<<endl;
        exit(1);
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
     * �����ļ������ƽ�����Ӧ��Ŀ¼
     * ���صľ��ǽ���Ŀ¼�������Ϣ����string�洢��������GUI����ʾ
     */
    update_buffer();
    for(auto i:dir_input_buffer){
        if(i.file_name==file_name){
            /*
             * ������Ҫ�޸�һ�µ�ǰĿ¼���ϼ�Ŀ¼
             * ������Ҫ�޸�һ�µ�ǰĿ¼���ϼ�Ŀ¼��inode��
             */
            parent_inode=ope_inode;
            ope_inode=i.i_number;
            current_dir.push_back('/');
            current_dir+=disk->get_file_name(ope_inode);
            //exist= true;
            break;
        }
    }
    disk->read_file(ope_inode);
    return;
}

void FileManagement::go_back() {
    update_buffer();
    string tmp=disk->get_file_name(ope_inode);
    if(ope_inode!=0)//������Ǹ�Ŀ¼�Ļ�
        for(int i=0;i<=tmp.size();i++)current_dir.pop_back();
    ope_inode=parent_inode;

}

// �����ļ������бȽ�
bool compareByName(const Inode& a, const Inode& b) {
    return a.file_name[0] < b.file_name[0];
}

// �����ļ���С���бȽ�
bool compareBySize(const Inode& a, const Inode& b) {
    return a.file_size < b.file_size;
}

// �����޸����ڽ��бȽ�
bool compareByModifiedTime(const Inode& a, const Inode& b) {
    return a.modified_time.compare(b.modified_time) < 0;
}

void FileManagement::show_disk_status() {
    disk->displayDiskStatus();
}

void FileManagement::sort_index(int choice) {
    /*
     * ɾ�����������ʹ�ö��ֲ��Ҳ���ɾ��
     * ʹ��STL�㷨�����չؼ��ֽ��п�������
     */
    switch (choice) {
        case 1:
            sort(disk->dir_output_buffer.begin(), disk->dir_output_buffer.end(), compareByName);
            break;
        case 2:
            sort(disk->dir_output_buffer.begin(), disk->dir_output_buffer.end(), compareBySize);
            break;
        case 3:
            sort(disk->dir_output_buffer.begin(), disk->dir_output_buffer.end(), compareByModifiedTime);
            break;
    }
}


FileManagement::FileManagement(Disk*root):disk(root){
    //�տ�ʼ�����ڸ�Ŀ¼
    ope_inode=0;
    parent_inode=0;
    current_dir="/";
}

// Ϊ��ʾ��Ŀ�ģ�����û�ж� disk ���г�ʼ��
// �������Ҫ����ʵ������ڹ��캯���н��� disk �ĳ�ʼ��

