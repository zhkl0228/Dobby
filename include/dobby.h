#ifndef dobby_h
#define dobby_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t addr32_t;
typedef uint64_t addr64_t;

#if defined(__arm64__) || defined(__aarch64__)
#define Tx(type) type##arm64
#define TX() type##ARM64
#define xT() arm64##type
#define XT() ARM64##type
typedef union _FPReg {
  __int128_t q;
  struct {
    double d1;
    double d2;
  } d;
  struct {
    float f1;
    float f2;
    float f3;
    float f4;
  } f;
} FPReg;

typedef struct _RegisterContext {
  uint64_t dmmpy_0; // dummy placeholder

  union {
    uint64_t x[29];
    struct {
      uint64_t x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22,
          x23, x24, x25, x26, x27, x28;
    } regs;
  } general;

  uint64_t fp;
  uint64_t lr;

  union {
    FPReg q[8];
    struct {
      FPReg q0, q1, q2, q3, q4, q5, q6, q7;
    } regs;
  } floating;
} RegisterContext;
#elif defined(__arm__)
#define Tx(type) type##arm
#define TX() type##ARM
#define xT() arm##type
#define XT() ARM##type
typedef struct _RegisterContext {
  uint32_t dummy_0;
  uint32_t dummy_1;

  union {
    uint32_t r[13];
    struct {
      uint32_t r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12;
    } regs;
  } general;

  uint32_t lr;
} RegisterContext;
#elif defined(_M_X64) || defined(__x86_64__)
typedef struct _RegisterContext {
  uint64_t dummy_0; // dummy placeholder => rsp align

  union {
    struct {
      uint64_t rax, rbx, rcx, rdx, rbp, rsp, rdi, rsi, r8, r9, r10, r11, r12, r13, r14, r15;
    } regs;
  } general;
  uint64_t flags;
} RegisterContext;
#endif

#define RT_FAILED -1
#define RT_SUCCESS 0
typedef enum _RetStatus { RS_FAILED = -1, RS_SUCCESS = 0 } RetStatus;

typedef enum _PackageType {
  kFunctionWrapper,
  kFunctionInlineHook,
  kDynamicBinaryInstrument
} PackageType,
    HookEntryType;

typedef struct _HookEntryInfo {
  uintptr_t hook_id;
  union {
    void *target_address;
    void *function_address;
    void *instruction_address;
  };
} HookEntryInfo;

#if 0
// wrap function with pre_call and post_call
typedef void (*PreCallTy)(RegisterContext *reg_ctx, const HookEntryInfo *info);
typedef void (*PostCallTy)(RegisterContext *reg_ctx, const HookEntryInfo *info);
int DobbyWrap(void *function_address, PreCallTy pre_call, PostCallTy post_call);
#endif

// replace function
int DobbyHook(void *function_address, void *replace_call, void **origin_call);

// dynamic binary instrument for instruction
typedef void (*DBICallTy)(RegisterContext *reg_ctx, const HookEntryInfo *info);
int DobbyInstrument(void *instr_address, DBICallTy dbi_call);

// iterate symbol table and find symbol
void *DobbySymbolResolver(const char *image_name, const char *symbol_name);

// bxx branch plugin
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_X64) || defined(__x86_64__)
void dobby_enable_near_branch_trampoline();
void dobby_disable_near_branch_trampoline();
#endif

#ifdef __cplusplus
}
#endif

#endif
