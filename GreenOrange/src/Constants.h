#pragma once

constexpr int OPENGL_MAJOR_VERSION = 4;
constexpr int OPENGL_MINOR_VERSION = 3;
constexpr const char *GLSL_VERSION = "#version 430 core";

constexpr int INPUT_STRING_MAX_SIZE = 64;
constexpr int STRING_BUFFER_MAX_SIZE = 128;

constexpr const char *DND_PAYLOAD = "DND_PAYLOAD";

constexpr const char *UNIFORM_DIMENSIONS = "dimensions";
constexpr const char *UNIFORM_TIME = "time";

constexpr const char *REPLACE_TRANSFORMS = "#GO_REPLACE_TRANSFORMS";
constexpr const char *REPLACE_OBJECTS = "#GO_REPLACE_OBJECTS";
constexpr const char *REPLACE_OPERATORS = "#GO_REPLACE_OPERATORS";
constexpr const char *REPLACE_SCENE_SDFS = "#GO_REPLACE_SCENE_SDFS";

constexpr const char *REPLACE_MATERIAL_COUNT = "#GO_REPLACE_MATERIAL_COUNT";
constexpr const char *REPLACE_MATERIALS_INIT = "#GO_REPLACE_MATERIALS_INIT";

constexpr const char *REPLACE_LIGHTS_INIT = "#GO_REPLACE_LIGHTS_INIT";
constexpr const char *REPLACE_DIR_LIGHT_COUNT = "#GO_REPLACE_DIR_LIGHT_COUNT";
constexpr const char *REPLACE_POINT_LIGHT_COUNT = "#GO_REPLACE_POINT_LIGHT_COUNT";
constexpr const char *REPLACE_DIR_LIGHTS_SET = "#GO_REPLACE_DIR_LIGHTS_SET";
constexpr const char *REPLACE_POINT_LIGHTS_SET = "#GO_REPLACE_POINT_LIGHTS_SET";

constexpr const float DRAG_MAX = 9999.0f;
constexpr const float DRAG_MIN = -DRAG_MAX;
