#define xml_def(v,l) XML_ ## v
enum XML_enums
  {
#include "XMLtags.def"
  };
#undef xml_def


extern xml_tag_s xml_tags[];
