#pragma once
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"

// 所有通过stub代理对象调用的rpc方法，都走到这里了，统一做rpc调用方法的数据序列化和网络发送

class MprpcChannel: public google::protobuf::RpcChannel
{
public:
    void CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller, 
                    const google::protobuf::Message* request,
                    google::protobuf::Message* response, 
                    google::protobuf::Closure* done);
};