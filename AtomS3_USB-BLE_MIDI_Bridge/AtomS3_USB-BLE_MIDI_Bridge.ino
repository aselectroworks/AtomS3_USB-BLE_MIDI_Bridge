/*
MIT License

Copyright (c) 2023 aselectroworks

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else
#include <BLEMIDI_Transport.h>
#include <M5Unified.h>
#include <MIDI.h>
#include <Preferences.h>
#include <USB-MIDI.h>
#include <hardware/BLEMIDI_Client_ESP32.h>

#include "esp_log.h"

#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial0
#define USBSerial Serial
#else
#define HWSerial Serial
#endif

// MIDI Callback Option 
// #define MIDI_TIMECODE_QUARTER_FRAME_TRANSFER_SUPPORT
#define MIDI_CLOCK_TRANSFER_SUPPORT

#define DISCOVER_DEVICE_MAX (5)

// USB MIDI incetance
uint8_t usbMidiCableNumber = 0;
MIDI_NAMESPACE::MidiInterface<USBMIDI_NAMESPACE::usbMidiTransport> *UsbMIDI[3];
USBMIDI_CREATE_INSTANCE(0, UsbMIDI_1)
USBMIDI_CREATE_INSTANCE(1, UsbMIDI_2)
USBMIDI_CREATE_INSTANCE(2, UsbMIDI_3)

// BLE MIDI instance
BLEMIDI_NAMESPACE::BLEMIDI_Transport<BLEMIDI_NAMESPACE::BLEMIDI_Client_ESP32>
    *BleMIDI[3];
MIDI_NAMESPACE::MidiInterface<BLEMIDI_NAMESPACE::BLEMIDI_Transport<
                                  BLEMIDI_NAMESPACE::BLEMIDI_Client_ESP32>,
                              BLEMIDI_NAMESPACE::MySettings> *BleMIDI_Client[3];

BLEMIDI_NAMESPACE::BLEMIDI_Transport<BLEMIDI_NAMESPACE::BLEMIDI_Client_ESP32>
    BleMIDI_1("Cable1", false);
MIDI_NAMESPACE::MidiInterface<BLEMIDI_NAMESPACE::BLEMIDI_Transport<
                                  BLEMIDI_NAMESPACE::BLEMIDI_Client_ESP32>,
                              BLEMIDI_NAMESPACE::MySettings>
    BleMIDI_Client_1((BLEMIDI_NAMESPACE::BLEMIDI_Transport<
                      BLEMIDI_NAMESPACE::BLEMIDI_Client_ESP32> &)BleMIDI_1);
BLEMIDI_NAMESPACE::BLEMIDI_Transport<BLEMIDI_NAMESPACE::BLEMIDI_Client_ESP32>
    BleMIDI_2("Cable2", false);
MIDI_NAMESPACE::MidiInterface<BLEMIDI_NAMESPACE::BLEMIDI_Transport<
                                  BLEMIDI_NAMESPACE::BLEMIDI_Client_ESP32>,
                              BLEMIDI_NAMESPACE::MySettings>
    BleMIDI_Client_2((BLEMIDI_NAMESPACE::BLEMIDI_Transport<
                      BLEMIDI_NAMESPACE::BLEMIDI_Client_ESP32> &)BleMIDI_2);
BLEMIDI_NAMESPACE::BLEMIDI_Transport<BLEMIDI_NAMESPACE::BLEMIDI_Client_ESP32>
    BleMIDI_3("Cable3", false);
MIDI_NAMESPACE::MidiInterface<BLEMIDI_NAMESPACE::BLEMIDI_Transport<
                                  BLEMIDI_NAMESPACE::BLEMIDI_Client_ESP32>,
                              BLEMIDI_NAMESPACE::MySettings>
    BleMIDI_Client_3((BLEMIDI_NAMESPACE::BLEMIDI_Transport<
                      BLEMIDI_NAMESPACE::BLEMIDI_Client_ESP32> &)BleMIDI_3);

#define BLE_MIDI_DEVICE_COUNT 3

#include "midi_callback.h"

typedef enum {
    IDLE = 0,
    SCANNED,
    DISCOVER,
    CONNECT,
    DISCONNECT,
} StateEnum;
StateEnum state = IDLE;

// BLE Scanner
int scanTime = 3;  // In seconds
BLEScan *pBLEScan;

typedef enum {
    BLEDISABLE = 0,
    BLEDISCONNECT,
    BLECONNECT,
    BLERECONNECT,
} BleMidiDeviceStatus;

typedef struct {
    std::string name;
    NimBLEAddress bda;
    BleMidiDeviceStatus status;
} BleMidiDevice;
BleMidiDevice bleMidiDevice[BLE_MIDI_DEVICE_COUNT];
std::vector<BleMidiDevice> BleMidiDeviceList;
uint8_t BleMidiDeviceIdx = 0;

bool manualConnection = false;

TaskHandle_t UsbMidiReadTask_taskHandle = nullptr;
TaskHandle_t BleReadCB_taskHandle[BLE_MIDI_DEVICE_COUNT] = {nullptr};

// Prototypes
bool deleteReadCB[BLE_MIDI_DEVICE_COUNT] = {false};
void display_main(uint8_t inv_color_idx);
void display_information(char *str);

// Delay function
void wait_ms(uint32_t time) {
    uint32_t curTime = millis();
    while (millis() - curTime < time) {
        M5.update();
        delay(1);
    }
}

void bleConnectCB() {
    log_i("---------HandleConnected---------\r\n");
    if (manualConnection) {
        // Manual-Reconnect Callback
        log_i("Manual-connect Device: idx = %d\r\n", usbMidiCableNumber);
        bleMidiDevice[usbMidiCableNumber].status = BLECONNECT;
    }
    // state = IDLE;
}

void bleDisconnectCB() {
    log_i("---------HandleDisconnected---------\r\n");
    BLEClient *disconnectedDevice = BLEDevice::getDisconnectedClient();
    log_i("Disconnected Device Address is %s\r\n",
          disconnectedDevice->getPeerAddress().toString().c_str());
    // state = IDLE;
}

void setBleCallbacks() {
    // BLE MIDI Callback
    for (uint8_t i = 0; i < BLE_MIDI_DEVICE_COUNT; i++) {
        BleMIDI[i]->setHandleConnected(bleConnectCB);
        BleMIDI[i]->setHandleDisconnected(bleDisconnectCB);
    }
}

bool isScanning = false;
void scan() {
    isScanning = true;
    Serial2.printf("Scanning...\r\n");
    // Update Display
    display_information((char *)"Scanning...      ");

    // put your main code here, to run repeatedly:
    while (pBLEScan->isScanning())
        ;
    pBLEScan->stop();
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    while (pBLEScan->isScanning())
        ;
    isScanning = false;
    Serial2.printf("Devices found: %d\r\n", foundDevices.getCount());
    for (uint8_t i = 0; i < BLE_MIDI_DEVICE_COUNT; i++) {
        pBLEScan->erase(bleMidiDevice[i].bda);
    }
    Serial2.printf("Remove Registerd Devices\r\nRest Devices Count: %d\r\n",
                   foundDevices.getCount());

    // Initialize BLE MIDi Device List
    BleMidiDeviceIdx = 0;
    BleMidiDeviceList.clear();

    NimBLEAdvertisedDevice advertisedDevice;
    for (uint8_t i = 0; i < foundDevices.getCount(); i++) {
        if (i >= DISCOVER_DEVICE_MAX) break;
        advertisedDevice = foundDevices.getDevice(i);
        if (advertisedDevice.haveServiceUUID()) {
            for (uint8_t j = 0; j < advertisedDevice.getServiceUUIDCount();
                 j++) {
                if (strncmp(
                        advertisedDevice.getServiceUUID(j).toString().c_str(),
                        bleMidi::SERVICE_UUID, 36) == 0 &&
                    advertisedDevice.getName() != "") {
                    // BLE Device Has BLE Service UUID
                    Serial2.printf("--------------------------------------\n");
                    Serial2.printf("Advertised Device  %s \n",
                                   advertisedDevice.toString().c_str());
                    BleMidiDevice device = {};
                    device.name = advertisedDevice.getName();
                    device.bda = advertisedDevice.getAddress();
                    device.status = BLEDISABLE;
                    BleMidiDeviceList.push_back(device);
                }
            }
        }
    }

    if (BleMidiDeviceList.size() == 0) {
        display_nodevices();
    } else {
        display_frame();
        display_devices();
    }
    Serial2.println("Scan done!");
    pBLEScan->clearResults();  // delete results fromBLEScan buffer to
                               // release memory
}

/**
 * This function is called by xTaskCreatePinnedToCore() to perform a
 * multitask execution. In this task, read() is called every millisecond
 * (approx.). read() function performs connection, reconnection and scan-BLE
 * functions. Call read() method repeatedly to perform a successfull
 * connection with the server in case connection is lost.
 */
