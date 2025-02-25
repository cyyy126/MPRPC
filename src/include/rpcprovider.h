#pragma once
#include "google/protobuf/service.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <google/protobuf/descriptor.h>
#include <unordered_map>


// 框架提供的专门发布rpc服务的网络对象类

class RpcProvider
{
    public:
        // 这里是框架提供给外部接口用的，可以发布rpc方法的函数接口
        void NotifyService(google::protobuf::Service *service);

        // 启动rpc服务节点，开始提供rpc远程网络调用服务
        void Run();

    private:
        // 组合了Eventloop
        muduo::net::EventLoop m_eventLoop;

        // 首先定义服务类型信息，也就是服务里面的方法和方法描述
        struct ServiceInfo
        {
            google::protobuf::Service *m_service; // 保存服务对象
            std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap; // 保存服务方法和方法描述

        };

        // 保存注册成功的服务对象和服务的描述 也就是服务的名字和服务里面的方法
        std::unordered_map<std::string, ServiceInfo> m_serviceMap;

        // 新的socket连接回调
        void OnConnection(const muduo::net::TcpConnectionPtr&);
        // 已建立连接用户的读写事件回调
        void OnMessage(const muduo::net::TcpConnectionPtr& , muduo::net::Buffer*, muduo::Timestamp);
};