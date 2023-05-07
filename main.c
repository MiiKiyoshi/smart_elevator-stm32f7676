#include <stdio.h>
#include <stdlib.h>
#include "elevator.h"

int main(void){

	// 히프 생성
	HeapType* heap; 
	heap = create(); 
	init(heap);	
  
  for(int i=0; i<5; i++)
    el_call(heap);
  for(int i=0; i<5; i++)
    el_run(heap);
}
 
/*
int main(void)
{
	element e1 = { 10, 0 }, e2 = { 5, 1 }, e3 = { 30, 2 };

	element e4, e5, e6;
	HeapType* heap;

	heap = create(); 	// 히프 생성
	init(heap);	// 초기화

  // 삽입
	insert_min_heap(heap, e1);
	insert_min_heap(heap, e2);
	insert_min_heap(heap, e3);

	// 삭제
	e4 = delete_min_heap(heap);
	printf("< %d > %d ", e4.key, e4.floor);
	e5 = delete_min_heap(heap);
	printf("< %d > %d", e5.key, e5.floor);
	e6 = delete_min_heap(heap);
	printf("< %d > %d\n", e6.key, e6.floor);

	free(heap);
	return 0;
}
*/
