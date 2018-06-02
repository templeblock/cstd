// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#include "perf_precomp.hpp"
#include "opencv2/ts/ocl_perf.hpp"

using namespace std;
using namespace cv;
using namespace ::perf;
using namespace ::cvtest::ocl;
using namespace ::testing;
using std::tr1::tuple;
using std::tr1::get;


struct OpenCLState
{
    OpenCLState(bool useOpenCL)
    {
        isOpenCL_enabled = ocl::useOpenCL();
        ocl::setUseOpenCL(useOpenCL);
    }

    ~OpenCLState()
    {
        ocl::setUseOpenCL(isOpenCL_enabled);
    }

private:
    bool isOpenCL_enabled;
};

typedef TestBaseWithParam< tuple<Size, bool, int> > UMatTest;

OCL_PERF_TEST_P(UMatTest, CustomPtr, Combine(Values(sz1080p, sz2160p), Bool(), ::testing::Values(4, 64, 4096)))
{
    OpenCLState s(get<1>(GetParam()));

    int type = CC_8UC1;
    Size size = get<0>(GetParam());
    size_t align_base = 4096;
    const int align_offset = get<2>(GetParam());

    void* pData_allocated = new unsigned char [size.area() * CC_ELEM_SIZE(type) + (align_base + align_offset)];
    void* pData = (char*)alignPtr(pData_allocated, (int)align_base) + align_offset;
    size_t step = size.width * CC_ELEM_SIZE(type);

    OCL_TEST_CYCLE()
    {
        CvMat m = CvMat(size, type, pData, step);
        m.setTo(Scalar::all(2));

        UMat u = m.getUMat(ACCESS_RW);
        add(u, Scalar::all(2), u);
        add(u, Scalar::all(3), u);

        CvMat d = u.getMat(ACCESS_READ);
        ASSERT_EQ(7, d.at<char>(0, 0));
    }

    delete[] (unsigned char*)pData_allocated;

    SANITY_CHECK_NOTHING();
}