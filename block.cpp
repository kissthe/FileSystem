#include "FS.h"
string Disk::get_file_name(int i_number) {
    return inodes_blocks[i_number].file_name;
}
void Disk::read_file(int i_number) {
    for(auto i:inodes_blocks[i_number].direct_block){
        if(i!= nullptr){
            output_buffer+=i->content;
        }
    }
    if(inodes_blocks[i_number].disk_pointer.size()==1){
        for(int i=0;inodes_blocks[i_number].disk_pointer[0]->indirect_block[i]!= nullptr;i++){
            output_buffer+=inodes_blocks[i_number].disk_pointer[0]->indirect_block[i]->content;
        }
    }else{
        /*
         * ���ڶ������������
         */
        for(int i=1;i<inodes_blocks[i_number].disk_pointer.size();i++){
            for(int j=0;j<32&&inodes_blocks[i_number].disk_pointer[i]->indirect_block[j]!= nullptr;j++){
                output_buffer+=inodes_blocks[i_number].disk_pointer[i]->indirect_block[j]->content;
            }
        }
    }

}
void Disk::read_dir(int i_number) {
    /*
     * ��Ŀ¼����Ϣ���뻺������
     */

    parent_inode_number=inodes_blocks[i_number].direct_block[0]->index[0];//��¼һ���ϼ��ڵ�
    for(auto i:inodes_blocks[i_number].direct_block){
        if(i!= nullptr){
            for(int j=0;j<i->index_number;j++){
                if(i->index[j]!=parent_inode_number){//�����Ļ��Ͳ�����ϼ��ڵ���뻺������
                        dir_output_buffer.push_back(inodes_blocks[i->index[j]]);//����Ŀ¼��Ӧ�Ļ�������
                }
            }
        }

    }
}
string Disk::modify_time() {
    // ��ȡ��ǰʱ���
    auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // ��ʱ���ת��Ϊ����ʱ��ṹ
    std::tm* localTime = std::localtime(&currentTime);

    // ����һ���ַ��������洢ʱ���ַ���
    const int bufferSize = 80;
    char buffer[bufferSize];

    // ʹ�� std::strftime ��ʱ��ṹ��ʽ��Ϊ�ַ���
    std::strftime(buffer, bufferSize, "%Y/%m/%d %H:%M", localTime);
    return buffer;//���ص�ǰ����ʱ���ַ���
}

bool Disk::modify_file_name(int i_number, string new_name) {
    /*
     * �Ѷ�Ӧ�ļ�/�ļ��е�inode��Ϣ����
     */
    inodes_blocks[i_number].file_name=new_name;
    return true;
}

