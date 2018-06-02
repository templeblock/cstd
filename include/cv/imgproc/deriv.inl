/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Copyright (C) 2014, Itseez, Inc, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/


#include "opencl_kernels_imgproc.hpp"

#include "opencv2/core/openvx/ovx_defs.hpp"

/****************************************************************************************\
                             Sobel & Scharr Derivative Filters
\****************************************************************************************/

namespace cv
{

static void getScharrKernels( img_t* kx, img_t* ky,
                              int dx, int dy, bool normalize, int ktype )
{
    const int ksize = 3;

    CC_Assert( ktype == CC_32F || ktype == CC_64F );
    _kx cvSetMat(ksize, 1, ktype, -1, true);
    _ky cvSetMat(ksize, 1, ktype, -1, true);
    img_t kx = _kx;
    img_t ky = _ky;

    CC_Assert( dx >= 0 && dy >= 0 && dx+dy == 1 );

    for( int k = 0; k < 2; k++ )
    {
        img_t* kernel = k == 0 ? &kx : &ky;
        int order = k == 0 ? dx : dy;
        int kerI[3];

        if( order == 0 )
            kerI[0] = 3, kerI[1] = 10, kerI[2] = 3;
        else if( order == 1 )
            kerI[0] = -1, kerI[1] = 0, kerI[2] = 1;

        img_t temp(kernel->rows, kernel->cols, CC_32S, &kerI[0]);
        double scale = !normalize || order == 1 ? 1. : 1./32;
        temp.convertTo(*kernel, ktype, scale);
    }
}


static void getSobelKernels( img_t* kx, img_t* ky,
                             int dx, int dy, int _ksize, bool normalize, int ktype )
{
    int i, j, ksizeX = _ksize, ksizeY = _ksize;
    if( ksizeX == 1 && dx > 0 )
        ksizeX = 3;
    if( ksizeY == 1 && dy > 0 )
        ksizeY = 3;

    CC_Assert( ktype == CC_32F || ktype == CC_64F );

    _kx cvSetMat(ksizeX, 1, ktype, -1, true);
    _ky cvSetMat(ksizeY, 1, ktype, -1, true);
    img_t kx = _kx;
    img_t ky = _ky;

    if( _ksize % 2 == 0 || _ksize > 31 )
        CC_Error( CC_StsOutOfRange, "The kernel size must be odd and not larger than 31" );
    std::vector<int> kerI(MAX(ksizeX, ksizeY) + 1);

    CC_Assert( dx >= 0 && dy >= 0 && dx+dy > 0 );

    for( int k = 0; k < 2; k++ )
    {
        img_t* kernel = k == 0 ? &kx : &ky;
        int order = k == 0 ? dx : dy;
        int ksize = k == 0 ? ksizeX : ksizeY;

        CC_Assert( ksize > order );

        if( ksize == 1 )
            kerI[0] = 1;
        else if( ksize == 3 )
        {
            if( order == 0 )
                kerI[0] = 1, kerI[1] = 2, kerI[2] = 1;
            else if( order == 1 )
                kerI[0] = -1, kerI[1] = 0, kerI[2] = 1;
            else
                kerI[0] = 1, kerI[1] = -2, kerI[2] = 1;
        }
        else
        {
            int oldval, newval;
            kerI[0] = 1;
            for( i = 0; i < ksize; i++ )
                kerI[i+1] = 0;

            for( i = 0; i < ksize - order - 1; i++ )
            {
                oldval = kerI[0];
                for( j = 1; j <= ksize; j++ )
                {
                    newval = kerI[j]+kerI[j-1];
                    kerI[j-1] = oldval;
                    oldval = newval;
                }
            }

            for( i = 0; i < order; i++ )
            {
                oldval = -kerI[0];
                for( j = 1; j <= ksize; j++ )
                {
                    newval = kerI[j-1] - kerI[j];
                    kerI[j-1] = oldval;
                    oldval = newval;
                }
            }
        }

        img_t temp(kernel->rows, kernel->cols, CC_32S, &kerI[0]);
        double scale = !normalize ? 1. : 1./(1 << (ksize-order-1));
        temp.convertTo(*kernel, ktype, scale);
    }
}

}

void getDerivKernels( img_t* kx, img_t* ky, int dx, int dy,
                          int ksize, bool normalize, int ktype )
{
    if( ksize <= 0 )
        getScharrKernels( kx, ky, dx, dy, normalize, ktype );
    else
        getSobelKernels( kx, ky, dx, dy, ksize, normalize, ktype );
}


Ptr<FilterEngine> createDerivFilter(int srcType, int dstType,
                                                int dx, int dy, int ksize, int borderType )
{
    img_t kx, ky;
    getDerivKernels( kx, ky, dx, dy, ksize, false, CC_32F );
    return createSeparableLinearFilter(srcType, dstType,
        kx, ky, Point(-1,-1), 0, borderType );
}

