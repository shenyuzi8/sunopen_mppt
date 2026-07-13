#include "sunopen_mppt.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace sunopen_mppt {

static const char *const TAG = "sunopen_mppt";

static const uint8_t FACTORY_CMD[] = {0x01, 0x03, 0x9C, 0x40, 0x00, 0x64, 0x6B, 0xA5};

void SunopenMPPT::setup() {
  ESP_LOGI(TAG, "SunopenMPPT setup complete");
}

void SunopenMPPT::dump_config() {
  ESP_LOGCONFIG(TAG, "SunopenMPPT:");
  ESP_LOGCONFIG(TAG, "  Throttle: %d ms", this->throttle_);
  ESP_LOGCONFIG(TAG, "  Devices: %d", this->devices_.size());
}

float SunopenMPPT::get_setup_priority() const {
  return setup_priority::AFTER_BLUETOOTH;
}

void SunopenMPPT::loop() {
  if (!this->ready_) return;

  uint32_t now = millis();

  if (now - this->last_modbus_byte_ > RX_TIMEOUT && !this->rx_buffer_.empty()) {
    ESP_LOGW(TAG, "RX timeout, clearing buffer (had %d bytes)", this->rx_buffer_.size());
    this->rx_buffer_.clear();
    this->last_modbus_byte_ = now;
  }

  if (now - this->last_poll_ >= this->throttle_) {
    this->last_poll_ = now;
    this->poll_data_();
  }
}

void SunopenMPPT::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                       esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_CONNECT_EVT:
      ESP_LOGI(TAG, "MPPT connected");
      break;

    case ESP_GATTC_DISCONNECT_EVT:
      ESP_LOGI(TAG, "MPPT disconnected");
      this->ready_ = false;
      break;

    case ESP_GATTC_SEARCH_CMPL_EVT: {
      esp_bt_uuid_t svc_uuid;
      svc_uuid.len = ESP_UUID_LEN_128;
      memcpy(svc_uuid.uuid.uuid128, SERVICE_UUID_128, 16);

      esp_bt_uuid_t tx_uuid;
      tx_uuid.len = ESP_UUID_LEN_128;
      memcpy(tx_uuid.uuid.uuid128, TX_CHAR_UUID_128, 16);

      esp_bt_uuid_t rx_uuid;
      rx_uuid.len = ESP_UUID_LEN_128;
      memcpy(rx_uuid.uuid.uuid128, RX_CHAR_UUID_128, 16);

      esp_gattc_service_elem_t service_result;
      uint16_t service_count = 1;

      esp_ble_gattc_get_service(gattc_if, param->search_cmpl.conn_id,
                                &svc_uuid, &service_result, &service_count, 0);

      if (service_count > 0) {
        ESP_LOGI(TAG, "Service found: start=0x%04X, end=0x%04X",
                 service_result.start_handle, service_result.end_handle);

        esp_gattc_char_elem_t tx_result;
        uint16_t tx_count = 1;
        esp_ble_gattc_get_char_by_uuid(gattc_if, param->search_cmpl.conn_id,
                                       service_result.start_handle, service_result.end_handle,
                                       tx_uuid, &tx_result, &tx_count);
        if (tx_count > 0) {
          this->tx_handle_ = tx_result.char_handle;
          ESP_LOGI(TAG, "TX char found: handle=%d", tx_result.char_handle);
        } else {
          ESP_LOGW(TAG, "TX char not found!");
        }

        esp_gattc_char_elem_t rx_result;
        uint16_t rx_count = 1;
        esp_ble_gattc_get_char_by_uuid(gattc_if, param->search_cmpl.conn_id,
                                       service_result.start_handle, service_result.end_handle,
                                       rx_uuid, &rx_result, &rx_count);
        if (rx_count > 0) {
          this->rx_handle_ = rx_result.char_handle;
          ESP_LOGI(TAG, "RX char found: handle=%d", rx_result.char_handle);
        } else {
          ESP_LOGW(TAG, "RX char not found!");
        }

        this->enable_notify_();
      } else {
        ESP_LOGW(TAG, "Service 0xFFE0 not found!");
      }
      break;
    }

    case ESP_GATTC_REG_FOR_NOTIFY_EVT:
      ESP_LOGI(TAG, "Notify registered successfully");
      this->ready_ = true;
      this->last_poll_ = millis();
      break;

    case ESP_GATTC_NOTIFY_EVT:
      ESP_LOGD(TAG, "Notify: %d bytes", param->notify.value_len);
      for (uint16_t i = 0; i < param->notify.value_len; i++) {
        if (!this->parse_modbus_byte_(param->notify.value[i])) {
          this->rx_buffer_.clear();
        }
      }
      break;

    default:
      break;
  }
}

void SunopenMPPT::enable_notify_() {
  if (this->rx_handle_ == 0) return;
  esp_ble_gattc_register_for_notify(
      this->parent()->get_gattc_if(),
      this->parent()->get_remote_bda(),
      this->rx_handle_);
}

void SunopenMPPT::poll_data_() {
  this->write_command(std::vector<uint8_t>(FACTORY_CMD, FACTORY_CMD + sizeof(FACTORY_CMD)));
}

void SunopenMPPT::write_command(const std::vector<uint8_t> &data) {
  if (this->tx_handle_ == 0) return;
  esp_ble_gattc_write_char(
      this->parent()->get_gattc_if(),
      this->parent()->get_conn_id(),
      this->tx_handle_,
      data.size(),
      const_cast<uint8_t*>(data.data()),
      ESP_GATT_WRITE_TYPE_RSP,
      ESP_GATT_AUTH_REQ_NONE);
}

bool SunopenMPPT::parse_modbus_byte_(uint8_t byte) {
  this->rx_buffer_.push_back(byte);
  this->last_modbus_byte_ = millis();

  size_t len = this->rx_buffer_.size();
  if (len < 5) return true;

  uint8_t function = this->rx_buffer_[1];

  if (function == 0x03) {
    uint8_t byte_count = this->rx_buffer_[2];
    size_t expected_len = 3 + byte_count + 2;
    if (len >= expected_len) {
      this->process_data_(this->rx_buffer_);
      return false;
    }
  } else if (function == 0x05 || function == 0x06) {
    if (len >= 8) {
      ESP_LOGD(TAG, "Write response received (func=0x%02X)", function);
      return false;
    }
  } else if (function == 0x10) {
    if (len >= 8) {
      ESP_LOGD(TAG, "Write multiple response received");
      return false;
    }
  } else if (function >= 0x80) {
    if (len >= 5) {
      ESP_LOGW(TAG, "Modbus exception: func=0x%02X, code=%d", function, this->rx_buffer_[2]);
      return false;
    }
  } else {
    ESP_LOGW(TAG, "Unexpected function code: 0x%02X", function);
    return false;
  }

  if (len > 300) return false;
  return true;
}

void SunopenMPPT::process_data_(const std::vector<uint8_t> &data) {
  uint16_t computed = crc16(data.data(), data.size() - 2);
  uint16_t received = (data[data.size() - 1] << 8) | data[data.size() - 2];

  if (computed != received) {
    ESP_LOGW(TAG, "CRC mismatch: computed=0x%04X, received=0x%04X", computed, received);
    return;
  }

  for (auto *device : this->devices_) {
    if (device->address_ == data[0]) {
      device->on_modbus_data(data);
    }
  }
}

}  // namespace sunopen_mppt
}  // namespace esphome
