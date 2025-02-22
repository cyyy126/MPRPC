#include <iostream>
#include <string>
#include "test.pb.h"
using namespace fixbug;

int main()
{
    // 如何填写成员变量（LoginResponse）中的另一个成员变量（ResultCode），通过res.mutable_result获取成员对象的指针，从而设置值
    // LoginResponse res;
    // ResultCode *rc = res.mutable_result();

    // rc->set_errcode(1);
    // rc->set_errmsg("登录处理失败");

    GetFriendListsResonse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);

    User *user1 = rsp.add_friend_list();
    user1->set_name("zhang san");
    user1->set_age(15);
    user1->set_sex(User::MAN);

    User *user2 = rsp.add_friend_list();
    user2->set_name("li si");
    user2->set_age(20);
    user2->set_sex(User::WOMAN);
    std::cout << rsp.friend_list_size() << std::endl;

    const User& user = rsp.friend_list(1);

    std::cout << user.name() << std::endl;
    std::cout << user.age() << std::endl;
    std::cout << user.sex() << std::endl;

    return 0;
}

int main1()
{

    // 封装了Login请求对象的数据
    LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");

    // 对象数据序列化 =》 char*
    std::string send_str;
    if (req.SerializePartialToString(&send_str))
    {
        std::cout << send_str.c_str() << std::endl;
    }

    // 从send_str反序列化一个login请求对象
    LoginRequest reqB;
    if (reqB.ParseFromString(send_str))
    {
        std::cout << reqB.name() << std::endl;
        std::cout << reqB.pwd() << std::endl;
    }

    return 0;
}