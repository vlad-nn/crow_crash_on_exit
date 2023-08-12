if not exist build md build
cd build
rem config=RelWithDebInfo
set config=RelWithDebInfo
cmake .. -A x64 -T host=x64 -DCMAKE_CONFIGURATION_TYPES="%config%"
cmake --build . --config %config%
