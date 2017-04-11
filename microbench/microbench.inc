; -----------------------------------------------------------------------------------------
; microbench by rigaya
; -----------------------------------------------------------------------------------------
; The MIT License
;
; Copyright (c) 2017 rigaya
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
; THE SOFTWARE.
;
; --------------------------------------------------------------------------------------------

bits 64
default rel

extern INIT_DIVIDEND_F
extern INIT_DIVIDEND_D
extern INIT_DIVIDER_F
extern INIT_DIVIDER_D

%ifdef SIMD256
    %define STR_SIMD_WIDTH _256
%else
    %define STR_SIMD_WIDTH
%endif

%ifdef USE_VEX
    %define STR_VEX _vex
%else
    %define STR_VEX
%endif

%define OP_TYPE      0x0007
%define OP_ADD_IMD   0x0008
%define OP_SKIP_0_3  0x0010
%define OP_SKIP_0_3_SAME_REG  (0x0020 | OP_SKIP_0_3)
%define OP_DIV_F     (0x0040 | OP_SKIP_0_3)
%define OP_DIV_D     (0x0080 | OP_SKIP_0_3)
%define OP_SRC_HALF  (0x0100 | OP_SKIP_0_3)
%define OP_DST_HALF  (0x0200)
%define OP_SKIP_0_1  (0x0400)

%ifdef SIMD256
    %define rmm0  ymm0
    %define rmm1  ymm1
    %define rmm2  ymm2
    %define rmm3  ymm3
    %define rmm4  ymm4
    %define rmm5  ymm5
    %define rmm6  ymm6
    %define rmm7  ymm7
    %define rmm8  ymm8
    %define rmm9  ymm9
    %define rmm10 ymm10
    %define rmm11 ymm11
    %define rmm12 ymm12
    %define rmm13 ymm13
    %define rmm14 ymm14
    %define rmm15 ymm15
%else
    %define rmm0  xmm0
    %define rmm1  xmm1
    %define rmm2  xmm2
    %define rmm3  xmm3
    %define rmm4  xmm4
    %define rmm5  xmm5
    %define rmm6  xmm6
    %define rmm7  xmm7
    %define rmm8  xmm8
    %define rmm9  xmm9
    %define rmm10 xmm10
    %define rmm11 xmm11
    %define rmm12 xmm12
    %define rmm13 xmm13
    %define rmm14 xmm14
    %define rmm15 xmm15
%endif

%ifdef SIMD256
    %define regsize 32
%else
    %define regsize 16
%endif

%ifdef USE_VEX
    %define MOVAPS vmovaps
%else
    %define MOVAPS movaps
%endif


