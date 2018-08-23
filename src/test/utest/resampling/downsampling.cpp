/*
 * downsampling.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void downsample_2x(float *dst, const float *src, size_t count);
    void downsample_3x(float *dst, const float *src, size_t count);
    void downsample_4x(float *dst, const float *src, size_t count);
    void downsample_6x(float *dst, const float *src, size_t count);
    void downsample_8x(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void downsample_2x(float *dst, const float *src, size_t count);
        void downsample_3x(float *dst, const float *src, size_t count);
        void downsample_4x(float *dst, const float *src, size_t count);
        void downsample_6x(float *dst, const float *src, size_t count);
        void downsample_8x(float *dst, const float *src, size_t count);
    }
)

typedef void (* downsample_t)(float *dst, const float *src, size_t count);

UTEST_BEGIN("dsp.resampling", downsampling)

    void call(size_t times, const char *text, size_t align, downsample_t func1, downsample_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 100, 999)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s downsampling for %d -> %d samples, mask=0x%x...\n", text, int(count * times), int(count), int(mask));

                FloatBuffer src(count*times, align, mask & 0x01);
                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, src, count);
                func2(dst2, src, count);

                if (src.corrupted())
                    UTEST_FAIL_MSG("Source buffer corrupted");
                if (dst1.corrupted())
                    UTEST_FAIL_MSG("Destination buffer 1 corrupted");
                if (dst2.corrupted())
                    UTEST_FAIL_MSG("Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2))
                {
                    src.dump("src1");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
                }
            }
        }
    }

    UTEST_MAIN
    {
        // Do tests
        IF_ARCH_X86(call(2, "2x sse", 16, native::downsample_2x, sse::downsample_2x));
        IF_ARCH_X86(call(3, "3x sse", 16, native::downsample_3x, sse::downsample_3x));
        IF_ARCH_X86(call(4, "4x sse", 16, native::downsample_4x, sse::downsample_4x));
        IF_ARCH_X86(call(6, "6x sse", 16, native::downsample_6x, sse::downsample_6x));
        IF_ARCH_X86(call(8, "8x sse", 16, native::downsample_8x, sse::downsample_8x));
    }
UTEST_END;


