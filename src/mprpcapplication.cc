#include "mprpcapplication.h"


// 类外实现静态方法不用再static了
void MprpcApplication::Init(int argc, char **argv)
{
    
}

MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}