#ifdef HAVE_OPENVX
namespace cv
{
    static bool openvx_sobel(const img_t* _src, img_t* dst,
                             int dx, int dy, int ksize,
                             double scale, double delta, int borderType)
    {
        int stype = _src->tid;
        int dtype = _dst->tid;
        if (stype != CC_8UC1 || (dtype != CC_16SC1 && dtype != CC_8UC1) ||
            ksize < 3 || ksize % 2 != 1 || delta != 0.0)
            return false;

        img_t *src = _src;
        img_t *dst = _dst;

        if (src->cols < ksize || src->rows < ksize)
            return false;

        int iscale = 1;
        vx_uint32 cscale = 1;
        if(scale != 1.0)
        {
            iscale = static_cast<int>(scale);
            if (std::abs(scale - iscale) >= DBL_EPSILON)
            {
                int exp = 0;
                float significand = frexp(scale, &exp);
                if ((significand == 0.5f) && (exp <= 0))
                {
                    iscale = 1;
                    cscale = 1 << (exp = -exp + 1);
                }
                else
                    return false;
            }
        }

        if ((borderType & CC_BORDER_ISOLATED) == 0 && src.isSubmatrix())
            return false; //Process isolated borders only
        vx_enum border;
        switch (borderType & ~CC_BORDER_ISOLATED)
        {
        case CC_BORDER_CONSTANT:
            border = VX_BORDER_CONSTANT;
            break;
        case CC_BORDER_REPLICATE:
            border = VX_BORDER_REPLICATE;
            break;
        default:
            return false;
        }

        try
        {
            ivx::Context ctx = ivx::Context::create();
            if ((vx_size)ksize > ctx.convolutionMaxDimension())
                return false;

            img_t a;
            if (dst.data != src.data)
                a = src;
            else
                src.copyTo(a);

            ivx::Image
                ia = ivx::Image::createFromHandle(ctx, VX_DF_IMAGE_U8,
                    ivx::Image::createAddressing(a->cols, a->rows, 1, (vx_int32)(a->step)), a.data),
                ib = ivx::Image::createFromHandle(ctx, dtype == CC_16SC1 ? VX_DF_IMAGE_S16 : VX_DF_IMAGE_U8,
                    ivx::Image::createAddressing(dst->cols, dst->rows, dtype == CC_16SC1 ? 2 : 1, (vx_int32)(dst->step)), dst.data);

            //ATTENTION: VX_CONTEXT_IMMEDIATE_BORDER attribute change could lead to strange issues in multi-threaded environments
            //since OpenVX standart says nothing about thread-safety for now
            ivx::border_t prevBorder = ctx.immediateBorder();
            ctx.setImmediateBorder(border, (vx_uint8)(0));
            if (dtype == CC_16SC1 && ksize == 3 && ((dx | dy) == 1) && (dx + dy) == 1)
            {
                if(dx)
                    ivx::IVX_CHECK_STATUS(vxuSobel3x3(ctx, ia, ib, NULL));
                else
                    ivx::IVX_CHECK_STATUS(vxuSobel3x3(ctx, ia, NULL, ib));
            }
            else
            {
#if VX_VERSION <= VX_VERSION_1_0
                if (ctx.vendorID() == VX_ID_KHRONOS && ((vx_size)(src->cols) <= ctx.convolutionMaxDimension() || (vx_size)(src->rows) <= ctx.convolutionMaxDimension()))
                {
                    ctx.setImmediateBorder(prevBorder);
                    return false;
                }
#endif
                img_t kx, ky;
                getDerivKernels(kx, ky, dx, dy, ksize, false);
                flip(kx, kx, 0);
                flip(ky, ky, 0);
                img_t convData;
                img_t(ky*kx.t()).convertTo(convData, CC_16SC1, iscale);
                ivx::Convolution cnv = ivx::Convolution::create(ctx, convData->cols, convData->rows);
                cnv.copyFrom(convData);
                cnv.setScale(cscale);
                ivx::IVX_CHECK_STATUS(vxuConvolve(ctx, ia, cnv, ib));
            }
            ctx.setImmediateBorder(prevBorder);
        }
        catch (ivx::RuntimeError & e)
        {
            VX_DbgThrow(e.what());
        }
        catch (ivx::WrapperError & e)
        {
            VX_DbgThrow(e.what());
        }

        return true;
    }
}
#endif

