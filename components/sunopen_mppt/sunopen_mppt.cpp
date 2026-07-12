#include "sunopen_mppt.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sunopen_mppt {

static const char *const TAG = "sunopen_mppt";

static const uint8_t CMD1[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x1C, 0x84, 0x0F};
static const uint8_t CMD2[] = {0x01, 0x03, 0x00, 0x1C, 0x00, 0x2B, 0x45, 0x0E};

void SunopenMPPT::setup() {
  ESP_LOGD(TAG, "SunopenMPPT setup complete");
}

void SunopenMPPT::loop() {
  if (!this->ready_) return;
  
  uint32_t now = millis();
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
      uint16_t count = 0;
      // ESP-IDF 5.x API
      esp_ble_gattc_get_attr_count(gattc_if, param->search_cmpl.conn_id,
                                   ESP_GATT_DB_PRIMARY_SERVICE, SERVICE_UUID,
                                   &this->service_handle_, &count);
      
      if (count > 0) {
        esp_ble_gattc_get_char_by_uuid(gattc_if, param->search_cmpl.conn_id,
                                       this->service_handle_, 0x0000,
                                       TX_CHAR_UUID, &this->tx_handle_, &count);
        esp_ble_gattc_get_char_by_uuid(gattc_if, param->search_cmpl.conn_id,
                                       this->service_handle_, 0x0000,
                                       RX_CHAR_UUID, &this->rx_handle_, &count);
        ESP_LOGI(TAG, "TX: 0x%04X, RX: 0x%04X", this->tx_handle_, this->rx_handle_);
        this->enable_notify_();
      }
      break;
    }
      
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
      ESP_LOGI(TAG, "Notify registered");
      this->ready_ = true;
      this->last_poll_ = millis();
      break;
    }
      
    case ESP_GATTC_NOTIFY_EVT: {
      this->process_notify_data_(param->notify.value, param->notify.value_len);
      break;
    }
      
    default:
      break;
  }
}

void SunopenMPPT::enable_notify_() {
  if (this->rx_handle_ == 0) return;
  esp_ble_gattc_register_for_notify(this->parent()->get_gattc_if(),
                                    this->parent()->get_remote_bda(),
                                    this->rx_handle_);
}

void SunopenMPPT::poll_data_() {
  this->write_command_({CMD1, CMD1 + sizeof(CMD1)});
  delay(200);
  this->write_command_({CMD2, CMD2 + sizeof(CMD2)});
}

void SunopenMPPT::write_command_(const std::vector<uint8_t> &data) {
  if (this->tx_handle_ == 0) return;
  esp_ble_gattc_write_char(this->parent()->get_gattc_if(),
                          this->parent()->get_conn_id(),
                          this->tx_handle_,
                          data.size(),
                          const_cast<uint8_t*>(data.data()),
                          ESP_GATT_WRITE_TYPE_NO_RSP,
                          ESP_GATT_AUTH_REQ_NONE);
}

void SunopenMPPT::process_notify_data_(const uint8_t *data, size_t len) {
  ESP_LOGD(TAG, "Received %d bytes", len);
}

}  // namespace sunopen_mppt
}  // namespace esphome
