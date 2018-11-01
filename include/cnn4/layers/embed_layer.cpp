

#include "caffe/filler.hpp"
#include "caffe/layers/embed_layer.hpp"


namespace
{

  template <typename Dtype>
  void EmbedLayer<Dtype>::LayerSetUp(const vector<Blob<Dtype>*> & bottom,
                                     const vector<Blob<Dtype>*> & top)
  {
    N_ = this->param_->embed_param().num_output();
    CHECK_GT(N_, 0) << "EmbedLayer num_output must be positive.";
    K_ = this->param_->embed_param().input_dim();
    CHECK_GT(K_, 0) << "EmbedLayer input_dim must be positive.";
    bias_term_ = this->param_->embed_param().bias_term();
    // Check if we need to set up the weights
    if (this->blobs_.size() > 0) {
      LOG(INFO) << "Skipping parameter initialization";
    } else {
      if (bias_term_) {
        this->blobs_.resize(2);
      } else {
        this->blobs_.resize(1);
      }
      // Initialize the weights --
      // transposed from InnerProductLayer for spatial locality.
      vector<int> weight_shape(2);
      weight_shape[0] = K_;
      weight_shape[1] = N_;
      this->blobs_[0].reset(new Blob<Dtype>(weight_shape));
      // fill the weights
      SHARED_PTR<Filler<Dtype> > weight_filler(GetFiller<Dtype>(
            this->param_->embed_param().weight_filler()));
      weight_filler->Fill(this->blobs_[0].get());
      // If necessary, initialize and fill the bias term
      if (bias_term_) {
        vector<int> bias_shape(1, N_);
        this->blobs_[1].reset(new Blob<Dtype>(bias_shape));
        SHARED_PTR<Filler<Dtype> > bias_filler(GetFiller<Dtype>(
            this->param_->embed_param().bias_filler()));
        bias_filler->Fill(this->blobs_[1].get());
      }
    }  // parameter initialization
    this->param_propagate_down_.resize(this->blobs_.size(), true);
  }

  template <typename Dtype>
  void EmbedLayer<Dtype>::Reshape(const vector<Blob<Dtype>*> & bottom,
                                  const vector<Blob<Dtype>*> & top)
  {
    // Figure out the dimensions
    M_ = bottom[0]->count();
    vector<int> top_shape = bottom[0]->shape();
    top_shape.push_back(N_);
    top[0]->Reshape(top_shape);
    // Set up the bias multiplier
    if (bias_term_) {
      vector<int> bias_shape(1, M_);
      bias_multiplier_.Reshape(bias_shape);
      caffe_set(M_, Dtype(1), bias_multiplier_.mutable_data<Context>());
    }
  }

  template <typename Dtype>
  void EmbedLayer<Dtype>::Forward(CPUContext* context, const vector<Blob<Dtype>*> & bottom,
                                      const vector<Blob<Dtype>*> & top)
  {
    const Dtype* bottom_data = bottom[0]->data<Context>();
    const Dtype* weight = this->blobs_[0]->data<Context>();
    Dtype* top_data = top[0]->mutable_data<Context>();
    int index;
    for (int n = 0; n < M_; ++n) {
      index = static_cast<int>(bottom_data[n]);
      DCHECK_GE(index, 0);
      DCHECK_LT(index, K_);
      DCHECK_EQ(static_cast<Dtype>(index), bottom_data[n]) << "non-integer input";
      caffe_copy(N_, weight + index * N_, top_data + n * N_);
    }
    if (bias_term_) {
      const Dtype* bias = this->blobs_[1]->data<Context>();
      caffe_gemm<Dtype>(CblasNoTrans, CblasNoTrans, M_, N_, 1, Dtype(1),
                            bias_multiplier_.data<Context>(), bias, Dtype(1), top_data);
    }
  }

  template <typename Dtype>
  void EmbedLayer<Dtype>::Backward(CPUContext* context, const vector<Blob<Dtype>*> & top,
                                       const vector<Blob<Dtype>*> & bottom)
  {
    CHECK(!top[0]->propagate_down_) << "Can't backpropagate to EmbedLayer input.";
    if (this->blobs_[0]->propagate_down_) {
      const Dtype* top_diff = top[0]->diff<Context>();
      const Dtype* bottom_data = bottom[0]->data<Context>();
      // Gradient with respect to weight
      Dtype* weight_diff = this->blobs_[0]->mutable_diff<Context>();
      int index;
      for (int n = 0; n < M_; ++n) {
        index = static_cast<int>(bottom_data[n]);
        DCHECK_GE(index, 0);
        DCHECK_LT(index, K_);
        DCHECK_EQ(static_cast<Dtype>(index), bottom_data[n])
            << "non-integer input";
        caffe_axpy(N_, Dtype(1), top_diff + n * N_, weight_diff + index * N_);
      }
    }
    if (bias_term_ && this->blobs_[1]->propagate_down_) {
      const Dtype* top_diff = top[0]->diff<Context>();
      Dtype* bias_diff = this->blobs_[1]->mutable_diff<Context>();
      caffe_gemv<Dtype>(CblasTrans, M_, N_, Dtype(1), top_diff,
                            bias_multiplier_.data<Context>(), Dtype(1), bias_diff);
    }
  }

#ifdef CPU_ONLY
  STUB_GPU(EmbedLayer);
#endif

  INSTANTIATE_CLASS(EmbedLayer);
  REGISTER_LAYER_CLASS(Embed);

}  // namespace
