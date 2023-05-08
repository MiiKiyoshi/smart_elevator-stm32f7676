# Smart elevator (stm-32f767vg)

>  Department of Electronic Engineering, Myongji University

> 마이크로프로세서 응용 (2023-1) 2조 Smart Raise 팀 

현재 열심히 제작중

## 설계도

### 함수

```mermaid
%%{init: {'theme':'neutral'}}%%
flowchart LR
	%%-----------------box-----------------%%
	subgraph 함수
		subgraph I[UART 블루투스 통신]
			I_a(UART 입력)
			I_b(UART 출력)
		end	
		subgraph H[RFID 모듈]
			H_a(RFID 카드 인식)
		end	
		subgraph G[초음파 센서]
			G_a(엘레베이터와의 거리 측정)
		end	
		subgraph F[스텝 모터]
			F_a(분할각 단위로 회전)
			F_b(회전중 지연함수)
		end
	
		subgraph E[엘레베이터 작동 시스템]
			E_a(현재층 값)
			E_b(목적층으로 이동후 현재층 업데이트)
			E_c(피드백 시스템)
			E_d(작동 정지 및 버저 작동)
		end
	
		subgraph D[heap]
			D_a(목적층과 키값 힙에 넣기)
			D_b(힙에서 루트노드 빼기)
		end
	
		subgraph C[엘레베이터 호출 시스템]
			C_a(콘솔창으로 목적층값 입력 받고\n예상 대기시간 출력)
			C_b(현재 층수, 탑승인원 출력)
		end
	end
	
	
	%%-----------------link-----------------%%
	
	%%층수 입력 받아서 힙에 넣기%%
	C_a -->|목적층값| D_a 
	
	%%현재층값 출력%%
	E_b -->|현재층값 업데이트| E_a
    
    %%현재층값 이동%%
    E_a -->|현재층값 보내기| C_b
	
	%%힙에서 목적층 빼서 이동%%
	D_b -->|목적층| E_b
	
	%%엘레베이터와 스텝모터%%
	E_b -->|각도값| F_a
	
	%%초음파센서로 엘레베이터 도착 확인%%
	G_a ---|엘레베이터의 높이| E_c
	
	%%피드백 시스템%%
	E_b ---|층수의 고도| E_c 
	
	%%RFID 모듈%%
	H_a -->|허가받은 층| E_b
	
	%%작동 정지 및 버저 작동%%
	E_c -->|3회 실패시| E_d
	
	%%목적층 입력%%
	I_a -->|목적층| C_a
	
	%%현재 층수, 탑승인원 출력%%
	C_b -->|현재 층수, 탑승인원| I_b
	
	%%피드백 각도값%%
	E_c -->|각도값| F_a
	
	%%예상 대기시간 출력%%
	C_a -->|예상 대기시간| I_b
	
	%%스텝모터 회전중 지연%%
	F_a -->|각도값| F_b
```

### 코드

```mermaid
%%{init: {'theme':'neutral'}}%%
flowchart LR
subgraph 코드
	subgraph A[인터럽트]
			subgraph A_A[엘레베이터 작동]
			end
		end
		direction TB
		subgraph B[main]
			subgraph B_A[초기화]
				G_a(스텝 모터,RFID 모듈,무게 센서,온도 센서,쿨러,초음파 센서,버저)
			end
			subgraph B_B[while문]
				B_B_a(층수 입력받기)
			end
   	 end
	end
	
```



### FLOW CHART

<img src="./description/flowchart.png" alt="flowchart" style="zoom: 25%;" />

### PIN LAYOUT

<img src="./description/module.png" alt="module" style="zoom:25%;" />