bool bleMidiKeepAlive[3] = {false};
void ReadCB(void *parameter) {
    uint32_t curTime, prevTime = 0;
    Serial2.printf("READ Task is started on core: %d\r\n", xPortGetCoreID());
    Serial2.printf("Cable Number is %2d\r\n", *(uint8_t *)parameter);
    uint8_t taskCableNumber = *(uint8_t *)parameter;
    for (;;) {
        if (bleMidiDevice[taskCableNumber].status == BLECONNECT) {
            bleMidiKeepAlive[taskCableNumber] = true;
            BleMIDI_Client[taskCableNumber]->read();
            // vTaskDelay(1);
        } else if (!isScanning || !manualConnection) {
            while (pBLEScan->isScanning())
                ;
            BleMIDI_Client[taskCableNumber]->read();
            vTaskDelay(10);
        }
        // vTaskDelay(1 / portTICK_PERIOD_MS);  // Feed the watchdog of
        // FreeRTOS.
        //  Serial2.println(uxTaskGetStackHighWaterMark(NULL)); //Only for
        //  debug. You can see the watermark of the free resources assigned
        //  by the xTaskCreatePinnedToCore() function.
        if (deleteReadCB[taskCableNumber]) break;
    }
    vTaskDelay(1);

    Serial2.printf("READ Task is deleted: CN=%d\n", taskCableNumber);
    deleteReadCB[taskCableNumber] = false;
    BleReadCB_taskHandle[taskCableNumber] = NULL;
    vTaskDelete(BleReadCB_taskHandle[taskCableNumber]);
}

