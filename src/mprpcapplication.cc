#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

MprpcConfig MprpcApplication::m_config;
void ShowArgHelp()
{
    std::cout << "format: command -i <configfile>" << std::endl;  
}

// 类外实现静态方法不用再static了
void MprpcApplication::Init(int argc, char **argv)
{
    if (argc < 2)
    {
        ShowArgHelp();
        exit(EXIT_FAILURE);
    }
    int c = 0;
    std::string config_file;
    while((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        
        case '?':
            std::cout << "invaild args!" << std::endl;
            ShowArgHelp();
            exit(EXIT_FAILURE);

        case ':':
            std::cout << "need <configfile>" << std::endl;
            ShowArgHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }
    
    // 开始加载配置文件了 rpcserver_ip =  rpcserver =  zookeeper_ip     zookeeper_port = 
    // 这里不能用m_config.xx方法来访问它的成员方法，因为m_config是静态成员变量，这样访问则代表它是一个已经实例化的对象，是不行的
    //只能通过MprpcApplication类名来访问m_config
    m_config.LoadConfigFile(config_file.c_str());

    // std::cout << "rpcserverip" << m_config.Load("rpcserverip") << std::endl;
    // std::cout << "rpcserverport" << m_config.Load("rpcserverport") << std::endl;
    // std::cout << "zookeeperip" << m_config.Load("zookeeperip") << std::endl;
    // std::cout << "zookeeperport" << m_config.Load("zookeeperport") << std::endl;
}

MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}