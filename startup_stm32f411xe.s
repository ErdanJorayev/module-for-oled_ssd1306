.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16
.thumb

/* Символы из linker_script.ld */
.word _estack
.word _sdata
.word _edata
.word _sidata
.word _sbss
.word _ebss

.global g_pfnVectors
.global Reset_Handler

/* Таблица векторов прерываний */
.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
g_pfnVectors:
    .word _estack            /* Вершина стека */
    .word Reset_Handler      /* Обработчик сброса */
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler

/* Обработчик Reset */
.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    /* 1. Копирование секции .data из Flash в SRAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
    movs r3, #0
    b LoopCopyDataInit

CopyDataInit:
    ldr r4, [r2, r3]
    str r4, [r0, r3]
    adds r3, r3, #4

LoopCopyDataInit:
    adds r4, r0, r3
    cmp r4, r1
    bcc CopyDataInit

    /* 2. Обнуление секции .bss в SRAM */
    ldr r2, =_sbss
    ldr r4, =_ebss
    movs r3, #0
    b LoopFillZerobss

FillZerobss:
    str r3, [r2]
    adds r2, r2, #4

LoopFillZerobss:
    cmp r2, r4
    bcc FillZerobss

    /* 3. Прямой переход в main без вызова Си-библиотек */
    bl main

    /* Зацикливание, если main неожиданно завершился */
Default_Loop:
    b Default_Loop

/* Заглушки для базовых исключений */
.weak NMI_Handler
.thumb_set NMI_Handler, Default_Loop

.weak HardFault_Handler
.thumb_set HardFault_Handler, Default_Loop

.weak MemManage_Handler
.thumb_set MemManage_Handler, Default_Loop

.weak BusFault_Handler
.thumb_set BusFault_Handler, Default_Loop

.weak UsageFault_Handler
.thumb_set UsageFault_Handler, Default_Loop

.weak SVC_Handler
.thumb_set SVC_Handler, Default_Loop

.weak DebugMon_Handler
.thumb_set DebugMon_Handler, Default_Loop

.weak PendSV_Handler
.thumb_set PendSV_Handler, Default_Loop

.weak SysTick_Handler
.thumb_set SysTick_Handler, Default_Loop
