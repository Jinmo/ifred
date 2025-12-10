# IDA command palette & more

[![Build Status](https://jinmo123.visualstudio.com/idapkg/_apis/build/status/Jinmo.ifred?branchName=master)](https://jinmo123.visualstudio.com/idapkg/_build/latest?definitionId=1&branchName=master) ![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/jinmo/ifred)

[screenshot1][01]

## How to build

Tested on all operating systems that run IDA Pro.

| OS      | Arch   | Status |
| ------- | ------ | ------ |
| Linux   | x86_64 | ✅     |
| macOS   | x86_64 | ✅     |
| macOS   | arm64  | ✅     |
| Windows | x86_64 | ✅     |

Currently this repo supports IDA Pro with Qt6. For IDA Pro v9.1 and earlier
with Qt5 support head to [qt5 branch][04]. To build you need IDA Pro SDK.

For easier compiling, use Qt6 artifacts provided by [Binary Ninja Crew][05]

You can download prebuilt binaries with Qt6 from this [repo][06].

You can download older [prebuilt plugins][07] with Qt5 from azure pipelines.

## Python API

You can make a custom palette in IDAPython.

```py
from __palette__ import show_palette, Palette, Action
import random, string

myhandler = lambda item: sys.stdout.write('You selected: %s\n' % item.name)
random_str = lambda: "".join(random.choice(string.lowercase) for i in range(20))

entries = [Action(name=random_str(), # displayed text
    handler=myhandler, # callback
    id='action%d' % i # must be unique
    ) for i in range(20)]

show_palette(Palette('palette name here', 'placeholder here...', entries))
```

## C++ API

Currently cleaning up C++ API. See `standalone/` folder.

```cpp
#include <palette/api.h>
#define COUNT 100

QVector<Action> testItems() {
    QVector<Action> action_list;

    action_list.reserve(COUNT + 1);
    action_list.push_back(Action("std::runtime_error", "raise exception", ""));

    for (int i = 0; i < COUNT; i++) {
        auto id = QString::number(rand());
        action_list.push_back(Action(id, id, ""));
    }

    return action_list;
}

const QString TestPluginPath(const char* name) {
    // Don't worry! also packaged with bundle theme!
    // Just point a writable path
    return QString("./path_to_plugin_theme/") + name;
}

int main() {
    QApplication app(argc, argv);

    set_path_handler(TestPluginPath);

    show_palette("<test palette>", "Enter item name...", testItems(), [](const Action & action) {
        if (action.id() == "std::runtime_error") {
            throw std::runtime_error("raised!");
        }
        qDebug() << action.id() << action.description() << action.shortcut();
        return false;
    });

    app.exec();
}
```

## Changing theme

You can copy css, json files from `palette/res/theme/<name>/*` to
`%APPDATA%/Hex-rays/IDA Pro/plugins/palette/theme/`, like the existing
css, json files.

ayu white:

[screenshot2][02]

solarized dark:

[screenshot3][03]

[01]: screenshots/1.png
[02]: screenshots/2.png
[03]: screenshots/3.png
[04]: https://github.com/Jinmo/ifred/tree/qt5
[05]: https://github.com/Vector35/qt-artifacts/releases
[06]: https://github.com/blue-devil/ifred/releases
[07]: https://jinmo123.visualstudio.com/idapkg/_build/latest?definitionId=1&branchName=master
