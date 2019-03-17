# IDA command palette & further

[![Build status](https://ci.appveyor.com/api/projects/status/64ye84uag1v9rpn6?svg=true)](https://ci.appveyor.com/project/Jinmo/ifred)

## How to build

Currently tested on Windows. Install Qt 5.6.3 and IDA SDK, and follow steps in .appveyor.yml.

You can download [prebuilt plugins](https://ci.appveyor.com/project/Jinmo/ifred/build/artifacts) from appveyor.

## Python API

You can make a custom palette in IDAPython.

```py
from __palette__ import show_palette, Palette, Action
import random, string

myhandler = lambda item: sys.stdout.write('You selected: %s\n' % item.description)
random_str = lambda: "".join(random.choice(string.lowercase) for i in range(20))

entries = [Action(description=random_str(), # displayed text
    handler=myhandler, # callback
    id='action%d' % i # must be unique
    ) for i in range(20)]

show_palette(Palette(entries))
```

Currently cleaning up C++ API.