void Disk::add_index_in_dir(int pi_number,int ci_number) {
    /*
     * ��pi_number��Ŀ¼������ci_number����Ŀ
     * ����number����inode��
     */

    for(int i=0;i<12;i++){
        //if(inodes_blocks[pi_number].direct_block[i]!= nullptr){}
        //int count = inodes_blocks[pi_number].direct_block[i]->index_number;//��ǰ���̿����ŵ�Ŀ¼����

        if(inodes_blocks[pi_number].direct_block[i]== nullptr){

            //������Ҫ������һ�����п鲢��ָ���������ڿ��п�������Ŀ¼
            inodes_blocks[pi_number].direct_block[i]=&data_blocks[findFreeDataBlock()];
            //
            inodes_blocks[pi_number].direct_block[i]->index[0]=ci_number;//���´�����Ŀ¼���ļ���i_number�ż����ȥ
            //
            inodes_blocks[pi_number].direct_block[i]->index_number++;//���Ŀ¼������+1
            break;
        } else if(inodes_blocks[pi_number].direct_block[i]->index_number<64){
            /*
             * ���������ǿյĻ������������������һ���µ�Ŀ¼
             */
            int count = inodes_blocks[pi_number].direct_block[i]->index_number;

            inodes_blocks[pi_number].direct_block[i]->index[count]=ci_number;

            inodes_blocks[pi_number].direct_block[i]->index_number++;
            count++;

            break;
        } else cout<<"add index in dir failed"<<endl;
    }
    inodes_blocks[pi_number].modified_time=modify_time();//�����ϼ�Ŀ¼���޸�ʱ��
    //cout<<"add index in dir success!"<<endl;
}
void Disk::delete_index_in_dir(int pi_number, int ci_number) {
    /*
     * ɾ��pi_number����ci_number��Ӧ����Ŀ
     */
    for(int i=0;i<12;i++){
        if(inodes_blocks[pi_number].direct_block[i]== nullptr)return;
        int count = inodes_blocks[pi_number].direct_block[i]->index_number;//��ǰ���̿����ŵ�Ŀ¼����
            /*
             * һ��һ��ָ�������Ŀ�Ĳ���
             */
            for(int j=0;j<count;j++){
                if(inodes_blocks[pi_number].direct_block[i]->index[j]==ci_number){
                    /*
                     * �ҵ�ƥ����ˣ���Ҫɾ����Ԫ�أ����Ѻ����Ԫ����ǰ�ƶ�һ��
                     */
                    for(int k=j;k<count-1;k++){
                        inodes_blocks[pi_number].direct_block[i]->index[k]=inodes_blocks[pi_number].direct_block[i]->index[k+1];
                    }
                    inodes_blocks[pi_number].direct_block[i]->index_number--;
                    break;
                }
            }

    }
}


bool Disk::modify_file_recycled(int i_number) {
    inodes_blocks[i_number].recycled= true;//��Ϊ����״̬
}

int Disk::get_CurDirChild_number(int i_number) {
    int sum=1;
    for(auto i:inodes_blocks[i_number].direct_block){
        if(i!= nullptr)sum+=i->index_number-1;
    }
    return sum;
}

Disk::Disk(int blockCount,int blockSize) : blockCount(blockCount), blockSize(blockSize) {
    // ��ʼ��λʾͼ,��Ŀ¼һ��Ҫ��ʼ����
    i_bitmap[0]= 1;
    for (int i = 1; i < I_BMAP_NUM; ++i) {
        i_bitmap[i] = 0;
    }
    d_bitmap[0]= 1;
    for (int i = 1; i < D_BMAP_NUM; ++i) {
        d_bitmap[i] = 0;
    }

    //��ʼ��inode����,Ҫ�и�Ŀ¼���ļ�ͷ
    inodes_blocks[0].file_name="/";
    inodes_blocks[0].i_number=0;
    inodes_blocks[0].recycled= false;
    inodes_blocks[0].modified_time="2024/01/01 00:00";
    inodes_blocks[0].direct_block[0]=&data_blocks[0];//Ĭ�ϰѵ�һ����ָ���Ŀ¼
    inodes_blocks[0].file_type=DIRECTORY_TYPE;
    //��ʼ����������Ҫ�и�Ŀ¼����
    data_blocks[0].index_number=1;//����˸�Ŀ¼���������Ϊ1
    data_blocks[0].occupied= true;
    data_blocks[0].blockType=INDEX_BLOCK;
    data_blocks[0].index[0]=0;//��Ŀ¼���ϼ�Ŀ¼���Ǹ�Ŀ¼

    for (int i = 1; i < DATA_BLOCK_NUM; ++i) {
        data_blocks[i].occupied = false;
        /*
         * �Ѵ�����ݵ�����ȫ����Ϊ0
         */
        memset(data_blocks[i].content,0,256);
        memset(data_blocks[i].index,0,64);
        memset(data_blocks[i].indirect_block, 0,32);
        // ������Ҫ�������ֶν��г�ʼ��
    }

    // ��ʼ������վ����
    for (int i = 0; i < 20; ++i) {
        arr[i] = 0;
    }

}


Disk::~Disk() {
    // �����������н�����Դ������������еĻ�
    // ���磬�ͷŶ�̬������ڴ��
    // ������Ҫ��������������������������߼�
}
