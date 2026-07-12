import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import ble_client, modbus
from esphome.const import CONF_ID

DEPENDENCIES = ["ble_client"]
AUTO_LOAD = ["modbus", "sensor", "number", "switch", "text_sensor"]

sunopen_mppt_ns = cg.esphome_ns.namespace("sunopen_mppt")
SunopenMPPT = sunopen_mppt_ns.class_(
    "SunopenMPPT", 
    ble_client.BLEClientNode, 
    cg.Component
)

CONF_SUNOPEN_MPPT_ID = "sunopen_mppt_id"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SunopenMPPT),
}).extend(ble_client.BLE_CLIENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)