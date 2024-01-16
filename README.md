# 模拟实现资源管理器
本项目参考VSFS(Very Simple File System，来自操作系统导论)实现了简单的文件系统。主体包括磁盘模拟以及使用模拟的磁盘来对文件进行管理。

### 介绍

* 磁盘结构如下图所示
* ![Disk Structure](https://github.com/kissthe/FileSystem/assets/90964780/7c634df3-ead5-4926-a3f8-dace839fae2a)

* 文件模拟采用索引文件的形式，有直接块儿和间接块儿，如下图所示（仅作为示意，部分属性不同）
* ![File Structure](https://github.com/kissthe/FileSystem/assets/90964780/1de7bd3a-b872-42ca-9a93-0a4ad69df7e2)

### 功能

* 实现了新建，删除，读取，复制，粘贴，进入目录，返回上级目录等操作


  






