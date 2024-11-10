import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_DISABLED_BY_DEFAULT,
    CONF_RESTORE_MODE,
    CONF_ICON,
)

DEPENDENCIES = ["api"]
AUTO_LOAD = ["switch"]

ha_enabled_switch_ns = cg.esphome_ns.namespace("ha_enabled_switch")
HaEnabledSwitch = ha_enabled_switch_ns.class_("HaEnabledSwitch", cg.Component)
HaEnabledSwitchPtr = HaEnabledSwitch.operator("ptr")

HA_ENABLED_SWITCH_ID = "ha_enabled_switch_id"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(HaEnabledSwitch),
        cv.Optional(CONF_NAME, default="Manage light_switch over HA"): cv.string,
        cv.Optional(CONF_ICON, default="mdi:home-assistant"): cv.icon,
        cv.Optional(CONF_DISABLED_BY_DEFAULT, default=False): cv.boolean,
        cv.Optional(CONF_RESTORE_MODE, default="RESTORE_DEFAULT_OFF"): cv.enum(
            switch.RESTORE_MODES, upper=True, space="_"
        ),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = await switch.new_switch(config)
    await cg.register_component(var, config)
