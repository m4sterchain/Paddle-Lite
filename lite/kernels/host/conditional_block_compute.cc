// Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "lite/kernels/host/conditional_block_compute.h"

namespace paddle {
namespace lite {
namespace kernels {
namespace host {

void ConditionalBlockCompute::PrepareForRun() {
  auto& param = this->Param<param_t>();
  program_.reset(new RuntimeProgram(
      param.program_desc, param.exec_scope, param.block_idx));
}

void ConditionalBlockCompute::Run() {
  auto& param = this->Param<param_t>();
  bool need_run = true;
  if (param.is_scalar_condition) {
    auto* cond = param.cond;
    auto* cond_data = cond->data<bool>();
    need_run = cond_data[0];
  } else {
    for (auto input : param.inputs) {
      if (input == nullptr || !input->IsInitialized() ||
          input->dims().empty()) {
        need_run = false;
        break;
      }
    }
  }
  if (need_run) {
    program_->Run();
  }
}

}  // namespace host
}  // namespace kernels
}  // namespace lite
}  // namespace paddle

REGISTER_LITE_KERNEL(conditional_block,
                     kHost,
                     kAny,
                     kAny,
                     paddle::lite::kernels::host::ConditionalBlockCompute,
                     def)
    .BindInput("Input",
               {LiteType::GetTensorTy(TARGET(kHost),
                                      PRECISION(kAny),
                                      DATALAYOUT(kAny))})
    .BindInput("Cond",
               {LiteType::GetTensorTy(TARGET(kHost),
                                      PRECISION(kBool),
                                      DATALAYOUT(kAny))})
    .BindOutput("Out",
                {LiteType::GetTensorTy(TARGET(kHost),
                                       PRECISION(kAny),
                                       DATALAYOUT(kAny))})
    .BindOutput("Scope",
                {LiteType::GetTensorTy(TARGET(kHost),
                                       PRECISION(kAny),
                                       DATALAYOUT(kAny))})
    .Finalize();
