#pragma once

#include <string>
#include <cstdint>

struct sockaddr_in;

std::string socket_type_to_string(uint64_t type);
std::string socket_family_to_string(uint64_t family);
std::string sockaddr_to_string(const sockaddr_in * addr);
std::string event_mask_to_string(uint64_t mask);
