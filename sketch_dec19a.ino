#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>

// ================= WIFI =================
const char* ssid = "AutoFarming-GreenPulse";
const char* password = "dexAlpha";

// ================= PINS =================
#define DEVICE_A 2
#define DEVICE_B 4

#define SOIL_SERVO_PIN 18
#define RADAR_SERVO_PIN 19

#define MOISTURE_PIN 34
#define TRIG_PIN 26
#define ECHO_PIN 27

#define DHTPIN 25
#define DHTTYPE DHT11

// RGB LED (COMMON ANODE)
#define LED_R 32
#define LED_G 33
#define LED_B 16

// TCS3200
#define TCS_S0 14
#define TCS_S1 12
#define TCS_S2 13
#define TCS_S3 15
#define TCS_OUT 17

// ================= OBJECTS =================
WebServer server(80);
Servo soilServo, radarServo;
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;

// ================= VARIABLES =================
int moisture = 0;
float temperature = 0, humidity = 0, lux = 0;
int radarAngle = 0, radarDir = 1, radarDistance = 0;
unsigned long lastDHT = 0;

String timeOfDay = "Unknown";
String leafColor = "Unknown";
String plantNeeds = "Normal";

int redVal = 0, greenVal = 0, blueVal = 0;

// Moisture servo control
bool deployMoisture = false;
bool moistureDone = false;

// ================= RGB CONTROL =================
void setRGB(int r, int g, int b) {
  digitalWrite(LED_R, r > 0 ? LOW : HIGH);
  digitalWrite(LED_G, g > 0 ? LOW : HIGH);
  digitalWrite(LED_B, b > 0 ? LOW : HIGH);
}

// ================= ULTRASONIC =================
int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long d = pulseIn(ECHO_PIN, HIGH, 30000);
  return d == 0 ? 0 : d * 0.034 / 2;
}

// ================= COLOR SENSOR =================
int readColor(bool s2, bool s3) {
  digitalWrite(TCS_S2, s2);
  digitalWrite(TCS_S3, s3);
  delay(5);
  return pulseIn(TCS_OUT, LOW);
}

void detectLeafColor() {
  redVal   = readColor(LOW, LOW);
  greenVal = readColor(HIGH, HIGH);
  blueVal  = readColor(LOW, HIGH);

  if (greenVal < redVal && greenVal < blueVal)
    leafColor = "Green";
  else if (redVal < greenVal && redVal < blueVal)
    leafColor = "Yellowing";
  else
    leafColor = "Pale";
}

// ================= TIME LOGIC =================
void calculateTime() {
  if (lux < 50) timeOfDay = "Night";
  else if (lux < 300) timeOfDay = "Morning/Evening";
  else timeOfDay = "Day";
}

// ================= PLANT LOGIC =================
void evaluatePlant() {
  plantNeeds = "";

  bool needWater = moisture > 1800;
  bool needLight = lux < 100;
  bool needShadow = lux > 25000;

  if (needWater) plantNeeds += "Water, ";
  if (needLight) plantNeeds += "Light, ";
  if (needShadow) plantNeeds += "Shadow, ";
  if (temperature < 15) plantNeeds += "Warmth, ";
  if (temperature > 35) plantNeeds += "Cooling, ";
  if (leafColor == "Yellowing") plantNeeds += "Nutrition, ";

  if (plantNeeds.length() == 0) {
    plantNeeds = "Normal";
    setRGB(0, 255, 0);
  } else {
    plantNeeds.remove(plantNeeds.length() - 2);

    if (needWater) setRGB(0, 0, 255);
    else if (needLight) setRGB(255, 255, 0);
    else if (needShadow) setRGB(255, 0, 255);
  }
}