bool usbMidiKeepAlive[3] = {false};
void UsbMidiReadTask(void *parameter) {
    Serial2.printf("USB MIDI READ Task is started on core: %d\r\n",
                   xPortGetCoreID());
    for (;;) {
        for (uint8_t i = 0; i < USB_MIDI_NUM_CABLES; i++) {
            usbMidiKeepAlive[i] = true;
            UsbMIDI[i]->read();
        }
        // vTaskDelay(1);
    }
    vTaskDelay(1);

    Serial2.printf("USB MIDI READ Task is deleted\n");
    UsbMidiReadTask_taskHandle = NULL;
    vTaskDelete(UsbMidiReadTask_taskHandle);
}

// Preferences
Preferences prefs;
char prefs_namespace_name[3][6] = {"name1", "name2", "nama3"};
char prefs_namespace_bda[3][5] = {"bda1", "bda2", "bda3"};

uint8_t selecting_cn = 0;

void setup() {
    auto cfg = M5.config();

    M5.begin(cfg);
    M5.Display.setRotation(3);
    M5.Display.setTextSize(1);
    M5.Display.setTextWrap(false);
    //M5.Display.setBrightness(10); 

    Serial.end();
    Serial.begin(115200, SERIAL_8N1, 8, 7);  // Change Serial Port Assign
    // Serial2.begin(115200, SERIAL_8N1, 8, 7);  // Change Serial Port Assign
    Serial.setDebugOutput(true);

    // Change USB Device Descriptor Parameter
    // USB.VID(0x6666);
    // USB.PID(0x1234);
    // USB.firmwareVersion(0x0000);
    // USB.usbVersion(0x0200);
    // USB.usbPower(100);
    // USB.usbClass(TUSB_CLASS_AUDIO);
    // USB.usbSubClass(0x00);
    // USB.usbProtocol(0x00);
    // USB.usbAttributes(0x80);
    // USB.webUSB(false); 
    USB.productName("USB-BLE MIDI Bridge");
    USB.manufacturerName("A.S Electroworks");
    // USB.serialNumber("0000");
    // USB.webUSBURL(""); 
    tinyusb_add_string_descriptor("USB-BLE MIDI Bridge");

    // Initialize USB MIDI Device
    UsbMIDI[0] = &UsbMIDI_1;
    UsbMIDI[1] = &UsbMIDI_2;
    UsbMIDI[2] = &UsbMIDI_3;
    for (uint8_t i = 0; i < USB_MIDI_NUM_CABLES; i++) {
        UsbMIDI[i]->begin(MIDI_CHANNEL_OMNI);
    }
    xTaskCreatePinnedToCore(
        UsbMidiReadTask,  // See FreeRTOS for more multitask info
        "USBMIDI-READ", 4096, NULL, 1, &UsbMidiReadTask_taskHandle,
        1);  // Core0 or Core1

    // Assign BLE MIDI Device Pointer
    BleMIDI[0] = &BleMIDI_1;
    BleMIDI[1] = &BleMIDI_2;
    BleMIDI[2] = &BleMIDI_3;
    BleMIDI_Client[0] = &BleMIDI_Client_1;
    BleMIDI_Client[1] = &BleMIDI_Client_2;
    BleMIDI_Client[2] = &BleMIDI_Client_3;

    // BLE Callback
    setBleCallbacks();

    // MIDI Callback
    setMidiCallbacks();

    // Initialize BLE Scanner
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();  // create new scan
    pBLEScan->setActiveScan(
        true);  // active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);  // less or equal setInterval value

    // Initialize BLE MIDI Device
    for (uint8_t i = 0; i < BLE_MIDI_DEVICE_COUNT; i++) {
        prefs.begin(prefs_namespace_name[i]);
        prefs.begin(prefs_namespace_bda[i]);
        bleMidiDevice[i].name =
            prefs.getString(prefs_namespace_name[i], "No Device").c_str();
        bleMidiDevice[i].bda =
            NimBLEAddress(prefs.getString(prefs_namespace_bda[i], "").c_str());
        Serial2.printf("bleMidiDevice[%d].name = %s\r\n", i,
                       bleMidiDevice[i].name.c_str());
        if (bleMidiDevice[i].name != "No Device" ||
            bleMidiDevice[i].name == "") {
            // Re-Connect to previous BLE Devices
            usbMidiCableNumber = i;
            bleMidiDevice[i].status = BLERECONNECT;
            BleMIDI[i]->setName(bleMidiDevice[i].bda.toString().c_str());
            BleMIDI_Client[i]->begin(MIDI_CHANNEL_OMNI);
            xTaskCreatePinnedToCore(
                ReadCB,  // See FreeRTOS for more multitask info
                "BLEMIDI-READ", 4096, (uint8_t *)&i, 1,
                &BleReadCB_taskHandle[i],
                1);  // Core0 or Core1
            Serial2.printf("BLE MIDI Client %d Begun\r\n", i);
            wait_ms(100);
            while (BLEDevice::getScan()->isScanning())
                ;
        }
    }
    usbMidiCableNumber = 0;

    // Display Main Window
    display_header();
    display_frame();
    display_main(selecting_cn);
}

