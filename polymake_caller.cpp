#define INCLUDED_FROM_CALLER

#include "polymake_caller.h"

#include "polymake_tools.h"

#include <vector>

static auto type_map_translator = new std::map<std::string, jl_value_t**>();

void insert_type_in_map(std::string&& ptr_name, jl_value_t** var_space)
{
    type_map_translator->emplace(std::make_pair(ptr_name, var_space));
}

void set_julia_type(std::string name, void* type_address)
{
    jl_value_t** address;
    try {
        address = (*type_map_translator)[name];
    }
    catch (std::exception& e) {
        std::cerr << "This should never happen: " << name << std::endl;
        return;
    }
    memcpy(address, &type_address, sizeof(jl_value_t*));
}

#include "generated/polymake_call_function_feed_argument.h"

std::vector<std::string>
create_template_vector(jlcxx::ArrayRef<std::string> template_parameters)
{
    size_t                   number_templates = template_parameters.size();
    std::vector<std::string> return_vector(number_templates);
    for (size_t i = 0; i < number_templates; i++) {
        return_vector[i] = template_parameters[i];
    }
    return return_vector;
}

template <bool VoidContext = false>
using funcall_type = std::conditional_t<VoidContext,void,pm::perl::PropertyValue>;

// Visualization in polymake only works if the function is called and
// then immediately released,i.e. not converted to a property value
template<bool VoidContext = false>
auto polymake_call_function(
    std::string                     function_name,
    jlcxx::ArrayRef<std::string, 1> template_parameters,
    jlcxx::ArrayRef<jl_value_t*, 1> arguments)
    -> funcall_type<VoidContext>
{
    std::vector<std::string> template_vector =
        create_template_vector(template_parameters);
    size_t argument_list = arguments.size();
    auto   function = polymake::prepare_call_function(function_name, template_vector);
    for (auto arg : arguments)
        polymake_call_function_feed_argument(function, arg);
    return static_cast<funcall_type<VoidContext>>(function());
}

// Visualization in polymake only works if the method is called and
// then immediately released,i.e. not converted to a property value
template<bool VoidContext = false>
auto polymake_call_method(
    std::string                     function_name,
    pm::perl::BigObject*            object,
    jlcxx::ArrayRef<jl_value_t*, 1> arguments)
    -> funcall_type<VoidContext>
{
    auto   function = object->prepare_call_method(function_name);
    for (auto arg : arguments)
        polymake_call_function_feed_argument(function, arg);
    return static_cast<funcall_type<VoidContext>>(function());
}

void polymake_module_add_caller(jlcxx::Module& polymake)
{
    polymake.method("internal_call_function", &polymake_call_function<false>);
    polymake.method("internal_call_function_void",
                    &polymake_call_function<true>);
    polymake.method("internal_call_method", &polymake_call_method<false>);
    polymake.method("internal_call_method_void", &polymake_call_method<true>);
    polymake.method("set_julia_type", &set_julia_type);
}
