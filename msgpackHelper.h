﻿#pragma once

#include <msgpack.hpp>
#include <memory>

struct object_with_zone {

  object_with_zone(const msgpack::object& obj, std::unique_ptr<msgpack::zone> z)
      : object(obj), zone(std::move(z)) {}

  ~object_with_zone() = default;

  msgpack::object object;
  std::unique_ptr<msgpack::zone> zone;
};
