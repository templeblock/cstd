

#include "caffe/layers/dropout_layer.hpp"


namespace {

template <typename Dtype>
__global__ void DropoutForward(const int n, const Dtype* in,
    const unsigned int* mask, const unsigned int threshold, const Dtype scale,
    Dtype* out) {
  CUDA_KERNEL_LOOP(index, n) {
    out[index] = in[index] * (mask[index] > threshold) * scale;
  }
}

template <typename Dtype>
void DropoutLayer::Forward(GPUContext* context, const vector<Blob*>& bottom,
    const vector<Blob*>& top) {
  const Dtype* bottom_data = bottom[0]->data();
  Dtype* top_data = top[0]->mdata();
  const int count = bottom[0]->count();
  if (this->phase_ == TRAIN) {
    unsigned int* mask =
        static_cast<unsigned int*>(rand_vec_.mdata());
    caffe_gpu_rng_uniform(count, mask);
    // set thresholds
    // NOLINT_NEXT_LINE(whitespace/operators)
    DropoutForward<Dtype><<<CAFFE_GET_BLOCKS(count), CAFFE_CUDA_NUM_THREADS>>>(
        count, bottom_data, mask, uint_thres_, Dtype(scale_), top_data);
    CUDA_POST_KERNEL_CHECK;
  } else {
    caffe_copy(count, bottom_data, top_data);
  }
}

template <typename Dtype>
__global__ void DropoutBackward(const int n, const Dtype* in_diff,
    const unsigned int* mask, const unsigned int threshold, const Dtype scale,
    Dtype* out_diff) {
  CUDA_KERNEL_LOOP(index, n) {
    out_diff[index] = in_diff[index] * scale * (mask[index] > threshold);
  }
}

template <typename Dtype>
void DropoutLayer::Backward(GPUContext* context, const vector<Blob*>& top,
    const vector<bool>& propagate_down,
    const vector<Blob*>& bottom) {
  if (bottom[0]->propagate_down_) {
    const Dtype* top_diff = top[0]->gpu_diff();
    Dtype* bottom_diff = bottom[0]->gpu_mdiff();
    if (this->phase_ == TRAIN) {
      const unsigned int* mask =
          static_cast<const unsigned int*>(rand_vec_.data());
      const int count = bottom[0]->count();
      // NOLINT_NEXT_LINE(whitespace/operators)
      DropoutBackward<Dtype><<<CAFFE_GET_BLOCKS(count),
        CAFFE_CUDA_NUM_THREADS>>>(
          count, top_diff, mask, uint_thres_, Dtype(scale_), bottom_diff);
      CUDA_POST_KERNEL_CHECK;
    } else {
      caffe_copy(top[0]->count(), top_diff, bottom_diff);
    }
  }
}

INSTANTIATE_LAYER_GPU_FUNCS(DropoutLayer);

}  // namespace
