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
    case ESP_GATTC_CONNECT_EVT: {
      ESP_LOGI(TAG, "MPPT connected");
      break;
    }

    case ESP_GATTC_DISCONNECT_EVT: {
      ESP_LOGI(TAG, "MPPT disconnected");
      this->ready_ = false;
      break;
    }

    case ESP_GATTC_SEARCH_CMPL_EVT: {
      // 使用 ESP-IDF 5.x 的正确 API
      uint16_t count = 0;
      esp_bt_uuid_t svc_uuid;
      svc_uuid.len = ESP_UUID_LEN_16;
      svc_uuid.uuid.uuid16 = SERVICE_UUID;

      esp_bt_uuid_t tx_uuid;
      tx_uuid.len = ESP_UUID_LEN_16;
      tx_uuid.uuid.uuid16 = TX_CHAR_UUID;

      esp_bt_uuid_t rx_uuid;
      rx_uuid.len = ESP_UUID_LEN_16;
      rx_uuid.uuid.uuid16 = RX_CHAR_UUID;

      esp_ble_gattc_get_attr_count(gattc_if, param->search_cmpl.conn_id,
                                   ESP_GATT_DB_PRIMARY_SERVICE,
                                   svc_uuid, &count);

      if (count > 0) {
        uint16_t start_handle = 0;
        uint16_t end_handle = 0;

        // 获取服务句柄范围
        esp_ble_gattc_get_service(gattc_if, param->search_cmpl.conn_id,
                                  svc_uuid, &start_handle, &end_handle, 0);

        // 获取 TX 特征句柄
        esp_ble_gattc_get_char_by_uuid(gattc_if, param->search_cmpl.conn_id,
                                       start_handle, end_handle,
                                       tx_uuid, &this->tx_handle_, &count);

        // 获取 RX 特征句柄
        esp_ble_gattc_get_char_by_uuid(gattc_if, param->search_cmpl.conn_id,
                                       start_handle, end_handle,
                                       rx_uuid, &this->rx_handle_, &count);

        ESP_LOGI(TAG, "TX handle: %d, RX handle: %d",
                 this->tx_handle_, this->rx_handle_);
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
      ESP_LOGD(TAG, "Received %d bytes via notify", param->notify.value_len);
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
  ESP_LOGD(TAG, "Polling Modbus data");
  this->write_modbus_command_(std::vector<uint8_t>(CMD1, CMD1 + sizeof(CMD1)));
  delay(200);
  this->write_modbus_command_(std::vector<uint8_t>(CMD2, CMD2 + sizeof(CMD2)));
}

void SunopenMPPT::write_modbus_command_(const std::vector<uint8_t> &data) {
  if (this->tx_handle_ == 0) return;
  esp_ble_gattc_write_char(this->parent()->get_gattc_if(),
                          this->parent()->get_conn_id(),
                          this->tx_handle_,
                          data.size(),
                          const_cast<uint8_t*>(data.data()),
                          ESP_GATT_WRITE_TYPE_NO_RSP,
                          ESP_GATT_AUTH_REQ_NONE);
}

}  // namespace sunopen_mppt
}  // namespace esphome
