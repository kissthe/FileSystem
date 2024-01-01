#include "FS.h"


// ɾ��һ���ļ����б�ռ�õĴ��̿�
bool Disk::deleteBlock(int i_number) {

	// ���Ҹ�i_number����Ӧ��inode
	// ����inodeλ��ͼѰ�ҷǿ��е�inode
	int inode_id = -1;//��¼�ҵ���inode���±�
	for (int i = 0; i < I_BMAP_NUM; i++) {
		if (i_bitmap[i]) {
			// �Ƚϸ�inode��Ӧ��i_number�Ƿ��봫���i_numberƥ��
			if (inodes_blocks[i].i_number == i_number) {
				inode_id = i;
				break;
			}
		}
	}

	// δ��inode�����ҵ���Ӧ��inode
	if (inode_id == -1) {
		cout << "i_number��Ч" << endl;
		return false;
	}

	// ����inode��Ӧ�����ݿ�
	vector<Disk_Block*>& disk_pointer = inodes_blocks[inode_id].disk_pointer;
	for (Disk_Block* block : disk_pointer) {
		int block_id = block->block_id;

		// �����ҵ������ݿ�ָ�Ĭ����ֵ��ɾ����Ӧ����
		data_blocks[block_id].occupied = false;
		data_blocks[block_id].block_id = -1;
		data_blocks[block_id].block_size = 0;
		memset(data_blocks[block_id].content, 0, sizeof(Dir_Index) * 20);

		d_bitmap[block_id] = false;

		Inode& inode = inodes_blocks[inode_id];
		auto it = find(inode.disk_pointer.begin(), inode.disk_pointer.end(), &data_blocks[block_id]);

		if (it != inode.disk_pointer.end())
			inode.disk_pointer.erase(it);
	}

	// ɾ��inode���е�����
	inodes_blocks[inode_id].i_number = i_number;
	inodes_blocks[inode_id].recycled = true;
	inodes_blocks[inode_id].file_type = 1;
	inodes_blocks[inode_id].file_size = 0;
	inodes_blocks[inode_id].file_name = "";
	inodes_blocks[inode_id].file_block = 0;

	i_bitmap[inode_id] = false;

	cout << "ɾ���ɹ�" << endl;
	return true;
}

// ��ʾ���̵�״̬
void Disk::displayDiskStatus() {
	// ����inode����ռ�ڴ�
	int inode_memory_size = 0;
	for (int i = 0; i < I_BMAP_NUM; i++) {
		// �ҵ�һ���ǿ��е�inode
		if (i_bitmap[i]) {
			inode_memory_size = sizeof(inodes_blocks[i].i_number) +
				sizeof(inodes_blocks[i].recycled) +
				sizeof(inodes_blocks[i].file_type) +
				sizeof(inodes_blocks[i].file_size) +
				sizeof(inodes_blocks[i].file_block) +
				sizeof(inodes_blocks[i].file_name) +
				sizeof(inodes_blocks[i].modified_time) +
				sizeof(Disk_Block*) * inodes_blocks[i].disk_pointer.size();
		}
	}

	// ��������������ռ�ڴ�
	int data_memory_size = 0;
	for (int i = 0; i < D_BMAP_NUM; i++) {
		// �ҵ�һ���ǿ��е�inode
		if (d_bitmap[i]) {
			data_memory_size = sizeof(data_blocks[i].occupied) +
				sizeof(data_blocks[i].occupied) +
				sizeof(data_blocks[i].blockType) +
				sizeof(data_blocks[i].block_id) +
				sizeof(data_blocks[i].block_size);
			if (strlen(data_blocks[i].content) > 0)
				data_memory_size += sizeof(char[256]);
			else if (data_blocks[i].indirect_block)
				data_memory_size += sizeof(Disk_Block*) * 64;
			else if (data_blocks[i].index)
				data_memory_size += sizeof(Dir_Index) * 10;
		}
	}

	cout << "inode�ڴ��С" << inode_memory_size << "�ֽ�" << endl;
	cout << "���ݿ��ڴ��С" << data_memory_size << "�ֽ�" << endl;
}