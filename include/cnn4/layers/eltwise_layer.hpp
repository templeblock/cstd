#ifndef CAFFE_ELTWISE_LAYER_HPP_
#define CAFFE_ELTWISE_LAYER_HPP_







namespace
{

  /**
   * @brief Compute elementwise operations, such as product and sum,
   *        along multiple input Blobs.
   *
   * TODO(dox): thorough documentation for Forward, Backward, and proto params.
   */
  template <typename Dtype>
  class EltwiseLayer : public Layer<Dtype>
  {
  public:
    explicit EltwiseLayer()
      : Layer<Dtype>() {}
    virtual void LayerSetUp(const vector<Blob<Dtype>*> & bottom,
                            const vector<Blob<Dtype>*> & top);
    virtual void Reshape(const vector<Blob<Dtype>*> & bottom,
                         const vector<Blob<Dtype>*> & top);

    virtual inline const char* type() const { return "Eltwise"; }
    virtual inline int MinBottomBlobs() const { return 2; }
    virtual inline int ExactNumTopBlobs() const { return 1; }

  public:
    virtual void Forward(CPUContext* context, const vector<Blob<Dtype>*> & bottom,
                             const vector<Blob<Dtype>*> & top);
    virtual void Forward(GPUContext* context, const vector<Blob<Dtype>*> & bottom,
                             const vector<Blob<Dtype>*> & top);
    virtual void Backward(CPUContext* context, const vector<Blob<Dtype>*> & top,
                              const vector<Blob<Dtype>*> & bottom);
    virtual void Backward(GPUContext* context, const vector<Blob<Dtype>*> & top,
                              const vector<Blob<Dtype>*> & bottom);

    EltwiseParameter_EltwiseOp op_;
    vector<Dtype> coeffs_;
    Blob<int> max_idx_;

    bool stable_prod_grad_;
  };

}  // namespace

#endif  // CAFFE_ELTWISE_LAYER_HPP_