%macro inst_same_reg 3
    %if (((%3) & OP_SKIP_0_3) == (OP_SKIP_0_3))
        %if ((%3) & (OP_SRC_HALF) == OP_SRC_HALF)
            %define SRCMM0 xmm0
            %define SRCMM1 xmm1
        %else
            %define SRCMM0 rmm0
            %define SRCMM1 rmm1
        %endif
    %elif (((%3) & OP_SKIP_0_1) == (OP_SKIP_0_1))
        %if ((%3) & (OP_SRC_HALF) == OP_SRC_HALF)
            %define SRCMM0 xmm1
            %define SRCMM1 xmm2
        %else
            %define SRCMM0 rmm1
            %define SRCMM1 rmm2
        %endif
    %else
        %if ((%3) & (OP_SRC_HALF) == OP_SRC_HALF)
            %define SRCMM0 xmm %+ %2
            %define SRCMM1 xmm %+ %2
        %else
            %define SRCMM0 rmm %+ %2
            %define SRCMM1 rmm %+ %2
        %endif
    %endif

    %if ((%3) & (OP_DST_HALF) == OP_DST_HALF)
        %define DSTMM xmm %+ %2
    %else
        %define DSTMM rmm %+ %2
    %endif

    %if ((%3) & OP_ADD_IMD) != 0
        %ifdef USE_VEX
            %if   ((%3) & OP_TYPE) = 0
                v %+ %1 DSTMM, SRCMM0, 1
            %elif ((%3) & OP_TYPE) = 1
                v %+ %1 DSTMM, SRCMM0, 1
            %elif ((%3) & OP_TYPE) = 2
                v %+ %1 DSTMM, DSTMM, SRCMM0, 1
            %elif ((%3) & OP_TYPE) = 3
                v %+ %1 DSTMM, SRCMM1, SRCMM0, 1
            %elif ((%3) & OP_TYPE) = 4
                v %+ %1 DSTMM, DSTMM, SRCMM1, SRCMM0, 1
            %endif
        %else
            %if ((%3) & OP_TYPE) = 0
                %1 DSTMM, 1
            %elif ((%3) & OP_TYPE) = 1
                %1 DSTMM, SRCMM0, 1
            %elif ((%3) & OP_TYPE) = 2
                %1 DSTMM, SRCMM0, 1
            %elif ((%3) & OP_TYPE) = 3
                %1 DSTMM, SRCMM1, SRCMM0, 1
            %else
                %1 DSTMM, SRCMM1, SRCMM0, 1
            %endif
        %endif
    %else
        %ifdef USE_VEX
            %if   ((%3) & OP_TYPE) = 0
                v %+ %1 SRCMM0
            %elif ((%3) & OP_TYPE) = 1
                v %+ %1 DSTMM, SRCMM0
            %elif ((%3) & OP_TYPE) = 2
                v %+ %1 DSTMM, DSTMM, SRCMM0
            %elif ((%3) & OP_TYPE) = 3
                v %+ %1 DSTMM, SRCMM1, SRCMM0
            %elif ((%3) & OP_TYPE) = 4
                v %+ %1 DSTMM, DSTMM, SRCMM1, SRCMM0
            %endif
        %else
            %if ((%3) & OP_TYPE) = 0
                %1 DSTMM
            %elif ((%3) & OP_TYPE) = 1
                %1 DSTMM, SRCMM0
            %elif ((%3) & OP_TYPE) = 2
                %1 DSTMM, SRCMM0
            %elif ((%3) & OP_TYPE) = 3
                %1 DSTMM, SRCMM1, SRCMM0
            %else
                %1 DSTMM, SRCMM1, SRCMM0
            %endif
        %endif
    %endif
%endmacro

%macro test_throughput 2
    inst_same_reg %1, 0,  %2
    inst_same_reg %1, 1,  %2
    inst_same_reg %1, 2,  %2
    inst_same_reg %1, 3,  %2
    inst_same_reg %1, 4,  %2
    inst_same_reg %1, 5,  %2
    inst_same_reg %1, 6,  %2
    inst_same_reg %1, 7,  %2
    inst_same_reg %1, 8,  %2
    inst_same_reg %1, 9,  %2
    inst_same_reg %1, 10, %2
    inst_same_reg %1, 11, %2
    inst_same_reg %1, 12, %2
    inst_same_reg %1, 13, %2
    inst_same_reg %1, 14, %2
    inst_same_reg %1, 15, %2
%endmacro

%macro test_throughput_skip_0_3 2
    inst_same_reg %1, 4,  %2
    inst_same_reg %1, 5,  %2
    inst_same_reg %1, 6,  %2
    inst_same_reg %1, 7,  %2
    inst_same_reg %1, 8,  %2
    inst_same_reg %1, 9,  %2
    inst_same_reg %1, 10, %2
    inst_same_reg %1, 11, %2
    inst_same_reg %1, 12, %2
    inst_same_reg %1, 13, %2
    inst_same_reg %1, 14, %2
    inst_same_reg %1, 15, %2
%endmacro

