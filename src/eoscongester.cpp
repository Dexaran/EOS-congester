#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/system.hpp>
#include <eosiolib/transaction.hpp>

using namespace eosio;

class [[eosio::contract("eoscongester")]] eoscongester : public contract {
  public:
      using contract::contract;
    
        eoscongester( eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds ): eosio::contract(receiver, code, ds), counter(receiver, code.value)
    {}

struct [[eosio::table]] countert
    {
        uint64_t id;
        uint64_t value;
        uint64_t allow_cyclic;

        uint64_t primary_key() const { return id; }
        EOSLIB_SERIALIZE( countert, (id)(value)(allow_cyclic))
    };

typedef eosio::multi_index<"countert"_n, countert> countertable;
countertable counter;

[[eosio::action]]
void call(uint64_t ix, uint64_t id)
{
     // ID is the number of iterations
     // i  is a dummy value to prevent tx duplications

     eosio::print("DEBUG PRINT");

     uint64_t b = 0;

     // 200K iterations will consume 22 MS of CPU per call.

     for (uint64_t i=0; i < id; i++)
     {
          uint64_t j = i * i;
          uint64_t k = j * i;
          b = j * k;
     }

     eosio::print("B: ", b);
}

[[eosio::action]]
void clear()
{
     auto e = counter.find(0);
     if(e != counter.end())
     {
          counter.erase(e);
     }
}

[[eosio::action]]
void setstat(uint64_t s)
{
     auto e = counter.find(0);
     if(counter.begin() == counter.end())
     {
          counter.emplace(get_self(), [&](auto& p) {
               p.id = 0;
               p.value = 0;
               p.allow_cyclic = s;
          });

          eosio::print("AAAL   ", counter.find(0)->allow_cyclic);
     }
     else
     {
          counter.modify(e, get_self(), [&](auto& p) {
               p.allow_cyclic = s;
          });
     }
}

[[eosio::action]]
void set(eosio::name from, uint64_t delay, uint64_t defer, uint64_t repeat, uint64_t id)
{
     auto e = counter.find(0);

     if(counter.begin() == counter.end())
     {
          counter.emplace(get_self(), [&](auto& p) {
               p.id = 0;
               p.value = 0;
               p.allow_cyclic = 1;
          });
          e = counter.find(0);
     }

     eosio::print("E->cycles:    ", e->allow_cyclic);
     eosio_assert(e->allow_cyclic > 0, "Cycles disabled by owner");

     for (uint64_t i = 0; i<repeat; i++)
     {
          counter.modify(e, get_self(), [&](auto& s) {
               s.value++;
          });

          eosio::transaction c{};
          c.actions.emplace_back(
               eosio::permission_level(_self, "active"_n),
               _self,
               "call"_n,
               std::make_tuple(i, id));
               c.delay_sec = defer;
          c.send(e->value, _self, true);
     }

     eosio::transaction t{};
     t.actions.emplace_back(
          eosio::permission_level(_self, "active"_n),
          _self,
          "set"_n,
          std::make_tuple(from, delay, defer, repeat, id));
     t.delay_sec = delay;
     t.send(now(), _self, true);
}
};


EOSIO_DISPATCH( eoscongester, (set)(setstat)(call)(clear))