#ifndef METADATA_H
#define METADATA_H

#include <string>
#include <map>

class Metadata {
public:
    Metadata();
    Metadata(const std::string& name, double duration = 0.0);
    
    const std::string& getName() const;
    void setName(const std::string& name);
    
    double getDuration() const;
    void setDuration(double duration);
    
    // Get/set arbitrary metadata fields
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    const std::map<std::string, std::string>& getAllAttributes() const;
    
    // Formatted duration string (MM:SS)
    std::string getDurationString() const;
    
    // Serialize to string
    std::string toString() const;
    
    // Deserialize from string
    static Metadata fromString(const std::string& serialized);
    
private:
    std::string name;
    double duration; // in seconds
    std::map<std::string, std::string> attributes;
};

#endif // METADATA_H
