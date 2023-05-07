#include <stdio.h>
#include <stdlib.h>

#include "heap.h"

int current_floor = 11;

void el_current_floor(){
  printf("현재층: %d\n", current_floor);
}

void el_call(HeapType* heap){
  element call;

  printf("가고싶은 층수를 입력하시오: "); 
  scanf("%d", &call.floor);
  
  // |현재 층 - 목적 층|
  if((current_floor - call.floor) < 0){
    call.key = call.floor - current_floor;
  }
  else{
    call.key = current_floor - call.floor;
  }

  insert_min_heap(heap, call);
}

void el_run(HeapType* heap){

  element call = delete_min_heap(heap);

  printf("목적층: %d  ", call.floor);

  current_floor = call.floor; // 1초후에 엘레베이터의 현재층이 이전 목적층에 도착했다 가정
  printf("현재층: %d\n", call.floor);
}
