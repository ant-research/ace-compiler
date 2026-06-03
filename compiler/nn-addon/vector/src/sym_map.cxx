#include "nn/vector/sym_map.h"

namespace nn {
namespace vector {

// addr_datum map
ADDR_DATUM_PTR Get_map_datum(ADDR_DATUM_PTR old_sym, const FUNC_SCOPE* fscope,
                             MAP_DATUM& dmap) {
  MAP_DATUM::iterator iter = dmap.find(old_sym->Id());
  AIR_ASSERT(iter != dmap.end());

  ADDR_DATUM_PTR new_sym = fscope->Addr_datum(ADDR_DATUM_ID(iter->second));
  AIR_ASSERT(new_sym != ADDR_DATUM_PTR());
  return new_sym;
}

void Set_map_datum(ADDR_DATUM_PTR old_sym, ADDR_DATUM_PTR new_sym,
                   MAP_DATUM& dmap) {
  // Check for existence for old_sym
  AIR_ASSERT_MSG(dmap.find(old_sym->Id()) == dmap.end(),
                 "A variable is assigned multiple times.");
  std::pair<MAP_DATUM::iterator, bool> res =
      dmap.insert({old_sym->Id(), new_sym->Id()});
  AIR_ASSERT(res.second == true);
}

// preg map
PREG_PTR Get_map_preg(PREG_PTR old_sym, const FUNC_SCOPE* fscope,
                      MAP_PREG& pmap) {
  MAP_PREG::iterator iter = pmap.find(old_sym->Id());
  AIR_ASSERT(iter != pmap.end());

  PREG_PTR new_sym = fscope->Preg(PREG_ID(iter->second));
  return new_sym;
}

void Set_map_preg(PREG_PTR old_sym, PREG_PTR new_sym, MAP_PREG& pmap) {
  AIR_ASSERT_MSG(pmap.find(old_sym->Id()) == pmap.end(),
                 "A preg is assigned multiple times.");
  std::pair<MAP_PREG::iterator, bool> res =
      pmap.insert({old_sym->Id(), new_sym->Id()});
  AIR_ASSERT(res.second == true);
}

// preg-datum map
bool Is_preg_in_map(PREG_PTR old_sym, MAP_PREG_DATUM& pmap) {
  if (pmap.find(old_sym->Id()) != pmap.end())
    return true;
  else
    return false;
}

ADDR_DATUM_PTR Get_map_preg_datum(PREG_PTR old_sym, const FUNC_SCOPE* fscope,
                                  MAP_PREG_DATUM& pmap) {
  MAP_PREG_DATUM::iterator iter = pmap.find(old_sym->Id());
  AIR_ASSERT(iter != pmap.end());

  ADDR_DATUM_PTR new_sym = fscope->Addr_datum(ADDR_DATUM_ID(iter->second));
  AIR_ASSERT(new_sym != ADDR_DATUM_PTR());
  return new_sym;
}

void Set_map_preg_datum(PREG_PTR old_sym, ADDR_DATUM_PTR new_sym,
                        MAP_PREG_DATUM& pmap) {
  AIR_ASSERT_MSG(pmap.find(old_sym->Id()) == pmap.end(),
                 "A preg is assigned multiple times.");
  std::pair<MAP_PREG_DATUM::iterator, bool> res =
      pmap.insert({old_sym->Id(), new_sym->Id()});
  AIR_ASSERT(res.second == true);
}

}  // namespace vector
}  // namespace nn
