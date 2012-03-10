#include <abstractui.h>
#include <person.h>
#include <group.h>
#include <event.h>

#ifdef WITH_YAML
#include <yaml-cpp/yaml.h>
#endif /* WITH_YAML */

using namespace Core;

#ifdef WITH_YAML

void AbstractUI::dump(const std::string& base_fname) const
{
    YAML::Node out;
    
    for (std::map<objid_t, Object *>::const_iterator iter = objects_.begin();
         iter != objects_.end(); iter ++)
    {
        YAML::Node obj;
        obj["Object"] = iter->second->type();
        obj["ID"] = iter->first;
        // Just fuck-off, dude :)
        /*
        switch (iter->second->type()) {
        case PERSON:
            obj["VCard"] = *static_cast<Person *>(iter->second);
        break;
        case GROUP:
            obj["VCard"] = *static_cast<Group *>(iter->second);
        break;
        case EVENT:
            obj["VCard"] = *static_cast<Event *>(iter->second);
        break;
        default:
            obj["VCard"] = "No information";
        break;
        }
        */
        out.push_back(obj);
        
    }

    std::ofstream base(base_fname);

    base << out;
    
    base.close();
}

bool AbstractUI::load(const std::string& base_fname)
{
    YAML::Node in = YAML::LoadFile(base_fname);

    for (YAML::const_iterator iter = in.begin(); iter != in.end(); iter ++)
    {
        const obj_t type = (*iter)["Object"].as<obj_t>();
        const objid_t id = (*iter)["ID"].as<objid_t>();
        const std::map<const std::string, boost::any> vcard =
            (*iter)["VCard"].as<std::map<const std::string, boost::any>>();
        switch (type) {
        case PERSON:
            add_object<Person>(id, vcard);
        break;
        case GROUP:
            add_object<Group>(id, vcard);
        break;
        case EVENT:
            add_object<Event>(id, vcard);
        break;
        default:
            return false;
        break;
        }
    }
}
/* Template method in the .cpp? No way if it is not internal. Be careful. */
template <typename T>
void AbstractUI::add_object(objid_t id, const std::map<const std::string, boost::any>& fields)
{
    Object *new_obj = new T(id, *this);
    for (auto iter = fields.begin(); iter != fields.end(); iter ++)
    {
        new_obj->update(iter->first, iter->second);
    }
    objects_[id] = new_obj;
}

#endif /* WITH_YAML */

static const bool operator==(const boost::any& lhs, const boost::any& rhs)
    throw (boost::bad_any_cast)
{
    if (lhs.empty() != rhs.empty())
    {
        return false;
    }
    if (typeid(std::string) == lhs.type())
    {
        return boost::any_cast<std::string>(lhs)
            == boost::any_cast<std::string>(rhs);
    }
    if (typeid(time_t) == lhs.type())
    {
        return boost::any_cast<time_t>(lhs)
            == boost::any_cast<time_t>(rhs);
    }
    throw boost::bad_any_cast();
}

std::vector<Object *> AbstractUI::search(const std::vector<std::pair<std::string, boost::any>>& parameters)
{
    std::vector<Object *> results;
    for (auto m: objects_) {
        results.push_back(m.second);
    }
    for (std::pair<std::string, boost::any> parameter: parameters)
    {
        for (auto it = results.begin(); it != results.end(); it++)
        {
            try
            {
                if (parameter.second == (*it)->read(parameter.first))
                {
                    continue;
                }
            }
            catch (std::bad_cast)
            {}
            it = results.erase(it);
        }
    }

    for (Object * object: results)
    {
        cache_.push_back(object);
    }

    return results;
}

