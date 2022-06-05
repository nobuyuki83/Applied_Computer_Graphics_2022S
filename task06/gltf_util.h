//
// Created by Nobuyuki Umetani on 2022/06/05.
//

#ifndef GLTF_UTIL_H_
#define GLTF_UTIL_H_

#include <iostream>
#include <Eigen/Core>
#include <Eigen/LU>  // for inverse()
#include <Eigen/Geometry> // for quaternion

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"

bool GetArray_UInt(
    std::vector<unsigned int> &res,
    const tinygltf::Model &model,
    int iacc) {
  const tinygltf::Accessor &acc = model.accessors[iacc];
  const int ibv = acc.bufferView;
  const tinygltf::BufferView &bv = model.bufferViews[ibv];
  const int ibuff = bv.buffer;
  const tinygltf::Buffer &buff = model.buffers[ibuff];
  const size_t ncnt = acc.count;
  unsigned int nelem = 0;
  if (acc.type == TINYGLTF_TYPE_SCALAR) { nelem = 1; }
  else if (acc.type == TINYGLTF_TYPE_VEC3) { nelem = 3; }
  else if (acc.type == TINYGLTF_TYPE_VEC4) { nelem = 4; }
  else {
    std::cerr << "Error!->unknown type: " << acc.type << std::endl;
    assert(0);
    abort();
  }
  size_t ntot = ncnt * nelem;
  if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) { // unsigned short
    if (bv.byteStride != 0 && bv.byteStride != nelem * sizeof(unsigned short)) {
      std::cerr << "Error!-->unsuppoted not packed" << std::endl;
      assert(0);
      abort();
    }
    assert(bv.byteLength >= ntot * sizeof(unsigned short));
    const unsigned short *pdata = (unsigned short *) (buff.data.data() + bv.byteOffset + acc.byteOffset);
    res.assign(pdata, pdata + ntot);
    return true;
  } else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
    if (bv.byteStride != 0 && bv.byteStride != nelem * sizeof(unsigned char)) {
      std::cerr << "Error!-->unsuppoted not packed" << std::endl;
      assert(0);
      abort();
    }
    assert(bv.byteLength >= ntot * sizeof(unsigned char));
    const unsigned char *pdata = (unsigned char *) (buff.data.data() + bv.byteOffset + acc.byteOffset);
    res.assign(pdata, pdata + ntot);  // cast to unsingned int
  } else {
    std::cerr << "unknown accessor type: " << acc.componentType << std::endl;
    assert(0);
    abort();
  }
  return false;
}

bool GetArray_Double(
    std::vector<double> &res,
    const tinygltf::Model &model,
    int iacc) {
  const tinygltf::Accessor &acc = model.accessors[iacc];
  const int ibv = acc.bufferView;
  const tinygltf::BufferView &bv = model.bufferViews[ibv];
  const int ibuff = bv.buffer;
  const tinygltf::Buffer &buff = model.buffers[ibuff];
  const size_t ncnt = acc.count;
  unsigned int nelem = 0;
  if (acc.type == TINYGLTF_TYPE_SCALAR) { nelem = 1; }
  else if (acc.type == TINYGLTF_TYPE_VEC3) { nelem = 3; }
  else if (acc.type == TINYGLTF_TYPE_VEC4) { nelem = 4; }
  else if (acc.type == TINYGLTF_TYPE_MAT4) { nelem = 16; }
  else {
    std::cout << "Error!->unknown type: " << acc.type << std::endl;
    assert(0);
    abort();
  }
  size_t ntot = ncnt * nelem;
  if (acc.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) { // signed short
    if (bv.byteStride != 0 && bv.byteStride != nelem * sizeof(float)) {
      std::cout << "Error!-->unsuppoted not packed" << std::endl;
      assert(0);
      abort();
    }
    assert(bv.byteLength >= ntot * sizeof(float));
    const float *pdata = (float *) (buff.data.data() + bv.byteOffset + acc.byteOffset);
    res.assign(pdata, pdata + ntot);
    return true;
  }
  return false;
}

void LoadMeshInfo(
    std::vector<double> &aXYZ,
    std::vector<unsigned int> &aTri,
    std::vector<double> &aRigWeight,
    std::vector<unsigned int> &aRigJoint,
    const tinygltf::Model &model,
    int imsh,
    int iprimitive) {
  aXYZ.clear();
  aTri.clear();
  aRigJoint.clear();
  aRigWeight.clear();
  //
  const tinygltf::Primitive &primitive = model.meshes[imsh].primitives[iprimitive];
  GetArray_UInt(
      aTri,
      model, primitive.indices);
  {
    auto itr = primitive.attributes.find(std::string("POSITION"));
    if (itr != primitive.attributes.end()) {
      GetArray_Double(aXYZ,
                      model, itr->second);
    }
  }
  {
    auto itr = primitive.attributes.find(std::string("NORMAL"));
    if (itr != primitive.attributes.end()) {
    }
  }
  {
    auto itr = primitive.attributes.find(std::string("WEIGHTS_0"));
    if (itr != primitive.attributes.end()) {
      GetArray_Double(aRigWeight,
                      model, itr->second);
    }
  }
  {
    auto itr = primitive.attributes.find(std::string("JOINTS_0"));
    if (itr != primitive.attributes.end()) {
      GetArray_UInt(aRigJoint,
                    model, itr->second);
      std::cout << "has rig joint: " << aRigJoint.size() / 4 << std::endl;
    }
  }
}



#endif //GLTF_UTIL_H_
