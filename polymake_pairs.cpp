#include "polymake_includes.h"

#include "polymake_tools.h"

#include "polymake_functions.h"

#include "polymake_type_modules.h"


void polymake_module_add_pairs(jlcxx::Module& polymake)
{

    auto type = polymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>, jlcxx::TypeVar<2>>>(
            "StdPair", jlcxx::julia_type("Any", "Base" ));

        type.apply<std::pair<pm::Int,pm::Int>>([](auto wrapped) {
            typedef typename decltype(wrapped)::type WrappedT;

            wrapped.template constructor();
            wrapped.template constructor<int64_t, int64_t>();

            /*
            wrapped.method("_getindex", [](const WrappedT& P, int64_t n) {
                return std::get<static_cast<pm::Int>(n) - 1>>(P);
            });*/


            wrapped.method("first", [](WrappedT& P) {
                return P.first;
            });

            wrapped.method("second", [](WrappedT& P) {
                return P.second;
            });
        });

    polymake.method("to_pair_int", [](const pm::perl::PropertyValue& pv) {
        return to_SmallObject<std::pair<pm::Int, pm::Int>>(pv);
    });

}
