CC:=arm-linux-g++  #编译环境
TARGET:=./panclient  # 可执行文件名
FLAGS:=-I. -L. -lfont -lpthread -std=c++11 #链接库
SRCS:=$(wildcard ./*.cpp) 
OBJS:=$(patsubst %.cpp,%.o,$(SRCS)) 

$(TARGET):$(OBJS)  #使用变量
	$(CC) -o $@  $^   $(FLAGS)
	rm $(OBJS)
%.o:%.cpp 
	$(CC) -c $< -o $@  $(FLAGS)
clean:
	rm $(TARGET)
	rm $(OBJS)
send:
	#scp $(TARGET) root@192.168.64.222:/zayu/netpan
