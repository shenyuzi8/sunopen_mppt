#include "sunopen_mppt.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sunopen_mppt {

static const char *const TAG = "sunopen_mppt";

static const uint16_t SERVICE_UUID = 0xFFE0;
static const uint16_t TX_CHAR_UUID = 0xFFE1;
static const uint16_t RX_CHAR_UUID = 0xFFE2;

// Modbus 查询命令
static const uint8_t CMD1[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x1C, 0x84, 0x0F};
static const uint8_t CMD2[] = {0x01, 0x03, 0x00, 0x1C, 0x00, 0x2B, 0x45, 0x0E};

void SunopenMPPT::setup() {
  ESP_LOGD(TAG, "SunopenMPPT setup");
}

void SunopenMPPT::loop() {
  if (!this->ready_) {
    return;
  }
  
  uint32_t now = millis();
  if (now - this->last_poll_ >= POLL_INTERVAL) {
    this->last_poll_ = now;
    this->poll_data_();
  }
}

void SunopenMPPT::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                       esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_CONNECT_EVT:
      ESP_LOGI(TAG, "BLE connected");
      break;
      
    case ESP_GATTC_DISCONNECT_EVT:
      ESP_LOGI(TAG, "BLE disconnected");
      this->ready_ = false;
      break;
      
    case ESP_GATTC_SEARCH_CMPL_EVT:
      ESP_LOGI(TAG, "Service discovery complete");
      // 查找服务
      this->service_handle_ = 0;
      uint16_t count = 0;
      esp_gattc_db_attr_count(gattc_if, param->search_cmpl.conn_id, 
                             ESP_GATT_DB_PRIMARY_SERVICE, SERVICE_UUID, 
                             &this->service_handle_, &count);
      
      if (count > 0) {
        // 查找 TX 特征
        esp_gattc_char_by_uuid(gattc_if, param->search_cmpl.conn_id,
                              this->service_handle_, TX_CHAR_UUID,
                              &this->tx_handle_, &count);
        
        // 查找 RX 特征
        esp_gattc_char_by_uuid(gattc_if, param->search_cmpl.conn_id,
                              this->service_handle_, RX_CHAR_UUID,
                              &this->rx_handle_, &count);
        
        ESP_LOGI(TAG, "Found TX: 0x%04X, RX: 0x%04X", this->tx_handle_, this->rx_handle_);
        this->enable_notify_();
      }
      break;
      
    case ESP_GATTC_REG_FOR_NOTIFY_EVT:
      ESP_LOGI(TAG, "Notify registered");
      this->ready_ = true;
      this->last_poll_ = millis();
      break;
      
    case ESP_GATTC_NOTIFY_EVT:
      ESP_LOGD(TAG, "Received notify: %d bytes", param->notify.value_len);
      if (this->modbus_ != nullptr) {
        // 将 BLE 数据转发给 Modbus
        this->modbus_->receive(param->notify.value, param->notify.value_len);
      }
      break;
      
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
  ESP_LOGD(TAG, "Polling data");
  
  // 发送第一帧
  this->write_modbus_command_(std::vector<uint8_t>(CMD1, CMD1 + sizeof(CMD1)));
  delay(100);
  
  // 发送第二帧
  this->write_modbus_command_(std::vector<uint8_t>(CMD2, CMD2 + sizeof(CMD2)));
}

void SunopenMPPT::write_modbus_command_(const std::vector<uint8_t> &data) {
  if (this->tx_handle_ == 0) return;
  
  esp_gattc_write_char(this->parent()->get_gattc_if(),
                      this->parent()->get_conn_id(),
                      this->tx_handle_,
                      data.size(),
                      const_cast<uint8_t*>(data.data()),
                      ESP_GATT_WRITE_TYPE_NO_RSP,
                      ESP_GATT_AUTH_REQ_NONE);
}

}  // namespace sunopen_mppt
}  // namespace esphome