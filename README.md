# Room Sense

Wi-Fi Temperature in Every Room of your house

[Resulting Graph](images/screenshot_graphcs.png)

[Room Node](images/photo_room_node.jpg)

Room Sense is one of many IoT solutions for multi-room temperature monitoring. In this project, emphasis is placed on applying modern, affordable components like the ESP8266 and Raspberry Pi while maximizing ease-of-use.

## Features

1. Low-Cost Whole-Home Temperature Monitoring
2. Compact Size
3. Low Power Consumption (Battery or USB Charger)
4. Easy and Transparent
5. Great Data Visualization

#### System Operation

    ESP8266 (Wemos D1 Mini) ----> InfluxDB (RPi) <----> Grafana (RPi) <----> Web Browser <----> User

Each Room node uses the popular WiFi Espressif ESP8266 SoC running Arduino code. The Wemos D1 Mini dev board is used based on it's compact size, low-cost, modularity, and ability to be powered directly by USB charger.

The nodes communicate to a central Raspberry Pi running InfluxDB time-series database. Grafana allows you to create dashboards and browse data in desktop or mobile browser.

## Installation

1. Install & configure InfluxDB Docker Image
2. Install & configure Grafana Docker Image
3. Assemble room nodes
4. Setup Room node names
5. Flash Room nodes

TODO: Add detail about Docker

## TODO

1. ~~Add arduino code~~

2. Add docker images

3. Add photos & screenshots to README

4. Add links to README

## FAQ

#### Q: Can I use other ESP8266-based boards?
A: Yes! Just make sure to verify which pins are used for the DHT22 sensor.

#### Q: Where can I buy the Wemos D1 mini?
A: The Wemos D1 mini is availble on sites like Ebay.com or AliExpress.com.

#### Q: How can I use a Li Poly battery pack?

#### Q: Can I install without Docker?

#### Q: Which Raspberry Pi version do I need?
A: This has been tested on [Raspberry Pi 2 Model B](https://www.raspberrypi.org/products/raspberry-pi-2-model-b/), but later versions of the Raspberry Pi should work.

#### Q: Why do you use UDP instead of TCP? MQTT?
A: UDP is a session-less protocol which reduces network overhead which reduces transmission time and increases battery life as compared to TCP. Of course there is a trade-off: data transmission _may_ be less robust if there are physical layer issues such as packet loss or wireless interference. Similarily, MQTT introduces requires external libraries and additional server configuration, but can provide improved robustness. Ultimately UDP was chosen for simplicity, but feel free to fork and implement another protocol!

## Contributing

1. Fork it!
2. Create your feature branch: git checkout -b my-new-feature
3. Commit your changes: git commit -am 'Add some feature'
4. Push to the branch: git push origin my-new-feature
5. Submit a pull request :D

## Credits

TODO: Write credits

## License

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
