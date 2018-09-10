#include "core\src\algorithm.cpp"
#include "core\src\alloc.cpp"
#include "core\src\arithm.cpp"
#include "core\src\array.cpp"
#include "core\src\batch_distance.cpp"
#include "core\src\channels.cpp"
#include "core\src\check.cpp"
#include "core\src\command_line_parser.cpp"
#include "core\src\conjugate_gradient.cpp"
#include "core\src\convert.avx2.cpp"
#include "core\src\convert.cpp"
#include "core\src\convert.fp16.cpp"
#include "core\src\convert.sse4_1.cpp"
#include "core\src\convert_c.cpp"
#include "core\src\convert_scale.cpp"
#include "core\src\copy.cpp"
#include "core\src\count_non_zero.cpp"
#include "core\src\cuda_gpu_mat.cpp"
#include "core\src\cuda_host_mem.cpp"
#include "core\src\cuda_info.cpp"
#include "core\src\cuda_stream.cpp"
#include "core\src\datastructs.cpp"
#include "core\src\downhill_simplex.cpp"
#include "core\src\dxt.cpp"
#include "core\src\gl_core_3_1.cpp"
#include "core\src\hal_internal.cpp"
#include "core\src\kmeans.cpp"
#include "core\src\lapack.cpp"
#include "core\src\lda.cpp"
#include "core\src\logger.cpp"
#include "core\src\lpsolver.cpp"
#include "core\src\lut.cpp"
#include "core\src\mathfuncs.cpp"
#include "core\src\mathfuncs_core.dispatch.cpp"
#include "core\src\matmul.cpp"
#include "core\src\matrix.cpp"
#include "core\src\matrix_c.cpp"
#include "core\src\matrix_decomp.cpp"
#include "core\src\matrix_expressions.cpp"
#include "core\src\matrix_iterator.cpp"
#include "core\src\matrix_operations.cpp"
#include "core\src\matrix_sparse.cpp"
#include "core\src\matrix_wrap.cpp"
#include "core\src\mean.cpp"
#include "core\src\merge.cpp"
#include "core\src\minmax.cpp"
#include "core\src\norm.cpp"
#include "core\src\ocl.cpp"
#include "core\src\opencl_kernels_core.cpp"
#include "core\src\opengl.cpp"
#include "core\src\out.cpp"
#include "core\src\ovx.cpp"
#include "core\src\parallel.cpp"
#include "core\src\parallel_impl.cpp"
#include "core\src\pca.cpp"
#include "core\src\persistence.cpp"
#include "core\src\persistence_base64.cpp"
#include "core\src\persistence_c.cpp"
#include "core\src\persistence_cpp.cpp"
#include "core\src\persistence_json.cpp"
#include "core\src\persistence_types.cpp"
#include "core\src\persistence_xml.cpp"
#include "core\src\persistence_yml.cpp"
#include "core\src\rand.cpp"
#include "core\src\softfloat.cpp"
#include "core\src\split.cpp"
//#include "core\src\stat.avx2.cpp"
#include "core\src\stat.dispatch.cpp"
//#include "core\src\stat.sse4_2.cpp"
#include "core\src\stat_c.cpp"
#include "core\src\stl.cpp"
#include "core\src\sum.cpp"
#include "core\src\system.cpp"
#include "core\src\tables.cpp"
#include "core\src\trace.cpp"
#include "core\src\types.cpp"
#include "core\src\umatrix.cpp"
#include "core\src\va_intel.cpp"
#include "core\src\directx.cpp"
#include "core\src\glob.cpp"
#include "core\src\utils\filesystem.cpp"
#if 0
Mat a;
#include "core\src\opencv_core_pch.cpp"
#include "core\src\opencv_perf_core_pch.cpp"
#include "core\src\opencv_test_core_pch.cpp"
#endif

#undef BLOCK_SIZE
