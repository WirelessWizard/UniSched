#include <iostream>

#include <abstractui.h>
#include <abstractstorage.h>
#include <module.h>


namespace utils {

bool
select_modules (Core::AbstractUI **ui, Core::AbstractStorage **storage,
               std::vector<std::string>& args);

} // namespace utils end

