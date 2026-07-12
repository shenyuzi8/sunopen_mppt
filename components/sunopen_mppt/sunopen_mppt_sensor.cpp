#include "sunopen_mppt_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sunopen_mppt {

static const char *const TAG = "sunopen_mppt_sensor";

void SunopenMPPTSensor::on_modbus_data(const std::vector<uint8_t> &data) {
  // data format: [addr(1)][func(1)][byte_count(1)][reg_data(N)][crc_lo(1)][crc_hi(1)]
  
  if (data.size() < 5) return;
  
  uint8_t byte_count = data[2];
  const uint8_t *regs = &data[3];  // 寄存器数据开始位置
  
  // 第一帧数据：40000~40027 (56 字节寄存器数据)
  if (byte_count >= 56) {
    // 40000: 设备型号
    // 40001: 软件版本
    // 40002: 硬件版本
    // 40003: 最大功率支持
    // 40004: 输入输出最大电流
    // 40005: 光伏降压散热器温度 (精度 1℃)
    if (this->controller_temp_sensor_ != nullptr) {
      uint16_t val = (regs[10] << 8) | regs[11];
      this->controller_temp_sensor_->publish_state(val);
    }
    
    // 40014: BUCK 散热器温度 (精度 1℃)
    // 40020: 光伏输入电压 (精度 0.01V)
    if (this->pv_voltage_sensor_ != nullptr) {
      uint16_t val = (regs[40] << 8) | regs[41];
      this->pv_voltage_sensor_->publish_state(val * 0.01f);
    }
    
    // 40021: 光伏输入电流 (精度 0.01A)
    if (this->pv_current_sensor_ != nullptr) {
      uint16_t val = (regs[42] << 8) | regs[43];
      this->pv_current_sensor_->publish_state(val * 0.01f);
    }
    
    // 40022: 光伏输入功率 (精度 1W)
    if (this->pv_power_sensor_ != nullptr) {
      uint16_t val = (regs[44] << 8) | regs[45];
      this->pv_power_sensor_->publish_state(val);
    }
    
    // 40023: 光伏散热器温度 (精度 1℃)
    if (this->battery_temp_sensor_ != nullptr) {
      uint16_t val = (regs[46] << 8) | regs[47];
      this->battery_temp_sensor_->publish_state(val);
    }
    
    // 40025: 电池输出电压 (精度 0.01V)
    if (this->battery_voltage_sensor_ != nullptr) {
      uint16_t val = (regs[50] << 8) | regs[51];
      this->battery_voltage_sensor_->publish_state(val * 0.01f);
    }
    
    // 40026: 电池输出电流 (精度 0.01A)
    if (this->battery_current_sensor_ != nullptr) {
      uint16_t val = (regs[52] << 8) | regs[53];
      this->battery_current_sensor_->publish_state(val * 0.01f);
    }
    
    // 40027: 电池输出功率 (精度 1W)
    if (this->battery_power_sensor_ != nullptr) {
      uint16_t val = (regs[54] << 8) | regs[55];
      this->battery_power_sensor_->publish_state(val);
    }
    
    // 40010: 充电阶段
    // 40011: 充电状态 (0=未充电, 1=充电中)
    if (this->charging_status_sensor_ != nullptr) {
      uint16_t val = (regs[22] << 8) | regs[23];
      this->charging_status_sensor_->publish_state(val);
    }
  }
  
  // 第二帧数据：40028~40070 (86 字节寄存器数据)
  if (byte_count >= 86) {
    // 40028: 电池散热器温度 (精度 1℃)
    // 40030: 负载输出电流 (精度 0.1A)
    if (this->load_current_sensor_ != nullptr) {
      uint16_t val = (regs[60] << 8) | regs[61];
      this->load_current_sensor_->publish_state(val * 0.1f);
    }
    
    // 40031: 负载输出功率 (精度 1W)
    if (this->load_power_sensor_ != nullptr) {
      uint16_t val = (regs[62] << 8) | regs[63];
      this->load_power_sensor_->publish_state(val);
    }
    
    // 40035: 当前 SOC 值 (精度 0.1%)
    if (this->battery_soc_sensor_ != nullptr) {
      uint16_t val = (regs[70] << 8) | regs[71];
      this->battery_soc_sensor_->publish_state(val * 0.1f);
    }
    
    // 40037: 今日发电量 (精度 1Wh)
    if (this->today_energy_sensor_ != nullptr) {
      uint16_t val = (regs[74] << 8) | regs[75];
      this->today_energy_sensor_->publish_state(val);
    }
  }
  
  ESP_LOGD(TAG, "Sensor data updated successfully");
}

}  // namespace sunopen_mppt
}  // namespace esphome
