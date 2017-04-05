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

%include "microbench.inc"

section .code
    align 16

section .text

    run_check 0, por, 2
    run_check 1, por, 2
    run_check 0, pand, 2
    run_check 1, pand, 2
    run_check 0, pandn, 2 | OP_SKIP_0_3
    run_check 1, pandn, 2 | OP_SKIP_0_3
    run_check 0, pxor, 2 | OP_SKIP_0_3
    run_check 1, pxor, 2 | OP_SKIP_0_3

    run_check 0, psllw,  0 | OP_ADD_IMD
    run_check 1, psllw,  0 | OP_ADD_IMD
    run_check 0, pslldq, 0 | OP_ADD_IMD
    run_check 1, pslldq, 0 | OP_ADD_IMD
    
    run_check 0, paddw, 2
    run_check 1, paddw, 2
    run_check 0, paddsw, 2
    run_check 1, paddsw, 2
    run_check 0, paddd, 2
    run_check 1, paddd, 2
    run_check 0, paddq, 2
    run_check 1, paddq, 2
    run_check 0, pmullw, 2
    run_check 1, pmullw, 2
    run_check 0, pmuludq, 2
    run_check 1, pmuludq, 2
    run_check 0, pmulld, 2
    run_check 1, pmulld, 2
    run_check 0, pmaddwd, 2
    run_check 1, pmaddwd, 2
    run_check 0, pmaddubsw, 2
    run_check 1, pmaddubsw, 2

    run_check 0, pabsw, 1
    run_check 1, pabsw, 1
    run_check 0, pmaxsw, 2
    run_check 1, pmaxsw, 2
    run_check 0, pminsw, 2
    run_check 1, pminsw, 2
    run_check 0, pavgw, 2
    run_check 1, pavgw, 2
    run_check 0, psadbw, 2
    run_check 1, psadbw, 2
    
    run_check 0, punpckhwd, 2
    run_check 1, punpckhwd, 2
    run_check 0, palignr, 2 | OP_ADD_IMD
    run_check 1, palignr, 2 | OP_ADD_IMD
    run_check 0, pshufb, 2
    run_check 1, pshufb, 2
    run_check 0, pblendw, 2 | OP_ADD_IMD
    run_check 1, pblendw, 2 | OP_ADD_IMD
    run_check 0, pblendvb, 3 | OP_SKIP_0_3
    run_check 1, pblendvb, 3 | OP_SKIP_0_3

    run_check 0, addps, 2
    run_check 1, addps, 2
    run_check 0, addpd, 2
    run_check 1, addpd, 2
    run_check 0, mulps, 2
    run_check 1, mulps, 2
    run_check 0, mulpd, 2
    run_check 1, mulpd, 2
    run_check 0, divps, 2 | OP_SKIP_0_3 | OP_DIV_F
    run_check 1, divps, 2 | OP_SKIP_0_3 | OP_DIV_F
    run_check 0, divpd, 2 | OP_SKIP_0_3 | OP_DIV_D
    run_check 1, divpd, 2 | OP_SKIP_0_3 | OP_DIV_D
    run_check 0, sqrtps, 1 | OP_SKIP_0_3 | OP_DIV_F
    run_check 1, sqrtps, 1 | OP_SKIP_0_3 | OP_DIV_F
    run_check 0, sqrtpd, 1 | OP_SKIP_0_3 | OP_DIV_D
    run_check 1, sqrtpd, 1 | OP_SKIP_0_3 | OP_DIV_D
    run_check 0, rcpps, 1
    run_check 1, rcpps, 1
    run_check 0, rsqrtps, 1
    run_check 1, rsqrtps, 1
    run_check 0, maxps, 2
    run_check 1, maxps, 2
    run_check 0, maxpd, 2
    run_check 1, maxpd, 2
    run_check 0, minps, 2
    run_check 1, minps, 2
    run_check 0, minpd, 2
    run_check 1, minpd, 2

    run_check_pair 0, addps, 2, mulps, 2, 0
    run_check_pair 1, addps, 2, mulps, 2, 0
    run_check_pair 0, addps, 2, mulps, 2, 1
    run_check_pair 1, addps, 2, mulps, 2, 1
    run_check_pair 0, addpd, 2, mulpd, 2, 0
    run_check_pair 1, addpd, 2, mulpd, 2, 0
    run_check_pair 0, addpd, 2, mulpd, 2, 1
    run_check_pair 1, addpd, 2, mulpd, 2, 1