void loop() {
    static uint32_t curLoopTime = 0;
    if (millis() - curLoopTime > 10000) {
        curLoopTime = millis();
        Serial2.printf("MIDI KeepAlive: USB %d %d %d ", usbMidiKeepAlive[0],
                       usbMidiKeepAlive[1], usbMidiKeepAlive[2]);
        Serial2.printf("/ BLE %d %d %d\r\n", bleMidiKeepAlive[0],
                       bleMidiKeepAlive[1], bleMidiKeepAlive[2]);
        for (uint8_t i = 0; i < 3; i++) {
            usbMidiKeepAlive[i] = false;
            bleMidiKeepAlive[i] = false;
        }
    }
    // update Button State
    M5.update();

    switch (state) {
        case IDLE:
            if (M5.BtnA.wasReleased()) {
                Serial2.printf("State=%d: Button A was Released\n", state);
                // Select USB MIDI Cable Number
                usbMidiCableNumber = (usbMidiCableNumber + 1) % 3;
                display_main(usbMidiCableNumber);
            } else if (M5.BtnA.pressedFor(1000)) {
                Serial2.printf("State=%d: Button A was Pressed for 1000msec\n",
                               state);
                if (bleMidiDevice[usbMidiCableNumber].status != BLEDISABLE) {
                    // Disconnect Process
                    Serial2.printf("Disconnect Process\n");
                    deleteReadCB[usbMidiCableNumber] = true;
                    wait_ms(500);
                    if (bleMidiDevice[usbMidiCableNumber].status !=
                        BLERECONNECT) {
                        BleMIDI[usbMidiCableNumber]->end();
                    }
                    bleMidiDevice[usbMidiCableNumber].status = BLEDISABLE;
                    Serial2.printf("Delete Client\n");
                    BLEDevice::deleteClient(BLEDevice::getClientByPeerAddress(
                        bleMidiDevice[usbMidiCableNumber].bda));
                    display_information((char *)"Remove Device!!  ");
                    bleMidiDevice[usbMidiCableNumber].name = "No Device";
                    bleMidiDevice[usbMidiCableNumber].bda = NimBLEAddress("");
                    prefs.putString(
                        prefs_namespace_name[usbMidiCableNumber],
                        bleMidiDevice[usbMidiCableNumber].name.c_str());
                    prefs.putString(prefs_namespace_bda[usbMidiCableNumber],
                                    bleMidiDevice[usbMidiCableNumber]
                                        .bda.toString()
                                        .c_str());

                    wait_ms(1000);
                    display_frame();
                    display_main(usbMidiCableNumber);
                } else {
                    // Change State to SCANNED
                    scan();
                    state = SCANNED;
                }
                // Wait for Releasing BtnA
                while (!M5.BtnA.isReleased()) M5.update();
            } else {
                // Auto-Reconnect Callback, Scan Connected Devices
                for (uint8_t j = 0; j < BLE_MIDI_DEVICE_COUNT; j++) {
                    NimBLEClient *peer =
                        BLEDevice::getClientByPeerAddress(bleMidiDevice[j].bda);
                    if (peer == nullptr &&
                        bleMidiDevice[j].status == BLERECONNECT) {
                        Serial2.printf("Re-connecting Device: idx = %d\r\n", j);
                        display_main(usbMidiCableNumber);
                    } else if (peer == nullptr &&
                               bleMidiDevice[j].status != BLEDISABLE) {
                        Serial2.printf("Removed Device: idx = %d\r\n", j);
                        bleMidiDevice[j].status = BLEDISABLE;
                        display_main(usbMidiCableNumber);
                    } else if (peer != nullptr && !peer->isConnected() &&
                               bleMidiDevice[j].status != BLEDISCONNECT) {
                        Serial2.printf("Disconnect Device: idx = %d\r\n", j);
                        bleMidiDevice[j].status = BLEDISCONNECT;
                        display_main(usbMidiCableNumber);
                    } else if (peer != nullptr && peer->isConnected() &&
                               bleMidiDevice[j].status != BLECONNECT) {
                        Serial2.printf("Re-connect Device: idx = %d\r\n", j);
                        bleMidiDevice[j].status = BLECONNECT;
                        display_main(usbMidiCableNumber);
                    }
                }
            }
            break;
        case SCANNED:
            if (M5.BtnA.wasReleased()) {
                Serial2.printf("State=%d: Button A was Released\n", state);
                // Select BLE MIDi Device
                if (BleMidiDeviceList.size() == 0) {
                    display_frame();
                    display_main(usbMidiCableNumber);
                    state = IDLE;
                } else {
                    BleMidiDeviceIdx =
                        (BleMidiDeviceIdx + 1) % (BleMidiDeviceList.size() + 1);
                    display_devices();
                }
            } else if (M5.BtnA.pressedFor(1000)) {
                Serial2.printf("State=%d: Button A was Pressed for 1000msec\n",
                               state);
                // Connect to BLE MIDI Device
                if (BleMidiDeviceList.size() != 0) {
                    if (BleMidiDeviceIdx == BleMidiDeviceList.size()) {
                        // Returnb to Main(IDLE)
                        display_frame();
                        display_main(usbMidiCableNumber);
                        state = IDLE;
                        while (!M5.BtnA.isReleased()) M5.update();
                    } else {
                        manualConnection = true;
                        // Connect to Peer
                        display_information((char *)"Connecting...");
                        wait_ms(1000);
                        BleMIDI[usbMidiCableNumber]->setName(
                            BleMidiDeviceList[BleMidiDeviceIdx]
                                .bda.toString()
                                .c_str());
                        BleMIDI_Client[usbMidiCableNumber]->begin(
                            MIDI_CHANNEL_OMNI);
                        xTaskCreatePinnedToCore(
                            ReadCB,  // See FreeRTOS for more multitask info
                            "BLEMIDI-READ", 4096,
                            (uint8_t *)&usbMidiCableNumber, 1,
                            &BleReadCB_taskHandle[usbMidiCableNumber],
                            1);  // Core0 or Core1
                        Serial2.printf("BLE MIDI Client Begun\r\n");
                        while (BLEDevice::getScan()->isScanning())
                            ;
                        Serial2.printf("BLE Connceted Device : %s\r\n",
                                       BleMidiDeviceList[BleMidiDeviceIdx]
                                           .bda.toString()
                                           .c_str());
                        uint32_t curTime = millis();
                        while (millis() - curTime < 5000) {
                            if (bleMidiDevice[usbMidiCableNumber].status ==
                                BLECONNECT) {
                                break;
                            }
                        }
                        if (bleMidiDevice[usbMidiCableNumber].status !=
                            BLECONNECT) {
                            Serial2.printf("Fail to connect\r\n");
                            display_failtoconnect();
                            deleteReadCB[usbMidiCableNumber] = true;
                        } else {
                            Serial2.printf("Success to connect\r\n");
                            display_information((char *)"Connected!!      ");
                            bleMidiDevice[usbMidiCableNumber].name =
                                BleMidiDeviceList[BleMidiDeviceIdx].name;
                            bleMidiDevice[usbMidiCableNumber].bda =
                                BleMidiDeviceList[BleMidiDeviceIdx].bda;
                            prefs.putString(
                                prefs_namespace_name[usbMidiCableNumber],
                                bleMidiDevice[usbMidiCableNumber].name.c_str());
                            prefs.putString(
                                prefs_namespace_bda[usbMidiCableNumber],
                                bleMidiDevice[usbMidiCableNumber]
                                    .bda.toString()
                                    .c_str());
                        }
                        wait_ms(1000);
                        state = IDLE;
                        manualConnection = false;
                        while (!M5.BtnA.wasReleased() &&
                               bleMidiDevice[usbMidiCableNumber].status !=
                                   BLECONNECT)
                            M5.update();
                        display_frame();
                        display_main(usbMidiCableNumber);
                    }
                } else {
                    while (!M5.BtnA.isReleased()) M5.update();
                }
            }
            break;
        default:
            break;
    }
}

