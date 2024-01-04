#include "FS.h"
/*
 * �޸������
 * 1.ֱ�ӿ������û�����⣬���Ǽ�ӿ�����ܻ��е����⣺����˵��ӿ����ʵ����һ��װ��ָ��Ŀ����ͬһ���ļ���inode����ͬ�ģ���˲����ٷ���inode
 * 2.��ʵ����������Ǹ����������������Ҫ�ļ�ӿ����Ч���������ҵ�ʱ���Ǹ�ö����������Ϊ�����顣��������ʵ������ǡ������������ָ���Ǹ�block�����ŵ���Ŀ¼
 * Ҳ����˵���������������Ǹ����ܻ����Ͼ���������Ҫ�ķ����ӿ����Ч��������Ұ��Ǹ�pointer_to_pointer�Ǹ�����ע�͵���
 * 3.i_number�ź�inode�����Ӧ���±�һ�����ҵ�ʱҲû˵������ҵĹ�
 * 4.����������ﻹ������Ļ���������
 *
 */
using namespace std;

// ����������
vector<int> findFreeInodes(int num_inodes);
int findFreeDataBlock();
vector<int> findFreeDataBlocks(int num_blocks);

// �ļ��ķ��亯��
int Disk::allocateBlock_File(string file_name, string* input_buffer)
{
    if( input_buffer == nullptr ){
        cout<<"pointer to input_buffer is null"<<endl;
        exit(0);
    }
    int block_id = -1;
    int length = input_buffer->length(); // ���������ݳ��� 
    //int block_size = 256; // �������ݿ�256�ֽ�
    int block_size = blockSize;

    double num_blocks = (length + block_size - 1) / block_size; // ������Ҫ���ٸ����ݿ�
    int num_blocks_rounded = ceil(num_blocks); // ����ȡ��

    // Ĭ�Ϸ���һ��inode��Լ��һ��15��ָ�룬ǰ12��ֱ����������13��һ�������������14�����������������15���������������
    int num_inodes = 1;

    // ���������inode
    int inode_id = findFreeInode();
    if (inode_id == -1)
    {
        cout << "û���㹻�Ŀ��ÿ���inode��" << endl;
        return -1;
    }

    // ���inodeΪ��ռ��
    i_bitmap[inode_id] = 1;

    // ����inode������
    inodes_blocks[inode_id].i_number = inode_id;
    inodes_blocks[inode_id].recycled = false;
    inodes_blocks[inode_id].file_type = FILE_TYPE;//�ļ�����
    inodes_blocks[inode_id].file_name = file_name;
    inodes_blocks[inode_id].file_size=length;//����ļ������ж�������ʱ����Ѿ�ȷ����
    inodes_blocks[inode_id].modified_time=modify_time();
    //inodes_blocks[inode_id].file_block = 0;

//}

//---------------------�����ļ�����------------------------------

    // ��������������ݣ������ļ���С׼������
    int flag=0;//��¼��ǰ���������ַ���ʼλ��




    // �������ݿ�
    int remaining_blocks = num_blocks_rounded;//���ݿ�ĸ���

    int num_blocks_to_allocate = min(remaining_blocks, 12); // ������12��ֱ�ӿ�

    // ���� num_blocks_to_allocate ��ֱ�ӿ�
    vector<int> block_ids = findFreeDataBlocks(num_blocks_to_allocate);

    if (block_ids.size() < num_blocks_to_allocate)
    {
        cout << "û���㹻�Ŀ��ÿ������ݿ顣" << endl;
        return -1;//�˳��ú���
    }

    for (int i = 0; i < num_blocks_to_allocate; i++)
    {
        int block_id = block_ids[i];

        // ������ݿ�Ϊ��ռ��
        d_bitmap[block_id] = 1;

        // �������ݿ������
        data_blocks[block_id].occupied = true;
        data_blocks[block_id].blockType = FILE_BLOCK;
        //data_blocks[block_id].block_id = i_number;
        //data_blocks[block_id].block_size = blockSize;

        // ���ļ����ݴ����뻺�������Ƶ����ݿ���
        if (input_buffer != nullptr) {
            // ���� content �����ݿ�����ݻ�����
            string tmp;
            if(flag+256<input_buffer->size()){
                tmp = input_buffer->substr(flag,256);
                flag +=256;
                strcpy(data_blocks[block_id].content, tmp.c_str());
            } else{
                tmp = input_buffer->substr(flag);
                strcpy(data_blocks[block_id].content, tmp.c_str());
                //break;
            }


            //inodes_blocks[inode_id].file_size = input_buffer->size();
        }

        // ����inode�Ĵ���ָ�룬����������ݿ��ָ����ӵ���Ӧ��inode�Ĵ���ָ����
        inodes_blocks[inode_id].direct_block[i] = &data_blocks[block_id];
    }

    remaining_blocks -= num_blocks_to_allocate;

    if (remaining_blocks>0&&remaining_blocks <= 32) //��Ҫ����һ��һ����ӿ�
    {
        // ����һ����ӿ�
        int indirect_block_id = findFreeDataBlock();
        if (indirect_block_id == -1) {
            cout << "û�п��õĿ������ݿ顣" << endl;
            return -1;
        }
        // ������ݿ�Ϊ��ռ��
        d_bitmap[indirect_block_id] = 1;

        //�޸ļ�ӿ����Ϣ
        data_blocks[indirect_block_id].occupied = true;
        data_blocks[indirect_block_id].blockType = INDIRECT_BLOCK; // ��ӿ�
        data_blocks[indirect_block_id].block_id = inode_id;
        data_blocks[indirect_block_id].block_size = blockSize;

        // ������ļ�ӿ��ָ����ӵ�inode����Ϣ��
        inodes_blocks[inode_id].disk_pointer.push_back(&data_blocks[indirect_block_id]);
        //�������ݿ�

        //������Ҫ�������ݿ�洢����
        int num_blocks_to_allocate2 = min(remaining_blocks, 32); // ������32�����ݿ�

        // ���� num_blocks_to_allocate2 �����ݿ�
        vector<int> block_ids = findFreeDataBlocks(num_blocks_to_allocate2);
        if (block_ids.size() < num_blocks_to_allocate2)
        {
            cout << "û���㹻�Ŀ��ÿ������ݿ顣" << endl;
            return -1;
        }

        for (int i = 0; i < num_blocks_to_allocate2; i++)
        {
            int block_id2 = block_ids[i];

            // ������ݿ�Ϊ��ռ��
            d_bitmap[block_id2] = 1;

            // �������ݿ������
            data_blocks[block_id2].occupied = true;
            data_blocks[block_id2].blockType = FILE_BLOCK;
            data_blocks[block_id2].block_id = inode_id;
            data_blocks[block_id2].block_size = blockSize;

//            // ���ļ����ݴ����뻺�������Ƶ����ݿ���
//            if (input_buffer != nullptr) {
//                // ���� content �����ݿ�����ݻ�����
//                strcpy(data_blocks[block_id2].content, input_buffer->c_str());
//                inodes_blocks[inode_id].file_size = input_buffer->size();
//
//            }

            // ���ļ����ݴ����뻺�������Ƶ����ݿ���,��������һ����
            string tmp;
            if(flag+256<input_buffer->size()){
                tmp = input_buffer->substr(flag,256);
                flag +=256;
                strcpy(data_blocks[block_id2].content, tmp.c_str());
            } else{
                tmp = input_buffer->substr(flag);
                strcpy(data_blocks[block_id2].content, tmp.c_str());
                break;
            }
        }

        remaining_blocks = 0;
    }

    /*
     * ���ʣ����Ҫ�Ŀ��������32����Ҫ����һ��������ӿ��
     */
    if (remaining_blocks > 32) //��Ҫ������һ����ӿ飬��Ҫһ��������ӿ���������ģ���������С������256k���ļ���
    {
        // ����ʣ������ݿ飬��Щ���ݿ���Ҫͨ����ӿ��е�ָ����ָ��
        int num_indirect_blocks = ceil(remaining_blocks / 32.0); // ������Ҫ�ļ�ӿ���

        // ����һ��������ӿ�
        int indirect2_block_id = findFreeDataBlock();
        if (indirect2_block_id == -1) {
            cout << "û�п��õĿ������ݿ顣" << endl;
            return -1;
        }
        // ������ݿ�Ϊ��ռ��
        d_bitmap[indirect2_block_id] = 1;

        // �������ݿ������
        data_blocks[indirect2_block_id].occupied = true;
        data_blocks[indirect2_block_id].blockType = INDIRECT_BLOCK; // ������ӿ�
        data_blocks[indirect2_block_id].block_id = inode_id;
        data_blocks[indirect2_block_id].block_size = blockSize;

        // ������Ķ�����ӿ��ָ����ӵ���Ӧ��inode�Ĵ���ָ����
        inodes_blocks[inode_id].disk_pointer.push_back(&data_blocks[indirect2_block_id]);

        // ����һ����ӿ鲢��ָ����ӵ�������ӿ���
        for (int i = 0; i < num_indirect_blocks; i++)
        {
            // ����һ����ӿ�
            int indirect_block_id = findFreeDataBlock();
            if (indirect_block_id == -1) {
                cout << "û�п��õĿ������ݿ顣" << endl;
                return -1;
            }
            // ������ݿ�Ϊ��ռ��
            d_bitmap[indirect_block_id] = 1;

            // �������ݿ������
            data_blocks[indirect_block_id].occupied = true;
            data_blocks[indirect_block_id].blockType = INDIRECT_BLOCK; // ��ӿ�
            data_blocks[indirect_block_id].block_id = inode_id;
            data_blocks[indirect_block_id].block_size = blockSize;

            // �������һ����ӿ��ָ����ӵ�������ӿ���
            data_blocks[indirect2_block_id].indirect_block[i] = (&data_blocks[indirect_block_id]);
            inodes_blocks[inode_id].disk_pointer.push_back(&data_blocks[indirect_block_id]);//��Ŷ�����ӿ����ָ��
        }

        for (int i = 1; i <= num_indirect_blocks; i++) // i=0 ����ָ�������ӿ��ָ��
        {
            for (int j = 0; j < min(remaining_blocks, 32); j++)
            {
                // ����һ�����ݿ�
                int data_block_id = findFreeDataBlock();
                if (data_block_id == -1) {
                    cout << "û�п��õĿ������ݿ顣" << endl;
                    return -1;
                }
                // ������ݿ�Ϊ��ռ��
                d_bitmap[data_block_id] = 1;

                // �������ݿ������
                data_blocks[data_block_id].occupied = true;
                data_blocks[data_block_id].blockType = FILE_BLOCK;
                data_blocks[data_block_id].block_id = inode_id;
                data_blocks[data_block_id].block_size = blockSize;

//                // ���ļ����ݴ����뻺�������Ƶ����ݿ���
//                if (input_buffer != nullptr) {
//                    strcpy(data_blocks[data_block_id].content, input_buffer->c_str());
//                    inodes_blocks[inode_id].file_size = input_buffer->size();
//                }
                string tmp;
                if(flag+256<input_buffer->size()){
                    tmp = input_buffer->substr(flag,256);
                    flag +=256;
                    strcpy(data_blocks[data_block_id].content, tmp.c_str());
                } else{
                    tmp = input_buffer->substr(flag);
                    strcpy(data_blocks[data_block_id].content, tmp.c_str());
                    break;
                }

                // ��ָ���������ݿ��ָ����ӵ�һ����ӿ���
                inodes_blocks[inode_id].disk_pointer[i]->indirect_block[j] = &data_blocks[data_block_id];

            }

            remaining_blocks -= 32;
        }
    }
    return inode_id;
}

