# Code Review — ugly_sensor.c

## Review Summary

The original `ugly_sensor.c` contains memory-safety issues, incorrect control-flow handling, naming violations, and coding-guideline violations.

The review follows the coding guidelines and review-severity definitions from GET 2026 Session 1.

## Findings

### F-01: Incorrect SQR Macro

**Severity:** MUST

**Issue:**

The original macro is defined as:

    #define SQR(x) x*x

The macro does not bracket its parameter or the complete expression. For example, `SQR(1+2)` expands to `1+2*1+2`, producing `5` instead of `9`.

**Fix:**

Replace the macro with a function that calculates the square, or use a correctly parenthesized macro:

    #define SQR(x) ((x) * (x))

The function approach avoids this macro-expansion problem.

### F-02: Inconsistent Naming Convention

**Severity:** SHOULD

**Issue:**

Names such as `Reg_t`, `ReadReg`, and `InitSensor` use naming styles that do not follow the required `snake_case` convention.

**Fix:**

Use descriptive lowercase names with underscores, such as `reg_t`, `read_reg`, and `init_sensor`.

### F-03: Global Variables Lack the Required Prefix

**Severity:** SHOULD

**Issue:**

The original global variables `cnt` and `Regs` do not follow the required global-variable naming convention.

**Fix:**

Use descriptive names with the `g_` prefix, such as `g_count` and `g_regs`.

### F-04: Unsafe String Copy Causes Buffer Overflow

**Severity:** MUST

**Issue:**

The original code declares an eight-byte buffer:

    char DevName[8];

It then copies `"CameraSensor0"` into that buffer using `strcpy()`.The string requires more space than the buffer provides, including its terminating null character.

This can cause a buffer overflow and undefined behavior.

**Fix:**

Validate the input length and use a bounded copy operation, such as `snprintf()`, with the destination buffer's size.

### F-05: Unchecked Memory Allocation and Memory Leak

**Severity:** MUST

**Issue:**

The original code allocates memory using `malloc()` without checking whether the returned pointer is `NULL`.
If allocation fails, subsequent accesses through `Regs` may crash. The allocated memory is also never released.

**Fix:**

Check the allocation result before accessing the memory. Release the allocation using `free()` exactly once when it is no longer needed, including on applicable error paths.

### F-06: Array Out-of-Bounds Access

**Severity:** MUST

**Issue:**

The original loop uses:

    for (i = 0; i <= REG_CNT; i++)

The array contains four elements, so its valid indexes are `0` through `3`. The loop also accesses index `4`, which is outside the allocated array.This can corrupt memory or cause undefined behavior.

**Fix:**

Use a strict less-than condition:

    for (i = 0; i < REG_COUNT; i++)

**Guideline:** Review checklist — Verify array bounds and buffer sizes.

### F-07: Magic Numbers in the Implementation

**Severity:** SHOULD

**Issue:**

Values such as `0x3000`, `0x6000`, `0x6002`, `0x6004`, and `10` are embedded directly in the implementation.
Their purpose is not immediately clear, and changing them requires editing the code in multiple places.

**Fix:**

Define meaningful named constants, such as `SENSOR_REG_BASE`, `SENSOR_MODE_ONE`, and `TEMPERATURE_LIMIT`.

### F-08: Missing Braces Cause Misleading Control Flow

**Severity:** MUST

**Issue:**

The original code contains:

    if (Regs[0].Val == 0x6000) printf("sensor ok\n"); printf("init done\n");

Only the first `printf()` belongs to the `if` statement. The second executes regardless of the condition, despite the misleading indentation.

**Fix:**

Add braces around the conditional statement and place the unconditional initialization message outside the block if that behavior is intended.

### F-09: Switch Statement Lacks a Default Case

**Severity:** SHOULD

**Issue:**

The original `switch` handles only the two expected register values and has no `default` branch.
Unexpected values leave `cnt` dependent on its previous state, which can lead to incorrect behavior.

**Fix:**

Add a `default` branch that assigns a defined fallback value. Keep explicit `break` statements where needed.

### F-10: Return Variable May Be Uninitialized

**Severity:** MUST

**Issue:**

The original variable `ret` is assigned only when `cnt` is greater than `1` or equal to `1`.
If neither condition is true, `ret` remains uninitialized. Using it in the return expression can produce undefined behavior.

**Fix:**

Initialize `ret` when it is declared and ensure that all execution paths produce a defined result.

### F-11: Signed and Unsigned Integer Comparison

**Severity:** MUST

**Issue:**

The original code compares:

    unsigned int lim = 10;
    int t = -1;

    if (t < lim)

The signed value `t` is converted to an unsigned value for the comparison. Consequently, `-1` does not behave as an ordinary negative number in this comparison.

**Fix:**

Use compatible integer types for the comparison, or explicitly convert values when the conversion is appropriate.

### F-12: Unused Variable

**Severity:** SHOULD

**Issue:**

The original declaration includes `unused`, but the variable is never used.
This makes the code less clear and may generate compiler warnings.

**Fix:**

Remove the unused variable.

### F-13: Return Value from InitSensor Is Ignored

**Severity:** MUST

**Issue:**

The original `main()` calls `InitSensor()` without checking its return value.
Consequently, it cannot determine whether initialization succeeded or failed based on that result.

**Fix:**

Store and check the returned value, and report an appropriate program exit status.