// ================= HTML =================
String htmlPage() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body{background:black;color:#22c55e;font-family:monospace;text-align:center}
.box{border:1px solid #22c55e;padding:10px;margin:10px}
button{padding:10px;background:#22c55e;color:black;border:none;font-size:16px}
canvas{background:black}
</style>
</head>
<body>

<h2>AutoFarming-GreenPulse</h2>

<div class="box">
<b>Status</b><br>
Time acc. to Light: <span id="time"></span><br>
Temparature: <span id="t"></span>°C<br>
Environment Humidity: <span id="h"></span> %<br>
Soil Moisture: <span id="s"></span><br>
Light Intensity: <span id="l"></span> lux<br>
Leaf Colour: <span id="leaf"></span>
</div>

<div class="box">
<button onclick="fetch('/deploy')">Deploy Moisture Sensor</button>
</div>

<div class="box">
<b>Radar</b><br>
<canvas id="radar" width="300" height="150"></canvas><br>
Distance: <span id="d"></span> cm
</div>

<div class="box">
<b>Plant Needs</b><br>
<span id="need"></span>
</div>

<script>
const c=document.getElementById("radar");
const ctx=c.getContext("2d");

setInterval(()=>{
 fetch("/data").then(r=>r.json()).then(j=>{
  time.innerText=j.time;
  t.innerText=j.t;
  h.innerText=j.h;
  s.innerText=j.s;
  l.innerText=j.l;
  leaf.innerText=j.leaf;
  need.innerText=j.need;
  d.innerText=j.d;

  ctx.clearRect(0,0,300,150);
  ctx.strokeStyle="#22c55e";
  ctx.beginPath();
  ctx.arc(150,150,140,Math.PI,0);
  ctx.stroke();

  if(j.d>0){
    let x=150+j.d*Math.cos(j.a*Math.PI/180);
    let y=150-j.d*Math.sin(j.a*Math.PI/180);
    ctx.fillStyle="red";
    ctx.beginPath();
    ctx.arc(x,y,4,0,6.28);
    ctx.fill();
  }
 });
},400);
</script>
</body>
</html>
)rawliteral";
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(DEVICE_A, OUTPUT);
  pinMode(DEVICE_B, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  pinMode(TCS_S0, OUTPUT);
  pinMode(TCS_S1, OUTPUT);
  pinMode(TCS_S2, OUTPUT);
  pinMode(TCS_S3, OUTPUT);
  pinMode(TCS_OUT, INPUT);

  digitalWrite(TCS_S0, HIGH);
  digitalWrite(TCS_S1, LOW);

  soilServo.attach(SOIL_SERVO_PIN);
  radarServo.attach(RADAR_SERVO_PIN);
  soilServo.write(0);   // Retracted position

  dht.begin();
  Wire.begin(21,22);
  lightMeter.begin();

  WiFi.softAP(ssid, password);

  server.on("/", [](){ server.send(200,"text/html",htmlPage()); });

  server.on("/deploy", [](){
    deployMoisture = true;
    moistureDone = false;
    server.send(200,"text/plain","OK");
  });

  server.on("/data", [](){
    String leafInfo = leafColor + " (R:" + redVal + " G:" + greenVal + " B:" + blueVal + ")";
    String j="{\"time\":\""+timeOfDay+"\",\"t\":"+String(temperature)+",\"h\":"+String(humidity)+
             ",\"s\":"+String(moisture)+",\"l\":"+String(lux)+
             ",\"leaf\":\""+leafInfo+"\",\"need\":\""+plantNeeds+
             "\",\"a\":"+String(radarAngle)+",\"d\":"+String(radarDistance)+"}";
    server.send(200,"application/json",j);
  });

  server.begin();
}

// ================= LOOP =================
void loop() {
  server.handleClient();

  // Radar
  radarServo.write(radarAngle);
  radarDistance = getDistance();
  digitalWrite(DEVICE_B, (radarDistance > 0 && radarDistance < 30) ? HIGH : LOW);

  radarAngle += radarDir * 2;
  if (radarAngle >= 180 || radarAngle <= 0) radarDir *= -1;

  // Moisture servo logic (ONLY on button press)
  if (deployMoisture && !moistureDone) {
    soilServo.write(70);      // Deploy
    delay(800);
    moisture = analogRead(MOISTURE_PIN);
    delay(200);
    soilServo.write(0);       // Retract
    moistureDone = true;
    deployMoisture = false;
  }

  lux = lightMeter.readLightLevel();
  detectLeafColor();
  calculateTime();

  if (millis() - lastDHT > 1500) {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    lastDHT = millis();
  }

  evaluatePlant();
  delay(30);
}
