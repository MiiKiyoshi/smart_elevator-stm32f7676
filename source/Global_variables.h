//스텝모터
U08 stop_elevator = 1, init_motor_flag = 1; // 엘레베이터 정지, 모터 초기화모드로 시작
U08 EL_operation_flag, Forward_flag, Backward_flag, Feedback_flag;
int steps;
int curr_floor;

void stepmotor_init(void);
unsigned char stepForward(void);			// 시계방향 회전
unsigned char stepBackward(void);		// 반시계방향 회전
void Forward(double angle);
void Backward(double angle);
void move_EL(int dest_floor);
void feedback_EL(void);

// 1상 여자 방식의 구동 데이터
unsigned char step_data[4] = {0x00000010, 0x00000020, 0x00000040, 0x00000080};
// 구동 데이터를 위한 인덱스 
int step_index = -1;

//heap
#define MAX_ELEMENT 200

typedef struct {
	int key;
	int floor;
} element;

typedef struct {
	element heap[MAX_ELEMENT];
	int heap_size;
} HeapType;

HeapType* heap; 

//초음파 센서
#define numReadings 10

U08 Ulso_print_flag, Ulso_accuracy_flag;
double ulso_distance;

double readings[numReadings];
int readindex;
double total, ulso_accu_distance;