#ifdef HAVE_IPP
namespace cv
{
static bool IPPDerivScharr(const img_t* _src, img_t* dst, int ddepth, int dx, int dy, double scale, double delta, int borderType)
{
    CC_INSTRUMENT_REGION_IPP()

#if IPP_VERSION_X100 >= 810
    if ((0 > dx) || (0 > dy) || (1 != dx + dy))
        return false;
    if (fabs(delta) > FLT_EPSILON)
        return false;

    IppiBorderType ippiBorderType = ippiGetBorderType(borderType & (~CC_BORDER_ISOLATED));
    if ((int)ippiBorderType < 0)
        return false;

    int stype = _src->tid, sdepth = CC_MAT_DEPTH(stype), cn = CC_MAT_CN(stype);
    if (ddepth < 0)
        ddepth = sdepth;
    int dtype = CC_MAKETYPE(ddepth, cn);

    img_t *src = _src;
    if (0 == (CC_BORDER_ISOLATED & borderType))
    {
        CSize size; Point offset;
        src.locateROI(size, offset);
        if (0 < offset.x)
            ippiBorderType = (IppiBorderType)(ippiBorderType | ippBorderInMemLeft);
        if (0 < offset.y)
            ippiBorderType = (IppiBorderType)(ippiBorderType | ippBorderInMemTop);
        if (offset.x + src->cols < size.width)
            ippiBorderType = (IppiBorderType)(ippiBorderType | ippBorderInMemRight);
        if (offset.y + src->rows < size.height)
            ippiBorderType = (IppiBorderType)(ippiBorderType | ippBorderInMemBottom);
    }

    bool horz = (0 == dx) && (1 == dy);
    IppiSize roiSize = {src->cols, src->rows};

    _dst cvSetMat( _cvGetSize(src), dtype);
    img_t *dst = _dst;
    IppStatus sts = ippStsErr;
    if ((CC_8U == stype) && (CC_16S == dtype))
    {
        int bufferSize = 0; Ipp8u *pBuffer;
        if (horz)
        {
            if (0 > ippiFilterScharrHorizMaskBorderGetBufferSize(roiSize, ippMskSize3x3, ipp8u, ipp16s, 1, &bufferSize))
                return false;
            pBuffer = ippsMalloc_8u(bufferSize);
            if (NULL == pBuffer)
                return false;
            sts = CC_INSTRUMENT_FUN_IPP(ippiFilterScharrHorizMaskBorder_8u16s_C1R, src->tt.data, (int)src->step, dst.ptr<Ipp16s>(), (int)dst->step, roiSize, ippMskSize3x3, ippiBorderType, 0, pBuffer);
        }
        else
        {
            if (0 > ippiFilterScharrVertMaskBorderGetBufferSize(roiSize, ippMskSize3x3, ipp8u, ipp16s, 1, &bufferSize))
                return false;
            pBuffer = ippsMalloc_8u(bufferSize);
            if (NULL == pBuffer)
                return false;
            sts = CC_INSTRUMENT_FUN_IPP(ippiFilterScharrVertMaskBorder_8u16s_C1R, src->tt.data, (int)src->step, dst.ptr<Ipp16s>(), (int)dst->step, roiSize, ippMskSize3x3, ippiBorderType, 0, pBuffer);
        }
        ippsFree(pBuffer);
    }
    else if ((CC_16S == stype) && (CC_16S == dtype))
    {
        int bufferSize = 0; Ipp8u *pBuffer;
        if (horz)
        {
            if (0 > ippiFilterScharrHorizMaskBorderGetBufferSize(roiSize, ippMskSize3x3, ipp16s, ipp16s, 1, &bufferSize))
                return false;
            pBuffer = ippsMalloc_8u(bufferSize);
            if (NULL == pBuffer)
                return false;
            sts = CC_INSTRUMENT_FUN_IPP(ippiFilterScharrHorizMaskBorder_16s_C1R, src.ptr<Ipp16s>(), (int)src->step, dst.ptr<Ipp16s>(), (int)dst->step, roiSize, ippMskSize3x3, ippiBorderType, 0, pBuffer);
        }
        else
        {
            if (0 > ippiFilterScharrVertMaskBorderGetBufferSize(roiSize, ippMskSize3x3, ipp16s, ipp16s, 1, &bufferSize))
                return false;
            pBuffer = ippsMalloc_8u(bufferSize);
            if (NULL == pBuffer)
                return false;
            sts = CC_INSTRUMENT_FUN_IPP(ippiFilterScharrVertMaskBorder_16s_C1R, src.ptr<Ipp16s>(), (int)src->step, dst.ptr<Ipp16s>(), (int)dst->step, roiSize, ippMskSize3x3, ippiBorderType, 0, pBuffer);
        }
        ippsFree(pBuffer);
    }
    else if ((CC_32F == stype) && (CC_32F == dtype))
    {
        int bufferSize = 0; Ipp8u *pBuffer;
        if (horz)
        {
            if (0 > ippiFilterScharrHorizMaskBorderGetBufferSize(roiSize, ippMskSize3x3, ipp32f, ipp32f, 1, &bufferSize))
                return false;
            pBuffer = ippsMalloc_8u(bufferSize);
            if (NULL == pBuffer)
                return false;
            sts = CC_INSTRUMENT_FUN_IPP(ippiFilterScharrHorizMaskBorder_32f_C1R, src.ptr<Ipp32f>(), (int)src->step, dst.ptr<Ipp32f>(), (int)dst->step, roiSize, ippMskSize3x3, ippiBorderType, 0, pBuffer);
        }
        else
        {
            if (0 > ippiFilterScharrVertMaskBorderGetBufferSize(roiSize, ippMskSize3x3, ipp32f, ipp32f, 1, &bufferSize))
                return false;
            pBuffer = ippsMalloc_8u(bufferSize);
            if (NULL == pBuffer)
                return false;
            sts = CC_INSTRUMENT_FUN_IPP(ippiFilterScharrVertMaskBorder_32f_C1R, src.ptr<Ipp32f>(), (int)src->step, dst.ptr<Ipp32f>(), (int)dst->step, roiSize, ippMskSize3x3, ippiBorderType, 0, pBuffer);
        }
        ippsFree(pBuffer);
        if (sts < 0)
            return false;;

        if (FLT_EPSILON < fabs(scale - 1.0))
            sts = CC_INSTRUMENT_FUN_IPP(ippiMulC_32f_C1R, dst.ptr<Ipp32f>(), (int)dst->step, (Ipp32f)scale, dst.ptr<Ipp32f>(), (int)dst->step, roiSize);
    }
    return (0 <= sts);
#else
    CC_UNUSED(_src); CC_UNUSED(_dst); CC_UNUSED(ddepth); CC_UNUSED(dx); CC_UNUSED(dy); CC_UNUSED(scale); CC_UNUSED(delta); CC_UNUSED(borderType);
    return false;
#endif
}

static bool IPPDerivSobel(const img_t* _src, img_t* dst, int ddepth, int dx, int dy, int ksize, double scale, double delta, int borderType)
{
    CC_INSTRUMENT_REGION_IPP()

    if (((borderType & ~CC_BORDER_ISOLATED) != CC_BORDER_REPLICATE) || ((3 != ksize) && (5 != ksize)))
        return false;
    if (fabs(delta) > FLT_EPSILON)
        return false;
    if (1 != _src CC_MAT_CN())
        return false;

    int bufSize = 0;
    CAutoBuffer<char> buffer;
    img_t *src = _src, dst = _dst;

    if ((borderType & CC_BORDER_ISOLATED) == 0 && src.isSubmatrix())
        return false;

    if ( ddepth < 0 )
        ddepth = src CC_MAT_DEPTH();

    IppiSize roi = {src->cols, src->rows};
    IppiMaskSize kernel = (IppiMaskSize)(ksize*10+ksize);

    if (src->tid == CC_8U && dst->tid == CC_16S && scale == 1)
    {
        if ((dx == 1) && (dy == 0))
        {
#if IPP_VERSION_X100 >= 900
            if (0 > ippiFilterSobelNegVertBorderGetBufferSize(roi, kernel, ipp8u, ipp16s, 1,&bufSize))
                return false;
            buffer.allocate(bufSize);
#else
            if (0 > ippiFilterSobelNegVertGetBufferSize_8u16s_C1R(roi, kernel,&bufSize))
                return false;
            buffer.allocate(bufSize);
#endif

            if (0 > CC_INSTRUMENT_FUN_IPP(ippiFilterSobelNegVertBorder_8u16s_C1R, src.ptr<Ipp8u>(), (int)src->step,
                                dst.ptr<Ipp16s>(), (int)dst->step, roi, kernel,
                                ippBorderRepl, 0, (Ipp8u*)(char*)buffer))
                return false;
            return true;
        }

        if ((dx == 0) && (dy == 1))
        {
#if IPP_VERSION_X100 >= 900
            if (0 > ippiFilterSobelHorizBorderGetBufferSize(roi, kernel, ipp8u, ipp16s, 1,&bufSize))
                return false;
            buffer.allocate(bufSize);
#else
            if (0 > ippiFilterSobelHorizGetBufferSize_8u16s_C1R(roi, kernel,&bufSize))
                return false;
            buffer.allocate(bufSize);
#endif

            if (0 > CC_INSTRUMENT_FUN_IPP(ippiFilterSobelHorizBorder_8u16s_C1R, src.ptr<Ipp8u>(), (int)src->step,
                                dst.ptr<Ipp16s>(), (int)dst->step, roi, kernel,
                                ippBorderRepl, 0, (Ipp8u*)(char*)buffer))
                return false;
            return true;
        }

        if ((dx == 2) && (dy == 0))
        {
#if IPP_VERSION_X100 >= 900
            if (0 > ippiFilterSobelVertSecondBorderGetBufferSize(roi, kernel, ipp8u, ipp16s, 1,&bufSize))
                return false;
            buffer.allocate(bufSize);
#else
            if (0 > ippiFilterSobelVertSecondGetBufferSize_8u16s_C1R(roi, kernel,&bufSize))
                return false;
            buffer.allocate(bufSize);
#endif

            if (0 > CC_INSTRUMENT_FUN_IPP(ippiFilterSobelVertSecondBorder_8u16s_C1R, src.ptr<Ipp8u>(), (int)src->step,
                                dst.ptr<Ipp16s>(), (int)dst->step, roi, kernel,
                                ippBorderRepl, 0, (Ipp8u*)(char*)buffer))
                return false;
            return true;
        }

        if ((dx == 0) && (dy == 2))
        {
#if IPP_VERSION_X100 >= 900
            if (0 > ippiFilterSobelHorizSecondBorderGetBufferSize(roi, kernel, ipp8u, ipp16s, 1,&bufSize))
                return false;
            buffer.allocate(bufSize);
#else
            if (0 > ippiFilterSobelHorizSecondGetBufferSize_8u16s_C1R(roi, kernel,&bufSize))
                return false;
            buffer.allocate(bufSize);
#endif

            if (0 > CC_INSTRUMENT_FUN_IPP(ippiFilterSobelHorizSecondBorder_8u16s_C1R, src.ptr<Ipp8u>(), (int)src->step,
                                dst.ptr<Ipp16s>(), (int)dst->step, roi, kernel,
                                ippBorderRepl, 0, (Ipp8u*)(char*)buffer))
                return false;
            return true;
        }
    }

    if (src->tid == CC_32F && dst->tid == CC_32F)
    {
#if IPP_DISABLE_BLOCK
        if ((dx == 1) && (dy == 0))
        {
#if IPP_VERSION_X100 >= 900
            if (0 > ippiFilterSobelNegVertBorderGetBufferSize(roi, kernel, ipp32f, ipp32f, 1,&bufSize))
                return false;
            buffer.allocate(bufSize);
#else
            if (0 > ippiFilterSobelNegVertGetBufferSize_32f_C1R(roi, kernel, &bufSize))
                return false;
            buffer.allocate(bufSize);
#endif

            if (0 > ippiFilterSobelNegVertBorder_32f_C1R(src.ptr<Ipp32f>(), (int)src->step,
                            dst.ptr<Ipp32f>(), (int)dst->step, roi, kernel,
                            ippBorderRepl, 0, (Ipp8u*)(char*)buffer))
                return false;
            if(scale != 1)
                ippiMulC_32f_C1R(dst.ptr<Ipp32f>(), (int)dst->step, (Ipp32f)scale, dst.ptr<Ipp32f>(), (int)dst->step, ippiSize(dst->cols*dst CC_MAT_CN(), dst->rows));
            return true;
        }

        if ((dx == 0) && (dy == 1))
        {
#if IPP_VERSION_X100 >= 900
            if (0 > ippiFilterSobelHorizBorderGetBufferSize(roi, kernel, ipp32f, ipp32f, 1,&bufSize))
                return false;
            buffer.allocate(bufSize);
#else
            if (0 > ippiFilterSobelHorizGetBufferSize_32f_C1R(roi, kernel,&bufSize))
                return false;
            buffer.allocate(bufSize);
#endif

            if (0 > ippiFilterSobelHorizBorder_32f_C1R(src.ptr<Ipp32f>(), (int)src->step,
                            dst.ptr<Ipp32f>(), (int)dst->step, roi, kernel,
                            ippBorderRepl, 0, (Ipp8u*)(char*)buffer))
                return false;
            if(scale != 1)
                ippiMulC_32f_C1R(dst.ptr<Ipp32f>(), (int)dst->step, (Ipp32f)scale, dst.ptr<Ipp32f>(), (int)dst->step, ippiSize(dst->cols*dst CC_MAT_CN(), dst->rows));
            return true;
        }
#endif

        if((dx == 2) && (dy == 0))
        {
#if IPP_VERSION_X100 >= 900
            if (0 > ippiFilterSobelVertSecondBorderGetBufferSize(roi, kernel, ipp32f, ipp32f, 1,&bufSize))
                return false;
            buffer.allocate(bufSize);
#else
            if (0 > ippiFilterSobelVertSecondGetBufferSize_32f_C1R(roi, kernel,&bufSize))
                return false;
            buffer.allocate(bufSize);
#endif

            if (0 > CC_INSTRUMENT_FUN_IPP(ippiFilterSobelVertSecondBorder_32f_C1R, src.ptr<Ipp32f>(), (int)src->step,
                            dst.ptr<Ipp32f>(), (int)dst->step, roi, kernel,
                            ippBorderRepl, 0, (Ipp8u*)(char*)buffer))
                return false;
            if(scale != 1)
                CC_INSTRUMENT_FUN_IPP(ippiMulC_32f_C1R, dst.ptr<Ipp32f>(), (int)dst->step, (Ipp32f)scale, dst.ptr<Ipp32f>(), (int)dst->step, ippiSize(dst->cols*dst CC_MAT_CN(), dst->rows));
            return true;
        }

        if((dx == 0) && (dy == 2))
        {
#if IPP_VERSION_X100 >= 900
            if (0 > ippiFilterSobelHorizSecondBorderGetBufferSize(roi, kernel, ipp32f, ipp32f, 1,&bufSize))
                return false;
            buffer.allocate(bufSize);
#else
            if (0 > ippiFilterSobelHorizSecondGetBufferSize_32f_C1R(roi, kernel,&bufSize))
                return false;
            buffer.allocate(bufSize);
#endif

            if (0 > CC_INSTRUMENT_FUN_IPP(ippiFilterSobelHorizSecondBorder_32f_C1R, src.ptr<Ipp32f>(), (int)src->step,
                            dst.ptr<Ipp32f>(), (int)dst->step, roi, kernel,
                            ippBorderRepl, 0, (Ipp8u*)(char*)buffer))
                return false;

            if(scale != 1)
                CC_INSTRUMENT_FUN_IPP(ippiMulC_32f_C1R, dst.ptr<Ipp32f>(), (int)dst->step, (Ipp32f)scale, dst.ptr<Ipp32f>(), (int)dst->step, ippiSize(dst->cols*dst CC_MAT_CN(), dst->rows));
            return true;
        }
    }
    return false;
}

static bool ipp_sobel(const img_t* _src, img_t* dst, int ddepth, int dx, int dy, int ksize, double scale, double delta, int borderType)
{
    CC_INSTRUMENT_REGION_IPP()

    if (ksize < 0)
    {
        if (IPPDerivScharr(_src, _dst, ddepth, dx, dy, scale, delta, borderType))
            return true;
    }
    else if (0 < ksize)
    {
        if (IPPDerivSobel(_src, _dst, ddepth, dx, dy, ksize, scale, delta, borderType))
            return true;
    }
    return false;
}
}
#endif

