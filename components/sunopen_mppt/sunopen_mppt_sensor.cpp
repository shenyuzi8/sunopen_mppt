#include "sunopen_mppt_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sunopen_mppt {

static const char *const TAG = "sunopen_mppt_sensor";

void SunopenMPPTSensor::on_modbus_data(const std::vector<uint8_t> &data) {
  // data format: [addr][func][byte_count][reg_data...][crc_lo][crc_hi]
  // func = 0x03 (read holding registers)
  
  if (data.size() < 5) return;
  
  uint8_t byte_count = data[2];
  const uint8_t *reg_data = &data[3];
  
  // Register addresses (0-based)
  // 0x00: device model
  // 0x01: software version
  // 0x02: hardware version
  // 0x03: max power support
  // 0x04: PV/battery max current
  // 0x05: PV buck heatsink temp
  // 0x06: PV NTC status
  // 0x0A: charge stage code
  // 0x0B: charge status
  // 0x14: PV input voltage (0.01V)
  // 0x15: PV input current (0.01A)
  // 0x16: PV input power (W)
  // 0x17: PV heatsink temp (°C)
  // 0x19: battery voltage (0.01V)
  // 0x1A: battery current (0.01A)
  // 0x1B: battery power (W)
  // 0x1E: load current (0.1A)
  // 0x1F: load power (W)
  // 0x23: battery SOC (0.1%)
  // 0x25: today energy (Wh)

  if (byte_count >= 56) {  // Full data frame
    if (this->pv_voltage_sensor_ != nullptr) {
      uint16_t val = (reg_data[40] << 8) | reg_data[41];  // 0x14
      this->pv_voltage_sensor_->publish_state(val * 0.01f);
    }
    
    if (this->pv_current_sensor_ != nullptr) {
      uint16_t val = (reg_data[42] << 8) | reg_data[43];  // 0x15
      this->pv_current_sensor_->publish_state(val * 0.01f);
    }
    
    if (this->pv_power_sensor_ != nullptr) {
      uint16_t val = (reg_data[44] << 8) | reg_data[45];  // 0x16
      this->pv_power_sensor_->publish_state(val);
    }
    
    if (this->battery_voltage_sensor_ != nullptr) {
      uint16_t val = (reg_data[50] << 8) | reg_data[51];  // 0x19
      this->battery_voltage_sensor_->publish_state(val * 0.01f);
    }
    
    if (this->battery_current_sensor_ != nullptr) {
      uint16_t val = (reg_data[52] << 8) | reg_data[53];  // 0x1A
      this->battery_current_sensor_->publish_state(val * 0.01f);
    }
    
    if (this->battery_power_sensor_ != nullptr) {
      uint16_t val = (reg_data[54] << 8) | reg_data[55];  // 0x1B
      this->battery_power_sensor_->publish_state(val);
    }
    
    if (this->controller_temp_sensor_ != nullptr) {
      uint16_t val = (reg_data[10] << 8) | reg_data[11];  // 0x05
      this->controller_temp_sensor_->publish_state(val);
    }
  }
  
  if (byte_count >= 86) {  // Second frame data
    if (this->load_current_sensor_ != nullptr) {
      uint16_t val = (reg_data[60] << 8) | reg_data[61];  // 0x1E
      this->load_current_sensor_->publish_state(val * 0.1f);
    }
    
    if (this->load_power_sensor_ != nullptr) {
      uint16_t val = (reg_data[62] << 8) | reg_data[63];  // 0x1F
      this->load_power_sensor_->publish_state(val);
    }
    
    if (this->battery_soc_sensor_ != nullptr) {
      uint16_t val = (reg_data[70] << 8) | reg_data[71];  // 0x23
      this->battery_soc_sensor_->publish_state(val * 0.1f);
    }
    
    if (this->today_energy_sensor_ != nullptr) {
      uint16_t val = (reg_data[74] << 8) | reg_data[75];  // 0x25
      this->today_energy_sensor_->publish_state(val);
    }
    
    if (this->charging_status_sensor_ != nullptr) {
      uint16_t val = (reg_data[22] << 8) | reg_data[23];  // 0x0B
      this->charging_status_sensor_->publish_state(val);
    }
  }
}

}  // namespace sunopen_mppt
}  // namespace esphome