// Display Function
void display_frame() {
    M5.Lcd.fillRect(0, 51, M5.Display.width(), M5.Display.height(), BLACK);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(0, 35);
    M5.Lcd.printf("---------------------");
    M5.Lcd.setCursor(0, 115);
    M5.Lcd.printf("---------------------");
}

void display_main(uint8_t inv_color_idx) {
    M5.Lcd.setCursor(0, 51);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.printf(" CN Device            \n");
    for (uint8_t i = 0; i < 3; i++) {
        M5.Lcd.setTextColor(WHITE, BLACK);
        M5.Lcd.printf(" %2i ", i);
        if (bleMidiDevice[i].status == BLECONNECT) {
            if (inv_color_idx == i) {
                M5.Lcd.setTextColor(BLACK, CYAN);
            } else {
                M5.Lcd.setTextColor(CYAN, BLACK);
            }
        } else if (bleMidiDevice[i].status == BLEDISCONNECT ||
                   bleMidiDevice[i].status == BLERECONNECT) {
            if (inv_color_idx == i) {
                M5.Lcd.setTextColor(BLACK, RED);
            } else {
                M5.Lcd.setTextColor(RED, BLACK);
            }
        } else {
            if (inv_color_idx == i) {
                M5.Lcd.setTextColor(BLACK, WHITE);
            } else {
                M5.Lcd.setTextColor(WHITE, BLACK);
            }
        }
        M5.Lcd.printf("%s\n", bleMidiDevice[i].name.c_str());
    }
}

