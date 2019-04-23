/**
 * MIT License
 *
 * Copyright (c) 2019 Yoram
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef RENDOR_INCLUDE_CG_COMMON_INFOIMPORTER_H_
#define RENDOR_INCLUDE_CG_COMMON_INFOIMPORTER_H_

#include <string>
#include <future>
#include "cg/Mesh.h"

namespace cg {

class MeshInfo {
 private:
  std::vector<cg::Vertex> vertices_;
  std::vector<unsigned int> indices_;

 public:
  MeshInfo() {}

  MeshInfo(std::vector<cg::Vertex> vertices,
           std::vector<unsigned int> indices) : vertices_(vertices), indices_(indices) {}

  ~MeshInfo() {}

  const std::vector<cg::Vertex> Vertices() const { return vertices_; }
  const std::vector<unsigned int> Indices() const { return indices_; }
  size_t Simplify(size_t reduction, float error) {
    std::vector<unsigned int> simplified(indices_.size());
    size_t before = indices_.size();
    simplified.resize(meshopt_simplify(&simplified[0], &indices_[0], indices_.size(), &vertices_[0].position.x, vertices_.size(),
                                       sizeof(cg::Vertex), indices_.size() - reduction, error));
    indices_ = simplified;
    size_t opt = before - simplified.size();
    return opt;
  }
};

struct OptimizationStats {
  float acmr_before;
  float acmr_after;
  float atvr_before;
  float atvr_after;
  float overdraw_before;
  float overdraw_after;
  float overfetch_before;
  float overfetch_after;
  unsigned int indices_before;
  unsigned int indices_after;
};

class AsyncInfoImporter {
 private:
  std::future<cg::MeshInfo> future_;
  bool ready_ = false;
  float progress = 0.0f;
  OptimizationStats stats_;

 public:
  AsyncInfoImporter() = default;
  ~AsyncInfoImporter() {}

  void LoadAsync(const std::string &file);

  bool IsReady();

  bool IsBusy();

  cg::MeshInfo Get();

  OptimizationStats GetOptimizationStats() { return stats_; }

  float GetProgress() {
    return progress;
  }

 private:
  class Handler : public Assimp::ProgressHandler {
   private:
    AsyncInfoImporter *async_info_importer_;

   public:
    Handler(AsyncInfoImporter *i) : async_info_importer_(i) {}

    bool Update(float percentage) override {
      async_info_importer_->progress = percentage;
      return percentage==1.0f;
    }
  };

  static cg::MeshInfo ImportMeshInfo(AsyncInfoImporter *importer, const std::string &file, unsigned int index);
};

}

#endif //RENDOR_INCLUDE_CG_COMMON_INFOIMPORTER_H_
