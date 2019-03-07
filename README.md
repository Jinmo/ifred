## IDA command palette & further

### How to build

Currently tested on windows only, but maybe working with other OSes too.

```shell
git clone --recursive https://github.com/Jinmo/ifred.git
cd ifred

# you can use CMakeSettings.json for Visual Studio
# or wrapper script
ida-cmake/build.py -i <ida-sdk-path> -t 6.95 \
    --idaq-path "/Applications/IDA Pro 6.95.app/Contents/MacOS/"
```