void display_header() {
    M5.Lcd.fillRect(0, 0, 240, 35, BLACK);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("\n\n USB-BLE MIDI Bridge \n");
}

void display_nodevices() {
    display_frame();
    M5.Lcd.setCursor(0, 67);
    M5.Lcd.printf(" No BLE MIDI Found   \n");
    M5.Lcd.printf(" Click to Return     \n");
}

void display_failtoconnect() {
    display_frame();
    M5.Lcd.setCursor(0, 67);
    M5.Lcd.printf(" Fail to connect     \n");
    M5.Lcd.printf(" Click to Return     \n");
}

void display_devices() {
    display_frame();
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(0, 51);
    M5.Lcd.printf(" Discovered Devices  \n");
    M5.Lcd.setCursor(0, 59);
    for (uint8_t i = 0; i < BleMidiDeviceList.size() + 1; i++) {
        if (i > 5) {
            // Max List Device is 5
            break;
        }
        if (i == BleMidiDeviceIdx) {
            M5.Lcd.setTextColor(BLACK, WHITE);
        } else {
            M5.Lcd.setTextColor(WHITE, BLACK);
        }
        M5.Lcd.setCursor(10, M5.Lcd.getCursorY());
        if (i < BleMidiDeviceList.size()) {
            // List up BLE MIDi Device
            Serial2.printf("--------------------------------------\n");
            Serial2.printf("getName: %s\n", BleMidiDeviceList[i].name.c_str());
            Serial2.printf("getAddress: %s\n",
                           BleMidiDeviceList[i].bda.toString().c_str());
            M5.Lcd.printf("%s\n", BleMidiDeviceList[i].name.c_str());
        } else {
            M5.Lcd.printf("return\n");
        }
    }
}

void display_information(char *str) {
    display_frame();
    M5.Lcd.setCursor(0, 75);
    M5.Lcd.printf("   ");
    M5.Lcd.printf(str);
}

#endif