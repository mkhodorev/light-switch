from esphome import automation
from esphome.automation import Condition, maybe_simple_id
import esphome.codegen as cg
from esphome.components import mqtt, web_server
import esphome.config_validation as cv
from esphome.const import (
    CONF_DEVICE_CLASS,
    CONF_ENTITY_CATEGORY,
    CONF_ICON,
    CONF_ID,
    CONF_INVERTED,
    CONF_MQTT_ID,
    CONF_ON_TURN_OFF,
    CONF_ON_TURN_ON,
    CONF_RESTORE_MODE,
    CONF_TRIGGER_ID,
    CONF_WEB_SERVER,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_OUTLET,
    DEVICE_CLASS_SWITCH,
)
from esphome.core import CORE, coroutine_with_priority
from esphome.cpp_generator import MockObjClass
from esphome.cpp_helpers import setup_entity

DEPENDENCIES = ["api"]

light_switch_ns = cg.esphome_ns.namespace("light_switch")
LightSwitch = light_switch_ns.class_("LightSwitch", cg.EntityBase)
LightSwitchPtr = LightSwitch.operator("ptr")

validate_device_class = cv.one_of(*DEVICE_CLASSES, lower=True, space="_")


LIGHT_SWITCH_SCHEMA = (
    cv.ENTITY_BASE_SCHEMA.extend(web_server.WEBSERVER_SORTING_SCHEMA)
    .extend(
        {
            cv.Optional(CONF_DEVICE_CLASS): validate_device_class,
        }
    )
)

_UNDEF = object()


def light_switch_schema(
    class_: MockObjClass,
    *,
    icon: str = _UNDEF,
    entity_category: str = _UNDEF,
    device_class: str = _UNDEF,
) -> cv.Schema:
    schema = {cv.GenerateID(): cv.declare_id(class_)}

    for key, default, validator in [
        (CONF_ICON, icon, cv.icon),
        (CONF_ENTITY_CATEGORY, entity_category, cv.entity_category),
        (CONF_DEVICE_CLASS, device_class, validate_device_class),
    ]:
        if default is not _UNDEF:
            schema[cv.Optional(key, default=default)] = validator

    return BUTTON_SCHEMA.extend(schema)


async def setup_light_switch_core_(var, config):
    await setup_entity(var, config)

    if device_class := config.get(CONF_DEVICE_CLASS):
        cg.add(var.set_device_class(device_class))

    if web_server_config := config.get(CONF_WEB_SERVER):
        await web_server.add_entity_config(var, web_server_config)


async def register_light_switch(var, config):
    if not CORE.has_id(config[CONF_ID]):
        var = cg.Pvariable(config[CONF_ID], var)
    cg.add(cg.App.register_light_switch(var))
    await setup_light_switch_core_(var, config)


async def new_light_switch(config, *args):
    var = cg.new_Pvariable(config[CONF_ID], *args)
    await register_light_switch(var, config)
    return var


@coroutine_with_priority(100.0)
async def to_code(config):
    cg.add_global(light_switch_ns.using)
    cg.add_define("USE_LIGHT_SWITCH")