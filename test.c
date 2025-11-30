#include <stdint.h>

#define SBI_GEN_RESULT_CODES(x) \
    x(SBI_SUCCESS, 0, "Completed successfully") \
    x(SBI_ERR_FAILED, -1, "Failed") \
    x(SBI_ERR_NOT_SUPPORTED, -2, "Not supported") \
    x(SBI_ERR_INVALID_PARAM, -3, "Invalid parameter(s)") \
    x(SBI_ERR_DENIED, -4, "Denied or not allowed") \
    x(SBI_ERR_INVALID_ADDRESS, -5, "Invalid address(s)") \
    x(SBI_ERR_ALREADY_AVAILABLE, -6, "Already available") \
    x(SBI_ERR_ALREADY_STARTED, -7, "Already started") \
    x(SBI_ERR_ALREADY_STOPPED, -8, "Already stopped") \
    x(SBI_ERR_NO_SHMEM, -9, "Shared memory not available") \
    x(SBI_ERR_INVALID_STATE, -10, "Invalid state") \
    x(SBI_ERR_BAD_RANGE, -11, "Bad (or invalid) range") \
    x(SBI_ERR_TIMEOUT, -12, "Failed due to timeout") \
    x(SBI_ERR_IO, -13, "Input/Output error") \
    x(SBI_ERR_DENIED_LOCKED, -14, "Denied or not allowed due to lock status")

typedef uint64_t uXLEN;
typedef int64_t iXLEN;

typedef iXLEN Sbi_Result_Code;

enum {
#define X(name, code, string) name = code,
SBI_GEN_RESULT_CODES(X)
#undef X
};

typedef struct Sbi_Result {
    Sbi_Result_Code code;
    union {
        iXLEN value;
        uXLEN uvalue;
    } u;
} Sbi_Result;

static const char *sbi_code_to_string(long code) {
    switch (code) {
#define X(name, code, string) case name: return string;
        SBI_GEN_RESULT_CODES(X)
#undef X
        default: return "UNKNOWN";
    }
}

extern void print_string(const char *str);
extern Sbi_Result sbi_call(long eid, long fid, ...);
extern uint64_t get_time(void);

static void print_ulong(unsigned long value) {
    char buf[32];
    char *c = buf + sizeof(buf);
    *(--c) = '\0';
    do {
        *(--c) = '0' + value % 10;
        value /= 10;
    } while (value);
    print_string(c);
}

static void print_sbiret(Sbi_Result result) {
    print_ulong(result.u.uvalue);
    print_string("; ");
    print_string(sbi_code_to_string(result.code));
    print_string("\n");
}

void c_trap_handler(void) {
    print_string("trap ");
    long time = get_time();
    print_ulong(time);
    print_string("\n");
    sbi_call(0x54494D45, 0, (uint64_t)time + 10000000);
}

void foo(void) {
    print_string("hello from C!\n");
    Sbi_Result result = sbi_call(0x10, 1);
    print_sbiret(result);

    long time = get_time();
    result = sbi_call(0x54494D45, 0, (uint64_t)time + 10000000);
    print_sbiret(result);
}
