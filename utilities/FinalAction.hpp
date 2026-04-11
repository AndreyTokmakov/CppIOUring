/**============================================================================
Name        : FinalAction.hpp
Created on  : 19.10.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FinalAction.hpp
============================================================================**/

#ifndef CPPPROJECTS_FINALACTION_HPP
#define CPPPROJECTS_FINALACTION_HPP

#include <concepts>

namespace final_action
{
    template<typename Fn>
    struct [[nodiscard]] ScopeExit
    {
        template<typename Func>
        requires std::constructible_from<Fn, Func> &&
            std::is_nothrow_constructible_v<Fn, Func>
        explicit ScopeExit(Func func): exitFunction { std::forward<Func>(func) } {
        }

        ScopeExit(const ScopeExit&) = delete;
        ScopeExit(ScopeExit&&) noexcept = delete;

        ScopeExit& operator=(const ScopeExit&) = delete;
        ScopeExit& operator=(ScopeExit&&) noexcept = delete;

        ~ScopeExit() noexcept
        {
            if (callOnExit) {
                exitFunction();
            }
        }

        void release() noexcept {
            callOnExit = false;
        }

    private:

        [[no_unique_address]]
        Fn exitFunction;

        bool callOnExit { true };
    };

    template<typename Func>
    ScopeExit(Func) -> ScopeExit<Func>;
}

#endif //CPPPROJECTS_FINALACTION_HPP