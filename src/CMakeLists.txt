aux_source_directory(. SRC_LIST)

add_library(mprpc ${SRC_LIST})

# linux库文件下，没有muduo这个名称的库，只有分别以下的这些，记不住可以直接 sudo find /usr -name "libmuduo*" 找以muduo名称有关的所有库
# net库写前面，因为base依赖了net库， 一开始是add_library(mprpc SHARED ${SRC_LIST})编译的动态库，但是muduo库是静态库，链接的时候有问题
# 所以改成了编译成动态库
target_link_libraries(mprpc muduo_net muduo_base pthread)