import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import ble_client
from esphome.const import CONF_ID
from esphome import automation

DEPENDENCIES = ["ble_client"]
AUTO_LOAD = ["modbus", "modbus_controller", "sensor", "switch", "number", "text_sensor"]

sunopen_mppt_ns = cg.esphome_ns.namespace("sunopen_mppt")
SunopenMPPT = sunopen_mppt_ns.class_(
    "SunopenMPPT",
    ble_client.BLEClientNode,
    cg.Component
)

CONF_SUNOPEN_MPPT_ID = "sunopen_mppt_id"

# 关键：使用 ensure_list 允许列表格式
CONFIG_SCHEMA = cv.ensure_list(
    cv.Schema({
        cv.GenerateID(): cv.declare_id(SunopenMPPT),
    }).extend(ble_client.BLE_CLIENT_SCHEMA)
)

async def to_code(config):
    for conf in config:
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await ble_client.register_ble_node(var, conf)
