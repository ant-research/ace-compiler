#include <map>

#include "air/base/container.h"
#include "air/base/st.h"

namespace nn {
namespace vector {

using namespace air::base;

// Common functions to record the mapping between old_sym and new_sym.
// <ID, ID>
using MAP_DATUM      = std::map<ADDR_DATUM_ID, ADDR_DATUM_ID>;
using MAP_PREG       = std::map<PREG_ID, PREG_ID>;
using MAP_PREG_DATUM = std::map<PREG_ID, ADDR_DATUM_ID>;

// "datum-datum" map
ADDR_DATUM_PTR Get_map_datum(ADDR_DATUM_PTR old_sym, const FUNC_SCOPE* fscope,
                             MAP_DATUM& dmap);
void           Set_map_datum(ADDR_DATUM_PTR old_sym, ADDR_DATUM_PTR new_sym,
                             MAP_DATUM& dmap);

// "preg-preg" map
PREG_PTR Get_map_preg(PREG_PTR old_sym, const FUNC_SCOPE* fscope,
                      MAP_PREG& pmap);
void     Set_map_preg(PREG_PTR old_sym, PREG_PTR new_sym, MAP_PREG& pmap);

// "preg-datum" map
bool           Is_preg_in_map(PREG_PTR old_sym, MAP_PREG_DATUM& pmap);
ADDR_DATUM_PTR Get_map_preg_datum(PREG_PTR old_sym, const FUNC_SCOPE* fscope,
                                  MAP_PREG_DATUM& pmap);
void           Set_map_preg_datum(PREG_PTR old_sym, ADDR_DATUM_PTR new_sym,
                                  MAP_PREG_DATUM& pmap);

}  // namespace vector
}  // namespace nn
