; G8RTOS_SchedulerASM.s
; Holds all ASM functions needed for the scheduler
; Note: If you have an h file, do not have a C file and an S file of the same name

	; Functions Defined
	.def G8RTOS_Start, PendSV_Handler

	; Dependencies
	.ref CurrentlyRunningThread, G8RTOS_Scheduler

	.thumb		; Set to thumb mode
	.align 2	; Align by 2 bytes (thumb mode uses allignment by 2 or 4)
	.text		; Text section

; Need to have the address defined in file 
; (label needs to be close enough to asm code to be reached with PC relative addressing)
RunningPtr: .field CurrentlyRunningThread, 32

; G8RTOS_Start
;	Sets the first thread to be the currently running thread
;	Starts the currently running thread by setting Link Register to tcb's Program Counter
G8RTOS_Start:

	.asmfunc
	; Implement this
	CPSID I 
	
	LDR R0, RunningPtr
	LDR R1, [R0]
	LDR SP, [R1]
	POP {R4-R11}
	POP {R0-R3}
	POP {R12}
	ADD SP, SP, #4
	POP {LR}
	ADD SP, SP, #4
	LDR R0, RunningPtr
	LDR R1, [R0]
	STR SP, [R1]
	
	CPSIE I		;enable interrupts
	BX LR
	.endasmfunc

; PendSV_Handler
; - Performs a context switch in G8RTOS
; 	- Saves remaining registers into thread stack
;	- Saves current stack pointer to tcb
;	- Calls G8RTOS_Scheduler to get new tcb
;	- Set stack pointer to new stack pointer from new tcb
;	- Pops registers from thread stack
PendSV_Handler:
	
	.asmfunc
	;Implement this
	CPSID I
	PUSH {R4-R11}	
	
	LDR R0, RunningPtr	;load the pointer 
	LDR R1, [R0]		;get value at address pointed to
	STR SP, [R1]	;store MSP into SP field of current thread
	
	PUSH {LR}
	BL G8RTOS_Scheduler			;G8RTOS_Scheduler()
	POP {LR}
	
	LDR R0, RunningPtr	;load the pointer 
	LDR R1, [R0]		;get value at address pointed to
	LDR SP, [R1]
	POP {R4-R11}
	
	STR SP, [R1]			;store MSP into SP field of current thread
	
	
	CPSIE I
	BX LR
	.endasmfunc
	
	; end of the asm file
	.align
	.end
