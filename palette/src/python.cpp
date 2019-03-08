#include "qpalette_inner.h"
#include <pybind11/pybind11.h>

class QPythonPaletteInner : public QPaletteInner {
public:
    QPythonPaletteInner(QWidget *parent, QObject *prevItem) : QPaletteInner(parent, prevItem) {
	}
};