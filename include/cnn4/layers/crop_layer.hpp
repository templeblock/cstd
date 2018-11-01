#ifndef CAFFE_CROP_LAYER_HPP_
#define CAFFE_CROP_LAYER_HPP_

#include <utility>






namespace
{

  /**
   * @brief Takes a Blob and crop it, to the shape specified by the second input
   *  Blob, across all dimensions after the specified axis.
   *
   * TODO(dox): thorough documentation for Forward, Backward, and proto params.
   */

  template <typename Dtype>
  class CropLayer : public Layer<Dtype>
  {
  public:
    explicit CropLayer()
      : Layer<Dtype>() {}
    virtual void LayerSetUp(const vector<Blob<Dtype>*> & bottom,
                            const vector<Blob<Dtype>*> & top);
    virtual void Reshape(const vector<Blob<Dtype>*> & bottom,
                         const vector<Blob<Dtype>*> & top);

    virtual inline const char* type() const { return "Crop"; }
    virtual inline int ExactNumBottomBlobs() const { return 2; }
    virtual inline int ExactNumTopBlobs() const { return 1; }

  public:
    virtual void Forward(CPUContext* context, const vector<Blob<Dtype>*> & bottom,
                             const vector<Blob<Dtype>*> & top);
    virtual void Backward(CPUContext* context, const vector<Blob<Dtype>*> & top,
                              const vector<Blob<Dtype>*> & bottom);
    virtual void Forward(GPUContext* context, const vector<Blob<Dtype>*> & bottom,
                             const vector<Blob<Dtype>*> & top);
    virtual void Backward(GPUContext* context, const vector<Blob<Dtype>*> & top,
                              const vector<Blob<Dtype>*> & bottom);

    vector<int> offsets;

  private:
    // Recursive copy function.
    void crop_copy(const vector<Blob<Dtype>*> & bottom,
                   const vector<Blob<Dtype>*> & top,
                   const vector<int> & offsets,
                   vector<int> indices,
                   int cur_dim,
                   const Dtype* src_data,
                   Dtype* dest_data,
                   bool is_forward);

    // Recursive copy function: this is similar to crop_copy() but loops over all
    // but the last two dimensions to allow for ND cropping while still relying on
    // a CUDA kernel for the innermost two dimensions for performance reasons.  An
    // alterantive implementation could rely on the kernel more by passing
    // offsets, but this is problematic because of its variable length.
    // Since in the standard (N,C,W,H) case N,C are usually not cropped a speedup
    // could be achieved by not looping the application of the copy_kernel around
    // these dimensions.
    void crop_copy_gpu(const vector<Blob<Dtype>*> & bottom,
                       const vector<Blob<Dtype>*> & top,
                       const vector<int> & offsets,
                       vector<int> indices,
                       int cur_dim,
                       const Dtype* src_data,
                       Dtype* dest_data,
                       bool is_forward);
  };
}  // namespace

#endif  // CAFFE_CROP_LAYER_HPP_
