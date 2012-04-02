#include <abstractui.h>
#include <person.h>
#include <group.h>
#include <event.h>

#include <boost/format.hpp>

extern "C" {

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

};

class luaUI : public Core::AbstractUI {
friend int luaUI_create(lua_State *state);
friend int luaUI_search(lua_State *state);
friend int luaUI_remove(lua_State *state);
friend int luaUI_object_read(lua_State *state);
friend int luaUI_object_update(lua_State *state);
private:
    std::string script_;
    lua_State *vm_;
    std::vector<Core::Object *> objects_;
protected:
public:
    luaUI(std::vector<Core::Module *>* modules, void *handle);

    virtual void init(const std::vector<std::string>& args);
    virtual int run();
};

luaUI::luaUI(std::vector<Core::Module *>* modules, void *handle):
        AbstractUI("luaui", modules, handle)
{
}

void luaUI::init(const std::vector<std::string>& args)
{
    for (auto it = args.begin(); it != args.end(); it++)
    {
        if ("--lua" == *it)
        {
            script_ = *++it;
            continue;
        }
    }

    if (script_.empty())
    {
        std::cerr << "lua UI: script name not set!" << std::endl;
    }
}

static luaUI *self;

int luaUI_create(lua_State *state)
{
    if (lua_gettop(state) != 1)
    {
        lua_pushstring(state, "Incorrect argument!");
        lua_error(state);
        return 0;
    }
    std::string type = lua_tostring(state, 1);
    if ("person" == type)
    {
        self->objects_.push_back(self->create<Core::Person>());
    }
    else if ("group" == type)
    {
        self->objects_.push_back(self->create<Core::Group>());
    }
    else if ("event" == type)
    {
        self->objects_.push_back(self->create<Core::Event>());
    }
    else
    {
        lua_pushstring(state, "Unknown type!");
        lua_error(state);
        return 0;
    }

    lua_createtable(state, 0, 0);
    lua_pushstring(state, type.c_str());
    lua_setfield(state, -2, "__type");
    lua_pushnumber(state, self->objects_.size() - 1);
    lua_setfield(state, -2, "__id");
    lua_getglobal(state, "__object");
    lua_setmetatable(state, -2);
    return 1;
}

int luaUI_search(lua_State *state)
{
    if (lua_gettop(state) != 1 || lua_istable(state, 1))
    {
        lua_pushstring(state, "Incorrect argument!");
        lua_error(state);
        return 0;
    }

    lua_pushnil(state);
    std::map<std::string, boost::any> args;
    while(lua_next(state, -2))
    {
        if (!lua_isstring(state, -2) || lua_isstring(state, -1))    //TODO: add seacrh by time.
        {
            std::cerr << "lua UI: search argument is not string!" << std::endl;
            continue;
        }
        args[lua_tostring(state, -2)] = lua_tostring(state, -1);
        lua_pop(state, 1);
    }

    auto ret = self->search(args);

    int i = 1;
    lua_createtable(state, 0, 0);
    for (auto found : ret)
    {
        lua_createtable(state, 0, 0);
        switch (found->type())
        {
        case Core::PERSON:
            lua_pushstring(state, "person");
            break;
        case Core::GROUP:
            lua_pushstring(state, "group");
            break;
        case Core::EVENT:
            lua_pushstring(state, "event");
            break;
        default:
            lua_pushstring(state, "unknown");
            break;
        }
        lua_setfield(state, -2, "__type");
        lua_pushnumber(state, self->objects_.size());
        self->objects_.push_back(found);
        lua_setfield(state, -2, "__id");
        lua_getglobal(state, "__object");
        lua_setmetatable(state, -2);
        lua_setfield(state, -2, boost::str(boost::format("%u") % i++).c_str());
    }

    return 1;
}

int luaUI_remove(lua_State *state)
{
    if (lua_gettop(state) != 1 || !lua_istable(state, 1))
    {
        lua_pushstring(state, "Invalid argument!");
        lua_error(state);
        return 0;
    }
    lua_getfield(state, 1, "__id");
    if (!lua_isnumber(state, -1))
    {
        lua_pushstring(state, "This is not object!");
        lua_error(state);
        return 0;
    }
    self->remove(self->objects_.at(lua_tonumber(state, -1)));
    return 0;
}

