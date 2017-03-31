// -----------------------------------------------------------------------------------------
// microbench by rigaya
// -----------------------------------------------------------------------------------------
// The MIT License
//
// Copyright (c) 2017 rigaya
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// --------------------------------------------------------------------------------------------

#include "osdep.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <intrin.h>
#include <algorithm>
#include <chrono>
#include "cpu_info.h"

#define TEST_COUNT 20000
#define BLOCK_SIZE 96

#define PROTOTYPE(x) \
    extern int64_t runt_ ## x(uint32_t count); \
    extern int64_t runl_ ## x(uint32_t count); \
    extern int64_t runt_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runl_ ## x ## _vex_256(uint32_t count);

#define PROTOTYPE_VEX(x) \
    extern int64_t runt_ ## x ## _vex(uint32_t count); \
    extern int64_t runl_ ## x ## _vex(uint32_t count); \
    extern int64_t runt_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runl_ ## x ## _vex_256(uint32_t count);

#define PROTOTYPE_PAIR(x) \
    extern int64_t runtp_ ## x(uint32_t count); \
    extern int64_t runlp_ ## x(uint32_t count); \
    extern int64_t runtp_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runlp_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runts_ ## x(uint32_t count); \
    extern int64_t runls_ ## x(uint32_t count); \
    extern int64_t runts_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runls_ ## x ## _vex_256(uint32_t count);

#define PROTOTYPE_PAIR_VEX(x) \
    extern int64_t runtp_ ## x ## _vex(uint32_t count); \
    extern int64_t runlp_ ## x ## _vex(uint32_t count); \
    extern int64_t runtp_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runlp_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runts_ ## x ## _vex(uint32_t count); \
    extern int64_t runls_ ## x ## _vex(uint32_t count); \
    extern int64_t runts_ ## x ## _vex_256(uint32_t count); \
    extern int64_t runls_ ## x ## _vex_256(uint32_t count);

#ifdef __cplusplus
extern "C" {
#endif
    PROTOTYPE(por);
    PROTOTYPE(pand);
    PROTOTYPE(pandn);
    PROTOTYPE(pxor);
    PROTOTYPE(psllw);
    PROTOTYPE(pslldq);
    PROTOTYPE(paddw);
    PROTOTYPE(paddsw);
    PROTOTYPE(pmullw);
    PROTOTYPE(pmuludq);
    PROTOTYPE(pmulld);
    PROTOTYPE(pmaddwd);
    PROTOTYPE(pmaddubsw);
    PROTOTYPE(pabsw);
    PROTOTYPE(pmaxsw);
    PROTOTYPE(pminsw);
    PROTOTYPE(pavgw);
    PROTOTYPE(punpckhwd);
    PROTOTYPE(palignr);
    PROTOTYPE(pshufb);
    PROTOTYPE(pblendw);
    PROTOTYPE_VEX(pblendvb);
    PROTOTYPE(addps);
    PROTOTYPE(addpd);
    PROTOTYPE(mulps);
    PROTOTYPE(mulpd);
    PROTOTYPE(divps);
    PROTOTYPE(divpd);
    PROTOTYPE(sqrtps);
    PROTOTYPE(sqrtpd);
    PROTOTYPE(rcpps);
    PROTOTYPE(rsqrtps);
    PROTOTYPE_VEX(fmadd132ps);
    PROTOTYPE_VEX(fmadd132pd);
    PROTOTYPE(maxps);
    PROTOTYPE(maxpd);
    PROTOTYPE(minps);
    PROTOTYPE(minpd);
    PROTOTYPE_PAIR(addps_mulps);
    PROTOTYPE_PAIR(addpd_mulpd);
    PROTOTYPE_PAIR_VEX(fmadd132ps_addps);
    PROTOTYPE_PAIR_VEX(fmadd132pd_addpd);
    PROTOTYPE_PAIR_VEX(fmadd132ps_mulps);
    PROTOTYPE_PAIR_VEX(fmadd132pd_mulpd);
#ifdef __cplusplus
}
#endif

typedef int64_t(*func_check)(uint32_t count);

typedef struct {
    const char *name;
    func_check latency;
    func_check throughput;
    int flops_per_128op;
} check_inst_t;

