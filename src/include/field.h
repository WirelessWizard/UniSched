#pragma once

#include <string>
#include <set>
#include <utility>
#include <vector>

#include <managersobject.h>

namespace Core {

class Field {
public:
    enum Type {
        STRING,
        TIME,
        ENUM,
        LINK,
        VECTOR
    };

private:
    const std::string name_;
    const enum Type type_;

protected:
public:
    Field(const Type type, const std::string& name) throw ():
        type_(type), name_(name)
    {}

    const std::string& name() const throw()
    {
        return name_;
    }

    const Type type() const throw()
    {
        return type_;
    }

    virtual const Field& operator=(const Field& field) = 0;
};

class FieldString : public Field {
private:
    std::string value_;
protected:
public:
    FieldString(const std::string& name, const std::string& value = "")
        throw ():
        Field(STRING, name), value_(value)
    {}

    const std::string& value(const std::string& value) throw ()
    {
        return value_ = value;
    }

    const std::string& value() const throw ()
    {
        return value_;
    }

    virtual Field& operator=(const Field& field)
    {
        value(dynamic_cast<const FieldString&>(field).value());
        return *this;
    }
};

class FieldTime : public Field {
private:
    time_t value_;
protected:
public:
    FieldTime(const std::string& name, const time_t& value = 0)
        throw ():
        Field(TIME, name), value_(value)
    {}

    const time_t value(const time_t& value) throw ()
    {
        return value_ = value;
    }

    const time_t value() const throw ()
    {
        return value_;
    }

    virtual const Field& operator=(const Field& field)
    {
        value(dynamic_cast<const FieldTime&>(field).value());
        return *this;
    }
};

class FieldEnum : public Field {
private:
    const std::set<std::string>& valid_;
    std::string value_;

protected:
public:
    FieldEnum(const std::string& name, const std::set<std::string>& valid,
              const std::string& value):
        Field(ENUM, name), valid_(valid), value_(*valid.find(value))
    {}

    FieldEnum(const std::string& name, const std::set<std::string>& valid)
        throw ():
        Field(ENUM, name), valid_(valid), value_(*valid.begin())
    {}

    const std::string& value(const std::string& value)
    {
        return value_ = *(valid_.find(value));
    }

    const std::string& value() const throw()
    {
        return value_;
    }

    virtual const Field& operator=(const Field& field) 
    {
        value(dynamic_cast<const FieldEnum&>(field).value());
        return *this;
    }
};

class FieldLink : public Field {
private:
    std::pair<ManagersObject *, bool> value_;

protected:
public:
    FieldLink(const std::string& name,
                const std::pair<ManagersObject *, bool>& value) throw():
        Field(LINK, name), value_(value)
    {}

    
    const std::pair<ManagersObject *, bool>&
    value(const std::pair<ManagersObject *, bool>& value) throw()
    {
        return value_ = value;
    }

    const std::pair<ManagersObject *, bool>&
    value() const throw()
    {
        return value_;
    }

    virtual const Field& operator=(const Field& field)
    {
        value(dynamic_cast<const FieldLink&>(field).value());
        return *this;
    }
};

class FieldVector : public Field {
private:
    std::vector<ManagersObject *> vector_;
protected:
public:
    FieldVector(const std::string& name,
                const std::vector<ManagersObject *>& vector) throw():
        Field(VECTOR, name), vector_(vector)
    {}

    const std::vector<ManagersObject *>&
    vector(const std::vector<ManagersObject *>& vector) throw()
    {
        return vector_ = vector;
    }

    const std::vector<ManagersObject *>& vector() const throw()
    {
        return vector_;
    }

    virtual const Field& operator=(const Field& field)
    {
        vector(dynamic_cast<const FieldVector&>(field).vector());
        return *this;
    }

    void add(ManagersObject * object)
    {
        for (ManagersObject *obj : vector_)
        {
            if (obj == object)
            {
                return;
            }
        }
        vector_.push_back(object);
    }

    void del(ManagersObject * object)
    {
        for (auto it = vector_.begin(); it != vector_.end(); it++)
        {
            if (*it == object)
            {
                vector_.erase(it);
                return;
            }
        }
    }

    void commit(const FieldLink& field)
    {
        if (field.value().second)
            this->add(field.value().first);
        else
            this->del(field.value().first);
    }
};

}
