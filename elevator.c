#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

int current_floor = 11;

void el_call(HeapType* heap){
	element* call = (element*)malloc(sizeof(element)); // 요청 받기 전 동적메모리할당

  printf("가고싶은 층수를 입력하시오: "); 
  scanf("%d", &call->floor);
  
  // |현재 층 - 목적 층|
  if((current_floor - call->floor) < 0){
    call->key = call->floor - current_floor;
  }
  else{
    call->key = current_floor - call->floor;
  }

  insert_min_heap(heap, call);
}

void el_run(HeapType* heap){

  element call = delete_min_heap(heap);
  printf("목적층: %d\n", call.floor);

}
