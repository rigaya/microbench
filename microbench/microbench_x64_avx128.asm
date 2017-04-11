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

%define USE_RDTSCP 1
%define USE_VEX    1
%include "microbench.inc"

section .code
    align 16

section .text

    run_check 0, pblendvb, 4
    run_check 1, pblendvb, 4
    run_check 0, pblendd, 3 | OP_ADD_IMD
    run_check 1, pblendd, 3 | OP_ADD_IMD
    run_check 0, pbroadcastw, 1 | OP_SKIP_0_3_SAME_REG
    run_check 1, pbroadcastw, 1 | OP_SKIP_0_3_SAME_REG

    run_check 0, fmadd132ps, 3 | OP_SKIP_0_3
    run_check 1, fmadd132ps, 3 | OP_SKIP_0_3
    run_check 0, fmadd132pd, 3 | OP_SKIP_0_3
    run_check 1, fmadd132pd, 3 | OP_SKIP_0_3
    
    run_check_pair 0, fmadd132ps, 3 | OP_SKIP_0_1, addps, 2 | OP_SKIP_0_3, 0
    run_check_pair 1, fmadd132ps, 3, addps, 2, 0
    run_check_pair 0, fmadd132ps, 3 | OP_SKIP_0_1, addps, 2 | OP_SKIP_0_3, 1
    run_check_pair 1, fmadd132ps, 3, addps, 2, 1
    run_check_pair 0, fmadd132pd, 3 | OP_SKIP_0_1, addpd, 2 | OP_SKIP_0_3, 0
    run_check_pair 1, fmadd132pd, 3, addpd, 2, 0
    run_check_pair 0, fmadd132pd, 3 | OP_SKIP_0_1, addpd, 2 | OP_SKIP_0_3, 1
    run_check_pair 1, fmadd132pd, 3, addpd, 2, 1
    run_check_pair 0, fmadd132ps, 3 | OP_SKIP_0_1, mulps, 2 | OP_SKIP_0_3, 0
    run_check_pair 1, fmadd132ps, 3, mulps, 2, 0
    run_check_pair 0, fmadd132ps, 3 | OP_SKIP_0_1, mulps, 2 | OP_SKIP_0_3, 1
    run_check_pair 1, fmadd132ps, 3, mulps, 2, 1
    run_check_pair 0, fmadd132pd, 3 | OP_SKIP_0_1, mulpd, 2 | OP_SKIP_0_3, 0
    run_check_pair 1, fmadd132pd, 3, mulpd, 2, 0
    run_check_pair 0, fmadd132pd, 3 | OP_SKIP_0_1, mulpd, 2 | OP_SKIP_0_3, 1
    run_check_pair 1, fmadd132pd, 3, mulpd, 2, 1
