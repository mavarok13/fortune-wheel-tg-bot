#pragma once

#include "WheelItem.hpp"
#include "WheelMode.hpp"

#include <cstddef>
#include <string>
#include <vector>

class Wheel {
public:
    void setName(const std::string & name);

    void addItem(const std::string & name);
    void renameItem(const std::string& oldName, const std::string& newName);
    void removeItem(const std::string& name);
    void clear();
    void reset();

    const std::string & getName() const;
    const std::vector<WheelItem>& items() const;
    const WheelItem& itemAt(size_t index) const;
    bool containsItem(const std::string& name) const;
    void deactivateItem(size_t index);

    void setMode(WheelMode mode);
    WheelMode mode() const;
private:
    std::string name_;
    std::vector<WheelItem> items_;
    WheelMode mode_ = WheelMode::Choice;
};