int luaUI_object_read(lua_State *state)
{
    if (lua_gettop(state) != 2 || !lua_istable(state, 1) || !lua_isstring(state, 2))
    {
        lua_pushstring(state, "Invalid argument!");
        lua_error(state);
        return 0;
    }
    lua_getfield(state, 1, "__id");
    if (!lua_isnumber(state, -1))
    {
        lua_pushstring(state, "This is not object!");
        lua_error(state);
        return 0;
    }
    auto ret = self->objects_.at(lua_tonumber(state, -2))->read(lua_tostring(state, 2));
    if (ret.type() == typeid(const std::string))
    {
        lua_pushstring(state, boost::any_cast<const std::string&>(ret).c_str());
    }
    else if (ret.type() == typeid(const time_t))
    {
        lua_pushnumber(state, boost::any_cast<const time_t>(ret));
    }
    else if (ret.type() == typeid(const std::vector<Core::Object *>))
    {
        lua_createtable(state, 0, 0);
        int i = 0;
        for (auto object : boost::any_cast<const std::vector<Core::Object *>>(ret))
        {
            lua_createtable(state, 0, 0);
            switch (object->type())
            {
            case Core::PERSON:
                lua_pushstring(state, "person");
                break;
            case Core::GROUP:
                lua_pushstring(state, "group");
                break;
            case Core::EVENT:
                lua_pushstring(state, "event");
                break;
            default:
                lua_pushstring(state, "unknown");
                break;
            }
            lua_setfield(state, -2, "__type");
            lua_pushnumber(state, self->objects_.size());
            self->objects_.push_back(object);
            lua_setfield(state, -2, "__id");
            lua_getglobal(state, "__object");
            lua_setmetatable(state, -2);
            lua_setfield(state, -2, boost::str(boost::format("%u") % i++).c_str());
        }
    }
    else
    {
        lua_pushstring(state, "Unknown type!");
        lua_error(state);
        return 0;
    }
    return 1;
}

static int luaUI_object___index(lua_State *state)
{
    return luaUI_object_read(state);
}

int luaUI_object_update(lua_State *state)
{
    if (lua_gettop(state) != 3 || !lua_istable(state, 1) || !lua_isstring(state, 2) || (!lua_isnumber(state, 3) and !lua_isstring(state, 3)))
    {
        lua_pushstring(state, "Invalid arguments!");
        lua_error(state);
        return 0;
    }
    lua_getfield(state, 1, "__id");
    if (!lua_isnumber(state, -1))
    {
        lua_pushstring(state, "This is not table!");
        lua_error(state);
        return 0;
    }
    std::string name = lua_tostring(state, 2);
    boost::any value;
    if (lua_isnumber(state, 3))
    {
        value = (time_t) lua_tonumber(state, 3);
    }
    else
    {
        value = std::string(lua_tostring(state, 3));
    }
    self->objects_.at(lua_tonumber(state, -1))->update(name, value);
    return 0;
}

static int luaUI_object___newindex(lua_State *state)
{
    return luaUI_object_update(state);
}

int luaUI::run()
{
    self = this;
    vm_ = lua_open();
    lua_createtable(vm_, 0, 0);
    lua_pushcfunction(vm_, luaUI_object_read);
    lua_setfield(vm_, -2, "read");
    lua_pushcfunction(vm_, luaUI_object___index);
    lua_setfield(vm_, -2, "__index");
    lua_pushcfunction(vm_, luaUI_object_update);
    lua_setfield(vm_, -2, "update");
    lua_pushcfunction(vm_, luaUI_object___newindex);
    lua_setfield(vm_, -2, "__newindex");
    lua_setglobal(vm_, "__object");
    
    luaL_openlibs(vm_);
    lua_register(vm_, "create", luaUI_create);
    lua_register(vm_, "search", luaUI_create);
    lua_register(vm_, "remove", luaUI_create);

    luaL_loadfile(vm_, script_.c_str());
    lua_pcall(vm_, 0, LUA_MULTRET, 0);
    
    lua_close(vm_);

    return 0;
}

extern "C" {

void init(std::vector<Core::Module *>* modules, void *handle)
{
    new luaUI(modules, handle);
}

};


