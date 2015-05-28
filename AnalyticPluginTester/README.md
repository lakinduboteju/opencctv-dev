# Simple program to test your Analytic plugin (.so)

1. Install Boost library.
Follow instructions [here](https://github.com/lakinduboteju/opencctv-dev/tree/master/opencctv-dependencies/boost) to install Boost library on your system.
2. Go to Release directory of AnalyticPluginTester > Run ```make all```.
3. To run it, pass following command-line parameters:-
```
./AnalyticPluginTester <input_video_file_path> <analytic_plugin_dir_path> <analytic_plugin_filename>
```
Example:
``` sh
./AnalyticPluginTester ~/Videos/test.avi ~/git/opencctv-dev/MockAnalyticPlugin/Release libMockAnalyticPlugin.so
```
