#include "utils.h"

init_t::init_t(containers_lib_config_t* config) {
  containers_lib_config_t config_default;
  if (!config) {
    containers_lib_config_init(&config_default);
    config = &config_default;
  }
  containers_lib_init(config);
}

init_t::~init_t() {
  containers_lib_shutdown();
}
