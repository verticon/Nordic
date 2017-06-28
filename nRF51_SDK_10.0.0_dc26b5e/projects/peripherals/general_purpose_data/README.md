# General Purpose Data
### An application that provides several types of data whose values are user modifiable.

The application (Device Name = GeneralPurposeData) provides a service (UUID = DCBA3164-1212-EFDE-1523-785FEF13D123) with seven characteristics:  

1. User Description = Byte, Size = 1 bytes, UUID =  DCBA1533-1212-EFDE-1523-785FEF13D123
2. User Description = Short, Size = 2 bytes, UUID =  DCBA1534-1212-EFDE-1523-785FEF13D123
3. User Description = Int, Size = 4 bytes, UUID =  DCBA1535-1212-EFDE-1523-785FEF13D123
4. User Description = Long, Size = 8 bytes, UUID =  DCBA1536-1212-EFDE-1523-785FEF13D123
5. User Description = Float, Size = 4 bytes, UUID =  DCBA1537-1212-EFDE-1523-785FEF13D123
6. User Description = Double, Size = 8 bytes, UUID =  DCBA1538-1212-EFDE-1523-785FEF13D123
7. User Description = String, Size = 200 bytes, UUID =  DCBA1539-1212-EFDE-1523-785FEF13D123

Additionally, the application uses the pca10028's uart to present a menu which allows a user to examine and modify the value of any of the characteristics.
Segger's [JLink](https://www.segger.com/downloads/jlink) tools allow the menu to be displayed on a mac or pc via a serial port terminal program such as [Cool Term](http://freeware.the-meiers.org)  

A connected central can:  

* Read or write the state of any of the characteristics.
* Enable notifications; which are triggered when a user modifies a value via the menu.

The motivation for this application is the testing of centrals. It allows for easy generation of the various types of data that a central application might require. In particular, the [Bluetooth Explorer](https://github.com/verticon/BluetoothExplorer) application was tested using the General Purpose Data application.
