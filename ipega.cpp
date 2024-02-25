#include "ipega.h"

#include <iostream>

#define DEBUG_INFO 0
#define PRESSED 1

/******************************************************************************/
typedef enum {
  Type = 6,         //
  ButtonGroup = 7,  //
  State = 4,        //
  Value = 5         //
} Bytes;
/******************************************************************************/
typedef enum {
  Single = 1,  //
  Line = 2     //
} Types;
/******************************************************************************/
typedef enum {
  A = 0,                 //
  B = 1,                 //
  Y = 2,                 //
  X = 3,                 //
  LeftBumper = 4,        //
  RightBumper = 5,       //
  LeftTrigger = 6,       //
  RightTrigger = 7,      //
  Start = 8,             //
  Select = 9,            //
  Home = 10,             //
  LeftStickButton = 11,  //
  RightStickButton = 12  //
} Singles;
/******************************************************************************/
typedef enum {
  StickLeftHorizontal = 0,   //
  StickLeftVertical = 1,     //
  TriggerLeft = 2,           //
  StickRightHorizontal = 3,  //
  StickRightVertical = 4,    //
  TriggerRight = 5,          //
  DpadHorizontal = 6,        //
  DpadVertical = 7           //
} Lines;
/******************************************************************************/
typedef enum {
  Released = 0,  //
  Left1 = 1,     //
  Left2 = 128,   //
  Right1 = 255,  //
  Right2 = 127,  //
  Up1 = 1,       //
  Up2 = 128,     //
  Down1 = 255,   //
  Down2 = 127    //
} DpadValues;
/******************************************************************************/
/******************************************************************************/
Ipega::Ipega(const std::string& path_) {
  path = path_;
  file_descriptor = open(path.c_str(), O_RDONLY);
  if (file_descriptor < 0) {
    throw std::runtime_error("failed to open " + path);
  }
}
/******************************************************************************/
void Ipega::spin() {
  bytes_read = read(file_descriptor, buffer, sizeof(buffer));
  parseData(buffer, bytes_read);
  // printRaw();
}
/******************************************************************************/
void Ipega::printRaw() {
  if (bytes_read <= 0) {
    return;
  }
  for (int i = 0; i != bytes_read; ++i) {
    std::cout << buffer[i] << " : ";
  }
  std::cout << std::endl;
}
/******************************************************************************/
void Ipega::printState() {
  std::cout << "**********************************************" << std::endl;
  std::cout << "start_button: " << start_button.state << std::endl;
  std::cout << "select_button: " << select_button.state << std::endl;
  std::cout << "home_button: " << home_button.state << std::endl;
  std::cout << "------" << std::endl;
  std::cout << "dpad_left: " << dpad_left.state << std::endl;
  std::cout << "dpad_right: " << dpad_right.state << std::endl;
  std::cout << "dpad_up: " << dpad_up.state << std::endl;
  std::cout << "dpad_down: " << dpad_down.state << std::endl;
  std::cout << "------" << std::endl;
  std::cout << "A: " << A.state << std::endl;
  std::cout << "B: " << B.state << std::endl;
  std::cout << "Y: " << Y.state << std::endl;
  std::cout << "X: " << X.state << std::endl;
  std::cout << "------" << std::endl;
  std::cout << "bump_left: " << bump_left.state << std::endl;
  std::cout << "bump_right: " << bump_right.state << std::endl;
  std::cout << "------" << std::endl;
  std::cout << "trig_left: " << trig_left.state << " : " << trig_left.value
            << std::endl;
  std::cout << "trig_right: " << trig_right.state << " : " << trig_right.value
            << std::endl;
  std::cout << "------" << std::endl;
  std::cout << "joy_left: " << joy_left.x_value << " : " << joy_left.y_value
            << std::endl;
  std::cout << "joy_right: " << joy_right.x_value << " : " << joy_right.y_value
            << std::endl;
  std::cout << "------" << std::endl;
  std::cout << "left_stick_button: " << lsb.state << std::endl;
  std::cout << "right_stick_button: " << rsb.state << std::endl;
  std::cout << "**********************************************" << std::endl;
}
bool Ipega::getStartButton() { return start_button.state; }
/******************************************************************************/
bool Ipega::getSelectButton() { return select_button.state; }
/******************************************************************************/
bool Ipega::getHomeButton() { return home_button.state; }
/******************************************************************************/
bool Ipega::getDpadLeft() { return dpad_left.state; }
/******************************************************************************/
bool Ipega::getDpadRight() { return dpad_right.state; }
/******************************************************************************/
bool Ipega::getDpadUp() { return dpad_up.state; }
/******************************************************************************/
bool Ipega::getDpadDown() { return dpad_down.state; }
/******************************************************************************/
bool Ipega::getAButton() { return A.state; }
/******************************************************************************/
bool Ipega::getBButton() { return B.state; }
/******************************************************************************/
bool Ipega::getXButton() { return X.state; }
/******************************************************************************/
bool Ipega::getYButton() { return Y.state; }
/******************************************************************************/
bool Ipega::getBumperLeft() { return bump_left.state; }
/******************************************************************************/
bool Ipega::getBumperRight() { return bump_right.state; }
/******************************************************************************/
bool Ipega::getRightStickButton() { return rsb.state; }
/******************************************************************************/
bool Ipega::getLeftStickButton() { return lsb.state; }
/******************************************************************************/
std::tuple<bool, int> Ipega::getTriggerLeft() {
  return std::tuple<bool, int>(trig_left.state, trig_left.value);
}
/******************************************************************************/
std::tuple<bool, int> Ipega::getTriggerRight() {
  return std::tuple<bool, int>(trig_right.state, trig_right.value);
}
/******************************************************************************/
std::tuple<int, int> Ipega::getLeftStick() {
  return std::tuple<int, int>(joy_left.x_value, joy_left.y_value);
}
/******************************************************************************/
std::tuple<int, int> Ipega::getRightStick() {
  return std::tuple<int, int>(joy_right.x_value, joy_right.y_value);
}
/******************************************************************************/
void Ipega::parseData(uint8_t* data, int bytes) {
  if (bytes == 16) {
    // Parse as two 8-bytes messages
    parseData(data, 8);
    parseData(data + 8, 8);
    return;
  } else if (bytes == 8) {
    decodeData(data);
  } else {
    std::cerr << "Can not read the message of size " << bytes << std::endl;
  }
}
/******************************************************************************/
void Ipega::decodeData(uint8_t* data) {
  uint8_t type = data[Bytes::Type];

  if (DEBUG_INFO) {
    std::cout << "type: " << (int)type << std::endl;
  }

  if (type == Types::Single) {
    decodeSingleData(data);
  }
  if (type == Types::Line) {
    decodeLineData(data);
  }
  if (DEBUG_INFO) {
    std::cout << "---------------" << std::endl;
  }
}
/******************************************************************************/
void Ipega::decodeSingleData(uint8_t* data) {
  uint8_t state = data[Bytes::State];
  uint8_t button_single = data[Bytes::ButtonGroup];

  if (DEBUG_INFO) {
    std::cout << "button_single: " << (int)button_single << std::endl;
    std::cout << "state: " << (int)state << std::endl;
  }

  // New state for button
  bool state_ = (state == PRESSED);
  //
  if (button_single == Singles::A) {
    A.state = state_;
  }
  if (button_single == Singles::B) {
    B.state = state_;
  }
  if (button_single == Singles::Y) {
    Y.state = state_;
  }
  if (button_single == Singles::X) {
    X.state = state_;
  }
  if (button_single == Singles::LeftBumper) {
    bump_left.state = state_;
  }
  if (button_single == Singles::RightBumper) {
    bump_right.state = state_;
  }
  if (button_single == Singles::LeftTrigger) {
    trig_left.state = state_;
  }
  if (button_single == Singles::RightTrigger) {
    trig_right.state = state_;
  }
  if (button_single == Singles::Start) {
    start_button.state = state_;
  }
  if (button_single == Singles::Select) {
    select_button.state = state_;
  }
  if (button_single == Singles::Home) {
    home_button.state = state_;
  }
  if (button_single == Singles::LeftStickButton) {
    lsb.state = state_;
  }
  if (button_single == Singles::RightStickButton) {
    rsb.state = state_;
  }
}
/******************************************************************************/
void Ipega::decodeLineData(uint8_t* data) {
  uint8_t button_line = data[Bytes::ButtonGroup];

  if (DEBUG_INFO) {
    std::cout << "button_line: " << (int)button_line << std::endl;
  }

  if (button_line == Lines::DpadHorizontal) {
    decodeDpadHorizontal(data);
  }
  if (button_line == Lines::DpadVertical) {
    decodeDpadVertical(data);
  }
  if (button_line == Lines::TriggerLeft) {
    decodeTriggerLeft(data);
  }
  if (button_line == Lines::TriggerRight) {
    decodeTriggerRight(data);
  }
  if (button_line == Lines::StickLeftHorizontal) {
    decodeJoyLeftHorizontal(data);
  }
  if (button_line == Lines::StickLeftVertical) {
    decodeJoyLeftVertical(data);
  }
  if (button_line == Lines::StickRightHorizontal) {
    decodeJoyRightHorizontal(data);
  }
  if (button_line == Lines::StickRightVertical) {
    decodeJoyRightVertical(data);
  }
}
/******************************************************************************/
void Ipega::decodeDpadVertical(uint8_t* data) {
  uint8_t value1 = data[Bytes::State];
  uint8_t value2 = data[Bytes::Value];

  if (DEBUG_INFO) {
    std::cout << "value1: " << (int)value1 << std::endl;
    std::cout << "value2: " << (int)value2 << std::endl;
  }

  if (value1 == DpadValues::Down1 && value2 == DpadValues::Down2) {
    dpad_down.state = true;
  }
  if (value1 == DpadValues::Up1 && value2 == DpadValues::Up2) {
    dpad_up.state = true;
  }
  if (value1 == DpadValues::Released && value2 == DpadValues::Released) {
    dpad_down.state = false;
    dpad_up.state = false;
  }
}
/******************************************************************************/
void Ipega::decodeDpadHorizontal(uint8_t* data) {
  uint8_t value1 = data[Bytes::State];
  uint8_t value2 = data[Bytes::Value];

  if (DEBUG_INFO) {
    std::cout << "value1: " << (int)value1 << std::endl;
    std::cout << "value2: " << (int)value2 << std::endl;
  }

  if (value1 == DpadValues::Left1 && value2 == DpadValues::Left2) {
    dpad_left.state = true;
  }
  if (value1 == DpadValues::Right1 && value2 == DpadValues::Right2) {
    dpad_right.state = true;
  }
  if (value1 == DpadValues::Released && value2 == DpadValues::Released) {
    dpad_left.state = false;
    dpad_right.state = false;
  }
}
/******************************************************************************/
void Ipega::decodeJoyLeftVertical(uint8_t* data) {
  int8_t raw_value = data[Bytes::Value];
  int value = (int)raw_value * (-1);

  if (abs(value) < 5) {
    value = 0;
  }

  if (DEBUG_INFO) {
    std::cout << value << std::endl;
  }

  // Convert to make upper position positive value
  joy_left.y_value = value;
}
/******************************************************************************/
void Ipega::decodeJoyLeftHorizontal(uint8_t* data) {
  int8_t raw_value = data[Bytes::Value];
  int value = (int)raw_value;

  if (abs(value) < 5) {
    value = 0;
  }

  if (DEBUG_INFO) {
    std::cout << value << std::endl;
  }

  joy_left.x_value = value;
}
/******************************************************************************/
void Ipega::decodeJoyRightVertical(uint8_t* data) {
  int8_t raw_value = data[Bytes::Value];
  int value = (int)raw_value * (-1);

  if (abs(value) < 5) {
    value = 0;
  }

  if (DEBUG_INFO) {
    std::cout << value << std::endl;
  }

  // Convert to make upper position positive value
  joy_right.y_value = value;
}
/******************************************************************************/
void Ipega::decodeJoyRightHorizontal(uint8_t* data) {
  int8_t raw_value = data[Bytes::Value];
  int value = (int)raw_value;

  if (abs(value) < 5) {
    value = 0;
  }

  if (DEBUG_INFO) {
    std::cout << value << std::endl;
  }

  joy_right.x_value = value;
}
/******************************************************************************/
void Ipega::decodeTriggerLeft(uint8_t* data) {
  int8_t raw_value = data[Bytes::Value];
  // From [-128; 127] to [0; 255]
  int value = (int)raw_value + 128;

  if (DEBUG_INFO) {
    std::cout << "Value: " << value << std::endl;
  }

  trig_left.value = value;
}
/******************************************************************************/
void Ipega::decodeTriggerRight(uint8_t* data) {
  int8_t raw_value = data[Bytes::Value];
  // From [-128; 127] to [0; 255]
  int value = (int)raw_value + 128;

  if (DEBUG_INFO) {
    std::cout << "Value: " << value << std::endl;
  }

  trig_right.value = value;
}
/******************************************************************************/
