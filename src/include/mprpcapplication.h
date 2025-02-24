#pragma once
#include "mprpcconfig.h"

// 框架的基础类，负责框架的一些初始化操作
class MprpcApplication
{

public:
    static void Init(int argc, char **argv);

    static MprpcApplication& GetInstance();
private:
    // 因为要在Init这个静态成员方法内访问m_config成员变量，所以需要设置为静态成员变量，因为静态成员方法无法访问普通成员变量
    static MprpcConfig m_config;
    MprpcApplication(){}
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
};