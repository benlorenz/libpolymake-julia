#ifndef POLYMAKE_WRAP_FUNCTIONS
#define POLYMAKE_WRAP_FUNCTIONS


void initialize_polymake(bool interactive);

pm::perl::Object to_perl_object(const pm::perl::PropertyValue&);

std::string typeinfo_helper(const pm::perl::PropertyValue& p, bool demangle);

template <typename T> T to_SmallObject(const pm::perl::PropertyValue& pv)
{
    T obj = pv;
    return obj;
};

pm::Integer new_integer_from_bigint(jl_value_t*);

// We can do better templating here
template <typename T>
std::string show_small_object(const T& obj, bool print_typename = true)
{
    std::ostringstream buffer;
    auto               wrapped_buffer = wrap(buffer);
    if (print_typename) {
        wrapped_buffer << polymake::legible_typename(typeid(obj)) << pm::endl;
    }
    wrapped_buffer << obj;
    return buffer.str();
}

struct pm_VecOrMat_supported
{
    typedef jlcxx::ParameterList<int, pm::Integer, pm::Rational, double> value_type;
};

struct pm_Polynomial_supported
{
    typedef jlcxx::ParameterList<int, pm::Integer, pm::Rational, double,
        pm::Polynomial<int,int>, pm::Polynomial<pm::Integer,int>, pm::Polynomial<pm::Rational,int>, pm::Polynomial<double,int>> value_type;
};

#endif
