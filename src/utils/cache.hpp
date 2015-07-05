#pragma once
/*
   Copyright (c) 2015, Gyula Laszlo, Starschema

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   */

#include <cstdio>

#include <map>
#include <string>

#include "utils/logger.hpp"

namespace tableauFS
{
  template <class K, class V>
  struct Cache {
    // construct a cache without a name
    Cache() : name(""), cache() {}
    // construct a cache with a name
    Cache(std::string name) : name(std::move(name)), cache() {}
    ~Cache() {}

    template <class Pred>
    V get(const K& key, Pred pred)
    {
      if (cache.count(key) > 0) {
        log::info("[CACHE] Getting %s for '%s' from cache -----", name.c_str(),
                  key.c_str());
        return cache[key];
      }

      const auto res = pred();
      log::info("[CACHE] Saving %s '%s' to cache -----", name.c_str(),
                key.c_str());
      cache.emplace(std::make_pair(key, res));
      return res;
    }

    std::string name;
    std::map<K, V> cache;
  };
}
