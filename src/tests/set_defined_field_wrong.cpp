#include <assert.h>

#include <iostream>
#include <typeinfo>

#include <person.h>

#define TESTINGUI_NAME L"TESTINGUI_SET_DEFINED_FIELD_WRONG"
#include "testingui.h"


int TestingUI::run()
{
    Core::Object *object = create<Core::Person>();

    assert(object);

    try
    {
        object->update(L"name", 0);
    }
    catch (boost::bad_any_cast& e)
    {   
        std::wcout << L"Catched exception :) "<< e.what() << std::endl;
        return 0;
    }

    return -1;
}

