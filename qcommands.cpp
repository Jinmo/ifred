#include "qcommands.h"

QCommands::QCommands()
{
    filter_.setDynamicSortFilter(true);
    filter_.setSourceModel(&source_);

    setModel(&filter_);
    setItemDelegate(&item_delegate_);
    setLineWidth(0);
    setStyleSheet(R"(
                            QListView {border: none;}
                            QScrollBar:vertical {
                            border: none;
                            background: #f5f5f5;
                            width: 12px;
                            border-radius: 6px;
                            }
                            QScrollBar::handle:vertical {
                            background: #dadada;
                            min-height: 20px;
                            border-radius: 6px;
                            }
                            QScrollBar::add-line:vertical {
                            width: 0; height: 0;
                            subcontrol-position: bottom;
                            subcontrol-origin: margin;
                            }
                            QScrollBar::sub-line:vertical {
                            width: 0; height: 0;
                            subcontrol-position: top;
                            subcontrol-origin: margin;
                            }
                            )");

}
