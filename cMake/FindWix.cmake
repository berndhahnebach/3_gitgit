# Wix search

message(STATUS "Looking for WiX")
## Search for candle.exe
find_program(WIX_CANDLE_EXECUTABLE NAMES candle
  PATHS ${WIX_BINARY_DIR}
)

## Search for light.exe
find_program(WIX_LIGHT_EXECUTABLE NAMES light
  PATHS ${WIX_BINARY_DIR}
)

if (WIX_CANDLE_EXECUTABLE AND WIX_LIGHT_EXECUTABLE)
  message(STATUS "Looking for WiX - found")

  ## Set the WiX binary directory variable, but only if it wasn't already
  ## explicitly defined
  if (NOT WIX_BINARY_DIR)
    get_filename_component(WIX_BINARY_DIR ${WIX_LIGHT_EXECUTABLE} ABSOLUTE)
    get_filename_component(WIX_BINARY_DIR ${WIX_BINARY_DIR} PATH)
  endif(NOT WIX_BINARY_DIR)
else (WIX_CANDLE_EXECUTABLE AND WIX_LIGHT_EXECUTABLE)
  message(STATUS "Looking for WiX - not found.")
endif(WIX_CANDLE_EXECUTABLE AND WIX_LIGHT_EXECUTABLE)
