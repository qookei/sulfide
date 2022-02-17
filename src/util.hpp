#pragma once

// overloaded{...}

template <typename T>
struct inheritable_helper : T {};

template <typename T>
concept inheritable = requires {
	{ inheritable_helper<T>{} };
};

template <typename T>
concept lambda = inheritable<T> && requires {
	{ &T::operator() };
};

template <lambda ...Ts>
struct overloaded : Ts... { using Ts::operator()...; };

template <lambda ...Ts>
overloaded(Ts...) -> overloaded<Ts...>;
