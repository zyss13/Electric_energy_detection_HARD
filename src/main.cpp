#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

const char* ssid = "HUAWEI-dreamland";
const char* password = "83238099";
const char* mqtt_server = "124.221.117.134";
String modbus_data = ""; //接收返回的Modbus数据
const byte modbus_querry[] = { 0x01, 0x03, 0x00, 0x48, 0x00, 0x0A, 0x45, 0xDB }; //Modbus查询指令
char mqtt_json[200]; //返回mqtt的json字符数组
const int RELAY_SWITCH = D8;
int flag = 0;
int data[7];

WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback1(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.printf("payload:%d",i);
    Serial.println((char)payload[i]);
  }

  if ((char)payload[1] == '1') {
    digitalWrite(RELAY_SWITCH,HIGH);
    // Serial.println("==1");
    delay(1000);
  } else if((char)payload[1] == '0'){
    // digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    digitalWrite(RELAY_SWITCH,LOW);
    // Serial.println("==0");
    delay(1000);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("charger_Topic");
      // Serial.println("000");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  // pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  // pinMode(D8, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(RELAY_SWITCH, OUTPUT);
  // Serial.begin(9600);
  Serial.begin(4800); //读取功率计的通信波特率，用TX0，RX0
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback1);
  oled.setI2CAddress(0x3C*2);
  oled.begin();
  oled.enableUTF8Print();
}

void blink(){
  digitalWrite(2,HIGH);
  delay(500);
  digitalWrite(2,LOW);
  delay(500);
}

void readModbus() {
  Serial.write(modbus_querry, (sizeof(modbus_querry) / sizeof(modbus_querry[0])));
  delay(500);
  if (Serial.available()) {
    modbus_data = Serial.readString();
    Serial.println(modbus_data);
    flag = 1;
  }
}

// 将Modbus返回的16进制字符数组，转成10进制
int char2int(String str, int left, int right) {
  int result = 0;
  int weight = 1;
  for (int i = right-1; i >= left; i--) {
      int out_dec = (int)str[i];
      // Serial.print("out_dec_35:");
      // Serial.println(out_dec);
      result += out_dec * weight;
      weight = weight * 16 * 16;
  }

  return result;
}

// 解析Modbus返回数据
void resolve() {
  // int data[7];
  data[0] = char2int(modbus_data,3,5); //电压
  data[1] = char2int(modbus_data,5,7); //电流
  data[2] = char2int(modbus_data,7,9); //有功功率
  data[3] = char2int(modbus_data,9,13); //有功总电能，2个寄存器
  data[4] = char2int(modbus_data,13,15); //功率因数
  data[5] = char2int(modbus_data,15,19); //二氧化碳，2个寄存器
  data[6] = char2int(modbus_data,21,23); //频率
  // 封装成JSON格式
  sprintf(mqtt_json, "{\"voltage\":%d,\"current\":%d,\"c_power\":%d,\"all_enegy\":%d,\"power_factor\":%d,\"coo\":%d,\"hz\":%d}", data[0], data[1], data[2], data[3], data[4], data[5], data[6]);
}
void show_oled() {
  oled.setFont(u8g2_font_timR12_tf);
  oled.setFontPosTop();
  oled.setCursor(0,10);
  oled.print("Voltage: ");
  oled.print(data[0]/100);
  oled.print("V");
  oled.setCursor(0,30);
  oled.print("Current: ");
  oled.print(1.0*data[1]/100);
  oled.print("A");
  oled.setCursor(0,50);
  oled.print("Power: ");
  oled.print(data[2]);
  oled.print("W");
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  // Serial.println("111");
  client.loop();
  blink();
  readModbus();
  resolve();
  delay(100);
  if(flag){
    client.publish("up_data",mqtt_json);
    flag = 0;
  }
  oled.firstPage();
  do
  {
    show_oled();
  }while(oled.nextPage());

  delay(2000);

}


