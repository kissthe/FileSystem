#include "FS.h"


// 删除一个文件所有被占用的磁盘块
bool Disk::deleteBlock(int i_number) {

	// 查找该i_number所对应的inode
	// 遍历inode位视图寻找非空闲的inode
	int inode_id = -1;//记录找到的inode的下标
	for (int i = 0; i < I_BMAP_NUM; i++) {
		if (i_bitmap[i]) {
			// 比较该inode对应的i_number是否与传入的i_number匹配
			if (inodes_blocks[i].i_number == i_number) {
				inode_id = i;
				break;
			}
		}
	}

	// 未在inode表中找到对应的inode
	if (inode_id == -1) {
		cout << "i_number无效" << endl;
		return false;
	}

	// bool file_type;//文件类型 0-文件 1-目录
	// 删除的文件类型为文件，需要删除直接块和间接块的内容
	if (!inodes_blocks[inode_id].file_type) {
		// 删除直接块
		for (int i = 0; i < 12; i++) {
			Disk_Block* block = inodes_blocks[inode_id].direct_block[i];
			if (block == nullptr) {
				break;
			}
			else if (block->blockType == FILE_BLOCK && block->block_id == i_number) {
				// 找到该直接块在数据区域的位置
				int block_id = -1;
				for (int j = 0; j < DATA_BLOCK_NUM; j++) {
					if (&data_blocks[j] == block) {
						block_id = j;
						break;
					}
				}
				// 标记数据块为空闲
				d_bitmap[block_id] = false;
				// 清空数据块里的内容
				memset(block->content, 0, sizeof(block->content));
				// 恢复数据块其他信息为默认状态
				block->occupied = false;
				block->block_id = -1;
				block->block_size = 0;
				memset(block->content, 0, 256);
				for (int j = 0; j < 32; j++)
					block->indirect_block[j] = nullptr;
				for (int j = 0; j < 10; j++)
					block->index[j] = { 0 };
				// 更新inode的磁盘指针
				block = nullptr;
			}
		}

		// 删除间接块
		for (int i = 0; i < 32; i++) {
			Disk_Block* block = inodes_blocks[inode_id].disk_pointer[i];
			if (block == nullptr) {
				break;
			}
			else if (block->blockType == INDIRECT_BLOCK && block->block_id == i_number) {
				// 找到在数据区域的位置
				int block_id = -1;
				for (int j = 0; j < DATA_BLOCK_NUM; j++) {
					if (&data_blocks[j] == block) {
						block_id = j;
						break;
					}
				}
				// 标记数据块为空闲
				d_bitmap[block_id] = false;
				// 清空数据块的内容
				memset(block->content, 0, sizeof(block->content));
				// 恢复数据块其他信息为默认状态
				block->occupied = false;
				block->block_id = -1;
				block->block_size = 0;
				memset(block->content, 0, 256);
				for (int j = 0; j < 32; j++)
					block->indirect_block[j] = nullptr;
				for (int j = 0; j < 10; j++)
					block->index[j] = { 0 };
				// 更新间接块中的指针
				block = nullptr;
			}
		}
	}


	// 删除的文件类型为目录，需要删除直接块的内容
	if (inodes_blocks[inode_id].file_type) {
		// 删除直接块
		for (int i = 0; ; i++) {
			Disk_Block* block = inodes_blocks[inode_id].direct_block[i];
			if (block == nullptr) {
				break;
			}
			else if (block->blockType == FILE_BLOCK && block->block_id == i_number) {
				// 找到该直接块在数据区域的位置
				int block_id = -1;
				for (int j = 0; j < DATA_BLOCK_NUM; j++) {
					if (&data_blocks[j] == block) {
						block_id = j;
						break;
					}
				}
				// 标记数据块为空闲
				d_bitmap[block_id] = false;
				// 清空数据块里的内容
				memset(block->content, 0, sizeof(block->content));
				// 恢复数据块其他信息为默认状态
				block->occupied = false;
				block->block_id = -1;
				block->block_size = 0;
				memset(block->content, 0, 256);
				for (int j = 0; j < 32; j++)
					block->indirect_block[j] = nullptr;
				for (int j = 0; j < 10; j++)
					block->index[j] = { 0 };
				// 更新inode的磁盘指针
				block = nullptr;
			}
		}
	}

	// 删除inode块中的内容
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


	cout << "删除成功" << endl;
	return true;
}

// 显示磁盘的状态
void Disk::displayDiskStatus() {
	// 计算inode表所占内存
	int inode_memory_size = 0;
	for (int i = 0; i < I_BMAP_NUM; i++) {
		// 找到一个非空闲的inode
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

	// 计算数据区域所占内存
	int data_memory_size = 0;
	for (int i = 0; i < D_BMAP_NUM; i++) {
		// 找到一个非空闲的inode
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

	cout << "inode内存大小" << inode_memory_size << "字节" << endl;
	cout << "数据块内存大小" << data_memory_size << "字节" << endl;
}