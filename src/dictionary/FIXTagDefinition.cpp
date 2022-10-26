#include "FIXTagDefinition.hpp"

using namespace fix;

void FIXTagDefinition::add_location(const FIXTagLocation &val){
    locations_.insert(val);
}