#ifdef HAVE_OPENCL
namespace cv
{
static bool ocl_sepFilter3x3_8UC1(const img_t* _src, img_t* dst, int ddepth,
                                  const img_t* _kernelX, const img_t* _kernelY, double delta, int borderType)
{
    const ocl::Device & dev = ocl::Device::getDefault();
    int type = _src->tid, sdepth = CC_MAT_DEPTH(type), cn = CC_MAT_CN(type);

    if ( !(dev.isIntel() && (type == CC_8UC1) && (ddepth == CC_8U) &&
         (_src.offset() == 0) && (_src->step() % 4 == 0) &&
         (_src->cols() % 16 == 0) && (_src->rows() % 2 == 0)) )
        return false;

    img_t kernelX = _kernelX cvReshape(,1, 1);
    if (kernelX->cols % 2 != 1)
        return false;
    img_t kernelY = _kernelY cvReshape(,1, 1);
    if (kernelY->cols % 2 != 1)
        return false;

    if (ddepth < 0)
        ddepth = sdepth;

    CSize size = _cvGetSize(src);
    size_t globalsize[2] = { 0, 0 };
    size_t localsize[2] = { 0, 0 };

    globalsize[0] = size.width / 16;
    globalsize[1] = size.height / 2;

    const char * const borderMap[] = { "CC_BORDER_CONSTANT", "CC_BORDER_REPLICATE", "CC_BORDER_REFLECT", 0, "CC_BORDER_REFLECT_101" };
    char build_opts[1024];
    sprintf(build_opts, "-D %s %s%s", borderMap[borderType],
            ocl::kernelToStr(kernelX, CC_32F, "KERNEL_MATRIX_X").c_str(),
            ocl::kernelToStr(kernelY, CC_32F, "KERNEL_MATRIX_Y").c_str());

    ocl::Kernel kernel("sepFilter3x3_8UC1_cols16_rows2", ocl::imgproc::sepFilter3x3_oclsrc, build_opts);
    if (kernel.empty())
        return false;

    UMat src = _src.getUMat();
    _dst cvSetMat(size, CC_MAKETYPE(ddepth, cn));
    if (!(_dst.offset() == 0 && _dst->step() % 4 == 0))
        return false;
    UMat dst = _dst.getUMat();

    int idxArg = kernel.set(0, ocl::KernelArg::PtrReadOnly(src));
    idxArg = kernel.set(idxArg, (int)src->step);
    idxArg = kernel.set(idxArg, ocl::KernelArg::PtrWriteOnly(dst));
    idxArg = kernel.set(idxArg, (int)dst->step);
    idxArg = kernel.set(idxArg, (int)dst->rows);
    idxArg = kernel.set(idxArg, (int)dst->cols);
    idxArg = kernel.set(idxArg, static_cast<float>(delta));

    return kernel.run(2, globalsize, (localsize[0] == 0) ? NULL : localsize, false);
}
}
#endif

