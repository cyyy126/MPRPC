#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcApplication.h"
#include "rpcprovider.h"

/*
UserService原来是一个本地服务，提供了两个进程内的本地方法，Login和GetFriendLists
*/
class UserService : public fixbug::UserServiceRpc // 使用rpc在服务发布端(rpc服务提供者)
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "dong local service: Login" << std::endl;
        std::cout << "name:" << name << "password:" << pwd << std::endl;
        return true;
    }


    /*
    重写基类UserServiceRpc的虚函数, 下面这些方法都是框架直接调用的
    1. caller ===> Login(LoginRequest) ===> muduo ===> callee
    2. callee ===> Login(LoginRequest) ===> 交给下面重写的Login方法来做本地业务
    */
    void Login(::google::protobuf::RpcController* controller,
        const ::fixbug::LoginRequest* request,
        ::fixbug::LoginResponse* response,
        ::google::protobuf::Closure* done)
        {

            // 框架给业务上报了请求参数request， 应用获取相应数据做本地业务
            std::string name = request->name();
            std::string pwd = request->pwd();

            // 做本地业务
            bool login_result = Login(name, pwd);

            // 把响应写入框架 包括错误码，错误消息，返回值
            fixbug::ResultCode *rc = response->mutable_result();
            rc->set_errcode(0);
            rc->set_errmsg("");
            response->set_success(login_result);

            // 执行回调  执行响应对象数据的序列化和网络发送，都是由框架来完成的
            done->Run();
        }

};

int main(int argc, char **argv)
{
    // 调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    // provider是一个rpc网络服务对象，把UserServic对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());

    // 启动一个rpc服务发布节点，Run以后进入阻塞状态，等待远程的rpc调用请求
    provider.Run();
    return 0;
}