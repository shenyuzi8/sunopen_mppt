#pragma once

#include "esphome/core/component.h"
#include "esphome/components/ble_client/ble_client.h"
#include <vector>
#include <cstring>

namespace esphome {
namespace sunopen_mppt {

static const uint8_t SERVICE_UUID_128[16] = {
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0xE0, 0xFF, 0x00, 0x00
};

static const uint8_t TX_CHAR_UUID_128[16] = {
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0xE1, 0xFF, 0x00, 0x00
};

static const uint8_t RX_CHAR_UUID_128[16] = {
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0xE2, 0xFF, 0x00, 0x00
};

class SunopenMPPTDevice;

class SunopenMPPT : public ble_client::BLEClientNode, public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;

  void set_throttle(uint32_t throttle) { this->throttle_ = throttle; }
  void register_device(SunopenMPPTDevice *device) { this->devices_.push_back(device); }

  void write_command(const std::vector<uint8_t> &data);
  float get_setup_priority() const override;

 protected:
  void enable_notify_();
  void poll_data_();
  bool parse_modbus_byte_(uint8_t byte);
  void process_data_(const std::vector<uint8_t> &data);

  uint16_t tx_handle_{0};
  uint16_t rx_handle_{0};
  bool ready_{false};
  uint32_t last_poll_{0};
  uint32_t throttle_{5000};
  uint32_t write_lock_until_{0};

  std::vector<uint8_t> rx_buffer_;
  uint32_t last_modbus_byte_{0};
  static constexpr uint32_t RX_TIMEOUT = 100;

  std::vector<SunopenMPPTDevice *> devices_;
};

class SunopenMPPTDevice {
 public:
  void set_parent(SunopenMPPT *parent) { this->parent_ = parent; }
  void set_address(uint8_t address) { this->address_ = address; }
  virtual void on_modbus_data(const std::vector<uint8_t> &data) = 0;

  void write_command(const std::vector<uint8_t> &data) {
    this->parent_->write_command(data);
  }

 protected:
  friend SunopenMPPT;
  SunopenMPPT *parent_{nullptr};
  uint8_t address_{0x01};
};

}  // namespace sunopen_mppt
}  // namespace esphome
