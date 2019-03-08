#include "pypalette.h"
#include "qpypalette_inner.h"

EnterResult QPyPaletteInner::enter_callback() {
	auto* model = entries_->model();
	auto id = model->data(model->index(entries_->currentIndex().row(), 2)).toString();

	processEnterResult(true);

	return python_side_->trigger_action(id);
}