/*���亯�����õ����Ӻ�������ѯ���д��̿飩*/
//�ҿ���inode �ĺ���
int Disk::findFreeInode() {
    // ����inodeλ��ͼ
    for (int i = 0; i < I_BMAP_NUM; i++) {
        // ����ҵ�һ�����е�inode
        if (!i_bitmap[i]) {
            // ����inode������
            i_bitmap[i]= 1;//��ʾ��ռ��
            return i;
        }
    }

    // û���ҵ����е�inode
    return -1;
}
vector<int> Disk::findFreeInodes(int num_inodes)
{
    vector<int> free_inodes;

    // ����inodeλ��ͼ
    for (int i = 0; i < I_BMAP_NUM; i++) {
        // ����ҵ�һ�����е�inode
        if (!i_bitmap[i]) {
            // ��inode��������ӵ������
            free_inodes.push_back(i);
            i_bitmap[i]= 1;//��ʾ��ռ��

            // ����Ѿ��ҵ��㹻������inode������ѭ��
            if (free_inodes.size() == num_inodes) {
                break;
            }
        }
    }

    // ���û���ҵ��㹻�����Ŀ���inode������һ���յ�����
    if (free_inodes.size() < num_inodes) {
        free_inodes.clear();
    }

    return free_inodes;
}

