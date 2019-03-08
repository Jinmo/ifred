#include "palette_manager.h"
#include "qpalettecontainer.h"

QPaletteContainer* g_current_widget;

void show_palette(QPaletteInner *inner) {
	if (!g_current_widget)
		g_current_widget = new QPaletteContainer();

	g_current_widget->clear();
	g_current_widget->add(inner);
	g_current_widget->show();
	g_current_widget->showWidget(0);
}

void cleanup_palettes() {
	if (g_current_widget)
		delete g_current_widget;
}