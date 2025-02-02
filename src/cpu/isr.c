#include <cpu/isr.h>
#include <stdio.h>
#include <cpu/registers.h>

#define PROTECTION_VIOLATION (1 << 0)
#define CAUSED_BY_WRITE (1 << 1)
#define USER_MODE (1 << 2)
#define MALFORMED_TABLE (1 << 3)
#define INSTRUCTION_FETCH (1 << 4)

void divide_by_zero_handler(exception_stack_frame_t *stack_frame) {
    printf("Exception: Division by Zero\n");
    printf("    instruction pointer: 0x%x\n", stack_frame->instruction_pointer);
    printf("    code_segment: 0x%x\n", stack_frame->code_segment);
    printf("    cpu_flags: 0x%x\n", stack_frame->cpu_flags);
    printf("    stack_pointer: 0x%x\n", stack_frame->stack_pointer);
    printf("    stack_segment: 0x%x\n", stack_frame->stack_segment);
}

void breakpoint_exception_handler(exception_stack_frame_t *stack_frame) {
    printf("Exception: Breakpoint at 0x%x\n", stack_frame->instruction_pointer);
    printf("    instruction pointer: 0x%x\n", stack_frame->instruction_pointer);
    printf("    code_segment: 0x%x\n", stack_frame->code_segment);
    printf("    cpu_flags: 0x%x\n", stack_frame->cpu_flags);
    printf("    stack_pointer: 0x%x\n", stack_frame->stack_pointer);
    printf("    stack_segment: 0x%x\n", stack_frame->stack_segment);
}

void invalid_opcode_handler(exception_stack_frame_t *stack_frame) {
    printf("Exception: Invalid Opcode at 0x%x\n", stack_frame->instruction_pointer);
    printf("    instruction pointer: 0x%x\n", stack_frame->instruction_pointer);
    printf("    code_segment: 0x%x\n", stack_frame->code_segment);
    printf("    cpu_flags: 0x%x\n", stack_frame->cpu_flags);
    printf("    stack_pointer: 0x%x\n", stack_frame->stack_pointer);
    printf("    stack_segment: 0x%x\n", stack_frame->stack_segment);
}

void double_fault_handler(exception_stack_frame_t *stack_frame, uint64_t error_code __attribute__((unused))) {
    printf("Exception: Double Fault\n");
    printf("Stack Frame:\n");
    printf("    instruction pointer: 0x%x\n", stack_frame->instruction_pointer);
    printf("    code_segment: 0x%x\n", stack_frame->code_segment);
    printf("    cpu_flags: 0x%x\n", stack_frame->cpu_flags);
    printf("    stack_pointer: 0x%x\n", stack_frame->stack_pointer);
    printf("    stack_segment: 0x%x\n", stack_frame->stack_segment);
    __asm__ volatile("cli; hlt");
}

void page_fault_handler(exception_stack_frame_t *stack_frame, uint64_t error_code) {
    printf("Exception: Page Fault while Accessing 0x%x\n", cr2());
    switch(error_code) {
    case PROTECTION_VIOLATION:
        printf("ERROR_CODE: PROTECTION_VIOLATION\n");
        break;
    case CAUSED_BY_WRITE:
        printf("ERROR_CODE: CAUSED_BY_WRITE\n");
        break;
    case USER_MODE:
        printf("ERROR_CODE: USER_MODE\n");
        break;
    case MALFORMED_TABLE:
        printf("ERROR_CODE: MALFORMED_TABLE\n");
        break;
    case INSTRUCTION_FETCH:
        printf("ERROR_CODE: INSTRUCTION_FETCH\n");
        break;
    }
    printf("Stack Frame:\n");
    printf("    instruction pointer: 0x%x\n", stack_frame->instruction_pointer);
    printf("    code_segment: 0x%x\n", stack_frame->code_segment);
    printf("    cpu_flags: 0x%x\n", stack_frame->cpu_flags);
    printf("    stack_pointer: 0x%x\n", stack_frame->stack_pointer);
    printf("    stack_segment: 0x%x\n", stack_frame->stack_segment);

    __asm__ volatile("cli; hlt");
}

 __attribute__((naked))
void divide_by_zero_handler_wrapper(void) {
    pushaq();
    __asm__ volatile("movq %%rsp, %%rdi; addq $(9 * 8), %%rdi; call *%0" :: "r"(divide_by_zero_handler) : "rdi");
    popaq();
    __asm__ volatile("iretq");
    __builtin_unreachable();
}

__attribute__((naked))
void breakpoint_exception_handler_wrapper(void) {
    pushaq();
    __asm__ volatile("movq %%rsp, %%rdi; addq $(9 * 8), %%rdi; call *%0" :: "r"(breakpoint_exception_handler) : "rdi");
    popaq();
    __asm__ volatile("iretq");
    __builtin_unreachable();
}

__attribute__((naked))
void invalid_opcode_handler_wrapper(void) {
    pushaq();
    __asm__ volatile("movq %%rsp, %%rdi; addq $(9 * 8), %%rdi; call *%0" :: "r"(invalid_opcode_handler) : "rdi");
    popaq();
    __asm__ volatile("iretq");
    __builtin_unreachable();
}

__attribute__((naked))
void page_fault_handler_wrapper(void) {
    __asm__ volatile("popq %%rsi; movq %%rsp, %%rdi; subq $8, %%rsp; call *%0" :: "r"(page_fault_handler) : "rdi", "rsi");
    __builtin_unreachable();
}

__attribute__((naked))
void double_fault_handler_wrapper(void) {
    __asm__ volatile("popq %%rsi; movq %%rsp, %%rdi; subq $8, %%rsp; call *%0" :: "r"(double_fault_handler) : "rdi", "rsi");
    __builtin_unreachable();
}
