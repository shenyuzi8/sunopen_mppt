import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import ble_client
from esphome.const import CONF_ID, CONF_ADDRESS

DEPENDENCIES = ["ble_client"]
AUTO_LOAD = ["sensor", "switch", "number", "text_sensor", "binary_sensor"]

sunopen_mppt_ns = cg.esphome_ns.namespace("sunopen_mppt")
SunopenMPPT = sunopen_mppt_ns.class_("SunopenMPPT", ble_client.BLEClientNode, cg.Component)
SunopenMPPTDevice = sunopen_mppt_ns.class_("SunopenMPPTDevice")

CONF_SUNOPEN_MPPT_ID = "sunopen_mppt_id"
CONF_THROTTLE = "throttle"

CONFIG_SCHEMA = cv.ensure_list(
    cv.Schema({
        cv.GenerateID(): cv.declare_id(SunopenMPPT),
        cv.Optional(CONF_THROTTLE, default="5s"): cv.positive_time_period_milliseconds,
    }).extend(ble_client.BLE_CLIENT_SCHEMA)
)

async def to_code(config):
    for conf in config:
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await ble_client.register_ble_node(var, conf)
        if CONF_THROTTLE in conf:
            cg.add(var.set_throttle(conf[CONF_THROTTLE]))

def sunopen_mppt_device_schema(default_address=0x01):
    schema = {
        cv.GenerateID(CONF_SUNOPEN_MPPT_ID): cv.use_id(SunopenMPPT),
    }
    if default_address is None:
        schema[cv.Required(CONF_ADDRESS)] = cv.hex_uint8_t
    else:
        schema[cv.Optional(CONF_ADDRESS, default=default_address)] = cv.hex_uint8_t
    return cv.Schema(schema)

async def register_sunopen_mppt_device(var, config):
    parent = await cg.get_variable(config[CONF_SUNOPEN_MPPT_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(parent.register_device(var))
