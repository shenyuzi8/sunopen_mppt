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
    // 写线圈或写寄存器响应（固定 8 字节）
    if (len >= 8) {
      ESP_LOGD(TAG, "Write response received (func=0x%02X)", function);
      return false;
    }
  } else if (function == 0x10) {
    // 写多个寄存器响应（固定 8 字节）
    if (len >= 8) {
      ESP_LOGD(TAG, "Write multiple response received");
      return false;
    }
  } else if (function >= 0x80) {
    // 异常响应
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
