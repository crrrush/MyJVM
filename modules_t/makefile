# 编译器设置
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./ch02

# 目标文件
TARGET = jvm

# 源文件
SRCS = main.cpp

# 目标文件
OBJS = $(SRCS:.cpp=.o)

# 依赖库
LIBS = -lzip

# 默认目标
all: $(TARGET)

# 链接
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

# 编译规则
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
	rm -f $(OBJS) $(TARGET)

# 防止与同名文件冲突
.PHONY: all clean