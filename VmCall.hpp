#pragma once

#include "Zookeeper.hpp"

int vmcall_dispatch(Cpu &cpu, uint32_t call, uint32_t addr);
