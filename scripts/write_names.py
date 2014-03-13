#!/usr/bin/env python

print "#ifndef CONQUEST_NAMES_GEN_H_"
print "#define CONQUEST_NAMES_GEN_H_"

print "#include <string>"
print "#include <vector>"

print "const std::vector<std::string> SUPER_REGION_NAMES = {" + ','.join('"{}"'.format(x.strip()) for x in open('data/super_region_names')) + "};"
print "const std::vector<std::string> REGION_NAMES = {" + ','.join('"{}"'.format(x.strip()) for x in open('data/region_names')) + "};"


print "#endif //CONQUEST_NAMES_GEN_H_ "
