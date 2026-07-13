#include "sunopen_mppt_sensor.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace sunopen_mppt {

static const char *const TAG = "sunopen_mppt_sensor";

void SunopenMPPTSensor::on_modbus_data(const std::vector<uint8_t> &data) {
  if (data.size() < 5) {
    ESP_LOGW(TAG, "Data too short: %d bytes", data.size());
    return;
  }

  uint8_t byte_count = data[2];
  const uint8_t *regs = &data[3];

  ESP_LOGI(TAG, "Processing %d bytes of register data", byte_count);

  if (byte_count >= 200) {
    // 40020 (address 20): PV input voltage (0.01V)
    if (this->pv_voltage_sensor_ != nullptr) {
      uint16_t val = (regs[40] << 8) | regs[41];
      this->pv_voltage_sensor_->publish_state(val * 0.01f);
    }

    // 40021 (address 21): PV input current (0.01A)
    if (this->pv_current_sensor_ != nullptr) {
      uint16_t val = (regs[42] << 8) | regs[43];
      this->pv_current_sensor_->publish_state(val * 0.01f);
    }

    // 40022 (address 22): PV input power (1W)
    if (this->pv_power_sensor_ != nullptr) {
      uint16_t val = (regs[44] << 8) | regs[45];
      this->pv_power_sensor_->publish_state(val);
    }

    // 40025 (address 25): Battery voltage (0.01V)
    if (this->battery_voltage_sensor_ != nullptr) {
      uint16_t val = (regs[50] << 8) | regs[51];
      this->battery_voltage_sensor_->publish_state(val * 0.01f);
    }

    // 40026 (address 26): Battery output current (0.01A)
    if (this->battery_current_sensor_ != nullptr) {
      uint16_t val = (regs[52] << 8) | regs[53];
      this->battery_current_sensor_->publish_state(val * 0.01f);
    }

    // 40027 (address 27): Battery output power (1W)
    if (this->battery_power_sensor_ != nullptr) {
      uint16_t val = (regs[54] << 8) | regs[55];
      this->battery_power_sensor_->publish_state(val);
    }

    // 40030 (address 30): Load current (0.1A)
    if (this->load_current_sensor_ != nullptr) {
      uint16_t val = (regs[60] << 8) | regs[61];
      this->load_current_sensor_->publish_state(val * 0.1f);
    }

    // 40031 (address 31): Load power (1W)
    if (this->load_power_sensor_ != nullptr) {
      uint16_t val = (regs[62] << 8) | regs[63];
      this->load_power_sensor_->publish_state(val);
    }

    // 40035 (address 35): Battery SOC (0.1%)
    if (this->battery_soc_sensor_ != nullptr) {
      uint16_t val = (regs[70] << 8) | regs[71];
      this->battery_soc_sensor_->publish_state(val * 0.1f);
    }

    // 40028 (address 28): Battery heatsink temperature -> battery_temp (1°C)
    if (this->battery_temp_sensor_ != nullptr) {
      uint16_t val = (regs[56] << 8) | regs[57];
      this->battery_temp_sensor_->publish_state(val);
    }

    // 40005 (address 5): PV buck heatsink temperature -> controller_temp (1°C)
    if (this->controller_temp_sensor_ != nullptr) {
      uint16_t val = (regs[10] << 8) | regs[11];
      this->controller_temp_sensor_->publish_state(val);
    }

    // 40037 (address 37): Today energy (1Wh)
    if (this->today_energy_sensor_ != nullptr) {
      uint16_t val = (regs[74] << 8) | regs[75];
      this->today_energy_sensor_->publish_state(val);
    }

    // 40011 (address 11): Charging status
    if (this->charging_status_sensor_ != nullptr) {
      uint16_t val = (regs[22] << 8) | regs[23];
      this->charging_status_sensor_->publish_state(val);
    }

    ESP_LOGI(TAG, "All sensor values updated");
  } else {
    ESP_LOGW(TAG, "Not enough data: byte_count=%d (expected >=200)", byte_count);
  }
}

// ==================== Switch Implementation ====================

void SunopenMPPTSwitch::on_modbus_data(const std::vector<uint8_t> &data) {
  if (data.size() < 5) return;

  uint8_t byte_count = data[2];
  const uint8_t *regs = &data[3];

  if (byte_count >= 200) {
    // 40039 (address 39): Load switch status -> offset 78
    uint16_t val = (regs[78] << 8) | regs[79];
    ESP_LOGI(TAG, "Load switch register raw value: %d (0x%04X)", val, val);
    
    bool state = (val == 1);
    this->publish_state(state);
    ESP_LOGI(TAG, "Load switch status: %s", state ? "ON" : "OFF");
  }
}

void SunopenMPPTSwitch::write_state(bool state) {
  uint8_t value = state ? 0x01 : 0x00;
  
  // 40039 的 PLC 地址 = 0x9C47
  uint8_t cmd[] = {
    0x01,       // 设备地址
    0x06,       // 功能码：写单个寄存器
    0x9C, 0x47, // 寄存器地址 40039 (PLC 地址 0x9C47)
    0x00, value,// 写入值
    0x00, 0x00  // CRC 占位
  };
  
  // 计算 CRC16
  uint16_t crc = crc16(cmd, 6);
  cmd[6] = crc & 0xFF;
  cmd[7] = crc >> 8;
  
  this->write_command(std::vector<uint8_t>(cmd, cmd + 8));
  
  ESP_LOGI(TAG, "Load switch command sent: %s (addr=0x9C47, value=%d)", state ? "ON" : "OFF", value);
}

}  // namespace sunopen_mppt
}  // namespace esphome
