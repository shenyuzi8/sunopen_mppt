#include "sunopen_mppt_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sunopen_mppt {

static const char *const TAG = "sunopen_mppt_sensor";

void SunopenMPPTSensor::on_modbus_data(const std::vector<uint8_t> &data) {
  if (data.size() < 5) {
    ESP_LOGW(TAG, "Data too short: %d bytes", data.size());
    return;
  }
  
  uint8_t addr = data[0];
  uint8_t func = data[1];
  uint8_t byte_count = data[2];
  
  ESP_LOGI(TAG, "Modbus frame: addr=0x%02X, func=0x%02X, byte_count=%d, total_len=%d", 
           addr, func, byte_count, data.size());
  
  if (data.size() > 0) {
    ESP_LOGI(TAG, "Raw data (first 20 bytes): %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
             data.size() > 0 ? data[0] : 0, data.size() > 1 ? data[1] : 0,
             data.size() > 2 ? data[2] : 0, data.size() > 3 ? data[3] : 0,
             data.size() > 4 ? data[4] : 0, data.size() > 5 ? data[5] : 0,
             data.size() > 6 ? data[6] : 0, data.size() > 7 ? data[7] : 0,
             data.size() > 8 ? data[8] : 0, data.size() > 9 ? data[9] : 0,
             data.size() > 10 ? data[10] : 0, data.size() > 11 ? data[11] : 0,
             data.size() > 12 ? data[12] : 0, data.size() > 13 ? data[13] : 0,
             data.size() > 14 ? data[14] : 0, data.size() > 15 ? data[15] : 0,
             data.size() > 16 ? data[16] : 0, data.size() > 17 ? data[17] : 0,
             data.size() > 18 ? data[18] : 0, data.size() > 19 ? data[19] : 0);
  }
  
  const uint8_t *regs = &data[3];
  
  // First frame: registers 0x00-0x1B (0-27), byte_count should be 56
  if (byte_count >= 56) {
    ESP_LOGI(TAG, "Processing first frame (regs 0x00-0x1B)");
    
    // 40020: PV voltage (offset 40)
    uint16_t pv_voltage_raw = (regs[40] << 8) | regs[41];
    ESP_LOGI(TAG, "PV voltage raw: %d (0x%04X)", pv_voltage_raw, pv_voltage_raw);
    if (this->pv_voltage_sensor_ != nullptr) {
      this->pv_voltage_sensor_->publish_state(pv_voltage_raw * 0.01f);
    }
    
    // 40021: PV current (offset 42)
    uint16_t pv_current_raw = (regs[42] << 8) | regs[43];
    ESP_LOGI(TAG, "PV current raw: %d", pv_current_raw);
    if (this->pv_current_sensor_ != nullptr) {
      this->pv_current_sensor_->publish_state(pv_current_raw * 0.01f);
    }
    
    // 40022: PV power (offset 44)
    uint16_t pv_power_raw = (regs[44] << 8) | regs[45];
    ESP_LOGI(TAG, "PV power raw: %d", pv_power_raw);
    if (this->pv_power_sensor_ != nullptr) {
      this->pv_power_sensor_->publish_state(pv_power_raw);
    }
    
    // 40025: Battery voltage (offset 50)
    uint16_t bat_voltage_raw = (regs[50] << 8) | regs[51];
    ESP_LOGI(TAG, "Battery voltage raw: %d", bat_voltage_raw);
    if (this->battery_voltage_sensor_ != nullptr) {
      this->battery_voltage_sensor_->publish_state(bat_voltage_raw * 0.01f);
    }
    
    // 40026: Battery current (offset 52)
    uint16_t bat_current_raw = (regs[52] << 8) | regs[53];
    ESP_LOGI(TAG, "Battery current raw: %d", bat_current_raw);
    if (this->battery_current_sensor_ != nullptr) {
      this->battery_current_sensor_->publish_state(bat_current_raw * 0.01f);
    }
    
    // 40027: Battery power (offset 54)
    uint16_t bat_power_raw = (regs[54] << 8) | regs[55];
    ESP_LOGI(TAG, "Battery power raw: %d", bat_power_raw);
    if (this->battery_power_sensor_ != nullptr) {
      this->battery_power_sensor_->publish_state(bat_power_raw);
    }
    
    // 40005: Controller temp (offset 10)
    uint16_t ctrl_temp_raw = (regs[10] << 8) | regs[11];
    ESP_LOGI(TAG, "Controller temp raw: %d", ctrl_temp_raw);
    if (this->controller_temp_sensor_ != nullptr) {
      this->controller_temp_sensor_->publish_state(ctrl_temp_raw);
    }
    
    // 40011: Charging status (offset 22)
    uint16_t charge_status_raw = (regs[22] << 8) | regs[23];
    ESP_LOGI(TAG, "Charge status raw: %d", charge_status_raw);
    if (this->charging_status_sensor_ != nullptr) {
      this->charging_status_sensor_->publish_state(charge_status_raw);
    }
  }
  
  // Second frame: registers 0x1C-0x46 (28-70), byte_count should be 86
  if (byte_count >= 86) {
    ESP_LOGI(TAG, "Processing second frame (regs 0x1C-0x46)");
    
    // 40030: Load current (offset 60)
    uint16_t load_current_raw = (regs[60] << 8) | regs[61];
    ESP_LOGI(TAG, "Load current raw: %d", load_current_raw);
    if (this->load_current_sensor_ != nullptr) {
      this->load_current_sensor_->publish_state(load_current_raw * 0.1f);
    }
    
    // 40031: Load power (offset 62)
    uint16_t load_power_raw = (regs[62] << 8) | regs[63];
    ESP_LOGI(TAG, "Load power raw: %d", load_power_raw);
    if (this->load_power_sensor_ != nullptr) {
      this->load_power_sensor_->publish_state(load_power_raw);
    }
    
    // 40035: SOC (offset 70)
    uint16_t soc_raw = (regs[70] << 8) | regs[71];
    ESP_LOGI(TAG, "SOC raw: %d", soc_raw);
    if (this->battery_soc_sensor_ != nullptr) {
      this->battery_soc_sensor_->publish_state(soc_raw * 0.1f);
    }
    
    // 40037: Today energy (offset 74)
    uint16_t energy_raw = (regs[74] << 8) | regs[75];
    ESP_LOGI(TAG, "Today energy raw: %d", energy_raw);
    if (this->today_energy_sensor_ != nullptr) {
      this->today_energy_sensor_->publish_state(energy_raw);
    }
  }
  
  ESP_LOGI(TAG, "Sensor update complete");
}

}  // namespace sunopen_mppt
}  // namespace esphome
