#include <Eigen/Core>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <deque>

namespace py = pybind11;

// ----------------------------------------------------------

/**
 * blending src image to dist image
 * @param dist
 * @param src
 * @param src_mask
 * @return
 */
Eigen::MatrixX<double> CppPoissonBlending(
    const Eigen::MatrixX<unsigned char>& dist,
    const Eigen::MatrixX<unsigned char>& src,
    const Eigen::MatrixX<unsigned char>& src_mask,
    const std::array<int,2> offset,
    unsigned int num_iteration){
  assert(src.cols() == src_mask.cols() );
  assert(src.cols() == src_mask.cols() );
  Eigen::MatrixX<double> ret = dist.cast<double>();
  for(unsigned int itr=0;itr<num_iteration;++itr) {
    for (unsigned int src_i = 1; src_i < src.rows() - 1; ++src_i) {
      for (unsigned int src_j = 1; src_j < src.cols() - 1; ++src_j) {
        if (src_mask(src_i, src_j) == 0) { continue; }
        unsigned int ret_i = src_i + offset[0];
        unsigned int ret_j = src_j + offset[1];
        if (ret_i <= 0 || ret_i >= dist.rows() - 1) { continue; }
        if (ret_j <= 0 || ret_j >= dist.cols() - 1) { continue; }
        const double src_n = src(src_i, src_j+1); // north
        const double src_s = src(src_i, src_j-1); // south
        const double src_e = src(src_i+1, src_j); // east
        const double src_w = src(src_i-1, src_j); // west
        const double src_c = src(src_i, src_j); // center
        const double ret_n = ret(ret_i, ret_j+1);
        const double ret_s = ret(ret_i, ret_j-1);
        const double ret_e = ret(ret_i+1, ret_j);
        const double ret_w = ret(ret_i-1, ret_j);
        // write some code here to implement Poisson image editing
        double ret_c = src_c; // change this line
        ret_c = (ret_c>255.) ? 255. : ret_c;
        ret_c = (ret_c<0.) ? 0. : ret_c;
        ret(ret_i, ret_j) = ret_c;
      }
    }
  }
  return ret;
}

PYBIND11_MODULE(cppmodule, m) {
  m.doc() = "cppmodule";
  m.def("poisson_blending",
        &CppPoissonBlending,
        "A function to blend image using Poisson Image Editing",
        py::arg("dist"), py::arg("src"), py::arg("src_mask"),
        py::arg("offset"), py::arg("num_iteration"));
}


