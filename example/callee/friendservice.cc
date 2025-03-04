#include <iostream>
#include "friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "logger.h"

/*
FriendService原来是一个本地服务，提供了两个进程内的本地方法，Login和GetFriendLists
*/
class GetFriendListsService : public fixbug::GetFriendListsServiceRpc // 使用rpc在服务发布端(rpc服务提供者)
{
public:
    std::vector<std::string> GetFriendLists(uint32_t userid)
    {
        std::cout << "do GetFriendLists service! userid: " << userid << std::endl;
        std::vector<std::string> vec;
        vec.push_back("cy");
        vec.push_back("xkr");
        vec.push_back("hw");
        return vec;
    } 

    void GetFriendLists(::google::protobuf::RpcController* controller,
        const ::fixbug::GetFriendListsRequest* request,
        ::fixbug::GetFriendListsResponse* response,
        ::google::protobuf::Closure* done)
        {
             // 框架给业务上报了请求参数request， 应用获取相应数据做本地业务
             uint32_t userid = request->userid();
 
             // 做本地业务
             std::vector<std::string> friendList = GetFriendLists(userid);
 
             // 把响应写入框架 包括错误码，错误消息，返回值
             fixbug::ResultCode *rc = response->mutable_result();
             rc->set_errcode(0);
             rc->set_errmsg("");
             for(std::string &name : friendList)
             {
                std::string *p = response->add_friends();
                *p = name;
             }
 
             // 执行回调  执行响应对象数据的序列化和网络发送，都是由框架来完成的
             done->Run();
        }
    
};

int main(int argc, char **argv)
{
    LOG_INFO("first logger message!");
    LOG_ERR("%s:%s:%d", __FILE__, __FUNCTION__,  __LINE__);
    // 调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    
    // provider是一个rpc网络服务对象，把FriendServic对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new GetFriendListsService());

    // 启动一个rpc服务发布节点，Run以后进入阻塞状态，等待远程的rpc调用请求
    provider.Run();
    return 0;
}