%macro test_throughput_pair 5
    %if %5 = 0
        inst_same_reg %1, 0,  %2
        inst_same_reg %3, 1,  %4
        inst_same_reg %1, 2,  %2
        inst_same_reg %3, 3,  %4
        inst_same_reg %1, 4,  %2
        inst_same_reg %3, 5,  %4
        inst_same_reg %1, 6,  %2
        inst_same_reg %3, 7,  %4
        inst_same_reg %1, 8,  %2
        inst_same_reg %3, 9,  %4
        inst_same_reg %1, 10, %2
        inst_same_reg %3, 11, %4
        inst_same_reg %1, 12, %2
        inst_same_reg %3, 13, %4
        inst_same_reg %1, 14, %2
        inst_same_reg %3, 15, %4
        inst_same_reg %1, 0,  %2
        inst_same_reg %3, 1,  %4
        inst_same_reg %1, 2,  %2
        inst_same_reg %3, 3,  %4
        inst_same_reg %1, 4,  %2
        inst_same_reg %3, 5,  %4
        inst_same_reg %1, 6,  %2
        inst_same_reg %3, 7,  %4
        inst_same_reg %1, 8,  %2
        inst_same_reg %3, 9,  %4
        inst_same_reg %1, 10, %2
        inst_same_reg %3, 11, %4
        inst_same_reg %1, 12, %2
        inst_same_reg %3, 13, %4
        inst_same_reg %1, 14, %2
        inst_same_reg %3, 15, %4
    %else
        inst_same_reg %1, 0,  %2
        inst_same_reg %3, 0,  %4
        inst_same_reg %1, 1,  %2
        inst_same_reg %3, 1,  %4
        inst_same_reg %1, 2,  %2
        inst_same_reg %3, 2,  %4
        inst_same_reg %1, 3,  %2
        inst_same_reg %3, 3,  %4
        inst_same_reg %1, 4,  %2
        inst_same_reg %3, 4,  %4
        inst_same_reg %1, 5,  %2
        inst_same_reg %3, 5,  %4
        inst_same_reg %1, 6,  %2
        inst_same_reg %3, 6,  %4
        inst_same_reg %1, 7,  %2
        inst_same_reg %3, 7,  %4
        inst_same_reg %1, 8,  %2
        inst_same_reg %3, 8,  %4
        inst_same_reg %1, 9,  %2
        inst_same_reg %3, 9,  %4
        inst_same_reg %1, 10, %2
        inst_same_reg %3, 10, %4
        inst_same_reg %1, 11, %2
        inst_same_reg %3, 11, %4
        inst_same_reg %1, 12, %2
        inst_same_reg %3, 12, %4
        inst_same_reg %1, 13, %2
        inst_same_reg %3, 13, %4
        inst_same_reg %1, 14, %2
        inst_same_reg %3, 14, %4
        inst_same_reg %1, 15, %2
        inst_same_reg %3, 15, %4
    %endif
%endmacro

%macro test_throughput_pair_skip_0_3_part1 5
    %if %5 = 0
        inst_same_reg %3, 3,  %2
        inst_same_reg %1, 4,  %2
        inst_same_reg %3, 5,  %4
        inst_same_reg %1, 6,  %2
        inst_same_reg %3, 7,  %4
        inst_same_reg %1, 8,  %2
        inst_same_reg %3, 9,  %4
        inst_same_reg %1, 10, %2
        inst_same_reg %3, 11, %4
        inst_same_reg %1, 12, %2
        inst_same_reg %3, 13, %4
        inst_same_reg %1, 14, %2
        inst_same_reg %3, 15, %4
        inst_same_reg %1, 3,  %2
        inst_same_reg %3, 4,  %4
        inst_same_reg %1, 5,  %2
        inst_same_reg %3, 6,  %4
        inst_same_reg %1, 7,  %2
        inst_same_reg %3, 8,  %4
        inst_same_reg %1,  9, %2
        inst_same_reg %3, 10, %4
        inst_same_reg %1, 11, %2
        inst_same_reg %3, 12, %4
        inst_same_reg %1, 13, %2
        inst_same_reg %3, 14, %4
        inst_same_reg %1, 15, %2
    %else
        inst_same_reg %1, 3,  %2
        inst_same_reg %3, 3,  %4
        inst_same_reg %1, 4,  %2
        inst_same_reg %3, 4,  %4
        inst_same_reg %1, 5,  %2
        inst_same_reg %3, 5,  %4
        inst_same_reg %1, 6,  %2
        inst_same_reg %3, 6,  %4
        inst_same_reg %1, 7,  %2
        inst_same_reg %3, 7,  %4
        inst_same_reg %1, 8,  %2
        inst_same_reg %3, 8,  %4
        inst_same_reg %1, 9,  %2
        inst_same_reg %3, 9,  %4
        inst_same_reg %1, 10, %2
        inst_same_reg %3, 10, %4
        inst_same_reg %1, 11, %2
        inst_same_reg %3, 11, %4
        inst_same_reg %1, 12, %2
        inst_same_reg %3, 12, %4
        inst_same_reg %1, 13, %2
        inst_same_reg %3, 13, %4
        inst_same_reg %1, 14, %2
        inst_same_reg %3, 14, %4
        inst_same_reg %1, 15, %2
        inst_same_reg %3, 15, %4
    %endif
