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

	// 查找inode对应的数据块
	vector<Disk_Block*>& disk_pointer = inodes_blocks[inode_id].disk_pointer;
	for (Disk_Block* block : disk_pointer) {
		int block_id = block->block_id;

		// 将查找到的数据块恢复默认数值并删除相应内容
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

	// 删除inode块中的内容
	inodes_blocks[inode_id].i_number = i_number;
	inodes_blocks[inode_id].recycled = true;
	inodes_blocks[inode_id].file_type = 1;
	inodes_blocks[inode_id].file_size = 0;
	inodes_blocks[inode_id].file_name = "";
	inodes_blocks[inode_id].file_block = 0;

	i_bitmap[inode_id] = false;

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
				sizeof(inodes_blocks[i].modified_time) +
				sizeof(Disk_Block*) * inodes_blocks[i].disk_pointer.size();
		}
	}

	// 计算数据区域所占内存
	int data_memory_size = 0;
	for (int i = 0; i < D_BMAP_NUM; i++) {
		// 找到一个非空闲的inode
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

	cout << "inode内存大小" << inode_memory_size << "字节" << endl;
	cout << "数据块内存大小" << data_memory_size << "字节" << endl;
}