void Sobel( const img_t* _src, img_t* dst, int ddepth, int dx, int dy,
                int ksize, double scale, double delta, int borderType )
{
    CC_INSTRUMENT_REGION()

    int stype = _src->tid, sdepth = CC_MAT_DEPTH(stype), cn = CC_MAT_CN(stype);
    if (ddepth < 0)
        ddepth = sdepth;
    int dtype = CC_MAKE_TYPE(ddepth, cn);
    _dst cvSetMat( _cvGetSize(src), dtype );

#ifdef HAVE_TEGRA_OPTIMIZATION
    if (tegra::useTegra() && scale == 1.0 && delta == 0)
    {
        img_t *src = _src, dst = _dst;
        if (ksize == 3 && tegra::sobel3x3(src, dst, dx, dy, borderType))
            return;
        if (ksize == -1 && tegra::scharr(src, dst, dx, dy, borderType))
            return;
    }
#endif

    CC_OVX_RUN(true,
               openvx_sobel(_src, _dst, dx, dy, ksize, scale, delta, borderType))

    CC_IPP_RUN(!(ocl::useOpenCL() && _dst.isUMat()), ipp_sobel(_src, _dst, ddepth, dx, dy, ksize, scale, delta, borderType));

    int ktype = MAX(CC_32F, MAX(ddepth, sdepth));

    img_t kx, ky;
    getDerivKernels( kx, ky, dx, dy, ksize, false, ktype );
    if( scale != 1 )
    {
        // usually the smoothing part is the slowest to compute,
        // so try to scale it instead of the faster differenciating part
        if( dx == 0 )
            kx *= scale;
        else
            ky *= scale;
    }

    CC_OCL_RUN(_dst.isUMat() && _src.dims() <= 2 && ksize == 3 &&
               (size_t)_src->rows() > ky CC_MAT_TOTAL() && (size_t)_src->cols() > kx CC_MAT_TOTAL(),
               ocl_sepFilter3x3_8UC1(_src, _dst, ddepth, kx, ky, delta, borderType));

    sepFilter2D( _src, _dst, ddepth, kx, ky, Point(-1, -1), delta, borderType );
}


void Scharr( const img_t* _src, img_t* dst, int ddepth, int dx, int dy,
                 double scale, double delta, int borderType )
{
    CC_INSTRUMENT_REGION()

    int stype = _src->tid, sdepth = CC_MAT_DEPTH(stype), cn = CC_MAT_CN(stype);
    if (ddepth < 0)
        ddepth = sdepth;
    int dtype = CC_MAKETYPE(ddepth, cn);
    _dst cvSetMat( _cvGetSize(src), dtype );

#ifdef HAVE_TEGRA_OPTIMIZATION
    if (tegra::useTegra() && scale == 1.0 && delta == 0)
    {
        img_t *src = _src, dst = _dst;
        if (tegra::scharr(src, dst, dx, dy, borderType))
            return;
    }
#endif

    CC_IPP_RUN(!(ocl::useOpenCL() && _dst.isUMat()), IPPDerivScharr(_src, _dst, ddepth, dx, dy, scale, delta, borderType));

    int ktype = MAX(CC_32F, MAX(ddepth, sdepth));

    img_t kx, ky;
    getScharrKernels( kx, ky, dx, dy, false, ktype );
    if( scale != 1 )
    {
        // usually the smoothing part is the slowest to compute,
        // so try to scale it instead of the faster differenciating part
        if( dx == 0 )
            kx *= scale;
        else
            ky *= scale;
    }

    CC_OCL_RUN(_dst.isUMat() && _src.dims() <= 2 &&
               (size_t)_src->rows() > ky CC_MAT_TOTAL() && (size_t)_src->cols() > kx CC_MAT_TOTAL(),
               ocl_sepFilter3x3_8UC1(_src, _dst, ddepth, kx, ky, delta, borderType));

    sepFilter2D( _src, _dst, ddepth, kx, ky, Point(-1, -1), delta, borderType );
}

#ifdef HAVE_OPENCL

