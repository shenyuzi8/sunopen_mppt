#pragma once

#include "sunopen_mppt.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace sunopen_mppt {

class SunopenMPPTSensor : public SunopenMPPTDevice, public Component {
 public:
  void on_modbus_data(const std::vector<uint8_t> &data) override;

  void set_battery_voltage_sensor(sensor::Sensor *s) { battery_voltage_sensor_ = s; }
  void set_battery_current_sensor(sensor::Sensor *s) { battery_current_sensor_ = s; }
  void set_battery_power_sensor(sensor::Sensor *s) { battery_power_sensor_ = s; }
  void set_pv_voltage_sensor(sensor::Sensor *s) { pv_voltage_sensor_ = s; }
  void set_pv_current_sensor(sensor::Sensor *s) { pv_current_sensor_ = s; }
  void set_pv_power_sensor(sensor::Sensor *s) { pv_power_sensor_ = s; }
  void set_load_current_sensor(sensor::Sensor *s) { load_current_sensor_ = s; }
  void set_load_power_sensor(sensor::Sensor *s) { load_power_sensor_ = s; }
  void set_battery_soc_sensor(sensor::Sensor *s) { battery_soc_sensor_ = s; }
  void set_battery_temp_sensor(sensor::Sensor *s) { battery_temp_sensor_ = s; }
  void set_controller_temp_sensor(sensor::Sensor *s) { controller_temp_sensor_ = s; }
  void set_today_energy_sensor(sensor::Sensor *s) { today_energy_sensor_ = s; }
  void set_charging_status_sensor(sensor::Sensor *s) { charging_status_sensor_ = s; }

 protected:
  sensor::Sensor *battery_voltage_sensor_{nullptr};
  sensor::Sensor *battery_current_sensor_{nullptr};
  sensor::Sensor *battery_power_sensor_{nullptr};
  sensor::Sensor *pv_voltage_sensor_{nullptr};
  sensor::Sensor *pv_current_sensor_{nullptr};
  sensor::Sensor *pv_power_sensor_{nullptr};
  sensor::Sensor *load_current_sensor_{nullptr};
  sensor::Sensor *load_power_sensor_{nullptr};
  sensor::Sensor *battery_soc_sensor_{nullptr};
  sensor::Sensor *battery_temp_sensor_{nullptr};
  sensor::Sensor *controller_temp_sensor_{nullptr};
  sensor::Sensor *today_energy_sensor_{nullptr};
  sensor::Sensor *charging_status_sensor_{nullptr};
};

class SunopenMPPTSwitch : public SunopenMPPTDevice, public Component, public switch_::Switch {
 public:
  void on_modbus_data(const std::vector<uint8_t> &data) override;
  void write_state(bool state) override;

  void set_load_switch(switch_::Switch *s) { load_switch_ = s; }

 protected:
  switch_::Switch *load_switch_{nullptr};
};

}  // namespace sunopen_mppt
}  // namespace esphome
