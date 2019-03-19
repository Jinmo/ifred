#include "pypalette.h"
#include "qpypalette_inner.h"

EnterResult QPyPaletteInner::enter_callback(Action &action) {
	processEnterResult(true);

	return python_side_->trigger_action(action);
}