#ifndef CAFFE_FLATTEN_LAYER_HPP_
#define CAFFE_FLATTEN_LAYER_HPP_







namespace
{

  /**
   * @brief Reshapes the input Blob into flat vectors.
   *
   * Note: because this layer does not change the input values -- merely the
   * dimensions -- it can simply copy the input. The copy happens "virtually"
   * (thus taking effectively 0 real time) by setting, in Forward, the data
   * pointer of the top Blob to that of the bottom Blob (see Blob::ShareData),
   * and in Backward, the diff pointer of the bottom Blob to that of the top Blob
   * (see Blob::ShareDiff).
   */
  template <typename Dtype>
  class FlattenLayer : public Layer<Dtype>
  {
  public:
    explicit FlattenLayer()
      : Layer<Dtype>() {}
    virtual void Reshape(const vector<Blob<Dtype>*> & bottom,
                         const vector<Blob<Dtype>*> & top);

    virtual inline const char* type() const { return "Flatten"; }
    virtual inline int ExactNumBottomBlobs() const { return 1; }
    virtual inline int ExactNumTopBlobs() const { return 1; }

  public:
    /**
     * @param bottom input Blob vector (length 2+)
     *   -# @f$ (N \times C \times H \times W) @f$
     *      the inputs
     * @param top output Blob vector (length 1)
     *   -# @f$ (N \times CHW \times 1 \times 1) @f$
     *      the outputs -- i.e., the (virtually) copied, flattened inputs
     */
    virtual void Forward(CPUContext* context, const vector<Blob<Dtype>*> & bottom,
                             const vector<Blob<Dtype>*> & top);

    /**
     * @brief Computes the error gradient w.r.t. the concatenate inputs.
     *
     * @param top output Blob vector (length 1), providing the error gradient with
     *        respect to the outputs
     * @param propagate_down see Layer::Backward.
     * @param bottom input Blob vector (length K), into which the top error
     *        gradient is (virtually) copied
     */
    virtual void Backward(CPUContext* context, const vector<Blob<Dtype>*> & top,
                              const vector<Blob<Dtype>*> & bottom);
  };

}  // namespace

#endif  // CAFFE_FLATTEN_LAYER_HPP_
