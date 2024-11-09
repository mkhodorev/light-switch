import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["api"]
AUTO_LOAD = ["switch"]

ha_enabled_switch_ns = cg.esphome_ns.namespace("ha_enabled_switch")
HaEnabledSwitch = ha_enabled_switch_ns.class_("HaEnabledSwitch", cg.Component)
HaEnabledSwitchPtr = HaEnabledSwitch.operator("ptr")

HA_ENABLED_SWITCH_ID = "ha_enabled_switch_id"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(HaEnabledSwitch),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
