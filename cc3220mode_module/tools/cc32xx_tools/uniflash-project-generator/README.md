UniFlash Project Generator
======

Description
-----------

The `uniflash_project_generator` is a simple way to generate and flash UniFlash projects for CC3220R, CC3220S and CC3220SF.`uniflash_project_generator` located at ```[sdk_install_path]\tools\cc32xx_tools\uniflash-project-generator```

Generated project includes:
* Service Pack file from ```sdk_install_path\tools\cc32xx_tools\servicepack-cc3x20``` or ```sdk_install_path\tools\cc32xx_tools\servicepack-cc3x35``` dependes on selected device
* Trusted Root-Certificate Catalog from ```sdk_install_path\tools\cc32xx_tools\certificate-playground```
* Dummy Root-Certificate and Dummy User-Certificate from ```sdk_install_path\tools\cc32xx_tools\certificate-playground```

Requirements
-----------

| Application | Version |
| --- |  --- |
| UniFlash | 4.3.1.1835 |
| SLImageCreator | 1.0.19.6 |


Synopsis
-----------

```batch
uniflash_project_generator --name [PROJ_NAME] --device [DEV_TYPE] --mcu_bin [BIN_FILE] [EXTRA_OPTIONS]
```

Options
-----------

**--name**
: Unique project name

**--device**
: LaunchPad type. Available options : CC3220R, CC3220S, CC3220SF

**--mcu_bin**
: MCU image file

**--debug**
: Enable debug prints

**--flash**
: Flash device after creating a project 

**--help**
: Show help options


Examples
-----------

Creating uniflash project for network_terminal demo on CC3220SF launchpad without flashing the device

```batch
uniflash_project_generator --name network_terminal --device CC3220SF --mcu_bin <path_to_network_terminal_bin>
```

Creating and flashing uniflash project for network_terminal demo on CC3220SF launchpad 

```batch
uniflash_project_generator --name network_terminal --device CC3220SF --mcu_bin <path_to_network_terminal_bin> --flash
```

Creating and flashing uniflash project for network_terminal demo on CC3220SF launchpad with debug prints

```batch
uniflash_project_generator --name network_terminal --device CC3220SF --mcu_bin <path_to_network_terminal_bin> --flash --debug
```

