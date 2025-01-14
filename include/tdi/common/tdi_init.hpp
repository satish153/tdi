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
/** @file tdi_init.hpp
 *
 *  @brief Contains TDI Dev Manager APIs. These APIs help manage TdiInfo
 *  \n objects with respect to the devices and programs in the ecosystem.
 *
 *  \n Contains DevMgr, Device, Target, Flags
 */
#ifndef _TDI_INIT_HPP_
#define _TDI_INIT_HPP_

#include <functional>
#include <memory>
#include <mutex>

// tdi includes
#include <tdi/common/tdi_target.hpp>
#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_session.hpp>

namespace tdi {

/**
 * @brief Class which encapsulates static info of a device eg.
 * Arch type,
 * Mgrs it was started with, State information, TdiInfo of all
 * programs, Pipeline profile information.
 *
 * Static info means that none of this can be changed after
 * device add happens.
 */
class Device {
  public:
   Device(const tdi_dev_id_t &device_id,
          const tdi_arch_type_e &arch_type,
          std::vector<std::unique_ptr<tdi::ProgramConfig>> device_config,
          const std::vector<tdi_mgr_type_e> mgr_type_list)
       : device_id_(device_id),
         arch_type_(arch_type),
         device_config_(std::move(device_config)),
         mgr_type_list_(mgr_type_list){};

   /**
    * @brief Get the TdiInfo object corresponding to the program name
    *
    * @param[in] prog_name Name of the P4 program
    * @param[out] tdi_info TdiInfo Obj associated with the Device
    *    and the program name
    *
    * @return Status of the API call
    */
   tdi_status_t
   tdiInfoGet(const std::string &prog_name, const TdiInfo **tdi_info) const;

   /**
    * @brief Get a vector of loaded p4 program names on this device
    *
    * @param[out] p4_names Vector containing const string references to the
    * P4 names loaded on the device
    *
    * @return Status of the API call
    */
   tdi_status_t p4NamesGet(
       std::vector<std::reference_wrapper<const std::string>> &p4_names) const;

   tdi_status_t deviceConfigGet(
       const std::vector<std::unique_ptr<tdi::ProgramConfig>> **device_config)
       const;

   virtual tdi_status_t createSession(std::shared_ptr<tdi::Session> *session) const;
   virtual tdi_status_t createTarget(std::unique_ptr<tdi::Target> *target) const;
   virtual tdi_status_t createFlags(const uint64_t &flags_val,
                            std::unique_ptr<tdi::Flags> *flags) const;

  protected:
   tdi_dev_id_t device_id_;
  private:
   tdi_arch_type_e arch_type_;
   std::vector<std::unique_ptr<tdi::ProgramConfig>> device_config_;
   std::vector<tdi_mgr_type_e> mgr_type_list_;
   std::map<std::string, std::unique_ptr<TdiInfo>> tdi_info_map_;
   //std::map<std::string, std::shared_ptr<DeviceState>> tdi_dev_state_map_;
};


/**
 * @brief Class to manage Device per dev_id.<br>
 * <B>Creation: </B> Singleton....
 */
class DevMgr {
 public:
  /**
   * @brief Get the singleton ojbect
   *
   * @return Ref to Singleton DevMgr object
   */
  static DevMgr &getInstance();

  /**
   * @brief Get the TdiInfo object corresponding to the (device_id,
   * program name)
   *
   * @param[in] dev_id Device ID
   * @param[out] Device Device object
   *
   * @return Status of the API call
   */
  tdi_status_t deviceGet(const tdi_dev_id_t &dev_id,
                          const tdi::Device **device) const;

  /**
   * @brief Get a list of all device IDs currently added
   *
   * @param[out] device_id_list Set Contains list of device
   * IDs that are present.
   *
   * @return Status of the API call
   */
  tdi_status_t deviceIdListGet(std::set<tdi_dev_id_t> *device_id_list) const;

  /**
   * @brief Device Add function which creates a Device object and maintains it
   *
   * @param[in] device_id 
   * @param[in] cookie User defined cookie which platforms can use to
   * send any additional information they want to help with inititalization
   *
   * @return Status of API call
   */
  virtual tdi_status_t deviceAdd(
      const tdi_dev_id_t &device_id,
      const tdi_arch_type_e &arch_type,
      std::vector<std::unique_ptr<tdi::ProgramConfig>> &device_config,
      void *cookie);

  virtual tdi_status_t deviceRemove(const tdi_dev_id_t &device_id);

  DevMgr(DevMgr const &) = delete;
  DevMgr(DevMgr &&) = delete;
  DevMgr &operator=(const DevMgr &) = delete;
  DevMgr &operator=(DevMgr &&) = delete;

 protected:
  std::map<tdi_dev_id_t, std::unique_ptr<Device>> dev_map_;

 private:
  DevMgr(){};

  static std::mutex dev_mgr_instance_mutex;
  static DevMgr *dev_mgr_instance;
};  // DevMgr


/**
 * @brief Class to manage initialization of TDI <br>
 * <B>Creation: </B> Cannot be created
 */
class Init {
 public:
  /**
   * @brief TDI Module Init API. This function needs to be called to
   * initialize TDI. Some specific managers can be specified to be skipped
   * TDI initialization. This allows TDI session layer to not know about these
   * managers. By default, no mgr initialization is skipped if empty vector is
   * passed
   *
   * @param[in] mgr_type_list vector of mgrs to skip initializing. If
   * empty, don't skip anything
   * @return Status of the API call
   */
  static tdi_status_t tdiModuleInit(const std::vector<tdi_mgr_type_e> mgr_type_list);
};  // Init

}  // tdi

#endif  // _TDI_INIT_HPP_
