


#include "caffe/common.hpp"
#include "caffe/filler.hpp"

#include "caffe/layers/recurrent_layer.hpp"


namespace {

template <typename Dtype>
void RecurrentLayer::Forward(GPUContext* context, const vector<Blob*>& bottom,
    const vector<Blob*>& top) {
  // Hacky fix for test time... reshare all the shared blobs.
  // TODO: somehow make this work non-hackily.
  if (this->phase_ == TEST) {
    unrolled_net_->ShareWeights();
  }

  DCHECK_EQ(recur_input_blobs_.size(), recur_output_blobs_.size());
  if (!expose_hidden_) {
    for (int i = 0; i < recur_input_blobs_.size(); ++i) {
      const int count = recur_input_blobs_[i]->count();
      DCHECK_EQ(count, recur_output_blobs_[i]->count());
      const Dtype* timestep_T_data = recur_output_blobs_[i]->data();
      Dtype* timestep_0_data = recur_input_blobs_[i]->mdata();
      caffe_copy(count, timestep_T_data, timestep_0_data);
    }
  }

  if (bottom.size() < 2)
  {
	  Dtype* pcont = cont_input_blob_->mdata();
	  for (int i = 0; i<T_; i++)
	  {
      Dtype cont = Dtype(i == 0 ? 0 : 1);
		  Dtype* pconti = pcont + N_*i;
		  for (int j = 0; j<N_; j++)
		  {
			  pconti[j] = cont;
		  }
	  }
  }

  unrolled_net_->ForwardTo(last_layer_index_);

  if (expose_hidden_) {
    const int top_offset = (int)output_blobs_.size();
    for (int i = top_offset, j = 0; i < top.size(); ++i, ++j) {
      top[i]->ShareData(*recur_output_blobs_[j]);
    }
  }
}

INSTANTIATE_LAYER_GPU_FORWARD(RecurrentLayer);

}  // namespace
