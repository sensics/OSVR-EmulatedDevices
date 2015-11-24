# OSVR-EmulatedDevices

###Instructions
1. Configure the plugin with Cmake
2. Build the generated solution/project
3. Project will build two files : plugin DLL and OutputDeviceData.exe client app
4. Download (or build) OSVR-Core. Binaries available at osvr.github.io
5. Copy `com_osvr_emulated_devices.dll` over to OSVR-Core/bin/osvr-plugins-0
6. Copy `OutputDeviceData.exe` to OSVR-Core/bin folder
7. Copy `osvr_server_config.emulatedDevices.json` to OSVR-Core/bin 
8. Start `osvr_server.exe` by executing in cmd window `osvr_server osvr_server_config.emulatedDevices.json`
9. If successful, you will see a message similar to this 
<br>`PLUGIN: We have detected emulated device!` <br>
`[OSVR] Added device: com_osvr_emulated_devices/dev` <br>
`[OSVR] Path tree updated or connection detected` <br>
`[OSVR] Sending path tree to clients.` <br>
10. Run `OutputDeviceData.exe` to print the device reports