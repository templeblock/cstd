#ifndef CAFFE_INPUT_LAYER_HPP_
#define CAFFE_INPUT_LAYER_HPP_







namespace
{

  /**
   * @brief Provides data to the Net by assigning tops directly.
   *
   * This data layer is a container that merely holds the data assigned to it;
   * forward, backward, and reshape are all no-ops.
   */
  template <typename Dtype>
  class InputLayer : public Layer<Dtype>
  {
  public:
    explicit InputLayer()
      : Layer<Dtype>() {}
    virtual void LayerSetUp(const vector<Blob<Dtype>*> & bottom,
                            const vector<Blob<Dtype>*> & top);
    // Data layers should be shared by multiple solvers in parallel
    virtual inline bool ShareInParallel() const { return true; }
    // Data layers have no bottoms, so reshaping is trivial.
    virtual void Reshape(const vector<Blob<Dtype>*> & bottom,
                         const vector<Blob<Dtype>*> & top) {}

    virtual inline const char* type() const { return "Input"; }
    virtual inline int ExactNumBottomBlobs() const { return 0; }
    virtual inline int MinTopBlobs() const { return 1; }

  public:
    virtual void Forward(CPUContext* context, const vector<Blob<Dtype>*> & bottom,
                             const vector<Blob<Dtype>*> & top) {}
    virtual void Backward(CPUContext* context, const vector<Blob<Dtype>*> & top,
                              const vector<Blob<Dtype>*> & bottom) {}
  };

}  // namespace

#endif  // CAFFE_INPUT_LAYER_HPP_
