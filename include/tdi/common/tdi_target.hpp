/*
 * Copyright(c) 2021 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this software except as stipulated in the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/** @file tdi_target.hpp
 *
 *  @brief Contains common cpp defines like Target, Flags
 */
#ifndef _TDI_TARGET_HPP_
#define _TDI_TARGET_HPP_

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

// tdi includes
#include <tdi/common/tdi_defs.h>

namespace tdi {

class Device;

/**
 * @brief Contains pipeline Info of a basic TDI device.
 */
class P4Pipeline {
 public:
  P4Pipeline(const std::string &name,
             const std::string &binary_path,
             const std::vector<std::string> &pipeline_info_file_paths,
             const std::vector<uint32_t> &scope_vec)
      : name_(name),
        binary_path_(binary_path),
        pipeline_info_file_paths_(pipeline_info_file_paths),
        scope_vec_(scope_vec){};
  const std::string name_;
  const std::string binary_path_;
  const std::string context_path_;
  const std::vector<std::string> pipeline_info_file_paths_;
  const std::vector<uint32_t> scope_vec_;
};

/**
 * @brief This class contains Program configuration information
 * like program name, TDI json path and information regarding
 * all the pipeline_profiles of this Program. Maintained in 
 * tdi::Device
 */
class ProgramConfig {
 public:
  ProgramConfig(const std::string &prog_name,
                const std::vector<std::string> &tdi_info_file_paths,
                std::vector<std::unique_ptr<tdi::P4Pipeline>> p4_pipelines)
      : prog_name_(prog_name),
        tdi_info_file_paths_(tdi_info_file_paths),
        p4_pipelines_(std::move(p4_pipelines)){};
  const std::string prog_name_;
  const std::vector<std::string> tdi_info_file_paths_;
  const std::vector<std::unique_ptr<tdi::P4Pipeline>> p4_pipelines_;
};


enum tdi_target_core_enum_e {
  TDI_TARGET_DEV_ID = TDI_TARGET_CORE,
};
/**
 * @brief Can be constructed by \ref tdi::Device::createTarget()
 */
class Target {
 public:
  virtual ~Target() = default;
  virtual tdi_status_t setValue(const tdi_target_e &target, const uint32_t &value);
  virtual tdi_status_t getValue(const tdi_target_e &target, uint32_t *value);

 protected:
  Target(const tdi_dev_id_t &dev_id) : dev_id_(dev_id){};
 private:
  friend class tdi::Device;
  tdi_dev_id_t dev_id_;
};

/**
 * @brief Can be constructed by \ref tdi::Device::createFlags()
 */
class Flags {
  public:
   virtual ~Flags() = default;
   tdi_status_t setValue(const tdi_flags_e &flags, const bool &val);
   tdi_status_t getValue(const tdi_flags_e &flags, bool *val);
   const uint64_t &getFlags() { return flags_; };

  private:
   friend class Device;
   Flags(const uint64_t &flags) : flags_(flags){};
   uint64_t flags_ = 0;
};

}  // tdi

#endif  // _TDI_DEFS_HPP_
