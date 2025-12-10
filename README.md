# Reprint
A small playground for CRTP printers, variadic TMP tricks, and static dispatch patterns.

At least I warned you in the description; the code isn't exactly elegant, but I find the concept quite interesting. I'll keep it in mind and leave it here for archiving. I like templates, I don't know if you've noticed ;)

Coded entirely by me, and me alone, I'm trying to experiment with things like policy-based template programming and dynamic dispatch, but always with zero compile time. It's very raw, but I think there are some very interesting things to be gained from it. Well, until next time, and as always, feel free to contact me if you'd like to learn more about the magic and world of metaprogramming. I'm not an expert, but I'm passionate enough to know what I'm talking about ;)

## I find it extremely elegant and conceptually very promising.
#### CRTP + SFINAE + Zero-Overhead + Compile-Time Dispatch + Variadic TMP = very cool
```cpp
template<typename Variation>
class IPrinter {
public:
    template<typename... Args>
    auto print(Args&&... args) noexcept ->
        std::enable_if_t<std::is_void_v
            <decltype(std::declval<Variation>().print(std::declval<Args>()...))>,
        void> { static_cast<Variation*>(this)->print(std::forward<Args>(args)...); }
};

class ConsolePrinter : public IPrinter<ConsolePrinter> {
public:
    template<typename T>
    void print(const T& printable) const noexcept {
        std::cout << printable << std::endl;
    }
};
```

```cpp
int main() {
    IPrinter<ConsolePrinter> consolePrinter;

    consolePrinter.print("Hello World!");
    consolePrinter.print(67);
}
```

### As always, here's the rest of the code ;)
```cpp
// Copyright (c) December 2025 Félix-Olivier Dumas. All rights reserved.
// Licensed under the terms described in the LICENSE file

#include <iostream>
#include <string>

template<typename Variation>
class IPrinter {
public:
    template<typename... Args>
    auto print(Args&&... args) noexcept ->
        std::enable_if_t<std::is_void_v
            <decltype(std::declval<Variation>().print(std::declval<Args>()...))>,
        void> { static_cast<Variation*>(this)->print(std::forward<Args>(args)...); }
};

class ConsolePrinter : public IPrinter<ConsolePrinter> {
public:
    template<typename T>
    void print(const T& printable) const noexcept {
        std::cout << printable << std::endl;
    }
};

struct GenericStoragePolicy {
    template<typename T>
    struct Storage {
        [[nodiscard]] std::string to_string() const noexcept {
            return std::to_string(data_);
        }

    private:
        T data_{};
    };
};

struct ConsolePrintingPolicy {
    struct Printer {
        template<typename... Ts>
        void print(Ts&&... args) const noexcept {
            (..., (std::cout << args << std::endl));
        }
    };
};


template<typename StoragePolicy, typename PrintingPolicy>
class DynamicStorage {
public:
    void print_storage() const noexcept {
        printer_.print(storage_.to_string());
    }

private:
    typename StoragePolicy::template Storage<int> storage_;
    typename PrintingPolicy::Printer printer_;
};



class DynamicBase {
public:
    template<typename Derived, typename... Args>
    auto print(Args&&... args) noexcept ->
        std::enable_if_t<std::is_base_of_v<DynamicBase, Derived>
        && std::is_void_v<decltype(
            std::declval<Derived>().print(std::declval<Args>()...)
        )>, void> { static_cast<Derived*>(this)->print(std::forward<Args>(args)...); }
};

class StringPrinter : public DynamicBase {
public:
    void print(const std::string& printable) const noexcept {
        std::cout << printable << std::endl;
    }

private:
};

class IntegerPrinter : public DynamicBase {
public:
    void print(const int printable) const noexcept {
        std::cout << printable << std::endl;
    }
};


int main() {
    DynamicStorage<GenericStoragePolicy, ConsolePrintingPolicy> s;


    DynamicBase base;

    base.print<StringPrinter>("test");
    base.print<IntegerPrinter>(5);


    IPrinter<ConsolePrinter> consolePrinter;

    consolePrinter.print(5);
}
```
