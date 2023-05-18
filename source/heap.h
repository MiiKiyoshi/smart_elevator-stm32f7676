/*
 * @Author: 공기정 
 * @Date: 2023-05-15 22:24:38 
 * @Last Modified by: 공기정
 * @Last Modified time: 2023-05-15 22:24:58
 */


/***********************function prototype definition*************************/
void el_call(HeapType* heap, int dest_floor);
int el_run(HeapType* heap);
HeapType* create();
void init(HeapType* h);
void insert_min_heap(HeapType* h, element item);
element delete_min_heap(HeapType* h);
/*****************************************************************************/

int heap_num;
/*****************************************************************************/
void el_call(HeapType* heap, int dest_floor){
  element call;

  call.floor = dest_floor;
  
  // key = |현재 층 - 목적 층|
  if((curr_floor - call.floor) < 0){
    call.key = call.floor - curr_floor;
  }
  else{
    call.key = curr_floor - call.floor;
  }

  insert_min_heap(heap, call);
  heap_num++;
}

int el_run(HeapType* heap){
  
  if(heap_num != 0){
    element call = delete_min_heap(heap);
    heap_num--;
    return call.floor;
  }
  
  return curr_floor;
}

// 생성 함수
HeapType* create()
{
	return (HeapType*)malloc(sizeof(HeapType));
}

// 초기화 함수
void init(HeapType* h)
{
    memset(h, 0, sizeof(HeapType));
    h->heap_size = 0;
}
// 현재 요소의 개수가 heap_size인 히프 h에 item을 삽입한다.

// 삽입 함수
void insert_min_heap(HeapType* h, element item)
{
	int i;
	i = ++(h->heap_size);

	//  트리를 거슬러 올라가면서 부모 노드와 비교하는 과정
	while ((i != 1) && (item.key < h->heap[i / 2].key)) {
		h->heap[i] = h->heap[i / 2];
		i /= 2;
	}
	h->heap[i] = item;     // 새로운 노드를 삽입
}

// 삭제 함수
element delete_min_heap(HeapType* h)
{
	int parent, child;
	element item, temp;
	item = h->heap[1];
	temp = h->heap[(h->heap_size)--];
	parent = 1;
	child = 2;
	while (child <= h->heap_size) {
		// 현재 노드의 자식노드 중 더 큰 자식노드를 찾는다.
		if ((child < h->heap_size) &&
			(h->heap[child].key) > h->heap[child + 1].key)
			child++;
		if (temp.key <= h->heap[child].key) break;
		// 한 단계 아래로 이동
		h->heap[parent] = h->heap[child];
		parent = child;
		child *= 2;
	}
	h->heap[parent] = temp;
        
        return item;
}