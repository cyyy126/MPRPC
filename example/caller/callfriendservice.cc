#include <iostream>
#include "friend.pb.h"
#include "mprpcapplication.h"

int main(int argc, char **argv)
{
    // 整个程序启动以后，想使用mprpc框架来享受rpc调用服务，一定要先调用框架的初始化函数(只调用一次就行)
    MprpcApplication::Init(argc, argv);

    // 演示远程调用rpc发布的方法 Login

    fixbug::GetFriendListsServiceRpc_Stub stub(new MprpcChannel());
    // rpc方法的请求参数
    fixbug::GetFriendListsRequest request;
    request.set_userid(1);
    // rpc方法的响应
    fixbug::GetFriendListsResponse response;
    // 发起rpc方法的调用，同步rpc方法的调用过程
    stub.GetFriendLists(nullptr, &request, &response, nullptr);

    // 一次rpc调用完成，读取调用的结果
    MprpcController controller;
    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else
    {
        if (0 == response.result().errcode())
        {
            std::cout << "rpc GetFriendListsService response success: " << std::endl;
            int size = response.friends_size();
            for (int i = 0; i < size; i++)
            {
                std::cout << "index: " << (i + 1) << "name:" << response.friends(i) << std::endl;
            }
        }
        else
        {
            std::cout << "rpc GetFriendListsService response error: " << response.result().errmsg() << std::endl;
        }
    }

    return 0;
}
