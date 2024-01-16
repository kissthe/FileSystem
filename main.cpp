#include "FS.h"

int main() {
    Disk disk1(DATA_BLOCK_NUM, BLOCK_SIZE);
    FileManagement my_file(&disk1);
    bool flag = true;
    int choice;
    string file_name;

    while (flag) {
        my_file.print_current_dir();
        cout << "1. �г���ǰĿ¼\t\t2. ����ĳĿ¼\n";
        cout << "3. �½�Ŀ¼\t\t4. �½��ļ�\n";
        cout << "5. ɾ���ļ�/Ŀ¼\t\t6. �鿴����״̬\n";
        cout << "7. �����ϼ�Ŀ¼\t\t8. ��ȡ�ļ�\n";
        cout << "9. ������ʾ\t\t10. ����\n";
        cout << "11. ճ��\t\t12. ����\n";
        cout << "13. �˳�\n";

        cout << "���������ѡ��: ";
        scanf("%d",&choice);

        switch (choice) {
            case 1:
                system("cls");
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
                cout << "������Ҫ��ȡ���ļ���:" << endl;
                cin >> file_name;
                cout << my_file.get_file_content(file_name) << endl;
                break;
            case 9:
                cout << "sort:1.name 2.size 3.time" << endl;
                cin >> choice;
                my_file.print_sort_dir(choice);
                break;
            case 10:
                cout << "������Ҫ���Ƶ��ļ���:";
                cin >> file_name;
                my_file.copy(file_name);
                break;
            case 11:
                my_file.paste();
                break;
            case 12:
                cout << "������Ҫ���е��ļ���:";
                cin >> file_name;
                my_file.cut(file_name);
                break;
            case 13:
                flag = false;
                break;
            default:
                cout << "��Ч��ѡ�����������\n";
                break;
        }
    }

    return 0;
}
