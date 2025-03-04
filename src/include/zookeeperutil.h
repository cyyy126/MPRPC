#pragma once

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>

class zkClient
{
public:
    zkClient();
    ~zkClient();
    // zkClient启动连接zkServer
    void Start();
    // 在zkServer上根据指定的path创建znode节点
    void Create(const char *path, const char *data, int datalen, int state=0);
    // 根据参数指定的znode节点路径，获得znode节点的值
    std::string GetData(const char* path);
// zk的客户端句柄
private:
    zhandle_t *m_zhandle;

};
