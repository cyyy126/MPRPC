#include "rpcprovider.h"
#include <string.h>
#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include <functional>

/*
    service* 记录服务对象 =》 得到服务对象的描述信息 =》得到服务的名字以及里面的具体方法

*/

// 这里是框架提供给外部接口用的，可以发布rpc方法的函数接口

void RpcProvider::NotifyService(google::protobuf::Service *service)
{

    ServiceInfo service_info;

    // 获取了服务对象的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();

    // 获取了服务的名字
    const std::string service_name = pserviceDesc->name();
    std::cout << "service_name: " << service_name << std::endl;
    // 获取服务对象service里面方法的数量
    int methodCnt = pserviceDesc->method_count();

    for (int i = 0; i < methodCnt; i++)
    {
        // 获取服务对象指定下标的服务方法的描述（抽象描述）
        const google::protobuf::MethodDescriptor *pmethondDesc = pserviceDesc->method(i);
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
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        // 和rpc client的连接断开了
        conn->shutdown();
    }
}

/*
    在框架内部，RpcProvider和RpcConsumer协商好之间通信用的protobuf数据类型
    service_name method_name args     定义proto的message类型，进行数据头的序列化和反序列化
                                            service_name method_name args_size
    例如：16UserServiceLoginzhang san123456
    header_size(4个字节) + hearer_str + agrs_str
*/
// 已建立连接用户的读写事件回调  如果远程有一个rpc服务的调用请求，那么OnMessage方法就会响应
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn,
                            muduo::net::Buffer *buffer,
                            muduo::Timestamp)
{
    // 网络上接收的远程rpc调用请求的字符流    Login args
    std::string recv_buf = buffer->retrieveAllAsString();

    // 从字符流中读取前4个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char *)&header_size, 4, 0);

    // 根据header_size读取数据头的原始字符流，反序列化数据，得到rpc请求的详细消息
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (rpcHeader.ParseFromString(rpc_header_str))
    {
        // 数据反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.methond_name();
        args_size = rpcHeader.args_size();
    }
    else
    {
        // 数据反序列化失败
        std::cout << "rpc_header_str: " << rpc_header_str << "parse error!" << std::endl;
        return;
    }


    // 获取rpc方法参数的字符流数据
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    // 打印调试信息

    std::cout << "========================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "========================================" << std::endl;

    // 获取service对象和method对象
    auto it = m_serviceMap.find(service_name);
    if(it == m_serviceMap.end())
    {
        std::cout << "service_name: " << service_name << "is not exist!" << std::endl;
        return;
    }


    auto mit =it->second.m_methodMap.find(method_name);
    if(mit == it->second.m_methodMap.end())
    {
        std::cout << "method_name: " << method_name << "is not exist!" << std::endl;
        return;
    }

    // 获取service服务对象  new UserService
    google::protobuf::Service *service = it->second.m_service;
    // 获取method对象 Login
    const google::protobuf::MethodDescriptor *method = mit->second;

    // 生成rpc方法调用的请求request和响应response参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str))
    {
        std::cout << "request parse error, content: " << args_str << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // 给下面method方法的调用，绑定一个Closure的回调函数,protobuf自带接口，不用自己新建一个对象继承父类重写纯虚函数了
    google::protobuf::Closure* done = google::protobuf::NewCallback<RpcProvider, 
                                                                    const muduo::net::TcpConnectionPtr&, 
                                                                    google::protobuf::Message*>
                                                                    (this, 
                                                                    &RpcProvider::SendRpcResponse, conn, response);

    // 在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
    service->CallMethod(method, nullptr, request, response, done);


}

// Closure的回调操作，用于序列化rpc的响应和网络发送
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response)
{
    std::string response_str;
    if (response->SerializeToString(&response_str)) // response进行序列化
    {  
        // 序列化成功后，通过网络把rpc方法执行的结果发送给rpc调用方
        conn->send(response_str);
    }
    else
    {
        std::cout << "Serialize _response_str error!" << std::endl;
    }
    conn->shutdown(); // RpcProvider主动断开连接，模拟http的短连接服务
}   