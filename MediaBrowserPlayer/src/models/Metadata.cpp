#include "../../include/models/Metadata.h"
#include <sstream>
#include <iomanip>

Metadata::Metadata() : name(""), duration(0.0) {
}

Metadata::Metadata(const std::string& name, double duration)
    : name(name), duration(duration) {
}

const std::string& Metadata::getName() const {
    return name;
}

void Metadata::setName(const std::string& name) {
    this->name = name;
}

double Metadata::getDuration() const {
    return duration;
}

void Metadata::setDuration(double duration) {
    this->duration = duration;
}

void Metadata::setAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string Metadata::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    if (it != attributes.end()) {
        return it->second;
    }
    return "";
}

bool Metadata::hasAttribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

const std::map<std::string, std::string>& Metadata::getAllAttributes() const {
    return attributes;
}

std::string Metadata::getDurationString() const {
    int minutes = static_cast<int>(duration) / 60;
    int seconds = static_cast<int>(duration) % 60;
    
    std::stringstream ss;
    ss << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;
    return ss.str();
}

std::string Metadata::toString() const {
    std::stringstream ss;
    ss << name << "|" << duration << "|" << attributes.size();
    
    for (const auto& attr : attributes) {
        ss << "|" << attr.first << "|" << attr.second;
    }
    
    return ss.str();
}

Metadata Metadata::fromString(const std::string& serialized) {
    std::stringstream ss(serialized);
    std::string name;
    double duration;
    int attrCount;
    
    std::getline(ss, name, '|');
    ss >> duration;
    ss.ignore(); // Skip the '|'
    ss >> attrCount;
    ss.ignore(); // Skip the '|'
    
    Metadata metadata(name, duration);
    
    for (int i = 0; i < attrCount; ++i) {
        std::string key, value;
        std::getline(ss, key, '|');
        std::getline(ss, value, '|');
        metadata.setAttribute(key, value);
    }
    
    return metadata;
}
