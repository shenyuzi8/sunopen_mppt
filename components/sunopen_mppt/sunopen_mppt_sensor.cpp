void SunopenMPPTSwitch::on_modbus_data(const std::vector<uint8_t> &data) {
  if (data.size() < 5) return;

  uint8_t byte_count = data[2];
  const uint8_t *regs = &data[3];

  if (byte_count >= 200) {
    // 40013 (address 13): 当前负载状态 -> offset 26
    uint16_t val = (regs[26] << 8) | regs[27];
    ESP_LOGI(TAG, "Load status register 40013 raw value: %d", val);
    
    bool state = (val == 1);
    this->publish_state(state);
    ESP_LOGI(TAG, "Load status: %s", state ? "ON" : "OFF");
  }
}

void SunopenMPPTSwitch::write_state(bool state) {
  uint8_t value = state ? 0x01 : 0x00;
  
  // 40039 = 0x9C47，功能码 0x06，地址不需要偏移
  uint8_t cmd[] = {
    0x01,       // 设备地址
    0x06,       // 功能码：写单个寄存器
    0x9C, 0x47, // 寄存器地址 40039 (0x9C47)
    0x00, value,// 写入值
    0x00, 0x00  // CRC 占位
  };
  
  uint16_t crc = crc16(cmd, 6);
  cmd[6] = crc & 0xFF;
  cmd[7] = crc >> 8;
  
  this->write_command(std::vector<uint8_t>(cmd, cmd + 8));
  
  ESP_LOGI(TAG, "Load switch command (0x06 addr=0x9C47): %s", state ? "ON" : "OFF");
}
