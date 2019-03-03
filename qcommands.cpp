#include "qcommands.h"

QCommands::QCommands()
{
    filter_.setDynamicSortFilter(true);
    filter_.setSourceModel(&source_);

    setModel(&filter_);
    setItemDelegate(&item_delegate_);
    setLineWidth(0);
}
