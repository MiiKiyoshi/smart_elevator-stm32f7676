# -------------------settings-----------------------#
CC = gcc
TARGET = a.out
OBJS = main.o heap.o elevator.o
	# CC: 사용할 컴파일러 TARGET(@): 만들 실행파일 OBJS(^): 의존하는 놈들

# 컴파일 옵션
CFLAGS = -g -wall
	# -g: 디버그(gdb 이용) -wall: warning을 띄워줌

# 링크 옵션
LDFLAGS = -lm
	# -lm: 수학 라이브러리

# ---------------------Making-----------------------#
# all: 결론적으로 뭘 만들지
all: $(TARGET)

# 링크 과정
$(TARGET): $(OBJS)
	$(CC) -o $@ $^

# 컴파일 과정
.c.o:
	$(CC) -c -o $@ $<

# 다 만들었으면 오브젝트파일들 삭제
clean:
	rm -f $(OBJS) $(TARGET)
