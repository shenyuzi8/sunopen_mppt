#pragma once

#include "esphome/core/component.h"
#include "esphome/components/ble_client/ble_client.h"
#include <vector>

namespace esphome {
namespace sunopen_mppt {

class SunopenMPPT : public ble_client::BLEClientNode, public Component {
 public:
  void setup() override;
  void loop() override;
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;

  void set_throttle(uint32_t throttle) { this->throttle_ = throttle; }

 protected:
  void enable_notify_();
  void poll_data_();
  void write_modbus_command_(const std::vector<uint8_t> &data);

  uint16_t tx_handle_{0};
  uint16_t rx_handle_{0};
  uint16_t service_handle_{0};
  bool ready_{false};
  uint32_t last_poll_{0};
  uint32_t throttle_{5000};

  static constexpr uint16_t SERVICE_UUID = 0xFFE0;
  static constexpr uint16_t TX_CHAR_UUID = 0xFFE1;
  static constexpr uint16_t RX_CHAR_UUID = 0xFFE2;
};

}  // namespace sunopen_mppt
}  // namespace esphome
