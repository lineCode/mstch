#include <visitor/render_section.h>
#include <visitor/to_json.h>
#include "visitor/is_node_empty.h"
#include "in_inverted_section.h"
#include "outside_section.h"
#include "render_context.h"

using namespace mstch;

state::in_inverted_section::in_inverted_section(
        const std::string& section_name):
        section_name(section_name),
        skipped_openings(0)
{
}

std::string state::in_inverted_section::render(
        render_context& ctx,
        const token& token)
{
    switch(token.type()) {
    case token_type::section_close:
        if(token.content() == section_name && skipped_openings == 0) {
            std::ostringstream out;
            auto& section_node = ctx.get_node(section_name);
            if(boost::apply_visitor(visitor::is_node_empty(), section_node)) {
                auto empty = mstch::object{};
                out << render_context(empty, ctx).render(section_text.str());
            }
            ctx.set_state<outside_section>();
            return out.str();
        } else {
            skipped_openings--;
            section_text << token.raw();
        }
        break;
    case token_type::inverted_section_open:
    case token_type::section_open:
        skipped_openings++;
    case token_type::text:
    case token_type::variable:
    case token_type::unescaped_variable:
    case token_type::comment:
    case token_type::partial:
        section_text << token.raw();
        break;
    }
    return "";
}