%endmacro

%macro test_throughput_pair_skip_0_3_part2 5
    %if %5 = 0
        inst_same_reg %3, 3,  %2
        inst_same_reg %1, 4,  %2
        inst_same_reg %3, 5,  %4
        inst_same_reg %1, 6,  %2
        inst_same_reg %3, 7,  %4
        inst_same_reg %1, 8,  %2
        inst_same_reg %3, 9,  %4
        inst_same_reg %1, 10, %2
        inst_same_reg %3, 11, %4
        inst_same_reg %1, 12, %2
        inst_same_reg %3, 13, %4
        inst_same_reg %1, 14, %2
        inst_same_reg %3, 15, %4
        inst_same_reg %1, 3,  %2
        inst_same_reg %3, 4,  %4
        inst_same_reg %1, 5,  %2
        inst_same_reg %3, 6,  %4
        inst_same_reg %1, 7,  %2
        inst_same_reg xorps, 3, 2
        inst_same_reg xorps, 4, 2
        inst_same_reg xorps, 5, 2
        inst_same_reg xorps, 6, 2
        inst_same_reg xorps, 7, 2
    %else
        inst_same_reg %1, 3,  %2
        inst_same_reg %3, 3,  %4
        inst_same_reg %1, 4,  %2
        inst_same_reg %3, 4,  %4
        inst_same_reg %1, 5,  %2
        inst_same_reg %3, 5,  %4
        inst_same_reg %1, 6,  %2
        inst_same_reg %3, 6,  %4
        inst_same_reg %1, 7,  %2
        inst_same_reg %3, 7,  %4
        inst_same_reg %1, 8,  %2
        inst_same_reg %3, 8,  %4
        inst_same_reg %1, 9,  %2
        inst_same_reg %3, 9,  %4
        inst_same_reg %1, 10, %2
        inst_same_reg %3, 10, %4
        inst_same_reg %1, 11, %2
        inst_same_reg %3, 11, %4
        inst_same_reg xorps, 3, 2
        inst_same_reg xorps, 4, 2
        inst_same_reg xorps, 5, 2
        inst_same_reg xorps, 6, 2
        inst_same_reg xorps, 7, 2
    %endif
%endmacro

%macro test_latency 2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
    inst_same_reg %1, 5, %2
%endmacro

%macro test_latency_skip_0_3 2
    %if ((%2) & OP_SKIP_0_3_SAME_REG) == OP_SKIP_0_3_SAME_REG
        inst_same_reg %1, 0, %2
        inst_same_reg %1, 0, %2
        inst_same_reg %1, 0, %2
        inst_same_reg %1, 0, %2
        inst_same_reg %1, 0, %2
        inst_same_reg %1, 0, %2
        inst_same_reg %1, 0, %2
        inst_same_reg %1, 0, %2
        inst_same_reg %1, 0, %2
        inst_same_reg %1, 0, %2
        inst_same_reg %1, 0, %2
        inst_same_reg %1, 0, %2
    %else
        inst_same_reg %1, 5, %2
        inst_same_reg %1, 5, %2
        inst_same_reg %1, 5, %2
        inst_same_reg %1, 5, %2
        inst_same_reg %1, 5, %2
        inst_same_reg %1, 5, %2
        inst_same_reg %1, 5, %2
        inst_same_reg %1, 5, %2
        inst_same_reg %1, 5, %2
        inst_same_reg %1, 5, %2
        inst_same_reg %1, 5, %2
        inst_same_reg %1, 5, %2
    %endif
%endmacro

%macro test_latency_pair 4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
    inst_same_reg %1, 5, %2
    inst_same_reg %3, 5, %4
%endmacro

%macro init_reg_div 1
    %if ((%1) & OP_DIV_F) != 0
        MOVAPS rmm0, [INIT_DIVIDER_F]
        MOVAPS rmm4, [INIT_DIVIDEND_F]
    %else
        MOVAPS rmm0, [INIT_DIVIDER_D]
        MOVAPS rmm4, [INIT_DIVIDEND_D]
    %endif
    MOVAPS rmm5, rmm4
    MOVAPS rmm6, rmm4
    MOVAPS rmm7, rmm4
    MOVAPS rmm8, rmm4
    MOVAPS rmm9, rmm4
    MOVAPS rmm10, rmm4
    MOVAPS rmm11, rmm4
    MOVAPS rmm12, rmm4
    MOVAPS rmm13, rmm4
    MOVAPS rmm14, rmm4
    MOVAPS rmm15, rmm4
