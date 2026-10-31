# Tropical-Fruit-Bowl
<img width="716" height="368" alt="Week43DRender" src="https://github.com/user-attachments/assets/f5495ce5-9b91-4674-8ae4-9fa27263d223" />

# Description
A custom designed PCB that can track humidity, temperature, air pressure, and air quality in ppm. It also includes a gyrscope sensor. This project was heavily inspired by the Starbie and MIDI keyboard projects I did previously, and I wanted to combine them with some new components to see what I can make. This was a pretty exciting experience for me, because for once, I'm not following a guide anymore, and with the lessons and techniques I learned from the previous projects it's like building with LEGO. Once something becomes as fluent as LEGO building, I can let creativity take over.

# Features
* DHT11 Sensor
* MPU6050 gyro Sensor
* The new ENS160+AHT21 air quality sensor
* 2 low profile switches
* Potentiometer
* Rotary Encoder
* OLED

# BOM
| Item | Qty | Supplier | Cost/item | Total Cost | Link |
| --- | --- | --- | --- | --- | --- |
| 0.96 OLED | 1 | AliExpress | $4.38 | $4.38 | [Link](https://www.aliexpress.us/item/3256807427834365.html?src=google&src=google&albch=shopping&acnt=708-803-3821&isdl=y&slnk=&plac=&mtctp=&albbt=Google_7_shopping&aff_platform=google&aff_short_key=UneMJZVf&gclsrc=aw.ds&albagn=888888&ds_e_adid=&ds_e_matchtype=&ds_e_device=c&ds_e_network=x&ds_e_product_group_id=&ds_e_product_id=en3256807427834365&ds_e_product_merchant_id=5851713403&ds_e_product_country=US&ds_e_product_language=en&ds_e_product_channel=online&ds_e_product_store_id=&ds_url_v=2&albcp=19558607238&albag=&isSmbAutoCall=false&needSmbHouyi=false&gad_source=1&gad_campaignid=19566915268&gbraid=0AAAAAD6I-hF0RrES3rCrnIAsdOo0cNEg4&gclid=CjwKCAjwgJHXBhBGEiwAHohJq2PYiClm1aQFLBmOjzYlRhPSXxUHV9mpKWaNqAwxIaVBFqhxzUMuvRoCA2EQAvD_BwE&gatewayAdapt=glo2usa) |
| XIAO ESP32 C3 | 1 | Seeed Studio | $5.20 | $5.20 | [Link](https://www.seeedstudio.com/Seeed-XIAO-ESP32C3-p-5431.html) |
| MPU6050 | 1 | AliExpress | $3.00 | $3.00 | [Link](https://www.aliexpress.us/item/3256809482368154.html?src=google&snps=y&src=google&albch=shopping&acnt=708-803-3821&isdl=y&slnk=&plac=&mtctp=&albbt=Google_7_shopping&aff_platform=google&aff_short_key=UneMJZVf&gclsrc=aw.ds&albagn=888888&ds_e_adid=&ds_e_matchtype=&ds_e_device=c&ds_e_network=x&ds_e_product_group_id=&ds_e_product_id=en3256809482368154&ds_e_product_merchant_id=5851713403&ds_e_product_country=US&ds_e_product_language=en&ds_e_product_channel=online&ds_e_product_store_id=&ds_url_v=2&albcp=20542171667&albag=&isSmbAutoCall=false&needSmbHouyi=false&gad_source=1&gad_campaignid=18545443176&gbraid=0AAAAAD6I-hErqWWSEX2Slh2y8pGIo6r2n&gclid=Cj0KCQjw3YvXBhCDARIsAKB-ox_KDuVSshBCz_K8RTrfuol9tEvBqBcexhKj2D55FBoqYUsrFDhMWooaApfNEALw_wcB&gatewayAdapt=glo2usa) |
| DHT11 | 1 | AliExpress | $1.50 | $1.50 | [Link](https://www.aliexpress.us/item/3256808886855570.html?src=google&src=google&albch=shopping&acnt=708-803-3821&isdl=y&slnk=&plac=&mtctp=&albbt=Google_7_shopping&aff_platform=google&aff_short_key=UneMJZVf&gclsrc=aw.ds&albagn=888888&ds_e_adid=&ds_e_matchtype=&ds_e_device=c&ds_e_network=x&ds_e_product_group_id=&ds_e_product_id=en3256808886855570&ds_e_product_merchant_id=5851713403&ds_e_product_country=US&ds_e_product_language=en&ds_e_product_channel=online&ds_e_product_store_id=&ds_url_v=2&albcp=19558607238&albag=&isSmbAutoCall=false&needSmbHouyi=false&gad_source=1&gad_campaignid=19566915268&gbraid=0AAAAAD6I-hE3WxPZf8MIZjkbVNGkJNmog&gclid=Cj0KCQjw3YvXBhCDARIsAKB-ox_MIsLneSffZbkndQzzN2gGEmgWqn-IT0Po6Tv44LnMdjS0oC_BHe0aAvhYEALw_wcB&gatewayAdapt=glo2usa) |
| Kailh Choc Low-Profile Switches | 2 | AliExpress | $1.50 | $3.00 | [Link](https://www.aliexpress.us/item/3256808697103313.html?spm=a2g0o.productlist.main.1.13d14a7e068OmL&algo_pvid=4e5fe6cd-3562-4ef4-ac3e-085a2d4634fe&algo_exp_id=4e5fe6cd-3562-4ef4-ac3e-085a2d4634fe-0&pdp_ext_f=%7B%22order%22%3A%221475%22%2C%22spu_best_type%22%3A%22price%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21CAD%2111.40%212.38%21%21%217.81%211.63%21%402103128917919263557114360e0ebd%2112000047082945007%21sea%21CA%210%21ABX%211%210%21n_tag%3A-29910%3Bd%3A778dde27%3Bm03_new_user%3A-29895%3BpisId%3A5000000218640848&curPageLogUid=CXjsh9TOCyFt&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005008883418065%7C_p_origin_prod%3A&gatewayAdapt=glo2usa4itemAdapt) |
| RotaryEncoder_Alps_EC11E-Switch_Vertical_H20mm | 1 | AliExpress | $1.60 | $1.60 | [Link](https://www.aliexpress.us/item/3256807457768762.html?spm=a2g0o.productlist.main.1.52d22c6as0ZCnv&algo_pvid=9965a6c7-9d4c-44f9-a2d4-a43aa1b0ae45&algo_exp_id=9965a6c7-9d4c-44f9-a2d4-a43aa1b0ae45-0&pdp_ext_f=%7B%22order%22%3A%224739%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21USD%211.52%211.40%21%21%211.52%211.40%21%402101e7a317919489056767943e0e88%2112000041630839409%21sea%21US%213725344934%21X%211%210%21n_tag%3A-29919%3Bd%3A1bb91aa6%3Bm03_new_user%3A-29895%3BpisId%3A5000000219770668&curPageLogUid=AOe3dpbqRVrY&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005007644083514%7C_p_origin_prod%3A) |
| ENS160+AHT21 module | 1 | AliExpress | $4.10 | $4.10 | [Link](https://www.aliexpress.us/item/3256812532450753.html?src=google&src=google&albch=shopping&acnt=708-803-3821&isdl=y&slnk=&plac=&mtctp=&albbt=Google_7_shopping&aff_platform=google&aff_short_key=UneMJZVf&gclsrc=aw.ds&albagn=888888&ds_e_adid=&ds_e_matchtype=&ds_e_device=c&ds_e_network=x&ds_e_product_group_id=&ds_e_product_id=en3256812532450753&ds_e_product_merchant_id=5851713403&ds_e_product_country=US&ds_e_product_language=en&ds_e_product_channel=online&ds_e_product_store_id=&ds_url_v=2&albcp=19558607238&albag=&isSmbAutoCall=false&needSmbHouyi=false&gad_source=1&gad_campaignid=19566915268&gbraid=0AAAAAD6I-hE3WxPZf8MIZjkbVNGkJNmog&gclid=Cj0KCQjw3YvXBhCDARIsAKB-ox_0ZPK7KkAejpDl94aN83L3UZbFFqTooBbtk7go9Ezc6xkZ9xmlFmQaAojEEALw_wcB&gatewayAdapt=glo2usa) |
| Potentiometer_Bourns_PTA1543_Single_Slide | 1 | Mouser | $2.25 | $2.25 | [Link](https://www.mouser.com/en/ProductDetail/Bourns/PTA1543-2015CIA103?qs=%252B9%2Fcbd0IE0QZ1Dob%2FcnIYA%3D%3D) |
| SMD Capacitator | 1 | Digikey | $0.13 | $0.13 | [Link](https://www.digikey.com/en/products/detail/yageo/CC0603ZPY5V8BB104/1866826) |
| Axial Resistor | 1 | Digikey | $0.11 | $0.11 | [Link](https://www.digikey.com/en/products/detail/te-connectivity-passive-product/LR0204F10K/2381045) |
| PCB | 1 | JLCPCB | $6.90 | $6.90 | [Link](https://cart.jlcpcb.com/quote?stencilLayer=2&stencilWidth=100&stencilLength=100&stencilCounts=5&plateType=1&spm=Jlcpcb.Homepage.1010) |
| Estimated total shipping | 1 | N/A | $0.00 | $0.00 | N/A |
| **Grand Total** | | | | **$32.17** | |

# Cad Modeling

<img width="1065" height="522" alt="Screenshot 2026-10-29 at 10 39 27 AM" src="https://github.com/user-attachments/assets/7ad9bbb8-b1a8-4ebd-9318-a9d79218a040" />


The cad part was by far the most fun part of this project. I first had to list out what components I want to use because that's really important. Then I have to track down the correct symbols and footprints and sometimes sensors just need a pinheader connection to work. Wiring was pretty easy once I studied what power sources are mandatory and where data should go. I then added a ceramic capacitator to ease electrical noise since my board will be handling lots of data in real time. Bringing the board over to the PCB section I quickly designed the shape and I oriented and wired everything up pretty smoothly. It's like building with LEGO Technics.

# Firmware

## Description
The **Tropical Fruit Bowl** firmware is a an app built on Ardunion C++ designed to showcase the sensors. It features Starbie-like mechanics but the UI is really different. That is because it includes 3 interactive windows for data viewing. I spent the most time designing the firmware and you can see how it should work with the mermaid diagram below. I also decided to add a dedicated section for color customization just in case anyone wants to use this project for themselves. 

## Features
* Moves the cursor, inspired from Starbie, and highlights where your cursor is.
* Intro screen
* Dynamic datas.
* Components adjusts graphs to view liking.
* Asks if you want to exit, instead of just turning off instantly.

## Firmware Flow Chart

<img width="2542" height="1656" alt="diagram" src="https://github.com/user-attachments/assets/90ea9d07-5a52-44fa-9ac5-e390878e9d16" />


## Hardware Controls

| Hardware Component | Pin/Interface | Function in Firmware |
| :--- | :--- | :--- |
| **Switch 1 (SW1)** | `D8` | **Power / Menu:**|
| **Switch 2 (SW2)** | `D7` | **Select / Click:**|
| **MPU6050 IMU** | `I2C` | **Cursor Movement:**|
| **Slide Potentiometer (RV1)** | `D3 (Analog)` | **Y-Axis Graph Control:**|
| **Rotary Encoder (SW3)** | `D1 / D2` | **X-Axis Graph Control:**|

### Assembly
Assembling this really shouldn't be a problem because I designed the pcb so that all components will have space inbetween other components, so one shouldn't worry about overlapping components. The sensors especiallu the gyroscope an air quality sensors should come with pin headers in the package so I wouldn't buy generic pin heads seperately.
