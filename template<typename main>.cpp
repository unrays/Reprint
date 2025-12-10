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