%endmacro

%macro run_check 3
%if %1 = 0
    %define FUNC_TYPE t
%else
    %define FUNC_TYPE l
%endif
%ifdef USE_RDTSCP
    %define RDTSC_TYPE p
%else
    %define RDTSC_TYPE 
%endif

    global run %+ RDTSC_TYPE %+ FUNC_TYPE %+ _ %+ %2 %+ STR_VEX %+ STR_SIMD_WIDTH

    run %+ RDTSC_TYPE %+ FUNC_TYPE %+ _ %+ %2 %+ STR_VEX %+ STR_SIMD_WIDTH:
        push rbx
        push rsi
        mov esi, ecx
        mov r9, rsp
        sub rsp, regsize*10
        and rsp, -32
        MOVAPS [rsp+regsize*0], rmm6
        MOVAPS [rsp+regsize*1], rmm7
        MOVAPS [rsp+regsize*2], rmm8
        MOVAPS [rsp+regsize*3], rmm9
        MOVAPS [rsp+regsize*4], rmm10
        MOVAPS [rsp+regsize*5], rmm11
        MOVAPS [rsp+regsize*6], rmm12
        MOVAPS [rsp+regsize*7], rmm13
        MOVAPS [rsp+regsize*8], rmm14
        MOVAPS [rsp+regsize*9], rmm15
        
%if ((%3) & (OP_DIV_F) == OP_DIV_F) | ((%3) & (OP_DIV_D) == OP_DIV_D)
        init_reg_div %3
%else
        test_throughput xorps, 2
%endif
        
        xor rax, rax
        cpuid
%ifdef USE_RDTSCP
        rdtscp
%else
        rdtsc
%endif
        shl rdx, 32
        or rdx, rax
        mov r8, rdx

        align 16
    ._LOOP
        %if ((%3) & OP_SKIP_0_3) != 0
            %if (%1) = 0
                test_throughput_skip_0_3 %2, %3
                test_throughput_skip_0_3 %2, %3
                test_throughput_skip_0_3 %2, %3
                test_throughput_skip_0_3 %2, %3
                test_throughput_skip_0_3 %2, %3
                test_throughput_skip_0_3 %2, %3
                test_throughput_skip_0_3 %2, %3
                test_throughput_skip_0_3 %2, %3
            %else
                test_latency_skip_0_3 %2, %3
                test_latency_skip_0_3 %2, %3
                test_latency_skip_0_3 %2, %3
                test_latency_skip_0_3 %2, %3
                test_latency_skip_0_3 %2, %3
                test_latency_skip_0_3 %2, %3
                test_latency_skip_0_3 %2, %3
                test_latency_skip_0_3 %2, %3
            %endif
        %else
            %if %1 = 0
                test_throughput %2, %3
                test_throughput %2, %3
                test_throughput %2, %3
                test_throughput %2, %3
                test_throughput %2, %3
                test_throughput %2, %3
            %else
                test_latency %2, %3
                test_latency %2, %3
                test_latency %2, %3
                test_latency %2, %3
                test_latency %2, %3
                test_latency %2, %3
            %endif
        %endif

        dec esi
        jnz ._LOOP

%ifdef USE_RDTSCP
        rdtscp
%else
        mfence
        rdtsc
%endif
        shl rdx, 32
        or rax, rdx

        sub rax, r8

        MOVAPS rmm6,  [rsp+regsize*0]
        MOVAPS rmm7,  [rsp+regsize*1]
        MOVAPS rmm8,  [rsp+regsize*2]
        MOVAPS rmm9,  [rsp+regsize*3]
        MOVAPS rmm10, [rsp+regsize*4]
        MOVAPS rmm11, [rsp+regsize*5]
        MOVAPS rmm12, [rsp+regsize*6]
        MOVAPS rmm13, [rsp+regsize*7]
        MOVAPS rmm14, [rsp+regsize*8]
        MOVAPS rmm15, [rsp+regsize*9]

        mov rsp, r9

        pop rsi
        pop rbx
%ifdef USE_VEX
        vzeroupper
%endif
        ret
