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

	// bool file_type;//�ļ����� 0-�ļ� 1-Ŀ¼
	// ɾ�����ļ�����Ϊ�ļ�����Ҫɾ��ֱ�ӿ�ͼ�ӿ������
	if (!inodes_blocks[inode_id].file_type) {
		// ɾ��ֱ�ӿ�
		for (int i = 0; i < 12; i++) {
			Disk_Block* block = inodes_blocks[inode_id].direct_block[i];
			if (block == nullptr) {
				break;
			}
			else if (block->blockType == FILE_BLOCK && block->block_id == i_number) {
				// �ҵ���ֱ�ӿ������������λ��
				int block_id = -1;
				for (int j = 0; j < DATA_BLOCK_NUM; j++) {
					if (&data_blocks[j] == block) {
						block_id = j;
						break;
					}
				}
				// ������ݿ�Ϊ����
				d_bitmap[block_id] = false;
				// ������ݿ��������
				memset(block->content, 0, sizeof(block->content));
				// �ָ����ݿ�������ϢΪĬ��״̬
				block->occupied = false;
				block->block_id = -1;
				block->block_size = 0;
				memset(block->content, 0, 256);
				for (int j = 0; j < 32; j++)
					block->indirect_block[j] = nullptr;
				for (int j = 0; j < 10; j++)
					block->index[j] = { 0 };
				// ����inode�Ĵ���ָ��
				block = nullptr;
			}
		}

		// ɾ����ӿ�
		for (int i = 0; i < 32; i++) {
			Disk_Block* block = inodes_blocks[inode_id].disk_pointer[i];
			if (block == nullptr) {
				break;
			}
			else if (block->blockType == INDIRECT_BLOCK && block->block_id == i_number) {
				// �ҵ������������λ��
				int block_id = -1;
				for (int j = 0; j < DATA_BLOCK_NUM; j++) {
					if (&data_blocks[j] == block) {
						block_id = j;
						break;
					}
				}
				// ������ݿ�Ϊ����
				d_bitmap[block_id] = false;
				// ������ݿ������
				memset(block->content, 0, sizeof(block->content));
				// �ָ����ݿ�������ϢΪĬ��״̬
				block->occupied = false;
				block->block_id = -1;
				block->block_size = 0;
				memset(block->content, 0, 256);
				for (int j = 0; j < 32; j++)
					block->indirect_block[j] = nullptr;
				for (int j = 0; j < 10; j++)
					block->index[j] = { 0 };
				// ���¼�ӿ��е�ָ��
				block = nullptr;
			}
		}
	}


	// ɾ�����ļ�����ΪĿ¼����Ҫɾ��ֱ�ӿ������
	if (inodes_blocks[inode_id].file_type) {
		// ɾ��ֱ�ӿ�
		for (int i = 0; ; i++) {
			Disk_Block* block = inodes_blocks[inode_id].direct_block[i];
			if (block == nullptr) {
				break;
			}
			else if (block->blockType == FILE_BLOCK && block->block_id == i_number) {
				// �ҵ���ֱ�ӿ������������λ��
				int block_id = -1;
				for (int j = 0; j < DATA_BLOCK_NUM; j++) {
					if (&data_blocks[j] == block) {
						block_id = j;
						break;
					}
				}
				// ������ݿ�Ϊ����
				d_bitmap[block_id] = false;
				// ������ݿ��������
				memset(block->content, 0, sizeof(block->content));
				// �ָ����ݿ�������ϢΪĬ��״̬
				block->occupied = false;
				block->block_id = -1;
				block->block_size = 0;
				memset(block->content, 0, 256);
				for (int j = 0; j < 32; j++)
					block->indirect_block[j] = nullptr;
				for (int j = 0; j < 10; j++)
					block->index[j] = { 0 };
				// ����inode�Ĵ���ָ��
				block = nullptr;
			}
		}
	}

	// ɾ��inode���е�����
	for (int i = 0; i < INODE_NUM; i++) {
		if (i_bitmap[i] && inodes_blocks[i].i_number == i_number) {
			inodes_blocks[inode_id].i_number = -1;
			inodes_blocks[inode_id].recycled = true;
			inodes_blocks[inode_id].file_size = 0;
			inodes_blocks[inode_id].file_name = "";
			inodes_blocks[inode_id].file_block = 0;
			for (int j = 0; j < 12; j++) {
				inodes_blocks[i].direct_block[j] = nullptr;
			}
			inodes_blocks[i].disk_pointer.clear();

			i_bitmap[inode_id] = false;
		}
	}


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
				sizeof(Disk_Block*) * inodes_blocks[i].disk_pointer.size();
		}
	}

	// ��������������ռ�ڴ�
	int data_memory_size = 0;
	for (int i = 0; i < D_BMAP_NUM; i++) {
		// �ҵ�һ���ǿ��е�inode
		if (d_bitmap[i]) {
			data_memory_size = sizeof(data_blocks[i].occupied) +
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