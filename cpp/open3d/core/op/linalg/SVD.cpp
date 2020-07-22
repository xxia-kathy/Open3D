// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include "open3d/core/op/linalg/SVD.h"
#include <unordered_map>

namespace open3d {
namespace core {

void SVD(const Tensor &A, Tensor &U, Tensor &S, Tensor &VT) {
    // Check devices
    Device device = A.GetDevice();

    // Check dtypes
    Dtype dtype = A.GetDtype();
    if (dtype != Dtype::Float32 && dtype != Dtype::Float64) {
        utility::LogError(
                "Only tensors with Float32 or Float64 are supported, but "
                "received {}",
                DtypeUtil::ToString(dtype));
    }

    // Check dimensions
    SizeVector A_shape = A.GetShape();
    if (A_shape.size() != 2) {
        utility::LogError("Tensor A must be 2D, but got {}D", A_shape.size());
    }

    int m = A_shape[0], n = A_shape[1];
    U = Tensor::Empty({m, m}, dtype, device);
    S = Tensor::Empty({n}, dtype, device);
    VT = Tensor::Empty({n, n}, dtype, device);
    Tensor superb = Tensor::Empty({std::min(m, n) - 1}, dtype, device);

    const void *A_data = A.GetDataPtr();
    void *U_data = U.GetDataPtr();
    void *S_data = S.GetDataPtr();
    void *VT_data = VT.GetDataPtr();
    void *superb_data = superb.GetDataPtr();

    if (device.GetType() == Device::DeviceType::CUDA) {
#ifdef BUILD_CUDA_MODULE
        SVDCUDA(dtype, A_data, U_data, S_data, VT_data, superb_data, m, n);
#else
        utility::LogError("Unimplemented device.");
#endif
    } else {
        SVDCPU(dtype, A_data, U_data, S_data, VT_data, superb_data, m, n);
    }
}
}  // namespace core
}  // namespace open3d
