#Makefile

#自定义变量
CC		= gcc
#编译选项，生成所有警告、不优化、采用c++11标准、输出调试信息、只编译并生成目标文件
CFLAGS	= -Wall -O2 -g -c 
FILE    = ./test.c
#wildcard为Makefile模式匹配关键字，获取目标目录符合匹配模式的所有文件名
SRCS	= $(FILE) $(wildcard ./snowflake/*.c)
#patsubst为Makefile模式替换关键字，查找字符串SRCS中按空格分开的单词，并将符合模式%.cpp的字符串全部替换成%.o
OBJS	= $(patsubst ./%.c, ./%.o, $(SRCS))
EXES	= test
RM		= rm -f

#默认任务
default:
#默认任务要执行的命令，按上面的变量名替换为变量值后执行，前面必须有一个Tab符
	$(MAKE) $(EXES)

#模式匹配,冒号前者为目标项，冒号后面为依赖项
$(EXES): $(OBJS)
#$^表示规则中所有的依赖项，$@表示规则中的目标
	$(CC) $^ -lm -lpthread -o $@

# %模式自动匹配符
%.o: %.c
# $<表示规则中的第一个依赖项
	$(CC) $(CFLAGS) $< -o $@

#伪目标，声明clean为伪目标或标签，为了避免该清理任务与文件名相同而被错识别
.PHONY: clean
clean:
#清理之前的目标文件，以便下次完整的重新编译
	$(RM) $(OBJS) $(EXES)