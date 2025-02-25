#include "rpcprovider.h"
#include <string.h>
#include "mprpcapplication.h"
#include <functional>

/*
    service* 记录服务对象 =》 得到服务对象的描述信息 =》得到服务的名字以及里面的具体方法
    
*/

// 这里是框架提供给外部接口用的，可以发布rpc方法的函数接口

void RpcProvider::NotifyService(google::protobuf::Service *service)
{

    ServiceInfo service_info;

    // 获取了服务对象的描述信息
    const google::protobuf::ServiceDescriptor* pserviceDesc = service->GetDescriptor();

    // 获取了服务的名字
    const std::string service_name = pserviceDesc->name();
    std::cout << "service_name: " << service_name << std::endl;
    // 获取服务对象service里面方法的数量
    int methodCnt = pserviceDesc->method_count();

    for (int i = 0; i < methodCnt; i++)
    {
        // 获取服务对象指定下标的服务方法的描述（抽象描述）
        const  google::protobuf::MethodDescriptor* pmethondDesc = pserviceDesc->method(i);
        std::string method_name = pmethondDesc->name();
        std::cout << "method_name: " << method_name << std::endl;
        service_info.m_methodMap.insert({method_name, pmethondDesc});
    }
    service_info.m_service = service;

    m_serviceMap.insert({service_name, service_info});
    // 整理一下，首先大的是m_serviceMap 键是服务的名字，值是服务的类型信息
    // 然后呢服务的类型信息 service_info 是一个结构体，包含服务描述（抽象）和里面的方法
    // 方法m_methodMap呢包含方法的名字和方法描述（也是一个抽象的）例如一个服务叫UserService，里面有方法是Login，就是通过这层结构调用Login就行
}

// 启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象, 第三个参数表示名字
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
    // 绑定链接回调和消息读写回调方法， 分离了网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, 
                                std::placeholders::_2, std::placeholders::_3));


    // 设置muduo库的线程数量
    server.setThreadNum(4);


    std::cout << "RpcProvider start service ip: " << ip << " port: " << port << std::endl;
    // 启动网络服务
    server.start();
    m_eventLoop.loop();
}



// 新的socket连接回调
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr&)
{

}


// 已建立连接用户的读写事件回调
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp)
{

}