//�ҿ������ݿ� �ĺ���
int Disk::findFreeDataBlock()
{
    // �������ݿ�λ��ͼ
    for (int i = 0; i < D_BMAP_NUM; i++) {
        // ����ҵ�һ�����е����ݿ�
        if (!d_bitmap[i]) {
            // �������ݿ������
            d_bitmap[i]= 1;
            return i;
        }
    }

    // û���ҵ����е����ݿ�
    return -1;
}

vector<int> Disk::findFreeDataBlocks(int num_blocks)
{
    /*
     * ��λ��ͼ��Ѱ�ҿ���Ŀ�����ҵ��㹻�Ŀ���Ļ��ͷ�����Щ����ĺ�
     */
    vector<int> free_blocks;

    // �������ݿ�λ��ͼ
    for (int i = 0; i < D_BMAP_NUM; i++) {
        // ����ҵ�һ�����е����ݿ�
        if (!d_bitmap[i]) {
            // �����ݿ�������ӵ����п��б���
            free_blocks.push_back(i);
            d_bitmap[i]= 1;//ռ��

            // ����Ѿ��ҵ����㹻�Ŀ��п�
            if (free_blocks.size() == num_blocks) {
                // ����������ص����ݿ�����
                return free_blocks;
            }
        }
    }

    // ���û���ҵ��㹻�Ŀ��п飬�򷵻�һ���յ��б�
    return vector<int>();
}