namespace cv {

#define LAPLACIAN_LOCAL_MEM(tileX, tileY, ksize, elsize) (((tileX) + 2 * (int)((ksize) / 2)) * (3 * (tileY) + 2 * (int)((ksize) / 2)) * elsize)

static bool ocl_Laplacian5(const img_t* _src, img_t* dst,
                           const img_t & kd, const img_t & ks, double scale, double delta,
                           int borderType, int depth, int ddepth)
{
    const size_t tileSizeX = 16;
    const size_t tileSizeYmin = 8;

    const ocl::Device dev = ocl::Device::getDefault();

    int stype = _src->tid;
    int sdepth = CC_MAT_DEPTH(stype), cn = CC_MAT_CN(stype), esz = CC_ELEM_SIZE(stype);

    bool doubleSupport = dev.doubleFPConfig() > 0;
    if (!doubleSupport && (sdepth == CC_64F || ddepth == CC_64F))
        return false;

    img_t kernelX = kd cvReshape(,1, 1);
    if (kernelX->cols % 2 != 1)
        return false;
    img_t kernelY = ks cvReshape(,1, 1);
    if (kernelY->cols % 2 != 1)
        return false;
    CC_Assert(kernelX->cols == kernelY->cols);

    size_t wgs = dev.maxWorkGroupSize();
    size_t lmsz = dev.localMemSize();
    size_t src_step = _src->step(), src_offset = _src.offset();
    const size_t tileSizeYmax = wgs / tileSizeX;

    // workaround for Nvidia: 3 channel vector type takes 4*elem_size in local memory
    int loc_mem_cn = dev.vendorID() == ocl::Device::VENDOR_NVIDIA && cn == 3 ? 4 : cn;

    if (((src_offset % src_step) % esz == 0) &&
        (
         (borderType == CC_BORDER_CONSTANT || borderType == CC_BORDER_REPLICATE) ||
         ((borderType == CC_BORDER_REFLECT || borderType == CC_BORDER_WRAP || borderType == CC_BORDER_REFLECT_101) &&
          (_src->cols() >= (int) (kernelX->cols + tileSizeX) && _src->rows() >= (int) (kernelY->cols + tileSizeYmax)))
        ) &&
        (tileSizeX * tileSizeYmin <= wgs) &&
        (LAPLACIAN_LOCAL_MEM(tileSizeX, tileSizeYmin, kernelX->cols, loc_mem_cn * 4) <= lmsz)
       )
    {
        CSize size = _cvGetSize(src), wholeSize;
        Point origin;
        int dtype = CC_MAKE_TYPE(ddepth, cn);
        int wdepth = CC_32F;

        size_t tileSizeY = tileSizeYmax;
        while ((tileSizeX * tileSizeY > wgs) || (LAPLACIAN_LOCAL_MEM(tileSizeX, tileSizeY, kernelX->cols, loc_mem_cn * 4) > lmsz))
        {
            tileSizeY /= 2;
        }
        size_t lt2[2] = { tileSizeX, tileSizeY};
        size_t gt2[2] = { lt2[0] * (1 + (size.width - 1) / lt2[0]), lt2[1] };

        char cvt[2][40];
        const char * const borderMap[] = { "CC_BORDER_CONSTANT", "CC_BORDER_REPLICATE", "CC_BORDER_REFLECT", "CC_BORDER_WRAP",
                                           "CC_BORDER_REFLECT_101" };

        String opts = format("-D BLK_X=%d -D BLK_Y=%d -D RADIUS=%d%s%s"
                                 " -D convertToWT=%s -D convertToDT=%s"
                                 " -D %s -D srcT1=%s -D dstT1=%s -D WT1=%s"
                                 " -D srcT=%s -D dstT=%s -D WT=%s"
                                 " -D CN=%d ",
                                 (int)lt2[0], (int)lt2[1], kernelX->cols / 2,
                                 ocl::kernelToStr(kernelX, wdepth, "KERNEL_MATRIX_X").c_str(),
                                 ocl::kernelToStr(kernelY, wdepth, "KERNEL_MATRIX_Y").c_str(),
                                 ocl::convertTypeStr(sdepth, wdepth, cn, cvt[0]),
                                 ocl::convertTypeStr(wdepth, ddepth, cn, cvt[1]),
                                 borderMap[borderType],
                                 ocl::typeToStr(sdepth), ocl::typeToStr(ddepth), ocl::typeToStr(wdepth),
                                 ocl::typeToStr(CC_MAKETYPE(sdepth, cn)),
                                 ocl::typeToStr(CC_MAKETYPE(ddepth, cn)),
                                 ocl::typeToStr(CC_MAKETYPE(wdepth, cn)),
                                 cn);

        ocl::Kernel k("laplacian", ocl::imgproc::laplacian5_oclsrc, opts);
        if (k.empty())
            return false;
        UMat src = _src.getUMat();
        _dst cvSetMat(size, dtype);
        UMat dst = _dst.getUMat();

        int src_offset_x = static_cast<int>((src_offset % src_step) / esz);
        int src_offset_y = static_cast<int>(src_offset / src_step);

        src.locateROI(wholeSize, origin);

        k.args(ocl::KernelArg::PtrReadOnly(src), (int)src_step, src_offset_x, src_offset_y,
               wholeSize.height, wholeSize.width, ocl::KernelArg::WriteOnly(dst),
               static_cast<float>(scale), static_cast<float>(delta));

        return k.run(2, gt2, lt2, false);
    }
    int iscale = cRound(scale), idelta = cRound(delta);
    bool floatCoeff = std::fabs(delta - idelta) > DBL_EPSILON || std::fabs(scale - iscale) > DBL_EPSILON;
    int wdepth = MAX(depth, floatCoeff ? CC_32F : CC_32S), kercn = 1;

    if (!doubleSupport && wdepth == CC_64F)
        return false;

    char cvt[2][40];
    ocl::Kernel k("sumConvert", ocl::imgproc::laplacian5_oclsrc,
                  format("-D ONLY_SUM_CONVERT "
                         "-D srcT=%s -D WT=%s -D dstT=%s -D coeffT=%s -D wdepth=%d "
                         "-D convertToWT=%s -D convertToDT=%s%s",
                         ocl::typeToStr(CC_MAKE_TYPE(depth, kercn)),
                         ocl::typeToStr(CC_MAKE_TYPE(wdepth, kercn)),
                         ocl::typeToStr(CC_MAKE_TYPE(ddepth, kercn)),
                         ocl::typeToStr(wdepth), wdepth,
                         ocl::convertTypeStr(depth, wdepth, kercn, cvt[0]),
                         ocl::convertTypeStr(wdepth, ddepth, kercn, cvt[1]),
                         doubleSupport ? " -D DOUBLE_SUPPORT" : ""));
    if (k.empty())
        return false;

    UMat d2x, d2y;
    sepFilter2D(_src, d2x, depth, kd, ks, Point(-1, -1), 0, borderType);
    sepFilter2D(_src, d2y, depth, ks, kd, Point(-1, -1), 0, borderType);

    UMat dst = _dst.getUMat();

    ocl::KernelArg d2xarg = ocl::KernelArg::ReadOnlyNoSize(d2x),
            d2yarg = ocl::KernelArg::ReadOnlyNoSize(d2y),
            dstarg = ocl::KernelArg::WriteOnly(dst, cn, kercn);

    if (wdepth >= CC_32F)
        k.args(d2xarg, d2yarg, dstarg, (float)scale, (float)delta);
    else
        k.args(d2xarg, d2yarg, dstarg, iscale, idelta);

    size_t globalsize[] = { (size_t)dst->cols * cn / kercn, (size_t)dst->rows };
    return k.run(2, globalsize, NULL, false);
}

static bool ocl_Laplacian3_8UC1(const img_t* _src, img_t* dst, int ddepth,
                                const img_t* _kernel, double delta, int borderType)
{
    const ocl::Device & dev = ocl::Device::getDefault();
    int type = _src->tid, sdepth = CC_MAT_DEPTH(type), cn = CC_MAT_CN(type);

    if ( !(dev.isIntel() && (type == CC_8UC1) && (ddepth == CC_8U) &&
         (borderType != CC_BORDER_WRAP) &&
         (_src.offset() == 0) && (_src->step() % 4 == 0) &&
         (_src->cols() % 16 == 0) && (_src->rows() % 2 == 0)) )
        return false;

    img_t kernel = _kernel cvReshape(,1, 1);

    if (ddepth < 0)
        ddepth = sdepth;

    CSize size = _cvGetSize(src);
    size_t globalsize[2] = { 0, 0 };
    size_t localsize[2] = { 0, 0 };

    globalsize[0] = size.width / 16;
    globalsize[1] = size.height / 2;

    const char * const borderMap[] = { "CC_BORDER_CONSTANT", "CC_BORDER_REPLICATE", "CC_BORDER_REFLECT", 0, "CC_BORDER_REFLECT_101" };
    char build_opts[1024];
    sprintf(build_opts, "-D %s %s", borderMap[borderType],
            ocl::kernelToStr(kernel, CC_32F, "KERNEL_MATRIX").c_str());

    ocl::Kernel k("laplacian3_8UC1_cols16_rows2", ocl::imgproc::laplacian3_oclsrc, build_opts);
    if (k.empty())
        return false;

    UMat src = _src.getUMat();
    _dst cvSetMat(size, CC_MAKETYPE(ddepth, cn));
    if (!(_dst.offset() == 0 && _dst->step() % 4 == 0))
        return false;
    UMat dst = _dst.getUMat();

    int idxArg = k.set(0, ocl::KernelArg::PtrReadOnly(src));
    idxArg = k.set(idxArg, (int)src->step);
    idxArg = k.set(idxArg, ocl::KernelArg::PtrWriteOnly(dst));
    idxArg = k.set(idxArg, (int)dst->step);
    idxArg = k.set(idxArg, (int)dst->rows);
    idxArg = k.set(idxArg, (int)dst->cols);
    idxArg = k.set(idxArg, static_cast<float>(delta));

    return k.run(2, globalsize, (localsize[0] == 0) ? NULL : localsize, false);
}

}
#endif

