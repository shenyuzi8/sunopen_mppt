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

CONFIG_SCHEMA = switch.switch_schema(SunopenMPPTSwitch).extend(
    sunopen_mppt_device_schema()
)

async def to_code(config):
    var = await switch.new_switch(config)
    await cg.register_component(var, config)
    await register_sunopen_mppt_device(var, config)
