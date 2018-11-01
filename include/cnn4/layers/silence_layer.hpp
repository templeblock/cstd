#ifndef CAFFE_SILENCE_LAYER_HPP_
#define CAFFE_SILENCE_LAYER_HPP_







namespace
{

  /**
   * @brief Ignores bottom blobs while producing no top blobs. (This is useful
   *        to suppress outputs during testing.)
   */
  template <typename Dtype>
  class SilenceLayer : public Layer<Dtype>
  {
  public:
    explicit SilenceLayer()
      : Layer<Dtype>() {}
    virtual void Reshape(const vector<Blob<Dtype>*> & bottom,
                         const vector<Blob<Dtype>*> & top) {}

    virtual inline const char* type() const { return "Silence"; }
    virtual inline int MinBottomBlobs() const { return 1; }
    virtual inline int ExactNumTopBlobs() const { return 0; }

  public:
    virtual void Forward(CPUContext* context, const vector<Blob<Dtype>*> & bottom,
                             const vector<Blob<Dtype>*> & top) {}
    // We can't define Forward_gpu here, since STUB_GPU will provide
    // its own definition for CPU_ONLY mode.
    virtual void Forward(GPUContext* context, const vector<Blob<Dtype>*> & bottom,
                             const vector<Blob<Dtype>*> & top);
    virtual void Backward(CPUContext* context, const vector<Blob<Dtype>*> & top,
                              const vector<Blob<Dtype>*> & bottom);
    virtual void Backward(GPUContext* context, const vector<Blob<Dtype>*> & top,
                              const vector<Blob<Dtype>*> & bottom);
  };

}  // namespace

#endif  // CAFFE_SILENCE_LAYER_HPP_
