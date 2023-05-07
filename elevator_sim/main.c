#include <stdio.h>
#include <stdlib.h>
#include "elevator.h"

int main(void){

	// 히프 생성
	HeapType* heap; 
	heap = create(); 
	init(heap);	

  el_current_floor();
  for(int i=0; i<5; i++)
    el_call(heap);
  for(int i=0; i<5; i++)
    el_run(heap);

}
