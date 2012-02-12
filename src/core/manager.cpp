#include <manager.h>

using namespace Core;

void Manager::remove(ManagersObject * object)
{
    delete object;
}

const int Manager::new_id()
{
    const int id = objects_.size();
    objects_.reserve(objects_.size() + 1);
    return id;
}

void Manager::set_object(ManagersObject * object)
{
    objects_[object->id()] = object;
}
