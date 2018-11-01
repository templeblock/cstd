#ifdef USE_CUDNN


#include "caffe/layers/cudnn_pooling_layer.hpp"

namespace {

template <typename Dtype>
void CuDNNPoolingLayer<Dtype>::Forward(GPUContext* context, const vector<Blob<Dtype>*>& bottom,
    const vector<Blob<Dtype>*>& top) {
  const Dtype* bottom_data = bottom[0]->data<Context>();
  Dtype* top_data = top[0]->mutable_data<Context>();
  CUDNN_CHECK(cudnnPoolingForward(handle_, pooling_desc_,
        cudnn::dataType<Dtype>::one,
        bottom_desc_, bottom_data,
        cudnn::dataType<Dtype>::zero,
        top_desc_, top_data));
}

template <typename Dtype>
void CuDNNPoolingLayer<Dtype>::Backward(GPUContext* context, const vector<Blob<Dtype>*>& top,
    const vector<bool>& propagate_down, const vector<Blob<Dtype>*>& bottom) {
  if (!top[0]->propagate_down_) {
    return;
  }
  const Dtype* top_diff = top[0]->gpu_diff();
  const Dtype* top_data = top[0]->data<Context>();
  const Dtype* bottom_data = bottom[0]->data<Context>();
  Dtype* bottom_diff = bottom[0]->mutable_gpu_diff();
  CUDNN_CHECK(cudnnPoolingBackward(handle_, pooling_desc_,
        cudnn::dataType<Dtype>::one,
        top_desc_, top_data, top_desc_, top_diff,
        bottom_desc_, bottom_data,
        cudnn::dataType<Dtype>::zero,
        bottom_desc_, bottom_diff));
}

INSTANTIATE_LAYER_GPU_FUNCS(CuDNNPoolingLayer);

}  // namespace
#endif
