/*
 * const.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_X86_DSP_CONST_H_
#define CORE_X86_DSP_CONST_H_

namespace lsp
{
    // Parameters for SSE
    #define SSE_MULTIPLE                4
    #define SSE_ALIGN                   (SSE_MULTIPLE * sizeof(float))
    #define SFENCE                      __asm__ __volatile__ ( __ASM_EMIT("sfence") )
    #define EMMS                        __asm__ __volatile__ ( __ASM_EMIT("emms") )
    #define MOVNTPS                     "movaps"
    #define SSE_SVEC4(name, value)      static const float name[] __lsp_aligned16      = { value, value, value, value }
    #define SSE_UVEC4(name, value)      static const uint32_t name[] __lsp_aligned16   = { value, value, value, value }
    #define SSE_UVEC(name, a, b, c, d)  static const uint32_t name[] __lsp_aligned16   = { uint32_t(a), uint32_t(b), uint32_t(c), uint32_t(d) }
    #define SSE_X4VEC(x)                x, x, x, x

    #define MXCSR_IE                    (1 << 0)
    #define MXCSR_DE                    (1 << 1)
    #define MXCSR_ZE                    (1 << 2)
    #define MXCSR_OE                    (1 << 3)
    #define MXCSR_UE                    (1 << 4)
    #define MXCSR_PE                    (1 << 5)
    #define MXCSR_DAZ                   (1 << 6) /* Denormals are zeros flag */
    #define MXCSR_IM                    (1 << 7)
    #define MXCSR_DM                    (1 << 8)
    #define MXCSR_ZM                    (1 << 9)
    #define MXCSR_OM                    (1 << 10)
    #define MXCSR_UM                    (1 << 11)
    #define MXCSR_PM                    (1 << 12)
    #define MXCSR_RC_MASK               (3 << 13)
    #define MXCSR_RC_NEAREST            (0 << 13)
    #define MXCSR_RC_N_INF              (1 << 13)
    #define MXCSR_RC_P_INF              (2 << 13)
    #define MXCSR_RC_ZERO               (3 << 13)
    #define MXCSR_FZ                    (1 << 15) /* Flush to zero flag */

    namespace sse
    {
        /* Sine approximation coefficients */
        SSE_SVEC4(S0,  1.00000000000000000000);
        SSE_SVEC4(S1, -0.16666666666666665741);
        SSE_SVEC4(S2,  0.00833333333333333322);
        SSE_SVEC4(S3, -0.00019841269841269841);
        SSE_SVEC4(S4,  0.00000275573192239859);
        SSE_SVEC4(S5, -0.00000002505210838544);

        /* Cosine approximation coefficients */
        SSE_SVEC4(C0,  1.00000000000000000000);
        SSE_SVEC4(C1, -0.50000000000000000000);
        SSE_SVEC4(C2,  0.04166666666666666435);
        SSE_SVEC4(C3, -0.00138888888888888894);
        SSE_SVEC4(C4,  0.00002480158730158730);
        SSE_SVEC4(C5, -0.00000027557319223986);

        /* Logarithm approximation coefficients */
        SSE_SVEC4(L0, 7.0376836292E-2);
        SSE_SVEC4(L1, -1.1514610310E-1);
        SSE_SVEC4(L2, 1.1676998740E-1);
        SSE_SVEC4(L3, -1.2420140846E-1);
        SSE_SVEC4(L4, +1.4249322787E-1);
        SSE_SVEC4(L5, -1.6668057665E-1);
        SSE_SVEC4(L6, +2.0000714765E-1);
        SSE_SVEC4(L7, -2.4999993993E-1);
        SSE_SVEC4(L8, +3.3333331174E-1);
        SSE_SVEC4(L9, 0.5);
        SSE_SVEC4(LXE, -2.12194440e-4);

        /* Math constants */
        SSE_SVEC4(ZERO, 0.0f);
        SSE_SVEC4(ONE, 1.0f);
        SSE_SVEC4(PI,  M_PI);
        SSE_SVEC4(PI_2, M_PI_2);
        SSE_SVEC4(SQRT1_2, M_SQRT1_2);
        SSE_SVEC4(LN2, M_LN2);

        /* Sign Mask */
        SSE_UVEC4(X_SIGN,  0x7fffffff);
        SSE_SVEC4(X_HALF,  0.5f);
        SSE_UVEC4(X_MANT,  0x007fffff);
        SSE_UVEC4(X_MMASK, 0x0000007f);
        SSE_SVEC4(X_AMP_THRESH, AMPLIFICATION_THRESH);
        SSE_UVEC4(X_P_DENORM, 0x00800000);
        SSE_UVEC4(X_N_DENORM, 0x80800000);

        /* Positive and negative infinities */
        SSE_UVEC4(X_P_INF,  0x7f800000);
        SSE_UVEC4(X_N_INF,  0xff800000);
        SSE_UVEC4(X_P_INFM1,0x7f7fffff);
        SSE_UVEC4(X_N_INFM1,0xff7fffff);
        SSE_UVEC4(X_ZERO_M1,0xffffffff);
        SSE_UVEC4(X_CMASK,  0x00ff00ff);

        /* Saturation replacement */
        SSE_SVEC4(SX_P_INF, FLOAT_SAT_P_INF);
        SSE_SVEC4(SX_N_INF, FLOAT_SAT_N_INF);
        SSE_SVEC4(SX_P_NAN, FLOAT_SAT_P_NAN);
        SSE_SVEC4(SX_N_NAN, FLOAT_SAT_N_NAN);

        /* Miscellaneous vectors */
        SSE_UVEC(X_MASK0001, -1, 0, 0, 0);

        static const float XFFT_W_RE[] __lsp_aligned16 =
        {
            SSE_X4VEC(0.0000000000000000f),
            SSE_X4VEC(0.0000000000000000f),
            SSE_X4VEC(0.7071067811865475f),
            SSE_X4VEC(0.9238795325112868f),
            SSE_X4VEC(0.9807852804032305f),
            SSE_X4VEC(0.9951847266721969f),
            SSE_X4VEC(0.9987954562051724f),
            SSE_X4VEC(0.9996988186962042f),
            SSE_X4VEC(0.9999247018391445f),
            SSE_X4VEC(0.9999811752826011f),
            SSE_X4VEC(0.9999952938095762f),
            SSE_X4VEC(0.9999988234517019f),
            SSE_X4VEC(0.9999997058628822f),
            SSE_X4VEC(0.9999999264657179f),
            SSE_X4VEC(0.9999999816164293f),
            SSE_X4VEC(0.9999999954041073f),
            SSE_X4VEC(0.9999999988510268f)
        };

        static const float XFFT_W_IM[] __lsp_aligned16 =
        {
            SSE_X4VEC(1.0000000000000000f),
            SSE_X4VEC(1.0000000000000000f),
            SSE_X4VEC(0.7071067811865475f),
            SSE_X4VEC(0.3826834323650898f),
            SSE_X4VEC(0.1950903220161283f),
            SSE_X4VEC(0.0980171403295606f),
            SSE_X4VEC(0.0490676743274180f),
            SSE_X4VEC(0.0245412285229123f),
            SSE_X4VEC(0.0122715382857199f),
            SSE_X4VEC(0.0061358846491545f),
            SSE_X4VEC(0.0030679567629660f),
            SSE_X4VEC(0.0015339801862848f),
            SSE_X4VEC(0.0007669903187427f),
            SSE_X4VEC(0.0003834951875714f),
            SSE_X4VEC(0.0001917475973107f),
            SSE_X4VEC(0.0000958737990960f),
            SSE_X4VEC(0.0000479368996031f)
        };

        static const float XFFT_A_RE[] __lsp_aligned16 =
        {
            1.0000000000000000f, 0.7071067811865475f, 0.0000000000000001f, -0.7071067811865475f,
            1.0000000000000000f, 0.9238795325112868f, 0.7071067811865475f, 0.3826834323650898f,
            1.0000000000000000f, 0.9807852804032305f, 0.9238795325112868f, 0.8314696123025452f,
            1.0000000000000000f, 0.9951847266721969f, 0.9807852804032305f, 0.9569403357322089f,
            1.0000000000000000f, 0.9987954562051724f, 0.9951847266721969f, 0.9891765099647810f,
            1.0000000000000000f, 0.9996988186962042f, 0.9987954562051724f, 0.9972904566786902f,
            1.0000000000000000f, 0.9999247018391445f, 0.9996988186962042f, 0.9993223845883495f,
            1.0000000000000000f, 0.9999811752826011f, 0.9999247018391445f, 0.9998305817958234f,
            1.0000000000000000f, 0.9999952938095762f, 0.9999811752826011f, 0.9999576445519639f,
            1.0000000000000000f, 0.9999988234517019f, 0.9999952938095762f, 0.9999894110819284f,
            1.0000000000000000f, 0.9999997058628822f, 0.9999988234517019f, 0.9999973527669782f,
            1.0000000000000000f, 0.9999999264657179f, 0.9999997058628822f, 0.9999993381915255f,
            1.0000000000000000f, 0.9999999816164293f, 0.9999999264657179f, 0.9999998345478677f,
            1.0000000000000000f, 0.9999999954041073f, 0.9999999816164293f, 0.9999999586369661f,
            1.0000000000000000f, 0.9999999988510268f, 0.9999999954041073f, 0.9999999896592415f
        };

        static const float XFFT_A_IM[] __lsp_aligned16 =
        {
            0.0000000000000000f, 0.7071067811865475f, 1.0000000000000000f, 0.7071067811865476f,
            0.0000000000000000f, 0.3826834323650898f, 0.7071067811865475f, 0.9238795325112867f,
            0.0000000000000000f, 0.1950903220161283f, 0.3826834323650898f, 0.5555702330196022f,
            0.0000000000000000f, 0.0980171403295606f, 0.1950903220161283f, 0.2902846772544624f,
            0.0000000000000000f, 0.0490676743274180f, 0.0980171403295606f, 0.1467304744553617f,
            0.0000000000000000f, 0.0245412285229123f, 0.0490676743274180f, 0.0735645635996674f,
            0.0000000000000000f, 0.0122715382857199f, 0.0245412285229123f, 0.0368072229413588f,
            0.0000000000000000f, 0.0061358846491545f, 0.0122715382857199f, 0.0184067299058048f,
            0.0000000000000000f, 0.0030679567629660f, 0.0061358846491545f, 0.0092037547820598f,
            0.0000000000000000f, 0.0015339801862848f, 0.0030679567629660f, 0.0046019261204486f,
            0.0000000000000000f, 0.0007669903187427f, 0.0015339801862848f, 0.0023009691514258f,
            0.0000000000000000f, 0.0003834951875714f, 0.0007669903187427f, 0.0011504853371138f,
            0.0000000000000000f, 0.0001917475973107f, 0.0003834951875714f, 0.0005752427637321f,
            0.0000000000000000f, 0.0000958737990960f, 0.0001917475973107f, 0.0002876213937629f,
            0.0000000000000000f, 0.0000479368996031f, 0.0000958737990960f, 0.0001438106983686f
        };
    }

    namespace sse
    {
        static inline bool __lsp_forced_inline sse_aligned(const void *ptr)         { return !(ptrdiff_t(ptr) & (SSE_ALIGN - 1));  };
        static inline ptrdiff_t __lsp_forced_inline sse_offset(const void *ptr)     { return (ptrdiff_t(ptr) & (SSE_ALIGN - 1));   };
        static inline size_t __lsp_forced_inline sse_multiple(size_t count)         { return count & (SSE_MULTIPLE - 1);           };
    }
}

#endif /* CORE_X86_DSP_CONST_H_ */