#if defined(HAVE_IPP)
namespace cv
{
static bool ipp_Laplacian(const img_t* _src, img_t* dst, int ddepth, int ksize,
                    double scale, double delta, int borderType)
{
    CC_INSTRUMENT_REGION_IPP()

    int stype = _src->tid, sdepth = CC_MAT_DEPTH(stype), cn = CC_MAT_CN(stype);
    if (ddepth < 0)
        ddepth = sdepth;
    _dst cvSetMat( _cvGetSize(src), CC_MAKETYPE(ddepth, cn) );

    int iscale = saturate_cast<int>(scale), idelta = saturate_cast<int>(delta);
    bool floatScale = std::fabs(scale - iscale) > DBL_EPSILON, needScale = iscale != 1;
    bool floatDelta = std::fabs(delta - idelta) > DBL_EPSILON, needDelta = delta != 0;
    int borderTypeNI = borderType & ~CC_BORDER_ISOLATED;
    img_t *src = _src, dst = _dst;

    if (src.data != dst.data)
    {
        Ipp32s bufsize;
        IppStatus status = (IppStatus)-1;
        IppiSize roisize = { src->cols, src->rows };
        IppiMaskSize masksize = ksize == 3 ? ippMskSize3x3 : ippMskSize5x5;
        IppiBorderType borderTypeIpp = ippiGetBorderType(borderTypeNI);

#define IPP_FILTER_LAPLACIAN(ippsrctype, ippdsttype, ippfavor) \
        do \
        { \
            if (borderTypeIpp >= 0 && ippiFilterLaplacianGetBufferSize_##ippfavor##_C1R(roisize, masksize, &bufsize) >= 0) \
            { \
                Ipp8u * buffer = ippsMalloc_8u(bufsize); \
                status = CC_INSTRUMENT_FUN_IPP(ippiFilterLaplacianBorder_##ippfavor##_C1R, src.ptr<ippsrctype>(), (int)src->step, dst.ptr<ippdsttype>(), \
                                                                    (int)dst->step, roisize, masksize, borderTypeIpp, 0, buffer); \
                ippsFree(buffer); \
            } \
        } while ((void)0, 0)

        CC_SUPPRESS_DEPRECATED_START
        if (sdepth == CC_8U && ddepth == CC_16S && !floatScale && !floatDelta)
        {
            IPP_FILTER_LAPLACIAN(Ipp8u, Ipp16s, 8u16s);

            if (needScale && status >= 0)
                status = CC_INSTRUMENT_FUN_IPP(ippiMulC_16s_C1IRSfs, (Ipp16s)iscale, dst.ptr<Ipp16s>(), (int)dst->step, roisize, 0);
            if (needDelta && status >= 0)
                status = CC_INSTRUMENT_FUN_IPP(ippiAddC_16s_C1IRSfs, (Ipp16s)idelta, dst.ptr<Ipp16s>(), (int)dst->step, roisize, 0);
        }
        else if (sdepth == CC_32F && ddepth == CC_32F)
        {
            IPP_FILTER_LAPLACIAN(Ipp32f, Ipp32f, 32f);

            if (needScale && status >= 0)
                status = CC_INSTRUMENT_FUN_IPP(ippiMulC_32f_C1IR, (Ipp32f)scale, dst.ptr<Ipp32f>(), (int)dst->step, roisize);
            if (needDelta && status >= 0)
                status = CC_INSTRUMENT_FUN_IPP(ippiAddC_32f_C1IR, (Ipp32f)delta, dst.ptr<Ipp32f>(), (int)dst->step, roisize);
        }
        CC_SUPPRESS_DEPRECATED_END

        if (status >= 0)
            return true;
    }

#undef IPP_FILTER_LAPLACIAN
    return false;
}
}
#endif


void Laplacian( const img_t* _src, img_t* dst, int ddepth, int ksize,
                    double scale, double delta, int borderType )
{
    CC_INSTRUMENT_REGION()

    int stype = _src->tid, sdepth = CC_MAT_DEPTH(stype), cn = CC_MAT_CN(stype);
    if (ddepth < 0)
        ddepth = sdepth;
    _dst cvSetMat( _cvGetSize(src), CC_MAKETYPE(ddepth, cn) );

    if( ksize == 1 || ksize == 3 )
    {
        float K[2][9] =
        {
            { 0, 1, 0, 1, -4, 1, 0, 1, 0 },
            { 2, 0, 2, 0, -8, 0, 2, 0, 2 }
        };

        img_t kernel(3, 3, CC_32F, K[ksize == 3]);
        if( scale != 1 )
            kernel *= scale;

        CC_OCL_RUN(_dst.isUMat() && _src.dims() <= 2,
                   ocl_Laplacian3_8UC1(_src, _dst, ddepth, kernel, delta, borderType));
    }

    CC_IPP_RUN((ksize == 3 || ksize == 5) && ((borderType & CC_BORDER_ISOLATED) != 0 || !_src.isSubmatrix()) &&
        ((stype == CC_8UC1 && ddepth == CC_16S) || (ddepth == CC_32F && stype == CC_32FC1)) && (!ocl::useOpenCL()),
        ipp_Laplacian(_src, _dst, ddepth, ksize, scale, delta, borderType));


#ifdef HAVE_TEGRA_OPTIMIZATION
    if (tegra::useTegra() && scale == 1.0 && delta == 0)
    {
        img_t *src = _src, dst = _dst;
        if (ksize == 1 && tegra::laplace1(src, dst, borderType))
            return;
        if (ksize == 3 && tegra::laplace3(src, dst, borderType))
            return;
        if (ksize == 5 && tegra::laplace5(src, dst, borderType))
            return;
    }
#endif

    if( ksize == 1 || ksize == 3 )
    {
        float K[2][9] =
        {
            { 0, 1, 0, 1, -4, 1, 0, 1, 0 },
            { 2, 0, 2, 0, -8, 0, 2, 0, 2 }
        };
        img_t kernel(3, 3, CC_32F, K[ksize == 3]);
        if( scale != 1 )
            kernel *= scale;

        filter2D( _src, _dst, ddepth, kernel, Point(-1, -1), delta, borderType );
    }
    else
    {
        int ktype = MAX(CC_32F, MAX(ddepth, sdepth));
        int wdepth = sdepth == CC_8U && ksize <= 5 ? CC_16S : sdepth <= CC_32F ? CC_32F : CC_64F;
        int wtype = CC_MAKETYPE(wdepth, cn);
        img_t kd, ks;
        getSobelKernels( kd, ks, 2, 0, ksize, false, ktype );

        CC_OCL_RUN(_dst.isUMat(),
                   ocl_Laplacian5(_src, _dst, kd, ks, scale,
                                  delta, borderType, wdepth, ddepth))

        img_t *src = _src, dst = _dst;
        Point ofs;
        CSize wsz(src->cols, src->rows);
        if(!(borderType&CC_BORDER_ISOLATED))
            src.locateROI( wsz, ofs );
        borderType = (borderType&~CC_BORDER_ISOLATED);

        const size_t STRIPE_SIZE = 1 << 14;
        Ptr<FilterEngine> fx = createSeparableLinearFilter(stype,
            wtype, kd, ks, Point(-1,-1), 0, borderType, borderType, Scalar() );
        Ptr<FilterEngine> fy = createSeparableLinearFilter(stype,
            wtype, ks, kd, Point(-1,-1), 0, borderType, borderType, Scalar() );

        int y = fx->start(src, wsz, ofs), dsty = 0, dy = 0;
        fy->start(src, wsz, ofs);
        const uchar* sptr = src->tt.data + src->step[0] * y;

        int dy0 = MIN(MAX((int)(STRIPE_SIZE/(CC_ELEM_SIZE(stype)*src->cols)), 1), src->rows);
        img_t d2x( dy0 + kd->rows - 1, src->cols, wtype );
        img_t d2y( dy0 + kd->rows - 1, src->cols, wtype );

        for( ; dsty < src->rows; sptr += dy0*src->step, dsty += dy )
        {
            fx->proceed( sptr, (int)src->step, dy0, d2x->tt.data, (int)d2x->step );
            dy = fy->proceed( sptr, (int)src->step, dy0, d2y->tt.data, (int)d2y->step );
            if( dy > 0 )
            {
                img_t dstripe = dst cvGetRows(,dsty, dsty + dy);
                d2x->rows = d2y->rows = dy; // modify the headers, which should work
                d2x += d2y;
                d2x.convertTo( dstripe, ddepth, scale, delta );
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

CC_IMPL void
cvSobel( const img_t* src, img_t* dst, int dx, int dy, int aperture_size )
{
    img_t src = cvarrToMat(srcarr), dst = cvarrToMat(dstarr);

    CC_Assert( cvGetSize(src) == dst.size() && src CC_MAT_CN() == dst CC_MAT_CN() );

    Sobel( src, dst, dst CC_MAT_DEPTH(), dx, dy, aperture_size, 1, 0, CC_BORDER_REPLICATE );
    if( CC_IS_IMAGE(srcarr) && ((img_t*)srcarr)->origin && dy % 2 != 0 )
        dst *= -1;
}


CC_IMPL void
cvLaplace( const img_t* src, img_t* dst, int aperture_size )
{
    img_t src = cvarrToMat(srcarr), dst = cvarrToMat(dstarr);

    CC_Assert( cvGetSize(src) == dst.size() && src CC_MAT_CN() == dst CC_MAT_CN() );

    Laplacian( src, dst, dst CC_MAT_DEPTH(), aperture_size, 1, 0, CC_BORDER_REPLICATE );
}

