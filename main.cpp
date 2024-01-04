#include "FS.h"

int main() {
    Disk disk1(DATA_BLOCK_NUM, BLOCK_SIZE);
    FileManagement my_file(&disk1);
    bool flag = true;
    int choice;
    string file_name;

    while (flag) {
        my_file.print_current_dir();
        cout << "1. �г���ǰĿ¼\n";
        cout << "2. ����ĳĿ¼\n";
        cout << "3. �½�Ŀ¼\n";
        cout << "4. �½��ļ�\n";
        cout << "5. ɾ���ļ�/Ŀ¼\n";
        cout << "6. �鿴����״̬\n";
        cout << "7. �����ϼ�Ŀ¼\n";
        cout << "8. ��ȡ�ļ�\n";
        cout << "9. �˳�\n";

        cout << "���������ѡ��: ";
        cin >> choice;

        switch (choice) {
            case 1:
                my_file.print_dir_details();
                break;
            case 2:
                cout << "������Ҫ�����Ŀ¼��: ";
                cin >> my_file.input_buffer;
                my_file.cd_dir(my_file.input_buffer);
                break;
            case 3:
                cout << "������Ҫ�½���Ŀ¼��: ";
                cin >> file_name;
                my_file.create(DIRECTORY_TYPE, file_name);
                break;
            case 4:
                cout << "������Ҫ�½����ļ���: ";
                cin >> file_name;
                my_file.create(FILE_TYPE, file_name);
                break;
            case 5:
                cout << "������Ҫɾ�����ļ�/Ŀ¼��: ";
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
                cout<<"������Ҫ��ȡ���ļ���:"<<endl;
                cin>>file_name;
                cout<<my_file.get_file_content(file_name)<<endl;
                break;
            case 9:
                flag = false;
                break;
            default:
                cout << "��Ч��ѡ�����������\n";
                break;
        }
    }

    return 0;
}
