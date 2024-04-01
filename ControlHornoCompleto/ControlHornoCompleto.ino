#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <PID_v1.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección I2C, Número de columnas y filas
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {13, 10, A1, A0};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables para el PID
double setpoint = 25.0;  // Temperatura de referencia
double input = 0.0;      // Temperatura medida con PT100
double output = 0.0;     // Salida del PID
double kp = 1.0;         // Ganancia proporcional
double ki = 0.1;         // Ganancia integral
double kd = 0.01;        // Ganancia derivativa
PID pid(&input, &output, &setpoint, kp, ki, kd, DIRECT);

// Estados de la máquina de estados finitos
enum State {
  MENU_MAIN,
  MENU_PID_SETTINGS,
  MENU_OTHER_SETTINGS
};

State currentState = MENU_MAIN;

void setup() {
  lcd.begin(16, 2);
  lcd.print("Temperatura: ");
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    handleKeypadInput(key);
  }

  // Actualizar la temperatura con la PT100 (ajustar según tu hardware)
  input = readTemperature();

  // Actualizar el estado de la máquina de estados finitos
  updateStateMachine();

  // Controlar la salida del PID
  pid.Compute();

  // Mostrar información en la pantalla LCD
  displayTemperature();
  displayPIDOutput();
  displayMenu();
}

void handleKeypadInput(char key) {
  switch (currentState) {
    case MENU_MAIN:
      handleMainMenuInput(key);
      break;
    case MENU_PID_SETTINGS:
      handlePIDSettingsMenuInput(key);
      break;
    // Puedes agregar más casos según sea necesario para otros menús
  }
}

void handleMainMenuInput(char key) {
  switch (key) {
    case 'A':
      currentState = MENU_PID_SETTINGS;
      break;
    case 'B':
      // Puedes agregar código para otro menú o ajuste
      break;
    case 'C':
      // Puedes agregar código para otro menú o ajuste
      break;
    case 'D':
      lcd.clear();
      break;
    default:
      // Puedes agregar código para manejar otras teclas
      break;
  }
}

void handlePIDSettingsMenuInput(char key) {
  switch (key) {
    case 'A':
      // Puedes agregar código para ajustar parámetros del PID
      break;
    case 'B':
      // Puedes agregar código para otro menú o ajuste en las configuraciones PID
      break;
    case 'D':
      currentState = MENU_MAIN;
      break;
    default:
      // Puedes agregar código para manejar otras teclas
      break;
  }
}

void updateStateMachine() {
  // Puedes agregar lógica adicional según sea necesario para transiciones de estado
}

double readTemperature() {
  // Implementar lectura de temperatura con PT100
  // Devuelve un valor simulado para propósitos de demostración
  return random(20, 30);
}

void displayTemperature() {
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(input);
  lcd.print(" C   ");
}

void displayPIDOutput() {
  lcd.setCursor(0, 1);
  lcd.print("PID: ");
  lcd.print(output);
}

void displayMenu() {
  lcd.setCursor(10, 1);
  switch (currentState) {
    case MENU_MAIN:
      lcd.print("MENU");
      break;
    case MENU_PID_SETTINGS:
      lcd.print("PID ");
      break;
    // Puedes agregar más casos según sea necesario para otros menús
  }
}
