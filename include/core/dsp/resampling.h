/*
 * resampling.h
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_DSP_RESAMPLING_H_
#define INCLUDE_CORE_DSP_RESAMPLING_H_

#ifndef __CORE_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __CORE_DSP_DEFS */

namespace lsp
{
    //-----------------------------------------------------------------------
    // Resampling
    /** Resampling/oversampling funtion type.
     * Remember that destination buffer must be times greater and have additional gap (>=64 samples) at
     * the tail to contain complete convolution after resampling
     *
     * @param dst destination buffer
     * @param src source buffer
     * @param count number of samples to process
     */
    typedef void (* resampling_function_t)(float *dst, const float *src, size_t count);

    namespace dsp
    {
        /** Perform lanczos resampling, destination buffer must be cleared and contain only
         * convolution tail from previous resampling
         *
         * @param dst destination buffer of count*2 samples + 64 samples for convolution tail
         * @param src source buffer of count samples
         * @param count number of samples
         */
        extern void (* lanczos_resample_2x2)(float *dst, const float *src, size_t count);

        /** Perform lanczos resampling, destination buffer must be cleared and contain only
         * convolution tail from previous resampling
         *
         * @param dst destination buffer of count*2 samples + 64 samples for convolution tail
         * @param src source buffer of count samples
         * @param count number of samples
         */
        extern void (* lanczos_resample_2x3)(float *dst, const float *src, size_t count);

        /** Perform lanczos resampling, destination buffer must be cleared and contain only
         * convolution tail from previous resampling
         *
         * @param dst destination buffer of count*3 samples + 64 samples for convolution tail
         * @param src source buffer of count samples
         * @param count number of samples
         */
        extern void (* lanczos_resample_3x2)(float *dst, const float *src, size_t count);

        /** Perform lanczos resampling, destination buffer must be cleared and contain only
         * convolution tail from previous resampling
         *
         * @param dst destination buffer of count*3 samples + 64 samples for convolution tail
         * @param src source buffer of count samples
         * @param count number of samples
         */
        extern void (* lanczos_resample_3x3)(float *dst, const float *src, size_t count);

        /** Perform lanczos resampling, destination buffer must be cleared and contain only
         * convolution tail from previous resampling
         *
         * @param dst destination buffer of count*4 samples + 64 samples for convolution tail
         * @param src source buffer of count samples
         * @param count number of samples
         */
        extern void (* lanczos_resample_4x2)(float *dst, const float *src, size_t count);

        /** Perform lanczos resampling, destination buffer must be cleared and contain only
         * convolution tail from previous resampling
         *
         * @param dst destination buffer of count*4 samples + 64 samples for convolution tail
         * @param src source buffer of count samples
         * @param count number of samples
         */
        extern void (* lanczos_resample_4x3)(float *dst, const float *src, size_t count);

        /** Perform lanczos resampling, destination buffer must be cleared and contain only
         * convolution tail from previous resampling
         *
         * @param dst destination buffer of count*6 samples + 64 samples for convolution tail
         * @param src source buffer of count samples
         * @param count number of samples
         */
        extern void (* lanczos_resample_6x2)(float *dst, const float *src, size_t count);

        /** Perform lanczos resampling, destination buffer must be cleared and contain only
         * convolution tail from previous resampling
         *
         * @param dst destination buffer of count*6 samples + 64 samples for convolution tail
         * @param src source buffer of count samples
         * @param count number of samples
         */
        extern void (* lanczos_resample_6x3)(float *dst, const float *src, size_t count);

        /** Perform lanczos resampling, destination buffer must be cleared and contain only
         * convolution tail from previous resampling
         *
         * @param dst destination buffer of count*8 samples + 64 samples for convolution tail
         * @param src source buffer of count samples
         * @param count number of samples
         */
        extern void (* lanczos_resample_8x2)(float *dst, const float *src, size_t count);

        /** Perform lanczos resampling, destination buffer must be cleared and contain only
         * convolution tail from previous resampling
         *
         * @param dst destination buffer of count*8 samples + 64 samples for convolution tail
         * @param src source buffer of count samples
         * @param count number of samples
         */
        extern void (* lanczos_resample_8x3)(float *dst, const float *src, size_t count);

        /** Copy each even sample to output buffer
         *
         * @param dst destination buffer
         * @param src source buffer
         * @param count number of samples to process
         */
        extern void (* downsample_2x)(float *dst, const float *src, size_t count);

        /** Copy each 3rd sample to output buffer
         *
         * @param dst destination buffer
         * @param src source buffer
         * @param count number of samples to process
         */
        extern void (* downsample_3x)(float *dst, const float *src, size_t count);

        /** Copy each 4th sample to output buffer
         *
         * @param dst destination buffer
         * @param src source buffer
         * @param count number of samples to process
         */
        extern void (* downsample_4x)(float *dst, const float *src, size_t count);

        /** Copy each 6th sample to output buffer
         *
         * @param dst destination buffer
         * @param src source buffer
         * @param count number of samples to process
         */
        extern void (* downsample_6x)(float *dst, const float *src, size_t count);

        /** Copy each 8th sample to output buffer
         *
         * @param dst destination buffer
         * @param src source buffer
         * @param count number of samples to process
         */
        extern void (* downsample_8x)(float *dst, const float *src, size_t count);
    }
}

#endif /* INCLUDE_CORE_DSP_RESAMPLING_H_ */
