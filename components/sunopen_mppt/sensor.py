import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_ADDRESS,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_ENERGY,
    STATE_CLASS_MEASUREMENT,
    UNIT_VOLT,
    UNIT_AMPERE,
    UNIT_WATT,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    UNIT_WATT_HOURS,
)
from . import SunopenMPPTDevice, register_sunopen_mppt_device, sunopen_mppt_device_schema

DEPENDENCIES = ["sunopen_mppt"]

CONF_BATTERY_VOLTAGE = "battery_voltage"
CONF_BATTERY_CURRENT = "battery_current"
CONF_BATTERY_POWER = "battery_power"
CONF_PV_VOLTAGE = "pv_voltage"
CONF_PV_CURRENT = "pv_current"
CONF_PV_POWER = "pv_power"
CONF_LOAD_CURRENT = "load_current"
CONF_LOAD_POWER = "load_power"
CONF_BATTERY_SOC = "battery_soc"
CONF_BATTERY_TEMP = "battery_temp"
CONF_CONTROLLER_TEMP = "controller_temp"
CONF_TODAY_ENERGY = "today_energy"
CONF_CHARGING_STATUS = "charging_status"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SunopenMPPTSensor),
            cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_VOLTAGE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_BATTERY_CURRENT): sensor.sensor_schema(
                unit_of_measurement=UNIT_AMPERE,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_CURRENT,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_BATTERY_POWER): sensor.sensor_schema(
                unit_of_measurement=UNIT_WATT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_POWER,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_PV_VOLTAGE): sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_VOLTAGE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_PV_CURRENT): sensor.sensor_schema(
                unit_of_measurement=UNIT_AMPERE,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_CURRENT,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_PV_POWER): sensor.sensor_schema(
                unit_of_measurement=UNIT_WATT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_POWER,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_LOAD_CURRENT): sensor.sensor_schema(
                unit_of_measurement=UNIT_AMPERE,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_CURRENT,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_LOAD_POWER): sensor.sensor_schema(
                unit_of_measurement=UNIT_WATT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_POWER,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_BATTERY_SOC): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_BATTERY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_BATTERY_TEMP): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_CONTROLLER_TEMP): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_TODAY_ENERGY): sensor.sensor_schema(
                unit_of_measurement=UNIT_WATT_HOURS,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_ENERGY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_CHARGING_STATUS): sensor.sensor_schema(
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(sunopen_mppt_device_schema())
)

SunopenMPPTSensor = sunopen_mppt_ns.class_(
    "SunopenMPPTSensor", SunopenMPPTDevice, cg.Component
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await register_sunopen_mppt_device(var, config)

    if CONF_BATTERY_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_VOLTAGE])
        cg.add(var.set_battery_voltage_sensor(sens))
    if CONF_BATTERY_CURRENT in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_CURRENT])
        cg.add(var.set_battery_current_sensor(sens))
    if CONF_BATTERY_POWER in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_POWER])
        cg.add(var.set_battery_power_sensor(sens))
    if CONF_PV_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_PV_VOLTAGE])
        cg.add(var.set_pv_voltage_sensor(sens))
    if CONF_PV_CURRENT in config:
        sens = await sensor.new_sensor(config[CONF_PV_CURRENT])
        cg.add(var.set_pv_current_sensor(sens))
    if CONF_PV_POWER in config:
        sens = await sensor.new_sensor(config[CONF_PV_POWER])
        cg.add(var.set_pv_power_sensor(sens))
    if CONF_LOAD_CURRENT in config:
        sens = await sensor.new_sensor(config[CONF_LOAD_CURRENT])
        cg.add(var.set_load_current_sensor(sens))
    if CONF_LOAD_POWER in config:
        sens = await sensor.new_sensor(config[CONF_LOAD_POWER])
        cg.add(var.set_load_power_sensor(sens))
    if CONF_BATTERY_SOC in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_SOC])
        cg.add(var.set_battery_soc_sensor(sens))
    if CONF_BATTERY_TEMP in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_TEMP])
        cg.add(var.set_battery_temp_sensor(sens))
    if CONF_CONTROLLER_TEMP in config:
        sens = await sensor.new_sensor(config[CONF_CONTROLLER_TEMP])
        cg.add(var.set_controller_temp_sensor(sens))
    if CONF_TODAY_ENERGY in config:
        sens = await sensor.new_sensor(config[CONF_TODAY_ENERGY])
        cg.add(var.set_today_energy_sensor(sens))
    if CONF_CHARGING_STATUS in config:
        sens = await sensor.new_sensor(config[CONF_CHARGING_STATUS])
        cg.add(var.set_charging_status_sensor(sens))