#define CREATE_LIST(x, flops_128op) \
    { #x,       runl_ ## x, runt_ ## x, flops_128op }, \
    { #x "(256)", runl_ ## x ## _vex_256, runt_ ## x ## _vex_256, flops_128op * 2 }

#define CREATE_LIST_VEX(x, flops_128op) \
    { #x,       runl_ ## x ## _vex, runt_ ## x ## _vex, flops_128op }, \
    { #x "(256)", runl_ ## x ## _vex_256, runt_ ## x ## _vex_256, flops_128op * 2 }

#define CREATE_LIST_PAIR(x, flops_128op) \
    { #x "",       runlp_ ## x, runtp_ ## x, flops_128op }, \
    { #x "(256)", runlp_ ## x ## _vex_256, runtp_ ## x ## _vex_256, flops_128op * 2 }/*, \
    { #x "serial",       runls_ ## x, runts_ ## x, flops_128op }, \
    { #x "serial(256)", runls_ ## x ## _vex_256, runts_ ## x ## _vex_256, flops_128op * 2 }*/

#define CREATE_LIST_PAIR_VEX(x, flops_128op) \
    { #x "",       runlp_ ## x ## _vex, runtp_ ## x ## _vex, flops_128op }, \
    { #x "(256)", runlp_ ## x ## _vex_256, runtp_ ## x ## _vex_256, flops_128op * 2 }/*, \
    { #x "serial",       runls_ ## x ## _vex, runts_ ## x ## _vex, flops_128op }, \
    { #x "serial(256)", runls_ ## x ## _vex_256, runts_ ## x ## _vex_256, flops_128op * 2 }*/

check_inst_t check_list[] = {
    CREATE_LIST(por,    0),
    CREATE_LIST(pand,   0),
    CREATE_LIST(pandn,  0),
    CREATE_LIST(pxor,   0),
    CREATE_LIST(psllw,  0),
    CREATE_LIST(pslldq, 0),
    CREATE_LIST(paddw,  0),
    CREATE_LIST(paddsw, 0),
    CREATE_LIST(pabsw,  0),
    CREATE_LIST(pmullw, 0),
    CREATE_LIST(pmuldq, 0),
    CREATE_LIST(pmulld, 0),
    CREATE_LIST(pmaddwd, 0),
    CREATE_LIST(pmaddubsw, 0),
    CREATE_LIST(pmaxsw, 0),
    CREATE_LIST(pminsw, 0),
    CREATE_LIST(pavgw,  0),
    CREATE_LIST(punpckhwd,  0),
    CREATE_LIST(palignr,  0),
    CREATE_LIST(pshufb,  0),
    CREATE_LIST(pblendw, 0),
    CREATE_LIST_VEX(pblendvb, 0),
    CREATE_LIST(addps,  4),
    CREATE_LIST(addpd,  2),
    CREATE_LIST(mulps,  4),
    CREATE_LIST(mulpd,  2),
    CREATE_LIST_PAIR(addps_mulps, 4),
    CREATE_LIST_PAIR(addpd_mulpd, 2),
    CREATE_LIST_VEX(fmadd132ps, 8),
    CREATE_LIST_VEX(fmadd132pd, 4),
    CREATE_LIST_PAIR_VEX(fmadd132ps_addps, 6),
    CREATE_LIST_PAIR_VEX(fmadd132pd_addpd, 3),
    CREATE_LIST_PAIR_VEX(fmadd132ps_mulps, 6),
    CREATE_LIST_PAIR_VEX(fmadd132pd_mulpd, 3),
    CREATE_LIST(divps,  0),
    CREATE_LIST(divpd,  0),
    CREATE_LIST(sqrtps, 0),
    CREATE_LIST(sqrtpd, 0),
    CREATE_LIST(rcpps,   0),
    CREATE_LIST(rsqrtps, 0),
    CREATE_LIST(maxps,  0),
    CREATE_LIST(maxpd,  0),
    CREATE_LIST(minps,  0),
    CREATE_LIST(minpd,  0),
};

void warm_up() {
    for (int i = 0; i < 1000; i++) {
        runl_por(TEST_COUNT);
    }
}

double get_tick_per_clock() {
    const int loop_count = 1000;
    auto tick_min = runl_por(TEST_COUNT);
    for (int i = 0; i < loop_count; i++) {
        tick_min = std::min(tick_min, runl_por(TEST_COUNT));
    }
    return tick_min / ((double)BLOCK_SIZE * TEST_COUNT);
}

double get_tick_per_sec() {
    const int nMul = 100;
    runl_por(TEST_COUNT * nMul);
    auto start = std::chrono::high_resolution_clock::now();
    int64_t tick = runl_por(TEST_COUNT * nMul);
    auto fin = std::chrono::high_resolution_clock::now();
    double second = std::chrono::duration_cast<std::chrono::microseconds>(fin - start).count() * 1e-6;
    return tick / second;
}

int main(int argc, char **argv) {
    char buffer[256];
    getCPUInfo(buffer);
    fprintf(stdout, "%s\n", buffer);

    SetThreadAffinityMask(GetCurrentThread(), 1);
    const double tick_per_clock = get_tick_per_clock();
    const double tick_per_sec = get_tick_per_sec();
    fprintf(stdout, "core frequency: %d MHz\n", (int)(tick_per_sec / tick_per_clock * 1e-6 + 0.5));

    size_t max_name_len = 0;
    for (int i = 0; i < _countof(check_list); i++) {
        max_name_len = std::max(max_name_len, strlen(check_list[i].name));
    }

    const int loop_count = 25;
    for (int i = 0; i < _countof(check_list); i++) {
        auto tick_t = INT64_MAX;
        auto tick_l = INT64_MAX;
        for (int j = 0; j < loop_count; j++) {
            tick_t = std::min(tick_t, check_list[i].throughput(TEST_COUNT));
            tick_l = std::min(tick_l, check_list[i].latency(TEST_COUNT));
        }
        char name[256] = { 0 };
        strcpy(name, check_list[i].name);
        for (size_t j = strlen(name); j < max_name_len; j++) {
            name[j] = ' ';
        }
        fprintf(stdout, "%s:  T: %5.2f, L: %5.2f", name,
            TEST_COUNT * (double)BLOCK_SIZE * tick_per_clock / (double)tick_t,
            1.0 / (TEST_COUNT * (double)BLOCK_SIZE * tick_per_clock / (double)tick_l));
        if (check_list[i].flops_per_128op > 0) {
            double flops = TEST_COUNT * (double)BLOCK_SIZE * check_list[i].flops_per_128op / (tick_t / tick_per_sec);
            fprintf(stdout, ",  %5.1f GFLOPS", flops * 1e-9);
        }
        fprintf(stdout, "\n");
    }
}
