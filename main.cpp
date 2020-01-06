#include <iostream>
#include <tuple>

#include "resolve_template_args.h"


using namespace std;

enum class E { E1, E2, E3 };

template <int i>
void ft1(double x) {
    cout << "ft1(): i = " << i << ", x = " << x << endl;
}

struct callFt1 {
    template<int i> void operator()(double x) const {
        ft1<i>(x);
    }
};

template <int i, E e>
void ft2(double x) {
    cout << "ft2(): i = " << i << ", e = " << static_cast<int>(e) << ", x = " << x << endl;
}

struct callFt2 {
    template<int i, int e> void operator()(double x) const {
        ft2<i, static_cast<E>(e)>(x);
    }
};

template <int i, E e, int j>
void ft3(double x) {
    cout << "ft3(): i = " << i << ", e = " << static_cast<int>(e) << ", j = " << j << ", x = " << x << endl;
}

struct callFt3 {
    template<int i, int e, int j> void operator()(double x) const {
        ft3<i, static_cast<E>(e), j>(x);
    }
};

void f(int i, E e, int j, double x)
{
    resolve_template_args<
            template_arg<int>::seq<1,2,3>
            >(make_tuple(i), callFt1(), x);
    resolve_template_args<
            template_arg<int>::seq<1,2,3>,
            template_arg<E>::seq<E::E1,E::E2,E::E3>
            >(make_tuple(i,e), callFt2(), x);
    resolve_template_args<
            template_arg<int>::seq<1,2,3>,
            template_arg<E>::seq<E::E1,E::E2,E::E3>,
            template_arg<int>::seq<4,5,6>
            >(make_tuple(i,e,j), callFt3(), x);
}

int main(int argc, char **argv)
{
    auto i = 2;
    auto e = E::E2;
    auto j = 5;
    auto x = 1.23;
    if (argc > 1) {
        i = atoi(argv[1]);
        if (argc > 2) {
            e = static_cast<E>(atoi(argv[2]));
            if (argc > 3) {
                j = atoi(argv[3]);
                if (argc > 4)
                    x = atof(argv[4]);
            }
        }
    }
    try {
        f(i, e, j, x);
        return EXIT_SUCCESS;
    }
    catch(const exception& e) {
        cerr << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}
