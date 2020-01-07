#include <iostream>
#include <tuple>

#include "resolve_template_args.h"

using namespace std;

enum class E
{
    E1,
    E2,
    E3
};

template <int i>
void ft1(double x)
{
    cout << "ft1(): i = " << i << ", x = " << x << endl;
}

template <int i, E e>
void ft2(double x)
{
    cout << "ft2(): i = " << i << ", e = " << static_cast<int>(e)
         << ", x = " << x << endl;
}

template <int i, E e, int j>
void ft3(double x)
{
    cout << "ft3(): i = " << i << ", e = " << static_cast<int>(e)
         << ", j = " << j << ", x = " << x << endl;
}

void f(int i, E e, int j, double x)
{
    resolve_template_args<std::make_integer_sequence<int, 3>>(
        make_tuple(i), []<int I>(auto&& x) { ft1<I>(x); }, x);

    resolve_template_args<
        std::make_integer_sequence<int, 3>,
        std::integer_sequence<E, E::E2, E::E3>>(
        make_tuple(i, e),
        []<int i, int e>(auto&& x) { ft2<i, static_cast<E>(e)>(x); },
        x);

    resolve_template_args<
        std::integer_sequence<int, 1, 2, 3>,
        std::integer_sequence<E, E::E1, E::E2, E::E3>,
        std::integer_sequence<int, 4, 5, 6>>(
        make_tuple(i, e, j),
        []<int i, int e, int j>(auto&& x) { ft3<i, static_cast<E>(e), j>(x); },
        x);
}

int main(int argc, char** argv)
{
    auto i = 2;
    auto e = E::E2;
    auto j = 5;
    auto x = 1.23;
    if (argc > 1)
    {
        i = atoi(argv[1]);
        if (argc > 2)
        {
            e = static_cast<E>(atoi(argv[2]));
            if (argc > 3)
            {
                j = atoi(argv[3]);
                if (argc > 4)
                    x = atof(argv[4]);
            }
        }
    }
    try
    {
        f(i, e, j, x);
        return EXIT_SUCCESS;
    }
    catch (const exception& e)
    {
        cerr << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}
