import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID
from . import (
    SunopenMPPTDevice,
    sunopen_mppt_ns,
    register_sunopen_mppt_device,
    sunopen_mppt_device_schema,
)

DEPENDENCIES = ["sunopen_mppt"]

SunopenMPPTSwitch = sunopen_mppt_ns.class_(
    "SunopenMPPTSwitch", SunopenMPPTDevice, cg.Component, switch.Switch
)

CONF_LOAD_SWITCH = "load_switch"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SunopenMPPTSwitch),
            cv.Optional(CONF_LOAD_SWITCH): switch.switch_schema(),
        }
    )
    .extend(sunopen_mppt_device_schema())
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await register_sunopen_mppt_device(var, config)
    await switch.register_switch(var, config)

    if CONF_LOAD_SWITCH in config:
        sw = await switch.new_switch(config[CONF_LOAD_SWITCH])
        cg.add(var.set_load_switch(sw))
