#include "FS.h"

int main() {
    Disk disk1(DATA_BLOCK_NUM, BLOCK_SIZE);
    FileManagement my_file(&disk1);
    bool flag = true;
    int choice;
    string file_name;

    while (flag) {
        my_file.print_current_dir();
        cout << "1. 列出当前目录\n";
        cout << "2. 进入某目录\n";
        cout << "3. 新建目录\n";
        cout << "4. 新建文件\n";
        cout << "5. 删除文件/目录\n";
        cout << "6. 查看磁盘状态\n";
        cout << "7. 返回上级目录\n";
        cout << "8. 读取文件\n";
        cout << "9. 退出\n";

        cout << "请输入你的选项: ";
        cin >> choice;

        switch (choice) {
            case 1:
                my_file.print_dir_details();
                break;
            case 2:
                cout << "请输入要进入的目录名: ";
                cin >> my_file.input_buffer;
                my_file.cd_dir(my_file.input_buffer);
                break;
            case 3:
                cout << "请输入要新建的目录名: ";
                cin >> file_name;
                my_file.create(DIRECTORY_TYPE, file_name);
                break;
            case 4:
                cout << "请输入要新建的文件名: ";
                cin >> file_name;
                my_file.create(FILE_TYPE, file_name);
                break;
            case 5:
                cout << "请输入要删除的文件/目录名: ";
                cin >> file_name;
                my_file.remove(file_name);
                break;
            case 6:
                my_file.show_disk_status();
                break;
            case 7:
                my_file.go_back();
                break;
            case 8:
                cout<<"请输入要读取的文件名:"<<endl;
                cin>>file_name;
                cout<<my_file.get_file_content(file_name)<<endl;
                break;
            case 9:
                flag = false;
                break;
            default:
                cout << "无效的选项，请重新输入\n";
                break;
        }
    }

    return 0;
}
