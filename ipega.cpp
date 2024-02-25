#include "ipega.h"

#include <iostream>

#define DEBUG_INFO 1

/******************************************************************************/
typedef enum {
  Type = 6,             //
  ButtonLine = 7,       //
  ValueAdditional = 4,  //
  Value = 5             //
} Bytes;
/******************************************************************************/
typedef enum {
  Single = 1,  //
  Line = 2     //
} Types;
/******************************************************************************/
typedef enum {
  StickLeftHorizontal = 0,
  StickLeftVertical = 1,
  TriggerLeft = 2,
  StickRightHorizontal = 3,
  StickRightVertical = 4,
  TriggerRight = 5,
  DpadHorizontal = 6,
  DpadVertical = 7
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
void Ipega::parseData() {
  if (bytes_read == 16) {
    char additional[8];
    memcpy(additional, &buffer[8], 8);
    for (int i = 0; i != bytes_read; ++i) {
      std::cout << (int)buffer[i] << ";";
    }
    std::cout << std::endl;
    for (int i = 0; i != 8; ++i) {
      std::cout << (int)additional[i] << ";";
    }
    std::cout << std::endl;
  }
  char group = buffer[6];
  std::cout << (int)group << std::endl;
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
  std::cout << "---------------" << std::endl;
}
/******************************************************************************/
void Ipega::decodeSingleData(uint8_t* data) {}
/******************************************************************************/
void Ipega::decodeLineData(uint8_t* data) {
  uint8_t button_line = data[Bytes::ButtonLine];

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
  uint8_t value1 = data[Bytes::ValueAdditional];
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
    dpad_up.state = true;
  }
}
/******************************************************************************/
void Ipega::decodeDpadHorizontal(uint8_t* data) {
  uint8_t value1 = data[Bytes::ValueAdditional];
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
  if (DEBUG_INFO) {
    std::cout << value << std::endl;
  }

  joy_left.x_value = value;
}
/******************************************************************************/
void Ipega::decodeJoyRightVertical(uint8_t* data) {
  int8_t raw_value = data[Bytes::Value];
  int value = (int)raw_value * (-1);
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
  if (DEBUG_INFO) {
    std::cout << value << std::endl;
  }

  joy_left.x_value = value;
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
