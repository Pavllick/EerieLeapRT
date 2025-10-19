#include "metadata_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

void MetadataBlock::SetText(const std::string& block_id, const std::string& text) {
    std::string xml_string = "<" + block_id + "comment>";
    xml_string += text;
    xml_string += "</" + block_id + "comment>";

    text_ = xml_string;
}

} // namespace eerie_leap::subsys::mdf::mdf4
