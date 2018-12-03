#include "polymake_includes.h"
#include "polymake_tools.h"
#include "polymake_caller.h"
#include "polymake_functions.h"

#include <typeinfo>
#include <cxxabi.h>

void initialize_polymake()
{
    try {
        if (data.main_polymake_session == nullptr) {
            data.main_polymake_session = new polymake::Main;
            data.main_polymake_session->shell_enable();
            data.main_polymake_scope = new polymake::perl::Scope(
                data.main_polymake_session->newScope());
            std::cout << data.main_polymake_session->greeting() << std::endl;
        };
    }
    catch (const std::exception& e) {
        jl_error(e.what());
    }
}

polymake::perl::Object call_func_0args(std::string func) {
    return polymake::call_function(func);
}

polymake::perl::Object call_func_1args(std::string func, int arg1) {
    return polymake::call_function(func, arg1);
}

polymake::perl::Object call_func_2args(std::string func, int arg1, int arg2) {
    return polymake::call_function(func, arg1, arg2);
}

pm::perl::Object to_perl_object(pm::perl::PropertyValue v)
{
    pm::perl::Object obj;
    v >> obj;
    return v;
}

std::string typeinfo_helper(pm::perl::PropertyValue p, bool demangle) {
    PropertyValueHelper ph(p);

    if (!ph.is_defined()) {
      return "undefined";
    }
    if (ph.is_boolean()) {
      return "bool";
    }
    switch (ph.classify_number()) {
    // primitives
    case PropertyValueHelper::number_is_zero:
    case PropertyValueHelper::number_is_int:
      return "int";
    case PropertyValueHelper::number_is_float:
      return "double";

    // with typeinfo ptr (nullptr for Objects)
    case PropertyValueHelper::number_is_object:
      // some non-primitive Scalar type with typeinfo (e.g. Rational)
    case PropertyValueHelper::not_a_number:
      // a c++ type with typeinfo or a perl Object
      {
        const std::type_info* ti = ph.get_canned_typeinfo();
        if (ti == nullptr) {
          return "perl::Object";
        }
        // demangle:
        int status = -1;
        std::unique_ptr<char, void(*)(void*)> res {
          abi::__cxa_demangle(ti->name(), NULL, NULL, &status),
          std::free
        };
        return (status==0 && demangle) ? res.get() : ti->name();
      }
    }
    return "unknown";
}
