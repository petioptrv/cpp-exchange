#pragma once

#include <string>

namespace cppexchange {

    /**  Language codes to be used with the CPPExchange class */
    enum class LanguageCode { EN, DE, ES, FR };

    /**
     * @brief A class for saying hello in multiple languages
     */
    class CPPExchange {
        std::string name;

      public:
        /**
         * @brief Creates a new cpp-exchange
         * @param name the name to greet
         */
        CPPExchange(std::string name);

        /**
         * @brief Creates a localized string containing the greeting
         * @param lang the language to greet in
         * @return a string containing the greeting
         */
        std::string greet(LanguageCode lang = LanguageCode::EN) const;
    };

}  // namespace cppexchange
