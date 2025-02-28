#include <iostream>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "mprpcchannel.h"

int main(int argc, char **argv)
{
    // 整个程序启动以后，想使用mprpc框架来享受rpc调用服务，一定要先调用框架的初始化函数(只调用一次就行)
    MprpcApplication::Init(argc, argv);

    // 演示远程调用rpc发布的方法 Login

    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    // rpc方法的请求参数
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    // rpc方法的响应
    fixbug::LoginResponse response;
    // 发起rpc方法的调用，同步rpc方法的调用过程
    stub.Login(nullptr, &request, &response, nullptr);


    // 一次rpc调用完成，读取调用的结果
    if (0 == response.result().errcode())
    {
        std::cout << "rpc login response success: " << response.success() << std::endl;

    }
    else
    {
        std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
    }


    // 演示远程调用rpc发布的方法 Request
    fixbug::RegisterRequest req;
    req.set_id(1);
    req.set_name("li si");
    req.set_pwd("233125");
    // rpc方法的响应
    fixbug::RegisterResponse rsp;
    // 发起rpc方法的调用，同步rpc方法的调用过程
    stub.Register(nullptr, &req, &rsp, nullptr);


    // 一次rpc调用完成，读取调用的结果
    if (0 == rsp.result().errcode())
    {
        std::cout << "rpc register response success: " << rsp.success() << std::endl;

    }
    else
    {
        std::cout << "rpc register response error: " << rsp.result().errmsg() << std::endl;
    }

    return 0;
}
