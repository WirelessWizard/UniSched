#include <group.h>

Group::Group(std::string ID, std::string name, std::string description)
{
    ID_ = ID;
    name_ = name;
    description_ = description;
    calendar_ = new Calendar('c' + ID, "");
}

Group::~Group(void)
{
    delete calendar_;
}

std::string Group::get_Name(void)
{
    return name_;
}

std::string Group::get_ID(void)
{
    return ID_;
}

std::string Group::get_Description(void)
{
    return description_;
}

Calendar *Group::get_Calendar(void)
{
    return calendar_;
}

std::vector<Person*> *Group::get_People(void)
{
    return &people_;
}

void Group::add_Event(Event *adding)
{
    for (std::vector<Person *>::iterator it = people_.begin(); it != people_.end(); it ++)
        (*it)->add_Event(adding);
    calendar_->add_Event(adding);
}

void Group::delete_Event(Event *deleting)
{
    for (std::vector<Person *>::iterator it = people_.begin(); it != people_.end(); it ++)
        (*it)->delete_Event(deleting);
    calendar_->delete_Event(deleting);
}

void Group::add_Person(Person *adding)
{
    adding->add_Group(this);
    people_.push_back(adding);
}

void Group::delete_Person(Person *deleting)
{
    deleting->delete_Group(this);
    for (std::vector<Person*>::iterator it = people_.begin(); it != people_.end(); it ++)
        if (*it == deleting)
        {
            it = people_.erase(it);
            break;
        }
}

