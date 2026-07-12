#pragma once

#include "esphome/core/component.h"
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/modbus/modbus.h"

namespace esphome {
namespace sunopen_mppt {

class SunopenMPPT : public ble_client::BLEClientNode, public Component {
 public:
  void setup() override;
  void loop() override;
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;
  
  void set_modbus(modbus::Modbus *modbus) { this->modbus_ = modbus; }
  
 protected:
  void enable_notify_();
  void poll_data_();
  void write_modbus_command_(const std::vector<uint8_t> &data);
  
  modbus::Modbus *modbus_{nullptr};
  uint16_t tx_handle_{0};
  uint16_t rx_handle_{0};
  uint16_t cccd_handle_{0};
  uint16_t service_handle_{0};
  bool ready_{false};
  uint32_t last_poll_{0};
  static constexpr uint32_t POLL_INTERVAL = 5000;  // 5秒轮询
};

}  // namespace sunopen_mppt
}  // namespace esphome