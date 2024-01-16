#include "FS.h"

int main() {
    Disk disk1(DATA_BLOCK_NUM, BLOCK_SIZE);
    FileManagement my_file(&disk1);
    bool flag = true;
    int choice;
    string file_name;

    while (flag) {
        my_file.print_current_dir();
        cout << "1. 列出当前目录\t\t2. 进入某目录\n";
        cout << "3. 新建目录\t\t4. 新建文件\n";
        cout << "5. 删除文件/目录\t\t6. 查看磁盘状态\n";
        cout << "7. 返回上级目录\t\t8. 读取文件\n";
        cout << "9. 排序显示\t\t10. 复制\n";
        cout << "11. 粘贴\t\t12. 剪切\n";
        cout << "13. 退出\n";

        cout << "请输入你的选项: ";
        scanf("%d",&choice);

        switch (choice) {
            case 1:
                system("cls");
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
                cout << "请输入要读取的文件名:" << endl;
                cin >> file_name;
                cout << my_file.get_file_content(file_name) << endl;
                break;
            case 9:
                cout << "sort:1.name 2.size 3.time" << endl;
                cin >> choice;
                my_file.print_sort_dir(choice);
                break;
            case 10:
                cout << "输入需要复制的文件名:";
                cin >> file_name;
                my_file.copy(file_name);
                break;
            case 11:
                my_file.paste();
                break;
            case 12:
                cout << "输入需要剪切的文件名:";
                cin >> file_name;
                my_file.cut(file_name);
                break;
            case 13:
                flag = false;
                break;
            default:
                cout << "无效的选项，请重新输入\n";
                break;
        }
    }

    return 0;
}