%endmacro


%macro run_check_pair 6
%if %1 = 0
    %define FUNC_TYPE t
%else
    %define FUNC_TYPE l
%endif

%if %6 = 0
    %define PAIR_TYPE p
%else
    %define PAIR_TYPE s
%endif

%ifdef USE_RDTSCP
    %define RDTSC_TYPE p
%else
    %define RDTSC_TYPE 
%endif

    global run %+ RDTSC_TYPE %+ FUNC_TYPE %+ PAIR_TYPE %+ _ %+ %2 %+ _ %+ %4 %+ STR_VEX %+ STR_SIMD_WIDTH

    run %+ RDTSC_TYPE %+ FUNC_TYPE %+ PAIR_TYPE %+ _ %+ %2 %+ _ %+ %4 %+  STR_VEX %+ STR_SIMD_WIDTH:
        push rbx
        push rsi
        mov esi, ecx
        mov r9, rsp
        sub rsp, regsize*10
        and rsp, -32
        MOVAPS [rsp+regsize*0], rmm6
        MOVAPS [rsp+regsize*1], rmm7
        MOVAPS [rsp+regsize*2], rmm8
        MOVAPS [rsp+regsize*3], rmm9
        MOVAPS [rsp+regsize*4], rmm10
        MOVAPS [rsp+regsize*5], rmm11
        MOVAPS [rsp+regsize*6], rmm12
        MOVAPS [rsp+regsize*7], rmm13
        MOVAPS [rsp+regsize*8], rmm14
        MOVAPS [rsp+regsize*9], rmm15
        
%if ((%3) & (OP_DIV_F) == OP_DIV_F) | ((%3) & (OP_DIV_D) == OP_DIV_D)
        init_reg_div %3
%else
        test_throughput xorps, 2
%endif

        xor rax, rax
        cpuid
%ifdef USE_RDTSCP
        rdtscp
%else
        rdtsc
%endif
        shl rdx, 32
        or rdx, rax
        mov r8, rdx

        align 16
    ._LOOP
        %if ((%3) & OP_SKIP_0_1) != 0
            %if %1 = 0
                test_throughput_pair_skip_0_3_part1 %2, %3, %4, %5, %6
                test_throughput_pair_skip_0_3_part1 %2, %3, %4, %5, %6
                test_throughput_pair_skip_0_3_part1 %2, %3, %4, %5, %6
                test_throughput_pair_skip_0_3_part2 %2, %3, %4, %5, %6
            %else
                test_latency_pair %2, %3, %4, %5
                test_latency_pair %2, %3, %4, %5
                test_latency_pair %2, %3, %4, %5
                test_latency_pair %2, %3, %4, %5
                test_latency_pair %2, %3, %4, %5
                test_latency_pair %2, %3, %4, %5
            %endif
        %else
            %if %1 = 0
                test_throughput_pair %2, %3, %4, %5, %6
                test_throughput_pair %2, %3, %4, %5, %6
                test_throughput_pair %2, %3, %4, %5, %6
            %else
                test_latency_pair %2, %3, %4, %5
                test_latency_pair %2, %3, %4, %5
                test_latency_pair %2, %3, %4, %5
                test_latency_pair %2, %3, %4, %5
                test_latency_pair %2, %3, %4, %5
                test_latency_pair %2, %3, %4, %5
            %endif
        %endif

        dec esi
        jnz ._LOOP

%ifdef USE_RDTSCP
        rdtscp
%else
        mfence
        rdtsc
%endif
        shl rdx, 32
        or rax, rdx

        sub rax, r8

        MOVAPS rmm6,  [rsp+regsize*0]
        MOVAPS rmm7,  [rsp+regsize*1]
        MOVAPS rmm8,  [rsp+regsize*2]
        MOVAPS rmm9,  [rsp+regsize*3]
        MOVAPS rmm10, [rsp+regsize*4]
        MOVAPS rmm11, [rsp+regsize*5]
        MOVAPS rmm12, [rsp+regsize*6]
        MOVAPS rmm13, [rsp+regsize*7]
        MOVAPS rmm14, [rsp+regsize*8]
        MOVAPS rmm15, [rsp+regsize*9]

        mov rsp, r9
        
        pop rsi
        pop rbx
%ifdef USE_VEX
        vzeroupper
%endif
        ret
%endmacro