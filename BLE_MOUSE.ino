#include <BleMouse.h>

// Пины для подключения джойстика (обычно используются GPIO)
const int JOYSTICK_X_PIN = 34;  // Пин для оси X (обычно аналоговый)
const int JOYSTICK_Y_PIN = 35;  // Пин для оси Y (обычно аналоговый)
const int JOYSTICK_BTN_PIN = 32; // Пин для кнопки джойстика (опционально)

// Калибровочные значения (измените после определения нулевой точки)
int centerX = 0;
int centerY = 0;
const int DEAD_ZONE = 15;      // Мертвая зона для предотвращения дрожания
const int MAX_SPEED = 30;       // Максимальная скорость движения

BleMouse bleMouse("ESP32 Joystick Mouse", "ESP32", 100);

void setup() {
  Serial.begin(115200);
  
  // Настройка пинов
  pinMode(JOYSTICK_X_PIN, INPUT);
  pinMode(JOYSTICK_Y_PIN, INPUT);
  pinMode(JOYSTICK_BTN_PIN, INPUT_PULLUP);  // Кнопка с подтяжкой к питанию
  
  // Калибровка центральных значений
  delay(500);  // Небольшая задержка для стабилизации
  centerX = analogRead(JOYSTICK_X_PIN);
  centerY = analogRead(JOYSTICK_Y_PIN);
  
  // Вывод калибровочных значений в Serial
  Serial.println("=== Калибровка джойстика ===");
  Serial.print("Center X: ");
  Serial.println(centerX);
  Serial.print("Center Y: ");
  Serial.println(centerY);
  Serial.println("===========================");
  
  randomSeed(esp_random());
  bleMouse.begin();
}

void loop() {
  if (bleMouse.isConnected()) {
    // Чтение значений с джойстика
    int rawX = analogRead(JOYSTICK_X_PIN);
    int rawY = analogRead(JOYSTICK_Y_PIN);
    int btnState = digitalRead(JOYSTICK_BTN_PIN);
    
    // Вычисление отклонения от центра
    int deltaX = rawX - centerX;
    int deltaY = rawY - centerY;
    
    // Применяем мертвую зону
    if (abs(deltaX) < DEAD_ZONE) deltaX = 0;
    if (abs(deltaY) < DEAD_ZONE) deltaY = 0;
    
    // Преобразуем отклонение в скорость движения мыши
    int moveX = map(abs(deltaX), 0, 2048, 0, MAX_SPEED);
    int moveY = map(abs(deltaY), 0, 2048, 0, MAX_SPEED);
    
    // Учитываем направление
    if (deltaX < 0) moveX = -moveX;
    if (deltaY > 0) moveY = -moveY;  // Инвертируем Y, если нужно
    
    // Отправка движения мыши
    bleMouse.move(moveX, moveY);
    
    // Отправка клика при нажатии кнопки
    if (btnState == LOW) {  // Кнопка нажата (LOW при INPUT_PULLUP)
      bleMouse.click(MOUSE_LEFT);
      Serial.println("Click!");
    }
    
    // Вывод отладочной информации
    //Serial.print("Raw: X=");
    //Serial.print(rawX);
    //Serial.print(", Y=");
    //Serial.print(rawY);
    //Serial.print(" | Delta: X=");
    //Serial.print(deltaX);
    //Serial.print(", Y=");
    //Serial.print(deltaY);
    //Serial.print(" | Move: X=");
    //Serial.print(moveX);
    //Serial.print(", Y=");
    //Serial.println(moveY);
  }
  
  delay(15); 
}